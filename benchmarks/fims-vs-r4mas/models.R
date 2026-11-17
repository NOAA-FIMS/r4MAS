# Matched, deliberately small catch-at-age baseline. All units are fish and
# metric tons in the common specification; the r4MAS adapter converts to 1000s
# of fish and kg at its boundary.
make_case <- function(years = 10L, ages = 6L, seed = 20261111L) {
  set.seed(seed + years + ages)
  a <- seq_len(ages)
  M <- rep(0.2, ages)
  weight_kg <- 0.12 * a^1.4
  fleet_sel <- plogis(a - ages / 2)
  survey_sel <- plogis(a - ages / 3)
  F <- 0.18 + 0.04 * sin(seq_len(years) / 3)
  R0 <- 1e6
  N0 <- R0 * exp(-c(0, cumsum(head(M + F[1] * fleet_sel, -1))))
  N0[ages] <- N0[ages] / (1 - exp(-M[ages] - F[1] * fleet_sel[ages]))
  N <- matrix(0, years, ages)
  N[1, ] <- N0
  catches <- vulnerable <- numeric(years)
  for (y in seq_len(years)) {
    Z <- M + F[y] * fleet_sel
    catches[y] <- sum(N[y, ] * F[y] * fleet_sel / Z * (-expm1(-Z)) * weight_kg / 1000)
    vulnerable[y] <- sum(N[y, ] * survey_sel * weight_kg / 1000)
    if (y < years) {
      N[y + 1, 1] <- R0
      N[y + 1, 2:ages] <- head(N[y, ] * exp(-Z), -1)
      N[y + 1, ages] <- N[y + 1, ages] + N[y, ages] * exp(-Z[ages])
    }
  }
  q <- 0.002
  cv <- 0.2
  sigma <- sqrt(log1p(cv^2))
  observations <- q * vulnerable * exp(rnorm(years, 0, sigma))
  # A finite-logit approximation to the h=1 constant-recruitment limit avoids
  # conflating recruitment/SSB parameterization differences in this first case.
  list(years = years, ages = a, M = M, weight_kg = weight_kg, F = F,
       R0 = R0, h = 1 - 1e-12, N0 = N0, numbers = N, catch = catches,
       fleet_sel_a50 = ages / 2, survey_sel_a50 = ages / 3,
       q = q, start_q = 0.7 * q, cv = cv, sigma = sigma,
       survey = observations, vulnerable = vulnerable,
       analytic_q = exp(mean(log(observations / vulnerable))),
       seed = seed + years + ages)
}

# Reproduce r4MAS's native initial-F bisection to encode the *same* specified
# N0 using its initial-deviation coordinates. These are fixed inputs, not fits.
mas_initial_deviations <- function(spec) {
  a <- length(spec$ages)
  equilibrium <- spec$R0 * exp(-c(0, cumsum(head(spec$M, -1))))
  equilibrium[a] <- equilibrium[a] / (-expm1(-tail(spec$M, 1)))
  selectivity <- plogis(spec$ages - spec$fleet_sel_a50)
  low <- 0
  high <- 10
  catch_at <- function(f) sum((equilibrium / 2000) * spec$weight_kg *
      selectivity * f * (-expm1(-spec$M - f)) / (spec$M + f))
  repeat {
    midpoint <- (low + high) / 2
    if (high - midpoint < 0.0001) break
    if ((catch_at(midpoint) - spec$catch[1]) * (catch_at(high) - spec$catch[1]) <= 0)
      low <- midpoint else high <- midpoint
  }
  log(spec$N0 / equilibrium) + spec$M + midpoint + 0.5
}

