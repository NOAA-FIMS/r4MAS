# Quadra source snapshot

Source: https://github.com/NOAA-FIMS/Quadra.git
Commit: c6394b70cff8b233c976f57f29e2f1ec5fab0b35

Vendored headers for offline R package builds, including upstream Eigen and
LBFGSpp dependencies. License notices are retained in the headers and dependency
license files. Only headers and dependency license files are included.

Local patch: removed duplicate ThirdOrderScalar comparison overloads from
core/had_quadra.hpp; upstream defined the same six operators twice.

Local reporting patch: preserve covariance validation failure messages in
`estimate_fixed_effect_covariance_from_gradient`; upstream overwrote them with
a success message even when Hessian validation failed. Numerical behavior and
success flags are unchanged.
