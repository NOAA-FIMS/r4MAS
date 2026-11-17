#pragma once

#include "simulation.hpp"

#include <fstream>
#include <iomanip>
#include <ostream>
#include <stdexcept>
#include <string>

namespace quadra {
namespace sampling {

namespace detail {

inline void write_csv_field(std::ostream &out, const std::string &value) {
  if (value.find_first_of(",\"\r\n") == std::string::npos) {
    out << value;
    return;
  }
  out << '"';
  for (char character : value) {
    if (character == '"')
      out << '"';
    out << character;
  }
  out << '"';
}

inline void validate_output_stream(const std::ostream &out,
                                   const char *function) {
  if (!out)
    throw std::runtime_error(std::string(function) + ": write failed");
}

template <class Writer>
void write_csv_file(const std::string &path, Writer writer,
                    const char *function) {
  std::ofstream out(path);
  if (!out)
    throw std::runtime_error(std::string(function) +
                             ": failed to open output file: " + path);
  writer(out);
}

} // namespace detail

inline void write_posterior_draws_csv(std::ostream &out,
                                      const NutsWorkflowResult &result) {
  out << std::setprecision(17)
      << "chain,iteration,parameter,value,log_density\n";
  for (std::size_t chain = 0; chain < result.fit.chains.size(); ++chain) {
    const auto &chain_result = result.fit.chains[chain];
    if (chain_result.log_density.size() != chain_result.draws.size())
      throw std::runtime_error(
          "write_posterior_draws_csv: log-density dimension mismatch");
    for (std::size_t iteration = 0; iteration < chain_result.draws.size();
         ++iteration) {
      const auto &draw = chain_result.draws[iteration];
      if (draw.size() != result.parameter_names.size())
        throw std::runtime_error(
            "write_posterior_draws_csv: draw dimension mismatch");
      for (std::size_t parameter = 0; parameter < draw.size(); ++parameter) {
        out << chain + 1 << ',' << iteration + 1 << ',';
        detail::write_csv_field(out, result.parameter_names[parameter]);
        out << ',' << draw[parameter] << ','
            << chain_result.log_density[iteration] << '\n';
      }
    }
  }
  detail::validate_output_stream(out, "write_posterior_draws_csv");
}

inline void write_parameter_diagnostics_csv(std::ostream &out,
                                            const NutsWorkflowResult &result) {
  if (result.parameter_names.size() != result.fit.diagnostics.split_rhat.size())
    throw std::runtime_error(
        "write_parameter_diagnostics_csv: parameter dimension mismatch");
  out << std::setprecision(17) << "parameter,rhat,bulk_ess,tail_ess\n";
  for (std::size_t parameter = 0; parameter < result.parameter_names.size();
       ++parameter) {
    detail::write_csv_field(out, result.parameter_names[parameter]);
    out << ',' << result.fit.diagnostics.split_rhat[parameter] << ','
        << result.fit.diagnostics.bulk_ess[parameter] << ','
        << result.fit.diagnostics.tail_ess[parameter] << '\n';
  }
  detail::validate_output_stream(out, "write_parameter_diagnostics_csv");
}

inline void write_chain_diagnostics_csv(std::ostream &out,
                                        const NutsWorkflowResult &result) {
  out << std::setprecision(17)
      << "chain,acceptance,step_size,divergences,depth_hits,bfmi,"
         "mass_updates,mass_update_failures\n";
  for (std::size_t chain = 0; chain < result.fit.chains.size(); ++chain) {
    const auto &diagnostics = result.fit.chains[chain].diagnostics;
    out << chain + 1 << ',' << diagnostics.mean_acceptance << ','
        << diagnostics.step_size << ',' << diagnostics.divergences << ','
        << diagnostics.max_depth_hits << ',' << diagnostics.energy_bfmi << ','
        << diagnostics.mass_matrix_updates << ','
        << diagnostics.mass_matrix_update_failures << '\n';
  }
  detail::validate_output_stream(out, "write_chain_diagnostics_csv");
}

inline void
write_posterior_predictive_csv(std::ostream &out,
                               const PosteriorSimulationResult &result) {
  out << std::setprecision(17) << "chain,iteration,quantity,value\n";
  for (const auto &draw : result.draws) {
    if (draw.values.size() != result.quantity_names.size())
      throw std::runtime_error(
          "write_posterior_predictive_csv: draw dimension mismatch");
    for (std::size_t quantity = 0; quantity < draw.values.size(); ++quantity) {
      out << draw.chain + 1 << ',' << draw.iteration + 1 << ',';
      detail::write_csv_field(out, result.quantity_names[quantity]);
      out << ',' << draw.values[quantity] << '\n';
    }
  }
  detail::validate_output_stream(out, "write_posterior_predictive_csv");
}

inline void write_nuts_summary_csv(std::ostream &out,
                                   const NutsWorkflowResult &result) {
  out << std::setprecision(17) << "metric,value\n"
      << "health," << (result.health.passed ? "PASS" : "FAIL") << '\n'
      << "chains," << result.fit.chains.size() << '\n'
      << "parameters," << result.parameter_count() << '\n'
      << "draws," << result.total_draws() << '\n'
      << "warmup_per_chain," << result.options.sampler.warmup << '\n'
      << "samples_per_chain," << result.options.sampler.samples << '\n'
      << "max_tree_depth," << result.options.sampler.max_tree_depth << '\n'
      << "target_acceptance," << result.options.sampler.target_acceptance
      << '\n'
      << "divergence_threshold," << result.options.sampler.divergence_threshold
      << '\n'
      << "diagonal_metric,"
      << (result.options.sampler.adapt_diagonal_mass ? 1 : 0) << '\n'
      << "dense_metric," << (result.options.sampler.adapt_dense_mass ? 1 : 0)
      << '\n'
      << "reuse_ad_graph," << (result.options.sampler.reuse_ad_graph ? 1 : 0)
      << '\n'
      << "sampler_seed," << result.options.sampler.seed << '\n'
      << "initialization_seed," << result.options.initialization_seed << '\n'
      << "initial_jitter," << result.options.initial_jitter << '\n'
      << "parallel," << (result.options.parallel ? 1 : 0) << '\n'
      << "rhat_threshold," << result.options.health.max_rhat << '\n'
      << "bulk_ess_threshold," << result.options.health.min_bulk_ess << '\n'
      << "tail_ess_threshold," << result.options.health.min_tail_ess << '\n'
      << "bfmi_threshold," << result.options.health.min_bfmi << '\n'
      << "max_rhat," << result.health.max_rhat << '\n'
      << "min_bulk_ess," << result.health.min_bulk_ess << '\n'
      << "min_tail_ess," << result.health.min_tail_ess << '\n'
      << "min_bfmi," << result.health.min_bfmi << '\n'
      << "divergences," << result.health.divergences << '\n'
      << "depth_hits," << result.health.depth_hits << '\n'
      << "mass_update_failures," << result.health.mass_matrix_update_failures
      << '\n';
  detail::validate_output_stream(out, "write_nuts_summary_csv");
}

#define QUADRA_SAMPLING_CSV_FILE_OVERLOAD(function_name, result_type)          \
  inline void function_name(const std::string &path,                           \
                            const result_type &result) {                       \
    detail::write_csv_file(                                                    \
        path, [&](std::ostream &out) { function_name(out, result); },          \
        #function_name);                                                       \
  }

QUADRA_SAMPLING_CSV_FILE_OVERLOAD(write_posterior_draws_csv, NutsWorkflowResult)
QUADRA_SAMPLING_CSV_FILE_OVERLOAD(write_parameter_diagnostics_csv,
                                  NutsWorkflowResult)
QUADRA_SAMPLING_CSV_FILE_OVERLOAD(write_chain_diagnostics_csv,
                                  NutsWorkflowResult)
QUADRA_SAMPLING_CSV_FILE_OVERLOAD(write_posterior_predictive_csv,
                                  PosteriorSimulationResult)
QUADRA_SAMPLING_CSV_FILE_OVERLOAD(write_nuts_summary_csv, NutsWorkflowResult)

#undef QUADRA_SAMPLING_CSV_FILE_OVERLOAD

} // namespace sampling
} // namespace quadra
