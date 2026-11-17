#ifndef QUADRA_PUBLIC_STATS_LAPLACE_HPP
#define QUADRA_PUBLIC_STATS_LAPLACE_HPP

#include "../../../core/autodiff/model_gradient.hpp"
#include "../../../core/laplace/exact_laplace_gradient_engine.hpp"
#include "../../../core/laplace/laplace_implicit_derivatives.hpp"
#include "../../../core/laplace/laplace_objective.hpp"
#include "../../../core/laplace/random_effect_hessian.hpp"
#include "../../../core/laplace/reusable_random_effect_tape.hpp"
#include "../../../core/laplace/reusable_total_hdot_tape.hpp"
#include "../../../core/laplace/sparse_huu_factorization.hpp"
#include "../../../core/laplace/sparse_laplace_evaluation_result.hpp"
#include "../../../core/laplace/third_order_dense_hdot.hpp"
#include "../../../core/model/parameter_partition.hpp"

#include <Eigen/Dense>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <deque>
#include <exception>
#include <iomanip>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>
#include <vector>

namespace quadra {
namespace stats {

struct ExactLaplaceTimings {
  double objective_ms = 0.0;
  double factorization_ms = 0.0;
  double mode_sensitivity_ms = 0.0;
  double hdot_ms = 0.0;
  double hdot_validation_ms = 0.0;
  double hdot_direction_setup_ms = 0.0;
  double hdot_reverse_ms = 0.0;
  double hdot_contraction_ms = 0.0;
  double trace_ms = 0.0;
  double total_ms = 0.0;
};

struct ExactLaplaceResult {
  LaplaceObjectiveResult objective;
  std::vector<double> gradient;
  std::vector<int> active_directions;
  ExactLaplaceTimings timings;
  bool success = false;
};

struct LaplaceOptimizerOptions {
  int max_iterations = 100;
  int memory = 7;
  int print_every = 0;
  int gradient_table_every = 10;
  int max_evaluations = 0;
  std::vector<std::string> gradient_names;
  double gradient_tolerance = 1.0e-6;
  double step_tolerance = 1.0e-10;
  double initial_step_scale = 1.0;
  double maximum_direction_norm = 1.0;
  double minimum_step_scale = 1.0e-10;
  double sufficient_decrease = 1.0e-4;
};

struct LaplaceOptimizerIteration {
  int iteration = 0;
  double objective = 0.0;
  double gradient_norm = 0.0;
  double step_scale = 0.0;
  double step_norm = 0.0;
};

struct LaplaceOptimizerResult {
  std::vector<double> fixed;
  std::vector<double> random_mode;
  std::vector<double> full;
  std::vector<double> gradient;
  double objective = std::nan("");
  double gradient_norm = std::nan("");
  double step_norm = 0.0;
  int iterations = 0;
  bool converged = false;
  std::string message;
  ExactLaplaceResult evaluation;
  std::vector<LaplaceOptimizerIteration> history;
  int objective_evaluations = 0;
  int approximate_evaluations = 0;
  int exact_evaluations = 0;
  int exact_switch_iteration = -1;
};

// Stateful public Laplace surface. It automatically warm-starts the latent
// mode and reports the backend selected from the discovered Hessian structure.
template <class Model> class LaplaceEvaluator {
public:
  LaplaceEvaluator(Model &model, std::vector<double> random_initial,
                   const ParameterPartition &partition,
                   const LaplaceObjectiveOptions &options = {})
      : model_(&model), partition_(partition), options_(options),
        random_(std::move(random_initial)) {}

  LaplaceEvaluator(Model &model, std::vector<double> random_initial,
                   const ParameterSet &parameters,
                   const LaplaceObjectiveOptions &options = {})
      : LaplaceEvaluator(model, std::move(random_initial),
                         partition_parameters(parameters), options) {}

  LaplaceObjectiveResult evaluate(const std::vector<double> &fixed) {
    using Clock = std::chrono::steady_clock;
    const auto total_start = Clock::now();
    const auto tape_start = total_start;
    if (!tape_) {
      tape_.reset(new laplace::ReusableRandomEffectTape<Model>(
          *model_, fixed, random_, partition_,
          options_.compute_mixed_derivatives_m));
    }
    const auto tape_end = Clock::now();
    const std::size_t rebuilds_before = tape_->rebuild_count();
    auto evaluate_hessian = [&](const std::vector<double> &random) {
      return tape_->evaluate(fixed, random,
                             options_.newton_m.hessian_drop_tol_m);
    };
    const auto mode_start = Clock::now();
    RandomEffectNewtonResult newton =
        optimize_random_effects_newton_with_evaluator(
            fixed, random_, partition_, options_.newton_m, evaluate_hessian);
    const auto mode_end = Clock::now();

    LaplaceObjectiveResult result;
    result.fixed_m = fixed;
    result.u_hat_m = newton.u_hat_m;
    result.full_m = newton.full_m;
    result.joint_objective_m = newton.objective_value_m;
    result.gradient_norm_random_m = newton.gradient_norm_m;
    result.newton_iterations_m = newton.iterations_m;
    result.converged_m = newton.converged_m;
    result.message_m = newton.message_m;
    result.hessian_random_m = newton.hessian_random_m;
    result.gradient_random_m = newton.gradient_random_m;
    result.gradient_fixed_joint_m = newton.gradient_fixed_m;
    result.mixed_hessian_m = newton.mixed_hessian_m;
    result.reports_m = newton.reports_m;
    result.n_random_m = static_cast<int>(partition_.random_indices_m.size());
    result.tape_rebuilt_m = tape_->rebuild_count() != rebuilds_before;
    result.tape_setup_ms_m =
        std::chrono::duration<double, std::milli>(tape_end - tape_start)
            .count();
    result.mode_solve_ms_m =
        std::chrono::duration<double, std::milli>(mode_end - mode_start)
            .count();
    if (result.tape_rebuilt_m) {
      structured_runtime_.reset();
    }

    const auto logdet_start = Clock::now();
    try {
      Eigen::SparseMatrix<double> hessian =
          laplace_objective_add_diagonal_jitter(result.hessian_random_m,
                                                options_.logdet_jitter_m);
      const auto structured = structured_runtime_.evaluate(hessian);
      result.log_det_hessian_m = structured.logdet;
      result.logdet_ok_m = std::isfinite(structured.logdet);
      result.backend_m = structured.recommendation;
      result.structure_detected_m = structured.detected_structure;
    } catch (const std::exception &error) {
      result.logdet_ok_m = false;
      result.message_m +=
          std::string(" Structured logdet failed: ") + error.what();
    }
    result.logdet_ms_m =
        std::chrono::duration<double, std::milli>(Clock::now() - logdet_start)
            .count();

    if (result.logdet_ok_m) {
      result.laplace_objective_m =
          result.joint_objective_m + 0.5 * result.log_det_hessian_m;
      if (options_.include_constant_m) {
        result.laplace_objective_m -=
            0.5 * result.n_random_m * std::log(2.0 * M_PI);
      }
    } else {
      result.laplace_objective_m = std::nan("");
    }
    if (result.converged_m) {
      random_ = result.u_hat_m;
    }
    result.total_ms_m =
        std::chrono::duration<double, std::milli>(Clock::now() - total_start)
            .count();
    return result;
  }

  void reset_random(std::vector<double> random) { random_ = std::move(random); }
  const std::vector<double> &random_mode() const { return random_; }
  std::size_t tape_rebuild_count() const {
    return tape_ ? tape_->rebuild_count() : 0;
  }

private:
  Model *model_;
  ParameterPartition partition_;
  LaplaceObjectiveOptions options_;
  std::vector<double> random_;
  std::unique_ptr<laplace::ReusableRandomEffectTape<Model>> tape_;
  laplace::PersistentStructuredLaplaceRuntime structured_runtime_;
};

template <class Model> class ExactLaplaceEvaluator {
private:
  struct CombinedObjective {
    Model *model;
    ParameterPartition partition;

    template <class T> T operator()(const std::vector<T> &fixed_random) const {
      const std::size_t fixed_size = partition.fixed_indices_m.size();
      std::vector<T> fixed(fixed_random.begin(),
                           fixed_random.begin() + fixed_size);
      std::vector<T> random(fixed_random.begin() + fixed_size,
                            fixed_random.end());
      std::vector<T> full = merge_parameters(fixed, random, partition);
      ModelReportContext context;
      model->initialize(context);
      return model->template evaluate<T>(full, context);
    }
  };

public:
  ExactLaplaceEvaluator(
      Model &model, const std::vector<double> &discovery_fixed,
      const std::vector<double> &discovery_random,
      const ParameterPartition &partition,
      const LaplaceObjectiveOptions &objective_options = {},
      const laplace::ExactLaplaceGradientEngineOptions &engine_options = {})
      : model_(&model), partition_(partition),
        objective_options_(objective_options), last_random_(discovery_random),
        stream_dense_hdot_trace_(engine_options.stream_dense_hdot_trace),
        dense_hdot_bandwidth_(engine_options.dense_hdot_bandwidth) {
    // Exact marginal gradients require the joint fixed gradient and H_u,theta
    // even when a caller disables them for ordinary value-only evaluations.
    objective_options_.compute_mixed_derivatives_m = true;
    objective_evaluator_.reset(new LaplaceEvaluator<Model>(
        model, discovery_random, partition_, objective_options_));
    LaplaceObjectiveResult discovery =
        objective_evaluator_->evaluate(discovery_fixed);
    if (!discovery.converged_m || !discovery.logdet_ok_m) {
      throw std::runtime_error(
          "ExactLaplaceEvaluator: discovery Laplace evaluation failed: " +
          discovery.message_m);
    }

    laplace::RandomHessianPattern pattern;
    for (int outer = 0; outer < discovery.hessian_random_m.outerSize();
         ++outer) {
      for (Eigen::SparseMatrix<double>::InnerIterator entry(
               discovery.hessian_random_m, outer);
           entry; ++entry) {
        if (entry.row() >= entry.col()) {
          pattern.emplace_back(static_cast<int>(entry.row()),
                               static_cast<int>(entry.col()));
        }
      }
    }

    pattern_ = pattern;
    last_random_ = discovery.u_hat_m;
    const int fixed_size = static_cast<int>(partition_.fixed_indices_m.size());
    std::vector<int> all_directions;
    all_directions.reserve(static_cast<std::size_t>(fixed_size));
    for (int j = 0; j < fixed_size; ++j) {
      all_directions.push_back(j);
    }
    laplace::SparseHuuFactorization discovery_factor(
        discovery.hessian_random_m);
    bool direct_tridiagonal =
        discovery_factor.has_tridiagonal_selected_inverse();
    if (direct_tridiagonal) {
      for (const auto &entry : pattern_) {
        if (std::abs(entry.first - entry.second) > 1) {
          direct_tridiagonal = false;
          break;
        }
      }
    }
    const double discovery_density =
        static_cast<double>(discovery.hessian_random_m.nonZeros()) /
        static_cast<double>(
            std::max<Eigen::Index>(1, discovery.hessian_random_m.rows() *
                                          discovery.hessian_random_m.cols()));
    dense_third_order_ = !direct_tridiagonal && discovery_density > 0.5;
    if (engine_options.hdot_workers < 0) {
      throw std::invalid_argument(
          "ExactLaplaceEvaluator: hdot_workers cannot be negative");
    }
    if (dense_hdot_bandwidth_ < -1) {
      throw std::invalid_argument(
          "ExactLaplaceEvaluator: dense_hdot_bandwidth cannot be below -1");
    }
    if (dense_third_order_) {
      // The dense backend uses forward third-order polarization and never
      // consumes a reverse-Hdot tape. Keep every fixed direction active and
      // avoid recording an otherwise dead graph.
      active_directions_ = all_directions;
      return;
    }
    std::unique_ptr<laplace::ReusableTotalHdotTape<CombinedObjective>>
        prototype(new laplace::ReusableTotalHdotTape<CombinedObjective>(
            CombinedObjective{model_, partition_}, fixed_size,
            static_cast<int>(partition_.random_indices_m.size()), pattern_,
            all_directions, to_eigen(discovery_fixed),
            to_eigen(discovery.u_hat_m), engine_options.hdot_drop_tol));
    if (engine_options.discover_active_directions && fixed_size > 1) {
      laplace::SparseHuuFactorization discovery_factor(
          discovery.hessian_random_m);
      std::vector<Eigen::VectorXd> discovery_directions(
          static_cast<std::size_t>(fixed_size));
      for (int j = 0; j < fixed_size; ++j) {
        discovery_directions[static_cast<std::size_t>(j)] =
            (-discovery_factor.solve(
                 Eigen::VectorXd(discovery.mixed_hessian_m.col(j))))
                .eval();
      }
      auto direction =
          [&discovery_directions](int j) -> const Eigen::VectorXd & {
        return discovery_directions[static_cast<std::size_t>(j)];
      };
      const auto discovery_hdots = prototype->compute(
          to_eigen(discovery_fixed), to_eigen(discovery.u_hat_m), direction);
      for (int j = 0; j < fixed_size; ++j) {
        const auto &hdot = discovery_hdots[static_cast<std::size_t>(j)];
        if (hdot.nonZeros() > 0 &&
            hdot.norm() > engine_options.active_direction_discovery_tol) {
          active_directions_.push_back(j);
        }
      }
    } else {
      active_directions_ = all_directions;
    }
    std::size_t worker_count = 1;
    if (active_directions_.size() > 1) {
      const unsigned int hardware = std::thread::hardware_concurrency();
      const unsigned int automatic_workers =
          hardware == 0 ? 4u : std::min(4u, hardware);
      const std::size_t requested =
          engine_options.hdot_workers == 0
              ? automatic_workers
              : static_cast<std::size_t>(engine_options.hdot_workers);
      worker_count = std::min(active_directions_.size(), requested);
    }
    std::vector<std::vector<int>> worker_directions(worker_count);
    for (std::size_t k = 0; k < active_directions_.size(); ++k) {
      worker_directions[k % worker_count].push_back(active_directions_[k]);
    }
    hdot_tapes_.reserve(worker_count);
    hdot_worker_directions_ = worker_directions;
    prototype->set_active_directions(worker_directions[0]);
    hdot_tapes_.push_back(std::move(prototype));
    for (std::size_t worker = 1; worker < worker_count; ++worker) {
      hdot_tapes_.emplace_back(
          new laplace::ReusableTotalHdotTape<CombinedObjective>(
              *hdot_tapes_.front(), worker_directions[worker]));
    }
  }

  ExactLaplaceEvaluator(
      Model &model, const std::vector<double> &discovery_fixed,
      const std::vector<double> &discovery_random,
      const ParameterSet &parameters,
      const LaplaceObjectiveOptions &objective_options = {},
      const laplace::ExactLaplaceGradientEngineOptions &engine_options = {})
      : ExactLaplaceEvaluator(model, discovery_fixed, discovery_random,
                              partition_parameters(parameters),
                              objective_options, engine_options) {}

private:
  ExactLaplaceResult
  evaluate_impl(const std::vector<double> &fixed,
                const std::vector<double> &random_initial,
                const LaplaceObjectiveResult *precomputed_objective) {
    using Clock = std::chrono::steady_clock;
    const auto total_start = Clock::now();
    ExactLaplaceResult result;
    objective_evaluator_->reset_random(random_initial);
    const auto objective_start = Clock::now();
    result.objective = precomputed_objective != nullptr
                           ? *precomputed_objective
                           : objective_evaluator_->evaluate(fixed);
    const auto objective_end = Clock::now();
    result.timings.objective_ms = std::chrono::duration<double, std::milli>(
                                      objective_end - objective_start)
                                      .count();
    if (!result.objective.converged_m || !result.objective.logdet_ok_m) {
      result.gradient.assign(fixed.size(), std::nan(""));
      result.timings.total_ms =
          std::chrono::duration<double, std::milli>(Clock::now() - total_start)
              .count();
      return result;
    }

    const std::vector<double> &joint_fixed =
        result.objective.gradient_fixed_joint_m;
    const Eigen::MatrixXd &mixed = result.objective.mixed_hessian_m;
    auto cross_derivative = [&mixed](int fixed_index) {
      return mixed.col(fixed_index).eval();
    };

    const auto factorization_start = Clock::now();
    laplace::SparseHuuFactorization factor(result.objective.hessian_random_m);
    result.timings.factorization_ms = std::chrono::duration<double, std::milli>(
                                          Clock::now() - factorization_start)
                                          .count();
    const auto sensitivity_start = Clock::now();
    std::vector<Eigen::VectorXd> mode_directions(fixed.size());
    for (int fixed_index : active_directions_) {
      mode_directions[static_cast<std::size_t>(fixed_index)] =
          (-factor.solve(Eigen::VectorXd(cross_derivative(fixed_index))))
              .eval();
    }
    result.timings.mode_sensitivity_ms =
        std::chrono::duration<double, std::milli>(Clock::now() -
                                                  sensitivity_start)
            .count();
    auto u_direction =
        [&mode_directions](int fixed_index) -> const Eigen::VectorXd & {
      return mode_directions[static_cast<std::size_t>(fixed_index)];
    };
    const auto hdot_start = Clock::now();
    const Eigen::VectorXd fixed_eigen = to_eigen(fixed);
    const Eigen::VectorXd random_eigen = to_eigen(result.objective.u_hat_m);
    bool direct_tridiagonal_trace = factor.has_tridiagonal_selected_inverse();
    if (direct_tridiagonal_trace) {
      for (const auto &entry : pattern_) {
        if (std::abs(entry.first - entry.second) > 1) {
          direct_tridiagonal_trace = false;
          break;
        }
      }
    }
    const bool dense_third_order = dense_third_order_;
    std::vector<std::vector<double>> worker_trace_terms(hdot_tapes_.size());
    std::vector<std::vector<Eigen::SparseMatrix<double>>> worker_hdots(
        hdot_tapes_.size());
    std::vector<std::exception_ptr> worker_errors(hdot_tapes_.size());
    auto run_worker = [&](std::size_t worker) {
      try {
        if (direct_tridiagonal_trace) {
          worker_trace_terms[worker] = hdot_tapes_[worker]->compute_trace_terms(
              fixed_eigen, random_eigen, u_direction,
              [&factor](int row, int col) {
                return factor.tridiagonal_selected_inverse(row, col);
              });
        } else {
          worker_hdots[worker] = hdot_tapes_[worker]->compute(
              fixed_eigen, random_eigen, u_direction);
        }
      } catch (...) {
        worker_errors[worker] = std::current_exception();
      }
    };
    if (!dense_third_order && hdot_tapes_.size() == 1) {
      run_worker(0);
    } else if (!dense_third_order) {
      std::vector<std::thread> workers;
      workers.reserve(hdot_tapes_.size());
      for (std::size_t worker = 0; worker < hdot_tapes_.size(); ++worker) {
        workers.emplace_back(run_worker, worker);
      }
      for (std::thread &worker : workers) {
        worker.join();
      }
    }
    std::vector<Eigen::SparseMatrix<double>> hdots(fixed.size());
    std::vector<double> dense_trace_terms(fixed.size(), 0.0);
    double dense_trace_ms = 0.0;
    for (std::size_t worker = 0;
         !dense_third_order && worker < hdot_tapes_.size(); ++worker) {
      if (worker_errors[worker]) {
        std::rethrow_exception(worker_errors[worker]);
      }
      for (int direction : hdot_worker_directions_[worker]) {
        if (direct_tridiagonal_trace) {
          continue;
        }
        hdots[static_cast<std::size_t>(direction)] = std::move(
            worker_hdots[worker][static_cast<std::size_t>(direction)]);
      }
    }
    if (dense_third_order) {
      std::vector<double> combined = fixed;
      combined.insert(combined.end(), result.objective.u_hat_m.begin(),
                      result.objective.u_hat_m.end());
      std::vector<int> random_indices(result.objective.u_hat_m.size());
      for (std::size_t i = 0; i < random_indices.size(); ++i)
        random_indices[i] = static_cast<int>(fixed.size() + i);
      CombinedObjective objective{model_, partition_};
      const Eigen::MatrixXd identity = Eigen::MatrixXd::Identity(
          static_cast<Eigen::Index>(random_indices.size()),
          static_cast<Eigen::Index>(random_indices.size()));
      const Eigen::MatrixXd inverse = factor.solve(identity);
      Eigen::LLT<Eigen::MatrixXd> inverse_llt(inverse);
      if (inverse_llt.info() != Eigen::Success) {
        throw std::runtime_error(
            "ExactLaplaceEvaluator: inverse Cholesky factorization failed");
      }
      const Eigen::MatrixXd inverse_factor = inverse_llt.matrixL();
      const auto dense_trace_start = Clock::now();
      for (int j : active_directions_) {
        std::vector<double> total_direction(combined.size(), 0.0);
        total_direction[static_cast<std::size_t>(j)] = 1.0;
        const Eigen::VectorXd &direction =
            mode_directions[static_cast<std::size_t>(j)];
        for (int i = 0; i < direction.size(); ++i) {
          total_direction[fixed.size() + static_cast<std::size_t>(i)] =
              direction[i];
        }
        if (stream_dense_hdot_trace_) {
          // Stream inverse columns and polarized Hdot entries directly into
          // the trace. Neither matrix is materialized densely.
          if (dense_hdot_bandwidth_ < 0) {
            dense_trace_terms[static_cast<std::size_t>(j)] =
                laplace::dense_hdot_trace_third_order_factorized(
                    objective, combined, total_direction, random_indices,
                    inverse_factor);
          } else {
            dense_trace_terms[static_cast<std::size_t>(j)] =
                laplace::dense_hdot_trace_third_order_polarized(
                    objective, combined, total_direction, random_indices,
                    [&inverse](int column) {
                      return Eigen::VectorXd(inverse.col(column));
                    },
                    dense_hdot_bandwidth_);
          }
        } else {
          hdots[static_cast<std::size_t>(j)] =
              laplace::dense_hdot_third_order_polarized(
                  objective, combined, total_direction, random_indices)
                  .sparseView();
        }
      }
      dense_trace_ms = std::chrono::duration<double, std::milli>(
                           Clock::now() - dense_trace_start)
                           .count();
    }
    result.timings.hdot_ms =
        std::chrono::duration<double, std::milli>(Clock::now() - hdot_start)
            .count();
    if (!dense_third_order) {
      for (const auto &tape : hdot_tapes_) {
        const auto &timings = tape->last_timings();
        result.timings.hdot_validation_ms += timings.validation_ms;
        result.timings.hdot_direction_setup_ms += timings.direction_setup_ms;
        result.timings.hdot_reverse_ms += timings.reverse_ms;
        result.timings.hdot_contraction_ms += timings.contraction_ms;
      }
    }
    Eigen::VectorXd exact_gradient = to_eigen(joint_fixed);
    const auto trace_start = Clock::now();
    for (int j : active_directions_) {
      double trace_term = 0.0;
      if (direct_tridiagonal_trace) {
        for (std::size_t worker = 0; worker < hdot_tapes_.size(); ++worker) {
          if (!worker_trace_terms[worker].empty()) {
            trace_term +=
                worker_trace_terms[worker][static_cast<std::size_t>(j)];
          }
        }
      } else if (dense_third_order && stream_dense_hdot_trace_) {
        trace_term = dense_trace_terms[static_cast<std::size_t>(j)];
      } else {
        trace_term =
            factor.trace_inverse_times(hdots[static_cast<std::size_t>(j)]);
      }
      exact_gradient[j] += 0.5 * trace_term;
    }
    result.timings.trace_ms =
        std::chrono::duration<double, std::milli>(Clock::now() - trace_start)
            .count() +
        dense_trace_ms;
    result.gradient = from_eigen(exact_gradient);
    result.active_directions = active_directions_;
    result.success = true;
    last_random_ = result.objective.u_hat_m;
    result.timings.total_ms =
        std::chrono::duration<double, std::milli>(Clock::now() - total_start)
            .count();
    return result;
  }

public:
  ExactLaplaceResult evaluate(const std::vector<double> &fixed,
                              const std::vector<double> &random_initial) {
    return evaluate_impl(fixed, random_initial, nullptr);
  }

  ExactLaplaceResult evaluate(const std::vector<double> &fixed) {
    return evaluate(fixed, last_random_);
  }

  ExactLaplaceResult
  evaluate_from_objective(const LaplaceObjectiveResult &objective) {
    if (objective.fixed_m.size() != partition_.fixed_indices_m.size() ||
        objective.u_hat_m.size() != partition_.random_indices_m.size()) {
      throw std::invalid_argument(
          "ExactLaplaceEvaluator::evaluate_from_objective: dimensions "
          "changed");
    }
    return evaluate_impl(objective.fixed_m, objective.u_hat_m, &objective);
  }

  LaplaceObjectiveResult
  evaluate_objective(const std::vector<double> &fixed,
                     const std::vector<double> &random_initial) {
    objective_evaluator_->reset_random(random_initial);
    LaplaceObjectiveResult result = objective_evaluator_->evaluate(fixed);
    if (result.converged_m) {
      last_random_ = result.u_hat_m;
    }
    return result;
  }

  const std::vector<double> &random_mode() const { return last_random_; }
  std::size_t hdot_tape_rebuild_count() const {
    std::size_t count = 0;
    for (const auto &tape : hdot_tapes_) {
      count += tape->rebuild_count();
    }
    return count;
  }
  std::size_t hdot_worker_count() const { return hdot_tapes_.size(); }
  bool uses_dense_third_order() const { return dense_third_order_; }
  std::size_t hdot_shared_topology_owner_count() const {
    return hdot_tapes_.empty()
               ? 0
               : hdot_tapes_.front()->shared_topology_owner_count();
  }
  std::size_t hdot_shared_operation_owner_count() const {
    return hdot_tapes_.empty()
               ? 0
               : hdot_tapes_.front()->shared_operation_owner_count();
  }
  std::size_t hdot_operation_count() const {
    return hdot_tapes_.empty() ? 0 : hdot_tapes_.front()->operation_count();
  }
  std::size_t objective_tape_rebuild_count() const {
    return objective_evaluator_ ? objective_evaluator_->tape_rebuild_count()
                                : 0;
  }
  void set_dense_hdot_bandwidth(int bandwidth) {
    if (bandwidth < -1)
      throw std::invalid_argument("dense Hdot bandwidth cannot be below -1");
    dense_hdot_bandwidth_ = bandwidth;
  }
  int dense_hdot_bandwidth() const { return dense_hdot_bandwidth_; }

private:
  Model *model_;
  ParameterPartition partition_;
  LaplaceObjectiveOptions objective_options_;
  laplace::RandomHessianPattern pattern_;
  std::vector<int> active_directions_;
  std::unique_ptr<LaplaceEvaluator<Model>> objective_evaluator_;
  std::vector<
      std::unique_ptr<laplace::ReusableTotalHdotTape<CombinedObjective>>>
      hdot_tapes_;
  std::vector<std::vector<int>> hdot_worker_directions_;
  std::vector<double> last_random_;
  bool dense_third_order_ = false;
  bool stream_dense_hdot_trace_ = true;
  int dense_hdot_bandwidth_ = -1;

  static Eigen::VectorXd to_eigen(const std::vector<double> &values) {
    return Eigen::Map<const Eigen::VectorXd>(
        values.data(), static_cast<Eigen::Index>(values.size()));
  }

  static std::vector<double> from_eigen(const Eigen::VectorXd &values) {
    return std::vector<double>(values.data(), values.data() + values.size());
  }
};

namespace detail {

inline double dot(const std::vector<double> &left,
                  const std::vector<double> &right) {
  if (left.size() != right.size()) {
    throw std::invalid_argument("Laplace optimizer: vector length mismatch");
  }
  double result = 0.0;
  for (std::size_t i = 0; i < left.size(); ++i) {
    result += left[i] * right[i];
  }
  return result;
}

inline double norm(const std::vector<double> &values) {
  return std::sqrt(dot(values, values));
}

inline bool finite(const ExactLaplaceResult &evaluation) {
  if (!evaluation.success ||
      !std::isfinite(evaluation.objective.laplace_objective_m)) {
    return false;
  }
  for (double value : evaluation.gradient) {
    if (!std::isfinite(value)) {
      return false;
    }
  }
  return true;
}

inline bool finite(const LaplaceObjectiveResult &evaluation) {
  return evaluation.converged_m && evaluation.logdet_ok_m &&
         std::isfinite(evaluation.laplace_objective_m);
}

inline std::vector<double> subtract(const std::vector<double> &left,
                                    const std::vector<double> &right) {
  if (left.size() != right.size()) {
    throw std::invalid_argument("Laplace optimizer: vector length mismatch");
  }
  std::vector<double> result(left.size());
  for (std::size_t i = 0; i < left.size(); ++i) {
    result[i] = left[i] - right[i];
  }
  return result;
}

inline std::vector<double> add_scaled(const std::vector<double> &values,
                                      const std::vector<double> &direction,
                                      double scale) {
  if (values.size() != direction.size()) {
    throw std::invalid_argument("Laplace optimizer: vector length mismatch");
  }
  std::vector<double> result(values.size());
  for (std::size_t i = 0; i < values.size(); ++i) {
    result[i] = values[i] + scale * direction[i];
  }
  return result;
}

inline std::vector<double>
lbfgs_direction(const std::vector<double> &gradient,
                const std::deque<std::vector<double>> &steps,
                const std::deque<std::vector<double>> &gradient_changes) {
  if (steps.empty()) {
    std::vector<double> direction = gradient;
    for (double &value : direction) {
      value = -value;
    }
    return direction;
  }

  const std::size_t count = steps.size();
  std::vector<double> q = gradient;
  std::vector<double> alpha(count, 0.0);
  std::vector<double> rho(count, 0.0);
  for (std::size_t reverse = 0; reverse < count; ++reverse) {
    const std::size_t i = count - reverse - 1;
    const double curvature = dot(steps[i], gradient_changes[i]);
    if (!(curvature > 0.0) || !std::isfinite(curvature)) {
      continue;
    }
    rho[i] = 1.0 / curvature;
    alpha[i] = rho[i] * dot(steps[i], q);
    for (std::size_t k = 0; k < q.size(); ++k) {
      q[k] -= alpha[i] * gradient_changes[i][k];
    }
  }

  const double last_curvature = dot(steps.back(), gradient_changes.back());
  const double last_gradient_norm2 =
      dot(gradient_changes.back(), gradient_changes.back());
  const double scale = last_curvature > 0.0 && last_gradient_norm2 > 0.0
                           ? last_curvature / last_gradient_norm2
                           : 1.0;
  std::vector<double> direction = q;
  for (double &value : direction) {
    value *= scale;
  }
  for (std::size_t i = 0; i < count; ++i) {
    if (rho[i] == 0.0) {
      continue;
    }
    const double beta = rho[i] * dot(gradient_changes[i], direction);
    for (std::size_t k = 0; k < direction.size(); ++k) {
      direction[k] += steps[i][k] * (alpha[i] - beta);
    }
  }
  for (double &value : direction) {
    value = -value;
  }
  return direction;
}

inline void print_gradient_table(const std::string &optimizer, int iteration,
                                 double objective,
                                 const std::vector<double> &gradient,
                                 std::size_t random_effect_count,
                                 const LaplaceOptimizerOptions &options) {
  std::vector<std::size_t> order(gradient.size());
  for (std::size_t i = 0; i < order.size(); ++i)
    order[i] = i;
  std::sort(order.begin(), order.end(), [&](std::size_t lhs, std::size_t rhs) {
    return std::abs(gradient[lhs]) > std::abs(gradient[rhs]);
  });

  std::size_t within_tolerance = 0;
  for (double value : gradient) {
    if (std::isfinite(value) && std::abs(value) <= options.gradient_tolerance)
      ++within_tolerance;
  }
  const std::size_t max_index = order.empty() ? 0 : order.front();
  const double max_component =
      order.empty() ? 0.0 : std::abs(gradient[max_index]);
  const auto parameter_name = [&](std::size_t index) {
    return index < options.gradient_names.size()
               ? options.gradient_names[index]
               : "fixed_" + std::to_string(index);
  };

  std::cout << '\n'
            << '[' << optimizer << " gradient table]\n"
            << "  outer_iteration: " << iteration << '\n'
            << "  objective: " << objective << '\n'
            << "  fixed_effects: " << gradient.size() << '\n'
            << "  random_effects: " << random_effect_count << '\n'
            << "  tolerance: " << options.gradient_tolerance << '\n'
            << "  within_tolerance: " << within_tolerance << '/'
            << gradient.size() << '\n'
            << "  max_gc: " << max_component;
  if (!order.empty())
    std::cout << " (" << parameter_name(max_index) << ')';
  std::cout << "\n\n"
            << "  " << std::left << std::setw(38) << "parameter" << std::right
            << std::setw(16) << "gradient" << "  status\n"
            << "  " << std::string(38, '-') << "  " << std::string(14, '-')
            << "  -------------\n";
  for (std::size_t index : order) {
    const bool okay = std::isfinite(gradient[index]) &&
                      std::abs(gradient[index]) <= options.gradient_tolerance;
    std::cout << "  " << std::left << std::setw(38) << parameter_name(index)
              << std::right << std::scientific << std::setprecision(6)
              << (okay ? "\033[32m" : "\033[31m") << std::setw(16)
              << gradient[index] << "\033[0m" << "  "
              << (okay ? "within tol" : "above tol") << '\n';
  }
  std::cout << std::defaultfloat << std::flush;
}

} // namespace detail

// Optimize the exact Laplace marginal objective with a persistent evaluator.
// Objective and gradient are always computed together, and every line-search
// evaluation reuses the latent mode and both AD tapes owned by the evaluator.
template <class Model>
LaplaceOptimizerResult
optimize_laplace(ExactLaplaceEvaluator<Model> &evaluator,
                 const std::vector<double> &fixed_initial,
                 const LaplaceOptimizerOptions &options = {}) {
  if (fixed_initial.empty()) {
    throw std::invalid_argument(
        "optimize_laplace: fixed_initial cannot be empty");
  }
  if (options.max_iterations < 0 || options.memory < 0 ||
      options.print_every < 0 || options.gradient_table_every < 0 ||
      options.max_evaluations < 0 || !(options.gradient_tolerance >= 0.0) ||
      !(options.step_tolerance >= 0.0) || !(options.initial_step_scale > 0.0) ||
      !(options.maximum_direction_norm > 0.0) ||
      !(options.minimum_step_scale > 0.0) ||
      options.minimum_step_scale > options.initial_step_scale ||
      !(options.sufficient_decrease > 0.0 &&
        options.sufficient_decrease < 1.0)) {
    throw std::invalid_argument("optimize_laplace: invalid options");
  }

  LaplaceOptimizerResult result;
  std::vector<double> fixed = fixed_initial;
  ExactLaplaceResult current = evaluator.evaluate(fixed);
  ++result.exact_evaluations;
  if (!detail::finite(current)) {
    result.message = "Initial exact Laplace evaluation failed.";
    result.evaluation = std::move(current);
    return result;
  }

  std::deque<std::vector<double>> steps;
  std::deque<std::vector<double>> gradient_changes;
  int last_gradient_table_iteration = -1;
  for (int iteration = 0; iteration < options.max_iterations; ++iteration) {
    const double gradient_norm = detail::norm(current.gradient);
    result.history.push_back({iteration, current.objective.laplace_objective_m,
                              gradient_norm, 0.0, 0.0});
    if (options.print_every > 0 && iteration % options.print_every == 0) {
      std::cout << "[exact_laplace_lbfgs]\n"
                << "  event: iteration\n"
                << "  iteration: " << iteration << '\n'
                << "  objective: " << current.objective.laplace_objective_m
                << '\n'
                << "  gradient_norm: "
                << (gradient_norm <= options.gradient_tolerance ? "\033[32m"
                                                                : "\033[31m")
                << gradient_norm << "\033[0m\n"
                << "  objective_ms: " << current.timings.objective_ms << '\n'
                << "  hdot_ms: " << current.timings.hdot_ms << '\n'
                << "  total_ms: " << current.timings.total_ms << '\n'
                << std::flush;
    }
    if (options.print_every > 0 && options.gradient_table_every > 0 &&
        (iteration == 0 || iteration % options.gradient_table_every == 0)) {
      detail::print_gradient_table("exact_laplace_lbfgs", iteration,
                                   current.objective.laplace_objective_m,
                                   current.gradient,
                                   current.objective.u_hat_m.size(), options);
      last_gradient_table_iteration = iteration;
    }
    result.iterations = iteration;
    if (gradient_norm <= options.gradient_tolerance) {
      result.converged = true;
      result.message = "Exact Laplace gradient norm is below tolerance.";
      break;
    }

    std::vector<double> direction =
        detail::lbfgs_direction(current.gradient, steps, gradient_changes);
    double slope = detail::dot(current.gradient, direction);
    if (!(slope < 0.0) || !std::isfinite(slope)) {
      steps.clear();
      gradient_changes.clear();
      direction = current.gradient;
      for (double &value : direction) {
        value = -value;
      }
    }
    const double direction_norm = detail::norm(direction);
    if (direction_norm > options.maximum_direction_norm) {
      const double direction_scale =
          options.maximum_direction_norm / direction_norm;
      for (double &value : direction) {
        value *= direction_scale;
      }
    }
    slope = detail::dot(current.gradient, direction);

    bool accepted = false;
    double step_scale = options.initial_step_scale;
    int line_search_evaluations = 0;
    double line_search_ms = 0.0;
    std::vector<double> candidate_fixed;
    ExactLaplaceResult candidate;
    LaplaceObjectiveResult candidate_objective;
    while (step_scale >= options.minimum_step_scale) {
      if (options.max_evaluations > 0 &&
          result.objective_evaluations + result.exact_evaluations >=
              options.max_evaluations) {
        result.message = "Exact Laplace evaluation budget exhausted.";
        break;
      }
      candidate_fixed = detail::add_scaled(fixed, direction, step_scale);
      candidate_objective = evaluator.evaluate_objective(
          candidate_fixed, current.objective.u_hat_m);
      ++result.objective_evaluations;
      ++line_search_evaluations;
      line_search_ms += candidate_objective.total_ms_m;
      if (options.print_every > 0 && iteration % options.print_every == 0) {
        std::cout << "[exact_laplace_lbfgs]\n"
                  << "  event: line_search\n"
                  << "  iteration: " << iteration << '\n'
                  << "  attempt: " << line_search_evaluations << '\n'
                  << "  step_scale: " << step_scale << '\n'
                  << "  objective: " << candidate_objective.laplace_objective_m
                  << '\n'
                  << "  newton_iterations: "
                  << candidate_objective.newton_iterations_m << '\n'
                  << "  mode_ms: " << candidate_objective.mode_solve_ms_m
                  << '\n'
                  << "  logdet_ms: " << candidate_objective.logdet_ms_m << '\n'
                  << "  tape_rebuilt: "
                  << (candidate_objective.tape_rebuilt_m ? "true" : "false")
                  << '\n'
                  << "  total_ms: " << candidate_objective.total_ms_m << '\n'
                  << std::flush;
      }
      const double armijo = current.objective.laplace_objective_m +
                            options.sufficient_decrease * step_scale * slope;
      if (detail::finite(candidate_objective) &&
          candidate_objective.laplace_objective_m <= armijo) {
        accepted = true;
        break;
      }
      step_scale *= 0.5;
    }
    if (!accepted) {
      if (result.message.empty()) {
        result.message = "Exact Laplace L-BFGS line search failed.";
      }
      break;
    }

    if (options.max_evaluations > 0 &&
        result.objective_evaluations + result.exact_evaluations >=
            options.max_evaluations) {
      result.message = "Exact Laplace evaluation budget exhausted.";
      break;
    }

    candidate = evaluator.evaluate_from_objective(candidate_objective);
    ++result.exact_evaluations;
    if (!detail::finite(candidate)) {
      result.message =
          "Exact Laplace gradient failed at accepted line-search point.";
      break;
    }
    if (options.print_every > 0 && iteration % options.print_every == 0) {
      std::cout << "[exact_laplace_lbfgs]\n"
                << "  event: step_accepted\n"
                << "  iteration: " << iteration << '\n'
                << "  line_search_evaluations: " << line_search_evaluations
                << '\n'
                << "  line_search_ms: " << line_search_ms << '\n'
                << "  accepted_step_scale: " << step_scale << '\n'
                << std::flush;
    }

    std::vector<double> step = detail::subtract(candidate_fixed, fixed);
    std::vector<double> gradient_change =
        detail::subtract(candidate.gradient, current.gradient);
    const double curvature = detail::dot(step, gradient_change);
    if (options.memory > 0 && curvature > 1.0e-14 && std::isfinite(curvature)) {
      steps.push_back(step);
      gradient_changes.push_back(std::move(gradient_change));
      while (static_cast<int>(steps.size()) > options.memory) {
        steps.pop_front();
        gradient_changes.pop_front();
      }
    }

    result.step_norm = detail::norm(step);
    result.history.back().step_scale = step_scale;
    result.history.back().step_norm = result.step_norm;
    fixed = std::move(candidate_fixed);
    current = std::move(candidate);
    result.iterations = iteration + 1;
    if (result.step_norm <= options.step_tolerance) {
      result.converged = true;
      result.message = "Exact Laplace step norm is below tolerance.";
      break;
    }
  }

  if (!result.converged && result.message.empty()) {
    result.message = "Maximum exact Laplace L-BFGS iterations reached.";
  }
  result.fixed = fixed;
  result.random_mode = current.objective.u_hat_m;
  result.full = current.objective.full_m;
  result.gradient = current.gradient;
  result.objective = current.objective.laplace_objective_m;
  result.gradient_norm = detail::norm(current.gradient);
  if (options.print_every > 0 && options.gradient_table_every > 0 &&
      result.iterations != last_gradient_table_iteration) {
    detail::print_gradient_table("exact_laplace_lbfgs final", result.iterations,
                                 current.objective.laplace_objective_m,
                                 current.gradient,
                                 current.objective.u_hat_m.size(), options);
  }
  result.evaluation = std::move(current);
  return result;
}

// Hybrid exact-objective optimizer. A single persistent evaluator uses a
// band-limited gradient while its norm exceeds switch_gradient_norm. The
// transition is permanent and recomputes the current point exactly before
// convergence can be declared.
template <class Model>
LaplaceOptimizerResult
optimize_laplace_hybrid(ExactLaplaceEvaluator<Model> &evaluator,
                        const std::vector<double> &fixed_initial,
                        int approximate_bandwidth, double switch_gradient_norm,
                        const LaplaceOptimizerOptions &options = {}) {
  if (fixed_initial.empty() || approximate_bandwidth < 0 ||
      !(switch_gradient_norm > 0.0))
    throw std::invalid_argument("optimize_laplace_hybrid: invalid options");

  LaplaceOptimizerResult result;
  std::vector<double> fixed = fixed_initial;
  bool exact_phase = false;
  evaluator.set_dense_hdot_bandwidth(approximate_bandwidth);
  ExactLaplaceResult current = evaluator.evaluate(fixed);
  ++result.approximate_evaluations;
  if (!detail::finite(current)) {
    result.message = "Initial approximate Laplace evaluation failed.";
    result.evaluation = std::move(current);
    return result;
  }

  std::deque<std::vector<double>> steps;
  std::deque<std::vector<double>> gradient_changes;
  for (int iteration = 0; iteration < options.max_iterations; ++iteration) {
    double gradient_norm = detail::norm(current.gradient);
    if (!exact_phase && gradient_norm <= switch_gradient_norm) {
      evaluator.set_dense_hdot_bandwidth(-1);
      current = evaluator.evaluate(fixed, current.objective.u_hat_m);
      ++result.exact_evaluations;
      if (!detail::finite(current)) {
        result.message = "Exact evaluation failed at hybrid switch.";
        break;
      }
      exact_phase = true;
      result.exact_switch_iteration = iteration;
      gradient_norm = detail::norm(current.gradient);
    }

    result.history.push_back({iteration, current.objective.laplace_objective_m,
                              gradient_norm, 0.0, 0.0});
    if (options.print_every > 0 && iteration % options.print_every == 0) {
      std::cout << "[hybrid_laplace_lbfgs]\n"
                << "  event: iteration\n"
                << "  iteration: " << iteration << '\n'
                << "  phase: " << (exact_phase ? "exact" : "approximate")
                << '\n'
                << "  objective: " << current.objective.laplace_objective_m
                << '\n'
                << "  gradient_norm: "
                << (gradient_norm <= options.gradient_tolerance ? "\033[32m"
                                                                : "\033[31m")
                << gradient_norm << "\033[0m\n"
                << std::flush;
    }
    result.iterations = iteration;
    if (exact_phase && gradient_norm <= options.gradient_tolerance) {
      result.converged = true;
      result.message = "Hybrid optimizer exact gradient is below tolerance.";
      break;
    }

    std::vector<double> direction =
        detail::lbfgs_direction(current.gradient, steps, gradient_changes);
    double slope = detail::dot(current.gradient, direction);
    if (!(slope < 0.0) || !std::isfinite(slope)) {
      steps.clear();
      gradient_changes.clear();
      direction = current.gradient;
      for (double &value : direction)
        value = -value;
    }
    const double direction_norm = detail::norm(direction);
    if (direction_norm > options.maximum_direction_norm) {
      const double scale = options.maximum_direction_norm / direction_norm;
      for (double &value : direction)
        value *= scale;
    }
    slope = detail::dot(current.gradient, direction);

    bool accepted = false;
    double step_scale = options.initial_step_scale;
    std::vector<double> candidate_fixed;
    ExactLaplaceResult candidate;
    while (step_scale >= options.minimum_step_scale) {
      candidate_fixed = detail::add_scaled(fixed, direction, step_scale);
      try {
        candidate =
            evaluator.evaluate(candidate_fixed, current.objective.u_hat_m);
        if (exact_phase)
          ++result.exact_evaluations;
        else
          ++result.approximate_evaluations;
      } catch (const std::exception &) {
        step_scale *= 0.5;
        continue;
      }
      const double armijo = current.objective.laplace_objective_m +
                            options.sufficient_decrease * step_scale * slope;
      if (detail::finite(candidate) &&
          candidate.objective.laplace_objective_m <= armijo) {
        accepted = true;
        break;
      }
      step_scale *= 0.5;
    }
    if (!accepted) {
      result.message = "Hybrid Laplace L-BFGS line search failed.";
      break;
    }

    std::vector<double> step = detail::subtract(candidate_fixed, fixed);
    std::vector<double> gradient_change =
        detail::subtract(candidate.gradient, current.gradient);
    const double curvature = detail::dot(step, gradient_change);
    if (options.memory > 0 && curvature > 1.0e-14 && std::isfinite(curvature)) {
      steps.push_back(step);
      gradient_changes.push_back(std::move(gradient_change));
      while (static_cast<int>(steps.size()) > options.memory) {
        steps.pop_front();
        gradient_changes.pop_front();
      }
    }
    result.step_norm = detail::norm(step);
    result.history.back().step_scale = step_scale;
    result.history.back().step_norm = result.step_norm;
    fixed = std::move(candidate_fixed);
    current = std::move(candidate);
    result.iterations = iteration + 1;
    if (exact_phase && result.step_norm <= options.step_tolerance) {
      // Recheck the final point exactly even if the accepted candidate was
      // already exact; this keeps the convergence contract explicit.
      current = evaluator.evaluate(fixed, current.objective.u_hat_m);
      ++result.exact_evaluations;
      result.converged =
          detail::finite(current) &&
          detail::norm(current.gradient) <= options.gradient_tolerance;
      result.message = result.converged
                           ? "Hybrid optimizer passed exact final check."
                           : "Hybrid step stalled before exact convergence.";
      break;
    }
  }

  if (!result.converged && result.message.empty())
    result.message = "Maximum hybrid Laplace L-BFGS iterations reached.";
  result.fixed = fixed;
  result.random_mode = current.objective.u_hat_m;
  result.full = current.objective.full_m;
  result.gradient = current.gradient;
  result.objective = current.objective.laplace_objective_m;
  result.gradient_norm = detail::norm(current.gradient);
  result.evaluation = std::move(current);
  return result;
}

template <class Model>
LaplaceOptimizerResult optimize_laplace(
    Model &model, const std::vector<double> &fixed_initial,
    const std::vector<double> &random_initial,
    const ParameterPartition &partition,
    const LaplaceOptimizerOptions &optimizer_options = {},
    const LaplaceObjectiveOptions &objective_options = {},
    const laplace::ExactLaplaceGradientEngineOptions &engine_options = {}) {
  ExactLaplaceEvaluator<Model> evaluator(model, fixed_initial, random_initial,
                                         partition, objective_options,
                                         engine_options);
  return optimize_laplace(evaluator, fixed_initial, optimizer_options);
}

template <class Model>
LaplaceOptimizerResult optimize_laplace(
    Model &model, const std::vector<double> &fixed_initial,
    const std::vector<double> &random_initial, const ParameterSet &parameters,
    const LaplaceOptimizerOptions &optimizer_options = {},
    const LaplaceObjectiveOptions &objective_options = {},
    const laplace::ExactLaplaceGradientEngineOptions &engine_options = {}) {
  return optimize_laplace(model, fixed_initial, random_initial,
                          partition_parameters(parameters), optimizer_options,
                          objective_options, engine_options);
}

} // namespace stats
} // namespace quadra

#endif