build_r4mas <- function(spec, estimate = TRUE, module = NULL) {
  if (is.null(module)) module <- Rcpp::Module("rmas", PACKAGE = "r4MAS")
  model <- new(module$MASModel)
  model$Reset()
  refs <- list()
  add <- function(name, object) { refs[[name]] <<- object; object }
  area <- add("area", new(module$Area))
  area$name <- "shared_area"
  rec <- add("recruitment", new(module$BevertonHoltRecruitment))
  rec$R0$value <- spec$R0 / 1000
  rec$h$value <- spec$h
  rec$sigma_r$value <- 0.2
  rec$use_bias_correction <- FALSE
  rec$estimate_deviations <- FALSE
  rec$constrained_deviations <- FALSE
  rec$SetDeviations(rep(0, spec$years))
  growth <- add("growth", new(module$VonBertalanffyModified))
  weights <- rep(spec$weight_kg, spec$years)
  growth$SetUndifferentiatedCatchWeight(weights)
  growth$SetUndifferentiatedSurveyWeight(weights)
  growth$SetUndifferentiatedWeightAtSeasonStart(weights)
  growth$SetUndifferentiatedWeightAtSpawning(weights)
  growth$a_min$value <- 0.01; growth$a_max$value <- max(spec$ages)
  growth$c$value <- 0.3; growth$lmin$value <- 5; growth$lmax$value <- 50
  growth$alpha_f$value <- 0.000025; growth$alpha_m$value <- 0.000025
  growth$beta_f$value <- 2.9624; growth$beta_m$value <- 2.9624
  maturity <- add("maturity", new(module$Maturity))
  maturity$values <- plogis(3 * (spec$ages - 2.25))
  mortality <- add("M", new(module$NaturalMortality))
  mortality$SetValues(spec$M)
  movement <- add("movement", new(module$Movement))
  movement$connectivity_females <- 0
  movement$connectivity_males <- 0
  movement$connectivity_recruits <- 0
  initial <- add("initial", new(module$InitialDeviations))
  initial$values <- mas_initial_deviations(spec)
  initial$estimate <- FALSE
  pop <- add("population", new(module$Population))
  for (y in seq_len(spec$years)) pop$AddMovement(movement$id, y)
  pop$AddNaturalMortality(mortality$id, area$id, "undifferentiated")
  pop$AddMaturity(maturity$id, area$id, "undifferentiated")
  pop$AddRecruitment(rec$id, 1L, area$id)
  pop$SetInitialDeviations(initial$id, area$id, "undifferentiated")
  pop$SetGrowth(growth$id)
  pop$sex_ratio <- 0.5
  fm <- add("F", new(module$FishingMortality))
  fm$estimate <- FALSE
  fm$SetValues(spec$F)
  fleet_sel <- add("fleet_selectivity", new(module$LogisticSelectivity))
  fleet_sel$a50$value <- spec$fleet_sel_a50
  fleet_sel$slope$value <- 1
  survey_sel <- add("survey_selectivity", new(module$LogisticSelectivity))
  survey_sel$a50$value <- spec$survey_sel_a50
  survey_sel$slope$value <- 1
  catch <- add("catch", new(module$IndexData))
  catch$values <- spec$catch; catch$error <- rep(spec$cv, spec$years)
  survey_data <- add("survey_data", new(module$IndexData))
  survey_data$values <- spec$survey; survey_data$error <- rep(spec$cv, spec$years)
  catch_nll <- add("catch_nll", new(module$Lognormal)); catch_nll$use_bias_correction <- FALSE
  survey_nll <- add("survey_nll", new(module$Lognormal)); survey_nll$use_bias_correction <- FALSE
  fleet <- add("fleet", new(module$Fleet))
  fleet$AddIndexData(catch$id, "undifferentiated")
  fleet$SetIndexNllComponent(catch_nll$id)
  fleet$AddSelectivity(fleet_sel$id, 1L, area$id)
  fleet$AddFishingMortality(fm$id, 1L, area$id)
  survey <- add("survey", new(module$Survey))
  survey$AddIndexData(survey_data$id, "undifferentiated")
  survey$SetIndexNllComponent(survey_nll$id)
  survey$AddSelectivity(survey_sel$id, 1L, area$id)
  survey$q$value <- spec$start_q
  survey$q$min <- 0
  survey$q$max <- 1
  survey$q$estimated <- estimate
  model$nyears <- as.integer(spec$years); model$nseasons <- 1L
  model$nages <- as.integer(length(spec$ages)); model$ages <- spec$ages
  model$extended_plus_group <- max(spec$ages)
  model$catch_season_offset <- 0; model$spawning_season_offset <- 0; model$survey_season_offset <- 0
  model$print_interval <- 0L; model$tolerance <- 1e-7; model$max_iterations <- 200L
  # TRUE keeps the pipeline in the native uncertainty path, without the
  # unrelated MSY grid search performed by the legacy FALSE branch.
  model$compute_variance_for_derived_quantities <- TRUE
  model$AddFleet(fleet$id); model$AddSurvey(survey$id); model$AddPopulation(pop$id)
  list(model = model, refs = refs)
}

