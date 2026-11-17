#pragma once

#include "../autodiff.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <memory>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

namespace quadra {
namespace sampling {

struct LogDensityEvaluation {
  double log_density = -std::numeric_limits<double>::infinity();
  std::vector<double> gradient;
  bool finite = false;
};

template <class LogDensity>
LogDensityEvaluation evaluate_ad_log_density(LogDensity &log_density,
                                             const std::vector<double> &q) {
  had::ADGraph graph;
  had::g_ADGraph = &graph;
  std::vector<AD> q_ad;
  q_ad.reserve(q.size());
  for (double value : q)
    q_ad.emplace_back(value);

  LogDensityEvaluation out;
  out.gradient.resize(q.size());
  try {
    const AD target = log_density(q_ad);
    out.log_density = value_of(target);
    if (std::isfinite(out.log_density)) {
      had::SetAdjoint(target, 1.0);
      had::PropagateFirstOrderAdjoint();
      out.finite = true;
      for (std::size_t i = 0; i < q.size(); ++i) {
        out.gradient[i] = had::GetAdjoint(q_ad[i]);
        out.finite = out.finite && std::isfinite(out.gradient[i]);
      }
    }
  } catch (const std::exception &) {
    out.finite = false;
  }
  had::g_ADGraph = nullptr;
  return out;
}

template <class LogDensity> class ReusableAdLogDensity {
public:
  ReusableAdLogDensity(LogDensity &log_density,
                       const std::vector<double> &initial)
      : log_density_(&log_density) {
    q_ad_.reserve(initial.size());
    for (double value : initial)
      q_ad_.emplace_back(value);
    target_ad_ = (*log_density_)(q_ad_);
  }

  LogDensityEvaluation evaluate(const std::vector<double> &q) {
    if (q.size() != q_ad_.size())
      throw std::invalid_argument(
          "ReusableAdLogDensity: parameter dimension changed");
    had::g_ADGraph = &graph_;
    for (std::size_t i = 0; i < q.size(); ++i)
      had::SetValue(q_ad_[i], q[i]);
    graph_.Forward();
    target_ad_.val = graph_.vertices[target_ad_.varId].primal;

    LogDensityEvaluation out;
    out.log_density = target_ad_.val;
    out.gradient.resize(q.size());
    if (!std::isfinite(out.log_density))
      return out;
    graph_.ZeroFirstOrderAdjoints();
    had::SetAdjoint(target_ad_, 1.0);
    had::PropagateFirstOrderAdjoint();
    out.finite = true;
    for (std::size_t i = 0; i < q.size(); ++i) {
      out.gradient[i] = had::GetAdjoint(q_ad_[i]);
      out.finite = out.finite && std::isfinite(out.gradient[i]);
    }
    return out;
  }

  std::size_t vertex_count() const { return graph_.vertices.size(); }
  std::size_t unsupported_replay_vertex_count() const {
    std::size_t count = 0;
    for (std::size_t i = 0; i < graph_.vertices.size(); ++i) {
      const auto &vertex = graph_.vertices[i];
      if (vertex.op == had::OpCode::Independent && vertex.e1.to != i)
        ++count;
    }
    return count;
  }

private:
  LogDensity *log_density_;
  had::ADGraph graph_;
  std::vector<AD> q_ad_;
  AD target_ad_;
};

template <class LogDensity> class AdLogDensityEvaluator {
public:
  AdLogDensityEvaluator(LogDensity &target, const std::vector<double> &initial,
                        bool reuse_graph)
      : target_(&target) {
    if (reuse_graph)
      reusable_.reset(new ReusableAdLogDensity<LogDensity>(target, initial));
  }

  LogDensityEvaluation operator()(const std::vector<double> &q) {
    return reusable_ ? reusable_->evaluate(q)
                     : evaluate_ad_log_density(*target_, q);
  }

  bool reuses_graph() const { return static_cast<bool>(reusable_); }

private:
  LogDensity *target_;
  std::unique_ptr<ReusableAdLogDensity<LogDensity>> reusable_;
};

struct NutsOptions {
  int warmup = 500;
  int samples = 500;
  int max_tree_depth = 10;
  double target_acceptance = 0.8;
  double initial_step_size = 0.0;
  double divergence_threshold = 1000.0;
  bool adapt_diagonal_mass = true;
  // Dense adaptation takes precedence over diagonal adaptation. It is useful
  // for strongly correlated targets but costs O(d^2) per leapfrog step.
  bool adapt_dense_mass = false;
  // Disable for targets whose C++ control flow changes with parameter values.
  bool reuse_ad_graph = true;
  std::uint64_t seed = 5489u;
};

struct NutsDiagnostics {
  int leapfrog_steps = 0;
  int divergences = 0;
  int max_depth_hits = 0;
  int warmup_leapfrog_steps = 0;
  int warmup_divergences = 0;
  int warmup_max_depth_hits = 0;
  int mass_matrix_updates = 0;
  int mass_matrix_update_failures = 0;
  double mean_acceptance = 0.0;
  double warmup_mean_acceptance = 0.0;
  double energy_bfmi = std::numeric_limits<double>::quiet_NaN();
  double step_size = 0.0;
  std::vector<double> inverse_mass;
  std::vector<std::vector<double>> dense_inverse_mass;
};

struct NutsResult {
  std::vector<std::vector<double>> draws;
  std::vector<double> log_density;
  std::vector<double> energy;
  NutsDiagnostics diagnostics;
};

namespace detail {

struct EuclideanMetric {
  explicit EuclideanMetric(std::size_t dimension)
      : dimension(dimension), inverse_mass(dimension * dimension, 0.0),
        cholesky(dimension * dimension, 0.0) {
    for (std::size_t i = 0; i < dimension; ++i)
      inverse_mass[i * dimension + i] = cholesky[i * dimension + i] = 1.0;
  }

