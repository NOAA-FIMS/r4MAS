#pragma once

#include <Eigen/Dense>

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../had_quadra.hpp"

namespace quadra {
namespace laplace {

struct AdaptiveDirectionalBatchOptions {
  // Maximum graph-owned memory available after the graph has been recorded.
  std::size_t memory_budget_bytes = 256u * 1024u * 1024u;
  std::size_t maximum_batch_size = 64;
  std::size_t minimum_batch_size = 1;
};

struct AdaptiveDirectionalBatchPlan {
  std::size_t batch_size = 0;
  std::size_t graph_reserved_bytes = 0;
  std::size_t estimated_bytes_per_direction = 0;
  std::size_t estimated_batch_reserved_bytes = 0;
  bool constrained_by_memory = false;
};

struct AdaptiveDirectionalTraceResult {
  Eigen::VectorXd trace_terms;
  AdaptiveDirectionalBatchPlan plan;
  std::size_t batches_executed = 0;
  std::size_t peak_tracked_graph_bytes = 0;
};

// Estimate the incremental graph storage required by one directional lane.
// This deliberately errs high for the fixed-size containers. Dynamic Hessian
// nodes are model dependent and are covered by using an explicit budget rather
// than treating this estimate as a hard allocation guarantee.
inline std::size_t EstimateDirectionalLaneBytes(std::size_t vertex_count) {
  const std::size_t per_vertex =
      5 * sizeof(had::Real) + // dot, w-dot, so-w-dot, and two edge derivatives
      sizeof(had::Real) +     // diagonal directional Hessian value
      sizeof(had::BTree);     // sparse directional Hessian tree shell

  if (vertex_count > std::numeric_limits<std::size_t>::max() / per_vertex) {
    return std::numeric_limits<std::size_t>::max();
  }
  return vertex_count * per_vertex + 2 * sizeof(std::vector<had::Real>) +
         sizeof(std::vector<had::BTree>);
}

inline AdaptiveDirectionalBatchPlan PlanAdaptiveDirectionalBatch(
    const had::ADGraph &graph, std::size_t total_directions,
    const AdaptiveDirectionalBatchOptions &options = {}) {
  if (options.minimum_batch_size == 0 || options.maximum_batch_size == 0 ||
      options.minimum_batch_size > options.maximum_batch_size) {
    throw std::invalid_argument("invalid adaptive directional batch limits");
  }

  AdaptiveDirectionalBatchPlan plan;
  plan.graph_reserved_bytes =
      had::MeasureADGraphMemory(graph).total_tracked_reserved_bytes;
  plan.estimated_bytes_per_direction =
      EstimateDirectionalLaneBytes(graph.vertices.size());

  if (total_directions == 0) {
    return plan;
  }

  const std::size_t requested =
      std::min(total_directions, options.maximum_batch_size);
  std::size_t affordable = 0;
  if (options.memory_budget_bytes > plan.graph_reserved_bytes &&
      plan.estimated_bytes_per_direction > 0) {
    affordable = (options.memory_budget_bytes - plan.graph_reserved_bytes) /
                 plan.estimated_bytes_per_direction;
  }

  plan.batch_size =
      std::min(requested, std::max(options.minimum_batch_size, affordable));
  plan.batch_size = std::min(plan.batch_size, total_directions);
  plan.constrained_by_memory = plan.batch_size < requested;

  if (plan.estimated_bytes_per_direction !=
      std::numeric_limits<std::size_t>::max()) {
    plan.estimated_batch_reserved_bytes =
        plan.graph_reserved_bytes +
        plan.batch_size * plan.estimated_bytes_per_direction;
  } else {
    plan.estimated_batch_reserved_bytes =
        std::numeric_limits<std::size_t>::max();
  }
  return plan;
}

} // namespace laplace
} // namespace quadra
