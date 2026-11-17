#pragma once

#include "../../external/eigen/Eigen/Dense"
#include "../had_quadra.hpp"

#include <stdexcept>
#include <vector>

namespace quadra {
namespace laplace {

// Exact mixed third derivatives from the cubic directional derivative.
// The polarization identity extracts D^3 f[total_direction, e_a, e_b]
// without differencing Hessians or relying on reverse edge-pushing.
template <class Objective>
Eigen::MatrixXd
dense_hdot_third_order_polarized(Objective &&objective,
                                 const std::vector<double> &x,
                                 const std::vector<double> &total_direction,
                                 const std::vector<int> &random_indices) {
  if (x.size() != total_direction.size())
    throw std::invalid_argument("dense Hdot direction has wrong length");

  const int nr = static_cast<int>(random_indices.size());
  Eigen::MatrixXd out(nr, nr);
  for (int a = 0; a < nr; ++a) {
    for (int b = 0; b <= a; ++b) {
      auto cubic = [&](int st, int sa, int sb) {
        std::vector<double> direction = total_direction;
        for (double &value : direction)
          value *= st;
        direction[static_cast<std::size_t>(random_indices[a])] += sa;
        direction[static_cast<std::size_t>(random_indices[b])] += sb;
        return had::third_directional_derivative(objective, x, direction);
      };
      // Four-term polarization for a symmetric trilinear form. The usual
      // eight-term identity pairs opposite directions, whose cubic
      // directional derivatives differ only by sign.
      const double mixed =
          cubic(1, 1, 1) - cubic(1, 1, -1) - cubic(1, -1, 1) - cubic(-1, 1, 1);
      out(a, b) = out(b, a) = mixed / 24.0;
    }
  }
  return out;
}

// Compute trace(H^{-1} Hdot) without materializing either dense matrix.
// inverse_column(b) must return column b of H^{-1}. Symmetry lets each lower
// triangular Hdot entry contribute once, with off-diagonal entries doubled.
template <class Objective, class InverseColumnProvider>
double dense_hdot_trace_third_order_polarized(
    Objective &&objective, const std::vector<double> &x,
    const std::vector<double> &total_direction,
    const std::vector<int> &random_indices,
    InverseColumnProvider &&inverse_column, int maximum_bandwidth = -1) {
  if (x.size() != total_direction.size())
    throw std::invalid_argument("dense Hdot direction has wrong length");

  const int nr = static_cast<int>(random_indices.size());
  if (maximum_bandwidth < -1)
    throw std::invalid_argument("maximum Hdot bandwidth is invalid");
  double trace = 0.0;
  for (int b = 0; b < nr; ++b) {
    const Eigen::VectorXd column = inverse_column(b);
    if (column.size() != nr)
      throw std::invalid_argument("inverse column has wrong length");

    const int last_a = maximum_bandwidth < 0
                           ? nr - 1
                           : std::min(nr - 1, b + maximum_bandwidth);
    for (int a = b; a <= last_a; ++a) {
      auto cubic = [&](int st, int sa, int sb) {
        std::vector<double> direction = total_direction;
        for (double &value : direction)
          value *= st;
        direction[static_cast<std::size_t>(random_indices[a])] += sa;
        direction[static_cast<std::size_t>(random_indices[b])] += sb;
        return had::third_directional_derivative(objective, x, direction);
      };
      const double mixed =
          cubic(1, 1, 1) - cubic(1, 1, -1) - cubic(1, -1, 1) - cubic(-1, 1, 1);
      const double hdot = mixed / 24.0;
      trace += (a == b ? 1.0 : 2.0) * column[a] * hdot;
    }
  }
  return trace;
}

// Exact trace contraction using H^{-1} = L L'. For the symmetric trilinear
// form T = D^3 f and cubic P(v) = T[v,v,v],
//
//   T[d,l,l] = (P(d+l) + P(d-l) - 2 P(d)) / 6.
//
// Therefore trace(H^{-1} Hdot[d]) is the sum of T[d,l_k,l_k] over the
// columns of L. This needs 2*n_random + 1 directional evaluations instead of
// four evaluations for every lower-triangular Hdot entry.
template <class Objective>
double dense_hdot_trace_third_order_factorized(
    Objective &&objective, const std::vector<double> &x,
    const std::vector<double> &total_direction,
    const std::vector<int> &random_indices,
    const Eigen::MatrixXd &inverse_factor) {
  if (x.size() != total_direction.size())
    throw std::invalid_argument("dense Hdot direction has wrong length");
  const int nr = static_cast<int>(random_indices.size());
  if (inverse_factor.rows() != nr || inverse_factor.cols() != nr)
    throw std::invalid_argument("inverse factor has wrong dimensions");

  const double baseline =
      had::third_directional_derivative(objective, x, total_direction);
  double trace = 0.0;
  for (int column = 0; column < nr; ++column) {
    std::vector<double> plus = total_direction;
    std::vector<double> minus = total_direction;
    for (int row = 0; row < nr; ++row) {
      const double value = inverse_factor(row, column);
      const std::size_t index = static_cast<std::size_t>(random_indices[row]);
      plus[index] += value;
      minus[index] -= value;
    }
    const double plus_cubic =
        had::third_directional_derivative(objective, x, plus);
    const double minus_cubic =
        had::third_directional_derivative(objective, x, minus);
    trace += (plus_cubic + minus_cubic - 2.0 * baseline) / 6.0;
  }
  return trace;
}

} // namespace laplace
} // namespace quadra