fims_vector <- function(x, values, type = "constant") {
  x$resize(length(values))
  for (i in seq_along(values)) x[i]$value <- values[i]
  x$set_estimation_types(type)
  invisible(NULL)
}

build_fims <- function(spec, estimate = TRUE) {
  FIMS::clear()
  refs <- list()
  add <- function(name, object) { refs[[name]] <<- object; object }
  ny <- spec$years; na <- length(spec$ages)
  catch <- add("catch", new(FIMS::Catch, as.integer(ny)))
  catch$catch_data[] <- spec$catch
  index <- add("index", new(FIMS::Index, as.integer(ny)))
  index$index_data[] <- spec$survey
  fs <- add("fleet_selectivity", new(FIMS::LogisticSelectivity))
  fims_vector(fs$inflection_point, spec$fleet_sel_a50)
  fims_vector(fs$slope, 1)
  ss <- add("survey_selectivity", new(FIMS::LogisticSelectivity))
  fims_vector(ss$inflection_point, spec$survey_sel_a50)
  fims_vector(ss$slope, 1)
  fleet <- add("fleet", new(FIMS::Fleet))
  survey <- add("survey", new(FIMS::Fleet))
  for (obj in list(fleet, survey)) {
    obj$n_years$set(ny); obj$n_ages$set(na); obj$n_lengths$set(0L)
    obj$SetRequiresAgeLengthMapping(FALSE)
  }
  fims_vector(fleet$log_Fmort, log(spec$F))
  fims_vector(fleet$log_q, 0)
  fleet$SetSelectivityID(fs$get_id())
  fleet$SetObservedCatchDataID(catch$get_id())
  fims_vector(survey$log_Fmort, rep(-200, ny))
  fims_vector(survey$log_q, log(spec$start_q), if (estimate) "fixed_effects" else "constant")
  survey$SetSelectivityID(ss$get_id())
  survey$SetObservedIndexDataID(index$get_id())
  catch_dist <- add("catch_dist", new(FIMS::DlnormDistribution))
  fims_vector(catch_dist$log_sd, rep(log(spec$sigma), ny))
  catch_dist$set_observed_data(fleet$GetObservedCatchDataID())
  catch_dist$set_distribution_links("data", fleet$log_catch_expected$get_id())
  index_dist <- add("index_dist", new(FIMS::DlnormDistribution))
  fims_vector(index_dist$log_sd, rep(log(spec$sigma), ny))
  index_dist$set_observed_data(survey$GetObservedIndexDataID())
  index_dist$set_distribution_links("data", survey$log_index_expected$get_id())
  rec <- add("recruitment", new(FIMS::BevertonHoltRecruitment))
  process <- add("process", new(FIMS::LogDevsRecruitmentProcess))
  rec$SetRecruitmentProcessID(process$get_id())
  rec$n_years$set(ny)
  fims_vector(rec$log_rzero, log(spec$R0))
  fims_vector(rec$logit_steep, qlogis((spec$h - 0.2) / 0.8))
  fims_vector(rec$log_devs, rep(0, ny - 1L))
  growth <- add("growth", new(FIMS::EWAAGrowth))
  growth$n_years$set(ny)
  growth$ages[] <- spec$ages
  growth$weights[] <- spec$weight_kg / 1000
  maturity <- add("maturity", new(FIMS::LogisticMaturity))
  fims_vector(maturity$inflection_point, 2.25)
  fims_vector(maturity$slope, 3)
  pop <- add("population", new(FIMS::Population))
  pop$n_years$set(ny); pop$n_ages$set(na); pop$n_fleets$set(2L)
  pop$ages[] <- spec$ages
  fims_vector(pop$log_M, rep(log(spec$M), ny))
  fims_vector(pop$log_init_naa, log(spec$N0))
  pop$SetRecruitmentID(rec$get_id()); pop$SetGrowthID(growth$get_id()); pop$SetMaturityID(maturity$get_id())
  pop$AddFleet(fleet$get_id()); pop$AddFleet(survey$get_id())
  model <- add("model", new(FIMS::CatchAtAge))
  model$AddPopulation(pop$get_id())
  FIMS::CreateTMBModel()
  obj <- TMB::MakeADFun(data = list(), parameters = list(p = FIMS::get_fixed(), re = FIMS::get_random()),
                        DLL = "FIMS", silent = TRUE)
  list(obj = obj, refs = refs, model = model, survey = survey, population = pop)
}
