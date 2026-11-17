relative_error <- function(a, b) max(abs(a - b) / pmax(abs(b), 1e-12))
accuracy <- function(result, spec) {
  q <- result$q
  ref_biomass <- as.numeric(spec$numbers %*% (spec$weight_kg / 1000))
  ref_ssb <- as.numeric(spec$numbers %*% (spec$weight_kg / 1000 * plogis(3 * (spec$ages - 2.25)) * 0.5))
  # Diagnostic, NOT a change to model output: r4MAS omits age-1 recruits in
  # subsequent-year SSB. The raw discrepancy is retained in the accuracy table.
  juvenile <- c(0, rep(spec$R0 * spec$weight_kg[1] / 1000 * plogis(3 * (spec$ages[1] - 2.25)) * 0.5, spec$years - 1))
  adjusted_ssb <- result$ssb + if (result$engine == 'r4MAS') juvenile else 0
  sigma <- spec$sigma
  residuals <- log(spec$survey / (q * spec$vulnerable))
  analytic_nll <- 2 * spec$years * log(sigma) + sum(residuals^2) / (2 * sigma^2)
  estimate_error <- if (result$mode == 'fit') relative_error(q, spec$analytic_q) else relative_error(q, spec$start_q)
  se_error <- if (result$mode == 'fit') relative_error(result$se, q * sigma / sqrt(spec$years)) else NA_real_
  row <- data.frame(case = result$case, engine = result$engine, mode = result$mode,
    replicate = result$replicate, q = q, q_relative_error = estimate_error,
    se_q = result$se, se_relative_error = se_error,
    numbers_relative_error = relative_error(result$numbers, spec$numbers),
    biomass_relative_error = relative_error(result$biomass, ref_biomass),
    catch_max_abs_error_mt = max(abs(result$catch - spec$catch)),
    index_relative_error = relative_error(result$index, q * spec$vulnerable),
    ssb_raw_relative_error = relative_error(result$ssb, ref_ssb),
    ssb_convention_adjusted_relative_error = relative_error(adjusted_ssb, ref_ssb),
    common_nll_abs_error = abs(result$common_nll - analytic_nll),
    standardized_native_gradient = result$standardized_native_gradient,
    native_converged = result$converged)
  row$passed <- isTRUE(row$q_relative_error < 1e-7 &&
    row$numbers_relative_error < 1e-8 && row$biomass_relative_error < 1e-8 &&
    row$catch_max_abs_error_mt < 1.01e-5 && row$index_relative_error < 1e-8 &&
    row$ssb_convention_adjusted_relative_error < 1e-8 && row$common_nll_abs_error < 1e-5 &&
    (result$mode == 'fixed' || (is.finite(se_error) && se_error < 1e-5 && result$converged)))
  row
}

finalize_results <- function(results, checks, cases, output, source_root, limited=data.frame()) {
  if (!length(results)) {
    write.csv(if(length(checks)) do.call(rbind,checks) else data.frame(),file.path(output,'accuracy.csv'),row.names=FALSE)
    write.csv(limited,file.path(output,'resource-limits.csv'),row.names=FALSE)
    writeLines(c('# Benchmark resource limit', '',
      'No complete paired fit comparison was obtained within the worker budget.',
      'See resource-limits.csv and workers/*/stderr.log. No speed ranking is reported.'),
      file.path(output,'REPORT.md'))
    return(invisible(NULL))
  }
  write.csv(limited,file.path(output,'resource-limits.csv'),row.names=FALSE)
  validation <- do.call(rbind, checks)
  write.csv(validation, file.path(output, 'accuracy.csv'), row.names = FALSE)
  if (!all(validation$passed)) stop('Accuracy gate failed. Inspect accuracy.csv; no speed ranking produced.')
  raw <- do.call(rbind, lapply(results, function(x) data.frame(case = x$case, engine = x$engine,
    backend = x$backend, replicate = x$replicate, total_seconds = x$total_seconds,
    load_seconds = x$load_seconds, process_seconds = x$process_seconds,
    peak_process_rss_mib = x$peak_process_rss_mib, q = x$q, se_q = x$se,
    normalized_nll = x$common_nll, converged = x$converged,
    standardized_native_gradient = x$standardized_native_gradient,
    package_version = x$package_version, package_path = x$package_path, dll_md5 = x$dll_md5,
    native_json_bytes = x$native_json_bytes)))
  raw$included_in_summary <- raw$case %in% cases$case
  write.csv(raw, file.path(output, 'raw-results.csv'), row.names = FALSE)
  summary_raw <- raw[raw$included_in_summary, , drop=FALSE]
  stages <- do.call(rbind, lapply(results, function(x) data.frame(case = x$case,
    engine = x$engine, replicate = x$replicate, stage = names(x$stages), seconds = unname(x$stages))))
  write.csv(stages, file.path(output, 'stage-timings.csv'), row.names = FALSE)
  summary <- if (!nrow(summary_raw)) data.frame() else do.call(rbind, lapply(split(summary_raw, interaction(summary_raw$case, summary_raw$engine, drop = TRUE)), function(x)
    data.frame(case = x$case[1], engine = x$engine[1], repeats = nrow(x),
      median_ms = median(x$total_seconds) * 1000,
      q25_ms = unname(quantile(x$total_seconds, .25)) * 1000,
      q75_ms = unname(quantile(x$total_seconds, .75)) * 1000,
      median_process_ms = median(x$process_seconds) * 1000,
      median_peak_process_rss_mib = median(x$peak_process_rss_mib, na.rm = TRUE))))
  for (engine in unique(raw$engine))
    if (length(unique(raw$dll_md5[raw$engine==engine]))!=1L)
      stop('More than one binary was used for ',engine,'. No speed summary produced.')
  if (nrow(summary)) summary <- summary[order(match(summary$case, cases$case), summary$engine), ]
  write.csv(summary, file.path(output, 'summary.csv'), row.names = FALSE)
  if (!nrow(cases)) unlink(file.path(output,'benchmark.png'))
  source(file.path(source_root, 'report.R'))
  write_benchmark_report(output, cases, summary, validation, raw, source_root, limited)
}
