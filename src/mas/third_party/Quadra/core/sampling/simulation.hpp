#pragma once

#include "workflow.hpp"

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace quadra {
namespace sampling {

struct PosteriorSimulationOptions {
  std::size_t thin = 1;
  std::size_t max_draws = 0;
  std::uint64_t seed = 314159265u;
  bool require_healthy_fit = true;
};

struct PosteriorSimulationDraw {
  std::size_t chain = 0;
  std::size_t iteration = 0;
  std::vector<double> values;
};

struct PosteriorSimulationResult {
  std::vector<std::string> quantity_names;
  std::vector<PosteriorSimulationDraw> draws;
};

namespace detail {

inline std::uint64_t mix_simulation_seed(std::uint64_t value) {
  value += UINT64_C(0x9e3779b97f4a7c15);
  value = (value ^ (value >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
  value = (value ^ (value >> 27)) * UINT64_C(0x94d049bb133111eb);
  return value ^ (value >> 31);
}

inline void validate_quantity_names(const std::vector<std::string> &names) {
  if (names.empty())
    throw std::invalid_argument(
        "simulate_posterior: quantity names must not be empty");
  validate_parameter_names(names, names.size());
}

} // namespace detail

template <class Simulator>
PosteriorSimulationResult
simulate_posterior(const NutsWorkflowResult &posterior,
                   std::vector<std::string> quantity_names, Simulator simulator,
                   const PosteriorSimulationOptions &options = {}) {
  if (options.thin == 0)
    throw std::invalid_argument("simulate_posterior: thin must be positive");
  if (options.require_healthy_fit && !posterior.health.passed)
    throw std::runtime_error(
        "simulate_posterior: refusing to simulate from an unhealthy fit");
  detail::validate_quantity_names(quantity_names);

  PosteriorSimulationResult out;
  out.quantity_names = std::move(quantity_names);
  for (std::size_t chain = 0; chain < posterior.fit.chains.size(); ++chain) {
    const auto &draws = posterior.fit.chains[chain].draws;
    for (std::size_t iteration = 0; iteration < draws.size(); ++iteration) {
      if (iteration % options.thin != 0)
        continue;
      if (options.max_draws > 0 && out.draws.size() >= options.max_draws)
        return out;
      const std::uint64_t chain_key =
          detail::mix_simulation_seed(static_cast<std::uint64_t>(chain + 1));
      const std::uint64_t draw_key = detail::mix_simulation_seed(
          chain_key ^ static_cast<std::uint64_t>(iteration + 1));
      std::mt19937_64 rng(detail::mix_simulation_seed(options.seed ^ draw_key));
      std::vector<double> values = simulator(draws[iteration], rng);
      if (values.size() != out.quantity_names.size())
        throw std::runtime_error(
            "simulate_posterior: simulator output dimension changed");
      for (double value : values)
        if (!std::isfinite(value))
          throw std::runtime_error(
              "simulate_posterior: simulator returned a non-finite value");
      out.draws.push_back({chain, iteration, std::move(values)});
    }
  }
  return out;
}

} // namespace sampling
} // namespace quadra