  std::vector<double> velocity(const std::vector<double> &momentum) const {
    std::vector<double> out(dimension, 0.0);
    if (!dense) {
      for (std::size_t i = 0; i < dimension; ++i)
        out[i] = inverse_mass[i * dimension + i] * momentum[i];
      return out;
    }
    for (std::size_t i = 0; i < dimension; ++i)
      for (std::size_t j = 0; j < dimension; ++j)
        out[i] += inverse_mass[i * dimension + j] * momentum[j];
    return out;
  }

  template <class Rng, class Normal>
  std::vector<double> sample_momentum(Rng &rng, Normal &normal) const {
    std::vector<double> momentum(dimension), z(dimension);
    for (double &value : z)
      value = normal(rng);
    if (!dense) {
      for (std::size_t i = 0; i < dimension; ++i)
        momentum[i] = z[i] / cholesky[i * dimension + i];
      return momentum;
    }
    // If inverse_mass = L L', then L' p = z gives
    // Cov(p) = inverse(inverse_mass).
    for (std::size_t offset = 0; offset < dimension; ++offset) {
      const std::size_t i = dimension - 1 - offset;
      double value = z[i];
      for (std::size_t j = i + 1; j < dimension; ++j)
        value -= cholesky[j * dimension + i] * momentum[j];
      momentum[i] = value / cholesky[i * dimension + i];
    }
    return momentum;
  }

  bool set_inverse_mass(std::vector<double> candidate, bool use_dense) {
    std::vector<double> factor(candidate.size(), 0.0);
    for (std::size_t i = 0; i < dimension; ++i) {
      for (std::size_t j = 0; j <= i; ++j) {
        double value = candidate[i * dimension + j];
        for (std::size_t k = 0; k < j; ++k)
          value -= factor[i * dimension + k] * factor[j * dimension + k];
        if (i == j) {
          if (!(value > 0.0) || !std::isfinite(value))
            return false;
          factor[i * dimension + j] = std::sqrt(value);
        } else {
          factor[i * dimension + j] = value / factor[j * dimension + j];
        }
      }
    }
    inverse_mass = std::move(candidate);
    cholesky = std::move(factor);
    dense = use_dense;
    return true;
  }

