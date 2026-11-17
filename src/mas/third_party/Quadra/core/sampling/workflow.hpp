#pragma once

#include "multi_chain.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <random>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

namespace quadra {
namespace sampling {

struct NutsWorkflowOptions {
  NutsOptions sampler;
  NutsHealthThresholds health;
  std::size_t chains = 4;
  double initial_jitter = 0.01;
  std::uint64_t initialization_seed = 8675309u;
  bool parallel = true;
};

struct NutsWorkflowResult {
  std::vector<std::string> parameter_names;
  std::vector<std::vector<double>> initial_states;
  NutsWorkflowOptions options;
  MultiChainResult fit;
  NutsHealthAssessment health;

  std::size_t parameter_count() const { return parameter_names.size(); }

  std::size_t total_draws() const {
    std::size_t count = 0;
    for (const auto &chain : fit.chains)
      count += chain.draws.size();
    return count;
  }

  std::size_t worst_rhat_parameter() const {
    return static_cast<std::size_t>(
        std::max_element(fit.diagnostics.split_rhat.begin(),
                         fit.diagnostics.split_rhat.end()) -
        fit.diagnostics.split_rhat.begin());
  }

  std::size_t minimum_bulk_ess_parameter() const {
    return static_cast<std::size_t>(
        std::min_element(fit.diagnostics.bulk_ess.begin(),
                         fit.diagnostics.bulk_ess.end()) -
        fit.diagnostics.bulk_ess.begin());
  }

  std::size_t minimum_tail_ess_parameter() const {
    return static_cast<std::size_t>(
        std::min_element(fit.diagnostics.tail_ess.begin(),
                         fit.diagnostics.tail_ess.end()) -
        fit.diagnostics.tail_ess.begin());
  }
};

inline void validate_parameter_names(const std::vector<std::string> &names,
                                     std::size_t dimension) {
  if (names.size() != dimension)
    throw std::invalid_argument(
        "run_nuts_workflow: parameter names do not match fitted mode");
  std::unordered_set<std::string> unique;
  for (const auto &name : names) {
    if (name.empty() || !unique.insert(name).second)
      throw std::invalid_argument(
          "run_nuts_workflow: parameter names must be nonempty and unique");
  }
}

inline std::vector<std::vector<double>>
initialize_nuts_chains(const std::vector<double> &fitted_mode,
                       std::size_t chains, double jitter = 0.01,
                       std::uint64_t seed = 8675309u) {
  if (fitted_mode.empty() || chains < 2 || !(jitter >= 0.0) ||
      !std::isfinite(jitter))
    throw std::invalid_argument("initialize_nuts_chains: invalid arguments");
  for (double value : fitted_mode)
    if (!std::isfinite(value))
      throw std::invalid_argument(
          "initialize_nuts_chains: fitted mode must be finite");

  std::mt19937_64 rng(seed);
  std::normal_distribution<double> normal(0.0, jitter);
  std::vector<std::vector<double>> states(chains, fitted_mode);
  for (auto &state : states)
    for (double &value : state)
      value += normal(rng);
  return states;
}

template <class TargetFactory>
NutsWorkflowResult run_nuts_workflow(TargetFactory target_factory,
                                     const std::vector<double> &fitted_mode,
                                     std::vector<std::string> parameter_names,
                                     const NutsWorkflowOptions &options = {}) {
  validate_parameter_names(parameter_names, fitted_mode.size());
  NutsWorkflowResult out;
  out.parameter_names = std::move(parameter_names);
  out.options = options;
  out.initial_states = initialize_nuts_chains(fitted_mode, options.chains,
                                              options.initial_jitter,
                                              options.initialization_seed);
  out.fit = sample_nuts_chains(target_factory, out.initial_states,
                               options.sampler, options.parallel);
  out.health = assess_nuts_health(out.fit, options.health);
  return out;
}

} // namespace sampling
} // namespace quadra
