# Each invocation loads ONE modeling package: their Rcpp class names collide.
args <- commandArgs(TRUE)
stopifnot(length(args) == 5L)
spec <- readRDS(args[1]); engine <- args[2]; mode <- args[3]
result_path <- args[4]; library_path <- args[5]
if (nzchar(library_path)) .libPaths(c(library_path, .libPaths()))
script <- sub('^--file=', '', grep('^--file=', commandArgs(FALSE), value = TRUE)[1])
source(file.path(dirname(normalizePath(script)), 'models.R'))
setwd(dirname(normalizePath(result_path, mustWork = FALSE)))
stopifnot(engine %in% c('FIMS', 'r4MAS'), mode %in% c('fixed', 'fit'))
load_started <- as.numeric(Sys.time())
library(engine, character.only = TRUE)
invisible(loadNamespace('jsonlite')); invisible(loadNamespace('Rcpp'))
if (engine == 'FIMS') invisible(loadNamespace('TMB'))
module <- NULL
if (engine == 'r4MAS') {
  module <- Rcpp::Module('rmas', PACKAGE = 'r4MAS')
  # Match FIMS's .onLoad behavior: initialize the Rcpp class interfaces before
  # timing construction of an individual model.
  invisible(list(module$MASModel, module$Area, module$BevertonHoltRecruitment,
    module$VonBertalanffyModified, module$Maturity, module$NaturalMortality,
    module$Movement, module$InitialDeviations, module$Population,
    module$FishingMortality, module$LogisticSelectivity, module$IndexData,
    module$Lognormal, module$Fleet, module$Survey))
}
invisible(gc())
load_seconds <- as.numeric(Sys.time()) - load_started
now <- function() as.numeric(Sys.time())
started <- now()
if (engine == 'r4MAS') {
  context <- build_r4mas(spec, estimate = mode == 'fit', module = module)
  setup_done <- now()
  context$model$Run()
  run_done <- now()
  raw_json <- context$model$GetOutput()
  native <- jsonlite::fromJSON(raw_json, simplifyVector = FALSE)
  if (!identical(native$uncertainty$backend, 'Quadra'))
    stop('Install the current Quadra-enabled r4MAS before benchmarking.')
  pop <- native$population_dynamics$populations[[1]]$undifferentiated
  parameters <- native$estimated_parameters$parameters
  q <- if (mode == 'fit') parameters[[1]]$value else spec$start_q
  se <- if (mode == 'fit' && native$uncertainty$fixed_effects$success)
    native$uncertainty$fixed_effects$rows[[1]]$std_error else NA_real_
  components <- native$metrics$likelihood_components
  component_names <- vapply(components, function(x) x$name, character(1))
  component_values <- vapply(components, function(x) x$value, numeric(1))
  common_nll <- sum(component_values[!grepl('recruitment_likelihood|selectivity_likelihood', component_names)])
  value <- list(q = q, se = se,
    numbers = matrix(unlist(pop$numbers_at_age$values), nrow = spec$years, byrow = TRUE) * 1000,
    biomass = unlist(pop$biomass$values), ssb = unlist(pop$spawning_stock_biomass$values),
    catch = unlist(native$population_dynamics$fleets[[1]]$undifferentiated$catch_biomass$values),
    index = unlist(native$population_dynamics$surveys[[1]]$undifferentiated$survey_biomass$values),
    common_nll = common_nll, raw_nll = sum(component_values),
    native_gradient = if (mode == 'fit') parameters[[1]]$gradient_value else NA_real_,
    gradient_coordinate = 'q',
    converged = mode == 'fixed' || native$metrics$converged,
    status = native$metrics$optimizer_status,
    uncertainty_status = native$estimated_parameters$covariance_status)
  stages <- c(setup = setup_done - started,
    fit_uncertainty_native_export = run_done - setup_done,
    read_export = now() - run_done)
} else {
  context <- build_fims(spec, estimate = mode == 'fit')
  setup_done <- now()
  if (mode == 'fit') {
    opt <- stats::nlminb(context$obj$par, context$obj$fn, context$obj$gr,
      control = list(iter.max = 200L, eval.max = 1000L, rel.tol = 1e-12, x.tol = 1e-10))
    par <- opt$par
  } else {
    par <- context$obj$par
    opt <- list(convergence = 0L, message = 'fixed parameter evaluation')
  }
  fit_done <- now()
  q <- if (mode == 'fit') exp(unname(par[1])) else spec$start_q
  se <- NA_real_
  sd_success <- mode == 'fixed'
  if (mode == 'fit') {
    sdr <- TMB::sdreport(context$obj, par.fixed = par, getReportCovariance = FALSE)
    # FIMS estimates log(q); first-order transformation onto physical q.
    se <- q * summary(sdr, 'fixed')[1, 'Std. Error']
    sd_success <- is.finite(se) && isTRUE(sdr$pdHess)
  }
  sd_done <- now()
  report <- context$obj$report(par)
  FIMS::set_fixed(par)
  raw_json <- context$model$get_output()
  jsonlite::fromJSON(raw_json, simplifyVector = FALSE)
  ny <- spec$years; na <- length(spec$ages)
  normalizing_constant <- sum(log(spec$catch)) + sum(log(spec$survey)) + ny * log(2 * pi)
  value <- list(q = q, se = se,
    numbers = matrix(report$numbers_at_age[[1]], ncol = na, byrow = TRUE)[seq_len(ny), , drop = FALSE],
    biomass = head(report$biomass[[1]], ny), ssb = head(report$spawning_biomass[[1]], ny),
    catch = head(report$catch_expected[[1]], ny), index = head(report$index_expected[[2]], ny),
    common_nll = as.numeric(report$jnll) - normalizing_constant,
    raw_nll = as.numeric(report$jnll),
    native_gradient = if (mode == 'fit') as.numeric(context$obj$gr(par))[1] else NA_real_,
    gradient_coordinate = 'log(q)', converged = opt$convergence == 0L,
    status = opt$message, uncertainty_status = if (sd_success) 'TMB sdreport available' else 'TMB sdreport failed')
  stages <- c(setup_and_tape = setup_done - started, fit = fit_done - setup_done,
              uncertainty = sd_done - fit_done, native_export = now() - sd_done)
}
finished <- now()
value$engine <- engine
value$backend <- if (engine == 'FIMS') 'TMB' else 'Quadra'
value$mode <- mode
value$stages <- stages
value$total_seconds <- finished - started
value$load_seconds <- load_seconds
value$native_json_bytes <- nchar(raw_json, type = 'bytes')
value$package_version <- as.character(packageVersion(engine))
value$package_path <- find.package(engine)
value$dll_md5 <- unname(tools::md5sum(getLoadedDLLs()[[engine]][['path']]))
value$standardized_native_gradient <- abs(value$native_gradient) *
  if (engine == 'FIMS') (value$se / value$q) else value$se
saveRDS(value, result_path)
writeLines(raw_json, file.path(dirname(result_path), 'native-output.json'))
writeLines(capture.output(sessionInfo()), file.path(dirname(result_path), 'session-info.txt'))
