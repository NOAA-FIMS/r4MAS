#pragma once

#include "nuts.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <thread>
#include <vector>

namespace quadra {
namespace sampling {

struct MultiChainDiagnostics {
  std::vector<double> split_rhat;
  std::vector<double> bulk_ess;
  std::vector<double> tail_ess;
};

struct MultiChainResult {
  std::vector<NutsResult> chains;
  MultiChainDiagnostics diagnostics;
};

struct NutsHealthThresholds {
  double max_rhat = 1.01;
  double min_bulk_ess = 100.0;
  double min_tail_ess = 100.0;
  double min_bfmi = 0.3;
  int max_divergences = 0;
  int max_depth_hits = 0;
  int max_mass_matrix_update_failures = 0;
};

struct NutsHealthAssessment {
  bool passed = false;
  double max_rhat = std::numeric_limits<double>::quiet_NaN();
  double min_bulk_ess = std::numeric_limits<double>::quiet_NaN();
  double min_tail_ess = std::numeric_limits<double>::quiet_NaN();
  double min_bfmi = std::numeric_limits<double>::quiet_NaN();
  int divergences = 0;
  int depth_hits = 0;
  int mass_matrix_update_failures = 0;
};

namespace detail {

inline double quantile(std::vector<double> values, double probability) {
  if (values.empty())
    return std::numeric_limits<double>::quiet_NaN();
  std::sort(values.begin(), values.end());
  const double position = probability * static_cast<double>(values.size() - 1);
  const std::size_t lower = static_cast<std::size_t>(std::floor(position));
  const std::size_t upper = static_cast<std::size_t>(std::ceil(position));
  const double weight = position - lower;
  return (1.0 - weight) * values[lower] + weight * values[upper];
}

// Acklam's rational approximation to the inverse standard-normal CDF.
inline double inverse_normal_cdf(double probability) {
  const double a[] = {-3.969683028665376e+01, 2.209460984245205e+02,
                      -2.759285104469687e+02, 1.383577518672690e+02,
                      -3.066479806614716e+01, 2.506628277459239e+00};
  const double b[] = {-5.447609879822406e+01, 1.615858368580409e+02,
                      -1.556989798598866e+02, 6.680131188771972e+01,
                      -1.328068155288572e+01};
  const double c[] = {-7.784894002430293e-03, -3.223964580411365e-01,
                      -2.400758277161838e+00, -2.549732539343734e+00,
                      4.374664141464968e+00,  2.938163982698783e+00};
  const double d[] = {7.784695709041462e-03, 3.224671290700398e-01,
                      2.445134137142996e+00, 3.754408661907416e+00};
  if (!(probability > 0.0) || !(probability < 1.0))
    throw std::invalid_argument("inverse_normal_cdf probability out of range");
  if (probability < 0.02425) {
    const double q = std::sqrt(-2.0 * std::log(probability));
    return (((((c[0] * q + c[1]) * q + c[2]) * q + c[3]) * q + c[4]) * q +
            c[5]) /
           ((((d[0] * q + d[1]) * q + d[2]) * q + d[3]) * q + 1.0);
  }
  if (probability > 0.97575) {
    const double q = std::sqrt(-2.0 * std::log(1.0 - probability));
    return -(((((c[0] * q + c[1]) * q + c[2]) * q + c[3]) * q + c[4]) * q +
             c[5]) /
           ((((d[0] * q + d[1]) * q + d[2]) * q + d[3]) * q + 1.0);
  }
  const double q = probability - 0.5;
  const double r = q * q;
  return (((((a[0] * r + a[1]) * r + a[2]) * r + a[3]) * r + a[4]) * r + a[5]) *
         q /
         (((((b[0] * r + b[1]) * r + b[2]) * r + b[3]) * r + b[4]) * r + 1.0);
}

inline std::vector<std::vector<double>>
rank_normalize(const std::vector<std::vector<double>> &chains) {
  const std::size_t chain_count = chains.size();
  const std::size_t draws = chains.front().size();
  std::vector<std::pair<double, std::size_t>> ordered;
  ordered.reserve(chain_count * draws);
  for (std::size_t chain = 0; chain < chain_count; ++chain)
    for (std::size_t draw = 0; draw < draws; ++draw)
      ordered.emplace_back(chains[chain][draw], chain * draws + draw);
  std::sort(ordered.begin(), ordered.end(),
            [](const auto &left, const auto &right) {
              return left.first < right.first;
            });
  std::vector<double> ranks(ordered.size());
  for (std::size_t begin = 0; begin < ordered.size();) {
    std::size_t end = begin + 1;
    while (end < ordered.size() && ordered[end].first == ordered[begin].first)
      ++end;
    const double average_rank = 0.5 * (begin + 1 + end);
    for (std::size_t i = begin; i < end; ++i)
      ranks[ordered[i].second] = average_rank;
    begin = end;
  }
  std::vector<std::vector<double>> normalized(chain_count,
                                              std::vector<double>(draws));
  const double total = static_cast<double>(ordered.size());
  for (std::size_t chain = 0; chain < chain_count; ++chain)
    for (std::size_t draw = 0; draw < draws; ++draw) {
      const double probability =
          (ranks[chain * draws + draw] - 0.375) / (total + 0.25);
      normalized[chain][draw] = inverse_normal_cdf(probability);
    }
  return normalized;
}

inline std::vector<std::vector<double>>
split_parameter_chains(const std::vector<NutsResult> &chains,
                       std::size_t parameter) {
  std::size_t half = std::numeric_limits<std::size_t>::max();
  for (const auto &chain : chains)
    half = std::min(half, chain.draws.size() / 2);
  if (half < 2)
    return {};
  std::vector<std::vector<double>> split;
  split.reserve(2 * chains.size());
  for (const auto &chain : chains) {
    std::vector<double> first(half), second(half);
    for (std::size_t i = 0; i < half; ++i) {
      first[i] = chain.draws[i][parameter];
      second[i] = chain.draws[chain.draws.size() - half + i][parameter];
    }
    split.push_back(std::move(first));
    split.push_back(std::move(second));
  }
  return split;
}

inline double sample_variance(const std::vector<double> &x, double mean) {
  double sum = 0.0;
  for (double value : x) {
    const double difference = value - mean;
    sum += difference * difference;
  }
  return x.size() > 1 ? sum / static_cast<double>(x.size() - 1) : 0.0;
}

inline double split_rhat(const std::vector<std::vector<double>> &split) {
  if (split.empty())
    return std::numeric_limits<double>::quiet_NaN();
  const std::size_t m = split.size();
  const std::size_t n = split.front().size();
  std::vector<double> means(m), variances(m);
  for (std::size_t chain = 0; chain < m; ++chain) {
    for (double value : split[chain])
      means[chain] += value;
    means[chain] /= n;
    variances[chain] = sample_variance(split[chain], means[chain]);
  }
  double mean_of_means = 0.0;
  double within = 0.0;
  for (std::size_t chain = 0; chain < m; ++chain) {
    mean_of_means += means[chain];
    within += variances[chain];
  }
  mean_of_means /= m;
  within /= m;
  double between_sum = 0.0;
  for (double mean : means) {
    const double difference = mean - mean_of_means;
    between_sum += difference * difference;
  }
  const double between =
      static_cast<double>(n) * between_sum / static_cast<double>(m - 1);
  if (!(within > 0.0))
    return between == 0.0 ? 1.0 : std::numeric_limits<double>::infinity();
  const double variance =
      (static_cast<double>(n - 1) / n) * within + between / n;
  return std::sqrt(variance / within);
}

inline double
effective_sample_size(const std::vector<std::vector<double>> &split) {
  if (split.empty())
    return std::numeric_limits<double>::quiet_NaN();
  const std::size_t m = split.size();
  const std::size_t n = split.front().size();
  std::vector<double> means(m), variances(m);
  for (std::size_t chain = 0; chain < m; ++chain) {
    for (double value : split[chain])
      means[chain] += value;
    means[chain] /= n;
    variances[chain] = sample_variance(split[chain], means[chain]);
  }
  double within = 0.0;
  double mean_of_means = 0.0;
  for (std::size_t chain = 0; chain < m; ++chain) {
    within += variances[chain];
    mean_of_means += means[chain];
  }
  within /= m;
  mean_of_means /= m;
  double between_sum = 0.0;
  for (double mean : means) {
    const double difference = mean - mean_of_means;
    between_sum += difference * difference;
  }
  const double between =
      static_cast<double>(n) * between_sum / static_cast<double>(m - 1);
  const double variance =
      (static_cast<double>(n - 1) / n) * within + between / n;
  if (!(variance > 0.0))
    return static_cast<double>(m * n);

  std::vector<double> rho(n, 0.0);
  rho[0] = 1.0;
  for (std::size_t lag = 1; lag < n; ++lag) {
    double autocovariance = 0.0;
    for (std::size_t chain = 0; chain < m; ++chain) {
      double chain_covariance = 0.0;
      for (std::size_t i = 0; i + lag < n; ++i)
        chain_covariance += (split[chain][i] - means[chain]) *
                            (split[chain][i + lag] - means[chain]);
      autocovariance += chain_covariance / static_cast<double>(n - lag);
    }
    autocovariance /= m;
    rho[lag] = 1.0 - (within - autocovariance) / variance;
  }
  double correlation_sum = 0.0;
  double previous_pair = std::numeric_limits<double>::infinity();
  for (std::size_t lag = 1; lag + 1 < n; lag += 2) {
    double pair = rho[lag] + rho[lag + 1];
    if (pair < 0.0)
      break;
    pair = std::min(pair, previous_pair);
    correlation_sum += pair;
    previous_pair = pair;
  }
  const double ess =
      static_cast<double>(m * n) / std::max(1.0, 1.0 + 2.0 * correlation_sum);
  return std::min(static_cast<double>(m * n), ess);
}

inline std::vector<std::vector<double>>
indicator_chains(const std::vector<std::vector<double>> &split,
                 double threshold, bool lower) {
  auto indicators = split;
  for (auto &chain : indicators)
    for (double &value : chain)
      value = lower ? (value <= threshold ? 1.0 : 0.0)
                    : (value >= threshold ? 1.0 : 0.0);
  return indicators;
}

} // namespace detail

inline MultiChainDiagnostics
compute_multi_chain_diagnostics(const std::vector<NutsResult> &chains) {
  if (chains.size() < 2 || chains.front().draws.empty())
    throw std::invalid_argument(
        "multi-chain diagnostics require at least two nonempty chains");
  const std::size_t parameters = chains.front().draws.front().size();
  MultiChainDiagnostics out;
  out.split_rhat.resize(parameters);
  out.bulk_ess.resize(parameters);
  out.tail_ess.resize(parameters);
  for (std::size_t parameter = 0; parameter < parameters; ++parameter) {
    const auto split = detail::split_parameter_chains(chains, parameter);
    if (split.empty())
      throw std::invalid_argument(
          "multi-chain diagnostics require at least four draws per chain");
    std::vector<double> pooled;
    for (const auto &chain : split)
      pooled.insert(pooled.end(), chain.begin(), chain.end());
    const double lower = detail::quantile(pooled, 0.05);
    const double upper = detail::quantile(pooled, 0.95);
    const double median = detail::quantile(pooled, 0.5);
    const auto rank_normalized = detail::rank_normalize(split);
    auto folded = split;
    for (auto &chain : folded)
      for (double &value : chain)
        value = std::abs(value - median);
    const auto folded_rank_normalized = detail::rank_normalize(folded);
    out.split_rhat[parameter] =
        std::max(detail::split_rhat(rank_normalized),
                 detail::split_rhat(folded_rank_normalized));
    out.bulk_ess[parameter] = detail::effective_sample_size(rank_normalized);
    out.tail_ess[parameter] =
        std::min(detail::effective_sample_size(
                     detail::indicator_chains(split, lower, true)),
                 detail::effective_sample_size(
                     detail::indicator_chains(split, upper, false)));
  }
  return out;
}

inline NutsHealthAssessment
assess_nuts_health(const MultiChainResult &result,
                   const NutsHealthThresholds &thresholds = {}) {
  if (result.chains.empty() || result.diagnostics.split_rhat.empty() ||
      result.diagnostics.bulk_ess.empty() ||
      result.diagnostics.tail_ess.empty() ||
      result.diagnostics.bulk_ess.size() !=
          result.diagnostics.split_rhat.size() ||
      result.diagnostics.tail_ess.size() !=
          result.diagnostics.split_rhat.size())
    throw std::invalid_argument(
        "assess_nuts_health requires populated chains and diagnostics");
  if (!(thresholds.max_rhat >= 1.0) || !(thresholds.min_bulk_ess >= 0.0) ||
      !(thresholds.min_tail_ess >= 0.0) || !(thresholds.min_bfmi >= 0.0) ||
      !std::isfinite(thresholds.max_rhat) ||
      !std::isfinite(thresholds.min_bulk_ess) ||
      !std::isfinite(thresholds.min_tail_ess) ||
      !std::isfinite(thresholds.min_bfmi) || thresholds.max_divergences < 0 ||
      thresholds.max_depth_hits < 0 ||
      thresholds.max_mass_matrix_update_failures < 0)
    throw std::invalid_argument("assess_nuts_health: invalid thresholds");
  NutsHealthAssessment out;
  out.max_rhat = *std::max_element(result.diagnostics.split_rhat.begin(),
                                   result.diagnostics.split_rhat.end());
  out.min_bulk_ess = *std::min_element(result.diagnostics.bulk_ess.begin(),
                                       result.diagnostics.bulk_ess.end());
  out.min_tail_ess = *std::min_element(result.diagnostics.tail_ess.begin(),
                                       result.diagnostics.tail_ess.end());
  out.min_bfmi = std::numeric_limits<double>::infinity();
  for (const auto &chain : result.chains) {
    const auto &diagnostics = chain.diagnostics;
    out.divergences += diagnostics.divergences;
    out.depth_hits += diagnostics.max_depth_hits;
    out.mass_matrix_update_failures += diagnostics.mass_matrix_update_failures;
    out.min_bfmi = std::min(out.min_bfmi, diagnostics.energy_bfmi);
  }
  out.passed = std::isfinite(out.max_rhat) && std::isfinite(out.min_bulk_ess) &&
               std::isfinite(out.min_tail_ess) && std::isfinite(out.min_bfmi) &&
               out.max_rhat <= thresholds.max_rhat &&
               out.min_bulk_ess >= thresholds.min_bulk_ess &&
               out.min_tail_ess >= thresholds.min_tail_ess &&
               out.min_bfmi >= thresholds.min_bfmi &&
               out.divergences <= thresholds.max_divergences &&
               out.depth_hits <= thresholds.max_depth_hits &&
               out.mass_matrix_update_failures <=
                   thresholds.max_mass_matrix_update_failures;
  return out;
}

template <class TargetFactory>
MultiChainResult
sample_nuts_chains(TargetFactory target_factory,
                   const std::vector<std::vector<double>> &initial_states,
                   const NutsOptions &options = {}, bool parallel = true) {
  if (initial_states.size() < 2)
    throw std::invalid_argument(
        "sample_nuts_chains requires at least two chains");
  MultiChainResult out;
  out.chains.resize(initial_states.size());
  std::vector<std::exception_ptr> errors(initial_states.size());
  auto run = [&](std::size_t chain) {
    try {
      auto target = target_factory(chain);
      NutsOptions chain_options = options;
      chain_options.seed +=
          UINT64_C(0x9e3779b97f4a7c15) * static_cast<std::uint64_t>(chain + 1);
      out.chains[chain] =
          sample_nuts(target, initial_states[chain], chain_options);
    } catch (...) {
      errors[chain] = std::current_exception();
    }
  };
  if (parallel) {
    std::vector<std::thread> workers;
    workers.reserve(initial_states.size());
    for (std::size_t chain = 0; chain < initial_states.size(); ++chain)
      workers.emplace_back(run, chain);
    for (auto &worker : workers)
      worker.join();
  } else {
    for (std::size_t chain = 0; chain < initial_states.size(); ++chain)
      run(chain);
  }
  for (const auto &error : errors)
    if (error)
      std::rethrow_exception(error);
  out.diagnostics = compute_multi_chain_diagnostics(out.chains);
  return out;
}

} // namespace sampling
} // namespace quadra