  std::size_t dimension;
  std::vector<double> inverse_mass;
  std::vector<double> cholesky;
  bool dense = false;
};

inline double dot(const std::vector<double> &a, const std::vector<double> &b) {
  double value = 0.0;
  for (std::size_t i = 0; i < a.size(); ++i)
    value += a[i] * b[i];
  return value;
}

inline double kinetic(const std::vector<double> &momentum,
                      const EuclideanMetric &metric) {
  return 0.5 * dot(momentum, metric.velocity(momentum));
}

template <class LogDensity>
bool leapfrog(LogDensity &target, std::vector<double> &q,
              std::vector<double> &momentum, LogDensityEvaluation &state,
              const EuclideanMetric &metric, double step) {
  for (std::size_t i = 0; i < q.size(); ++i)
    momentum[i] += 0.5 * step * state.gradient[i];
  const auto velocity = metric.velocity(momentum);
  for (std::size_t i = 0; i < q.size(); ++i)
    q[i] += step * velocity[i];
  state = target(q);
  if (!state.finite)
    return false;
  for (std::size_t i = 0; i < q.size(); ++i)
    momentum[i] += 0.5 * step * state.gradient[i];
  return true;
}

inline bool no_u_turn(const std::vector<double> &q_minus,
                      const std::vector<double> &q_plus,
                      const std::vector<double> &p_minus,
                      const std::vector<double> &p_plus,
                      const EuclideanMetric &metric) {
  std::vector<double> delta(q_minus.size());
  const auto velocity_minus = metric.velocity(p_minus);
  const auto velocity_plus = metric.velocity(p_plus);
  for (std::size_t i = 0; i < delta.size(); ++i) {
    delta[i] = q_plus[i] - q_minus[i];
  }
  return dot(delta, velocity_minus) >= 0.0 && dot(delta, velocity_plus) >= 0.0;
}

struct Tree {
  std::vector<double> q_minus, q_plus, p_minus, p_plus, proposal;
  LogDensityEvaluation state_minus, state_plus, proposal_state;
  int valid = 0;
  bool keep_going = true;
  bool divergent = false;
  int leapfrog_steps = 0;
  double acceptance_sum = 0.0;
  int acceptance_count = 0;
};

template <class LogDensity, class Rng>
Tree build_tree(LogDensity &target, const std::vector<double> &q,
                const std::vector<double> &momentum,
                const LogDensityEvaluation &state, double log_slice,
                int direction, int depth, double step_size,
                const EuclideanMetric &metric, double initial_joint,
                double divergence_threshold, Rng &rng) {
  if (depth == 0) {
    Tree tree;
    tree.q_minus = tree.q_plus = tree.proposal = q;
    tree.p_minus = tree.p_plus = momentum;
    tree.state_minus = tree.state_plus = tree.proposal_state = state;
    const bool finite =
        leapfrog(target, tree.q_minus, tree.p_minus, tree.state_minus, metric,
                 direction * step_size);
    tree.q_plus = tree.proposal = tree.q_minus;
    tree.p_plus = tree.p_minus;
    tree.state_plus = tree.proposal_state = tree.state_minus;
    tree.leapfrog_steps = 1;
    const double joint =
        finite ? tree.state_minus.log_density - kinetic(tree.p_minus, metric)
               : -std::numeric_limits<double>::infinity();
    const double energy_error = initial_joint - joint;
    tree.valid = finite && log_slice <= joint ? 1 : 0;
    tree.divergent = !finite || !std::isfinite(energy_error) ||
                     energy_error > divergence_threshold;
    tree.keep_going =
        !tree.divergent && log_slice < joint + divergence_threshold;
    tree.acceptance_sum =
        finite ? std::min(1.0, std::exp(joint - initial_joint)) : 0.0;
    tree.acceptance_count = 1;
    return tree;
  }

  Tree left =
      build_tree(target, q, momentum, state, log_slice, direction, depth - 1,
                 step_size, metric, initial_joint, divergence_threshold, rng);
  if (!left.keep_going)
    return left;

  Tree right =
      direction < 0
          ? build_tree(target, left.q_minus, left.p_minus, left.state_minus,
                       log_slice, direction, depth - 1, step_size, metric,
                       initial_joint, divergence_threshold, rng)
          : build_tree(target, left.q_plus, left.p_plus, left.state_plus,
                       log_slice, direction, depth - 1, step_size, metric,
                       initial_joint, divergence_threshold, rng);
  Tree combined = left;
  if (direction < 0) {
    combined.q_minus = right.q_minus;
    combined.p_minus = right.p_minus;
    combined.state_minus = right.state_minus;
  } else {
    combined.q_plus = right.q_plus;
    combined.p_plus = right.p_plus;
    combined.state_plus = right.state_plus;
  }
  const int total_valid = left.valid + right.valid;
  std::uniform_real_distribution<double> uniform(0.0, 1.0);
  if (right.valid > 0 && total_valid > 0 &&
      uniform(rng) < static_cast<double>(right.valid) / total_valid) {
    combined.proposal = right.proposal;
    combined.proposal_state = right.proposal_state;
  }
  combined.valid = total_valid;
  combined.divergent = left.divergent || right.divergent;
  combined.leapfrog_steps += right.leapfrog_steps;
  combined.acceptance_sum += right.acceptance_sum;
  combined.acceptance_count += right.acceptance_count;
  combined.keep_going = left.keep_going && right.keep_going &&
                        no_u_turn(combined.q_minus, combined.q_plus,
                                  combined.p_minus, combined.p_plus, metric);
  return combined;
}

template <class LogDensity>
double reasonable_step_size(LogDensity &target, const std::vector<double> &q,
                            const LogDensityEvaluation &state,
                            const EuclideanMetric &metric) {
  std::vector<double> momentum(q.size(), 0.0);
  // A deterministic unit vector in whitened momentum coordinates.
  for (std::size_t offset = 0; offset < q.size(); ++offset) {
    const std::size_t i = q.size() - 1 - offset;
    double value = 1.0;
    for (std::size_t j = i + 1; j < q.size(); ++j)
      value -= metric.cholesky[j * q.size() + i] * momentum[j];
    momentum[i] = value / metric.cholesky[i * q.size() + i];
  }
  const double initial_joint = state.log_density - kinetic(momentum, metric);
  double step = 1.0;
  auto acceptance = [&](double candidate) {
    std::vector<double> q_trial = q;
    std::vector<double> p_trial = momentum;
    LogDensityEvaluation trial = state;
    if (!leapfrog(target, q_trial, p_trial, trial, metric, candidate))
      return 0.0;
    return std::exp(std::min(0.0, trial.log_density - kinetic(p_trial, metric) -
                                      initial_joint));
  };
  const bool increase = acceptance(step) > 0.5;
  for (int i = 0; i < 20; ++i) {
    const double next = increase ? step * 2.0 : step * 0.5;
    const double a = acceptance(next);
    step = next;
    if ((increase && a < 0.5) || (!increase && a > 0.5))
      break;
  }
  return std::max(1.0e-6, std::min(10.0, step));
}

} // namespace detail

template <class LogDensity>
NutsResult sample_nuts(LogDensity &target, std::vector<double> initial,
                       const NutsOptions &options = {}) {
  if (initial.empty() || options.warmup < 0 || options.samples <= 0 ||
      options.max_tree_depth <= 0 || !(options.target_acceptance > 0.0) ||
      !(options.target_acceptance < 1.0) ||
      !(options.initial_step_size >= 0.0) ||
      !std::isfinite(options.initial_step_size) ||
      !(options.divergence_threshold > 0.0) ||
      !std::isfinite(options.divergence_threshold))
    throw std::invalid_argument("sample_nuts: invalid options");

  std::mt19937_64 rng(options.seed);
  std::uniform_real_distribution<double> uniform(0.0, 1.0);
  std::normal_distribution<double> normal(0.0, 1.0);
  detail::EuclideanMetric metric(initial.size());
  AdLogDensityEvaluator<LogDensity> evaluator(target, initial,
                                              options.reuse_ad_graph);
  LogDensityEvaluation current = evaluator(initial);
  if (!current.finite)
    throw std::runtime_error("sample_nuts: initial state is non-finite");

  double step_size =
      options.initial_step_size > 0.0
          ? options.initial_step_size
          : detail::reasonable_step_size(evaluator, initial, current, metric);
  double mu = std::log(10.0 * step_size);
  double log_step_bar = std::log(step_size);
  double h_bar = 0.0;
  std::vector<double> mean(initial.size(), 0.0);
  std::vector<double> m2(initial.size() * initial.size(), 0.0);
  int mass_count = 0;

  NutsResult result;
  result.draws.reserve(static_cast<std::size_t>(options.samples));
  result.log_density.reserve(static_cast<std::size_t>(options.samples));
  result.energy.reserve(static_cast<std::size_t>(options.samples));
  const int total_iterations = options.warmup + options.samples;
  double sampling_acceptance_total = 0.0;
  double warmup_acceptance_total = 0.0;
  int sampling_acceptance_iterations = 0;
  int warmup_acceptance_iterations = 0;
  int dual_averaging_iteration = 0;

  int initial_buffer = std::min(75, options.warmup / 3);
  int terminal_buffer = std::min(50, options.warmup / 3);
  int slow_end = options.warmup - terminal_buffer;
  int mass_window_size = std::min(25, std::max(0, slow_end - initial_buffer));
  int mass_window_end = initial_buffer + mass_window_size;

  for (int iteration = 1; iteration <= total_iterations; ++iteration) {
    std::vector<double> momentum = metric.sample_momentum(rng, normal);
    const double initial_joint =
        current.log_density - detail::kinetic(momentum, metric);
    const double log_slice = initial_joint + std::log(uniform(rng));

    detail::Tree tree;
    tree.q_minus = tree.q_plus = tree.proposal = initial;
    tree.p_minus = tree.p_plus = momentum;
    tree.state_minus = tree.state_plus = tree.proposal_state = current;
    tree.valid = 1;
    int depth = 0;
    while (tree.keep_going && depth < options.max_tree_depth) {
      const int direction = uniform(rng) < 0.5 ? -1 : 1;
      detail::Tree extension =
          direction < 0
              ? detail::build_tree(evaluator, tree.q_minus, tree.p_minus,
                                   tree.state_minus, log_slice, direction,
                                   depth, step_size, metric, initial_joint,
                                   options.divergence_threshold, rng)
              : detail::build_tree(evaluator, tree.q_plus, tree.p_plus,
                                   tree.state_plus, log_slice, direction, depth,
                                   step_size, metric, initial_joint,
                                   options.divergence_threshold, rng);
      if (direction < 0) {
        tree.q_minus = extension.q_minus;
        tree.p_minus = extension.p_minus;
        tree.state_minus = extension.state_minus;
      } else {
        tree.q_plus = extension.q_plus;
        tree.p_plus = extension.p_plus;
        tree.state_plus = extension.state_plus;
      }
      const int total_valid = tree.valid + extension.valid;
      if (extension.valid > 0 && total_valid > 0 &&
          uniform(rng) < static_cast<double>(extension.valid) / total_valid) {
        tree.proposal = extension.proposal;
        tree.proposal_state = extension.proposal_state;
      }
      tree.valid = total_valid;
      tree.divergent = tree.divergent || extension.divergent;
      tree.leapfrog_steps += extension.leapfrog_steps;
      tree.acceptance_sum += extension.acceptance_sum;
      tree.acceptance_count += extension.acceptance_count;
      tree.keep_going = extension.keep_going &&
                        detail::no_u_turn(tree.q_minus, tree.q_plus,
                                          tree.p_minus, tree.p_plus, metric);
      ++depth;
    }
    initial = tree.proposal;
    current = tree.proposal_state;
    const double acceptance = tree.acceptance_count > 0
                                  ? tree.acceptance_sum / tree.acceptance_count
                                  : 0.0;

    if (iteration <= options.warmup) {
      result.diagnostics.warmup_leapfrog_steps += tree.leapfrog_steps;
      result.diagnostics.warmup_divergences += tree.divergent ? 1 : 0;
      result.diagnostics.warmup_max_depth_hits +=
          depth == options.max_tree_depth ? 1 : 0;
      warmup_acceptance_total += acceptance;
      ++warmup_acceptance_iterations;
      ++dual_averaging_iteration;
      const double eta = 1.0 / (dual_averaging_iteration + 10.0);
      h_bar =
          (1.0 - eta) * h_bar + eta * (options.target_acceptance - acceptance);
      const double log_step =
          mu - std::sqrt(static_cast<double>(dual_averaging_iteration)) / 0.05 *
                   h_bar;
      const double weight =
          std::pow(static_cast<double>(dual_averaging_iteration), -0.75);
      log_step_bar = weight * log_step + (1.0 - weight) * log_step_bar;
      step_size = std::exp(std::max(-20.0, std::min(5.0, log_step)));

      const bool adapt_mass =
          options.adapt_dense_mass || options.adapt_diagonal_mass;
      const bool in_mass_window =
          adapt_mass && iteration > initial_buffer && iteration <= slow_end;
      if (in_mass_window) {
        ++mass_count;
        std::vector<double> delta(initial.size());
        for (std::size_t i = 0; i < initial.size(); ++i) {
          delta[i] = initial[i] - mean[i];
          mean[i] += delta[i] / mass_count;
        }
        for (std::size_t i = 0; i < initial.size(); ++i)
          for (std::size_t j = 0; j < initial.size(); ++j)
            m2[i * initial.size() + j] += delta[i] * (initial[j] - mean[j]);
        if (iteration == mass_window_end && mass_count > 1) {
          std::vector<double> candidate(initial.size() * initial.size(), 0.0);
          const double sample_weight =
              static_cast<double>(mass_count) / (mass_count + 5.0);
          const double ridge = 1.0e-3 * 5.0 / (mass_count + 5.0);
          for (std::size_t i = 0; i < initial.size(); ++i)
            for (std::size_t j = 0; j < initial.size(); ++j) {
              if (options.adapt_dense_mass || i == j)
                candidate[i * initial.size() + j] =
                    sample_weight * 0.5 *
                    (m2[i * initial.size() + j] + m2[j * initial.size() + i]) /
                    (mass_count - 1);
              if (i == j)
                candidate[i * initial.size() + j] += ridge;
            }
          if (metric.set_inverse_mass(std::move(candidate),
                                      options.adapt_dense_mass)) {
            ++result.diagnostics.mass_matrix_updates;
            step_size = detail::reasonable_step_size(evaluator, initial,
                                                     current, metric);
            mu = std::log(10.0 * step_size);
            log_step_bar = std::log(step_size);
            h_bar = 0.0;
            dual_averaging_iteration = 0;
          } else {
            // A noisy or non-finite covariance estimate must not invalidate an
            // otherwise usable chain. Retain the last valid metric and report
            // the rejected update through diagnostics.
            ++result.diagnostics.mass_matrix_update_failures;
          }
          std::fill(mean.begin(), mean.end(), 0.0);
          std::fill(m2.begin(), m2.end(), 0.0);
          mass_count = 0;
          const int remaining = slow_end - mass_window_end;
          if (remaining > 0) {
            mass_window_size = std::min(2 * mass_window_size, remaining);
            mass_window_end += mass_window_size;
          }
        }
      }
      if (iteration == options.warmup)
        step_size = std::exp(std::max(-20.0, std::min(5.0, log_step_bar)));
    } else {
      result.diagnostics.leapfrog_steps += tree.leapfrog_steps;
      result.diagnostics.divergences += tree.divergent ? 1 : 0;
      result.diagnostics.max_depth_hits +=
          depth == options.max_tree_depth ? 1 : 0;
      sampling_acceptance_total += acceptance;
      ++sampling_acceptance_iterations;
      result.draws.push_back(initial);
      result.log_density.push_back(current.log_density);
      result.energy.push_back(-initial_joint);
    }
  }
  result.diagnostics.mean_acceptance =
      sampling_acceptance_total / std::max(1, sampling_acceptance_iterations);
  result.diagnostics.warmup_mean_acceptance =
      warmup_acceptance_total / std::max(1, warmup_acceptance_iterations);
  if (result.energy.size() > 1) {
    double mean_energy = 0.0;
    for (double value : result.energy)
      mean_energy += value;
    mean_energy /= result.energy.size();
    double variance_sum = 0.0;
    double difference_sum = 0.0;
    for (std::size_t i = 0; i < result.energy.size(); ++i) {
      const double centered = result.energy[i] - mean_energy;
      variance_sum += centered * centered;
      if (i > 0) {
        const double difference = result.energy[i] - result.energy[i - 1];
        difference_sum += difference * difference;
      }
    }
    if (variance_sum > 0.0)
      result.diagnostics.energy_bfmi =
          difference_sum / static_cast<double>(result.energy.size() - 1) /
          (variance_sum / static_cast<double>(result.energy.size()));
  }
  result.diagnostics.step_size = step_size;
  result.diagnostics.inverse_mass.resize(initial.size());
  for (std::size_t i = 0; i < initial.size(); ++i)
    result.diagnostics.inverse_mass[i] =
        metric.inverse_mass[i * initial.size() + i];
  if (options.adapt_dense_mass) {
    result.diagnostics.dense_inverse_mass.assign(
        initial.size(), std::vector<double>(initial.size()));
    for (std::size_t i = 0; i < initial.size(); ++i)
      for (std::size_t j = 0; j < initial.size(); ++j)
        result.diagnostics.dense_inverse_mass[i][j] =
            metric.inverse_mass[i * initial.size() + j];
  }
  return result;
}

} // namespace sampling
} // namespace quadra
