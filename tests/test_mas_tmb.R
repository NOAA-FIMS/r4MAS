library(r4MAS)
library(Rcpp)
library(TMB)

# Use examples from 'extdata' in 'inst' because it is loaded with R4MAS package
test_example_path <- system.file("extdata", package = "r4MAS")

# Find path of R4MAS dll
libs_path <- system.file("libs", package = "r4MAS")
dll_name <- paste("r4MAS", .Platform$dynlib.ext, sep = "")
dll_path <- file.path(libs_path, dll_name)

 # Load module
    r4mas <- Rcpp::Module("rmas", dyn.load(dll_path))
    test_path <- file.path(test_example_path, "externalom_example")
    load(file.path(test_path, "singlespecies.RData"))

    # General settings
    nyears <- om_input$nyr
    nseasons <- 1
    nages <- om_input$nages
    ages <- om_input$ages

    # Define area
    area1 <- new(r4mas$Area)
    area1$name <- "area1"



## Set-up OM (sigmaR = 0.4)
working_dir <- getwd()

maindir <- tempdir()
model_input <- ASSAMC::save_initial_input()
FIMS_C0_estimation <- ASSAMC::save_initial_input(
  base_case = TRUE,
  input_list = model_input,
  maindir = maindir,
  om_sim_num = 1,
  keep_sim_num = 1,
  figure_number = 1,
  seed_num = 9924,
  case_name = "FIMS_C0_estimation"
)

# generate om_input, om_output, and em_input
# using function from the model comparison project
ASSAMC::run_om(input_list = FIMS_C0_estimation)

on.exit(unlink(maindir, recursive = TRUE), add = TRUE)

setwd(working_dir)
on.exit(setwd(working_dir), add = TRUE)


    # Recruitment
    recruitment <- new(r4mas$BevertonHoltRecruitment)
    recruitment$R0$value <- om_input$R0 / 1000
    recruitment$R0$estimated <- TRUE
    recruitment$R0$phase <- 1
    recruitment$h$value <- om_input$h
    recruitment$h$estimated <- FALSE
    recruitment$h$phase <- 3
    recruitment$h$min <- 0.2001
    recruitment$h$max <- 1.0
    recruitment$sigma_r$value <- om_input$logR_sd
    recruitment$sigma_r$estimated <- FALSE
    recruitment$sigma_r$min <- 0
    recruitment$sigma_r$max <- 1.0
    recruitment$sigma_r$phase <- 2
    recruitment$estimate_deviations <- TRUE
    recruitment$constrained_deviations <- TRUE
    recruitment$deviations_min <- -15.0
    recruitment$deviations_max <- 15.0
    recruitment$deviation_phase <- 1
    recruitment$SetDeviations(om_input$logR.resid)
    recruitment$use_bias_correction <- FALSE

    # Growth
    growth <- new(r4mas$VonBertalanffyModified)
    empirical_weight <- rep(om_input$W.kg, times = om_input$nyr)
    survey_empirical_weight <- replicate(nages * nyears, 1.0)
    growth$SetUndifferentiatedCatchWeight(empirical_weight)
    growth$SetUndifferentiatedSurveyWeight(survey_empirical_weight)
    growth$SetUndifferentiatedWeightAtSeasonStart(empirical_weight)
    growth$SetUndifferentiatedWeightAtSpawning(empirical_weight)
    growth$a_min$value <- 0.01
    growth$a_max$value <- 10.0
    growth$c$value <- 0.3
    growth$lmin$value <- 5
    growth$lmax$value <- 50
    growth$alpha_f$value <- 0.000025
    growth$alpha_m$value <- 0.000025
    growth$beta_f$value <- 2.9624
    growth$beta_m$value <- 2.9624

    # Maturity
    maturity <- new(r4mas$Maturity)
    maturity$values <- om_input$mat.age

    # Natural Mortality
    natural_mortality <- new(r4mas$NaturalMortality)
    natural_mortality$SetValues(om_input$M.age)


    # Define Movement (only 1 area in this model)
    movement <- new(r4mas$Movement)
    movement$connectivity_females <- c(0.0)
    movement$connectivity_males <- c(0.0)
    movement$connectivity_recruits <- c(0.0)

    # Initial Deviations
    initial_deviations <- new(r4mas$InitialDeviations)
    initial_deviations$values <- rep(1.0, times = om_input$nages)
    initial_deviations$estimate <- TRUE
    #initial_deviations$phase <- 2





    population <- new(r4mas$Population)
    for (y in 1:(nyears))
    {
      population$AddMovement(movement$id, y)
    }

    population$AddNaturalMortality(natural_mortality$id, area1$id, "undifferentiated")
    population$AddMaturity(maturity$id, area1$id, "undifferentiated")
    population$AddRecruitment(recruitment$id, 1, area1$id)
    population$SetInitialDeviations(initial_deviations$id, area1$id, "undifferentiated")
    population$SetGrowth(growth$id)
    population$sex_ratio <- 0.5
    population$initial_f$value <- 0.0
    population$initial_f$estimated<- FALSE

    # Fishing Mortality
    fishing_mortality <- new(r4mas$FishingMortality)
    fishing_mortality$estimate <- TRUE
    fishing_mortality$phase <- 1
    fishing_mortality$min <- 0.0
    fishing_mortality$max <- 4
    fishing_mortality$SetValues(om_output$f)

    # Selectivity Model
    fleet_selectivity <- new(r4mas$LogisticSelectivity)
    fleet_selectivity$a50$value <- om_input$sel_fleet$fleet1$A50.sel
    fleet_selectivity$a50$estimated <- TRUE
    fleet_selectivity$a50$phase <- 2
    fleet_selectivity$a50$min <- 0.0
    fleet_selectivity$a50$max <- max(om_input$ages)

    fleet_selectivity$slope$value <- 1 / om_input$sel_fleet$fleet1$slope.sel
    fleet_selectivity$slope$estimated <- TRUE
    fleet_selectivity$slope$phase <- 2
    fleet_selectivity$slope$min <- 0
    fleet_selectivity$slope$max <- max(om_input$ages)

    survey_selectivity <- new(r4mas$LogisticSelectivity)
    survey_selectivity$a50$value <- om_input$sel_survey$survey1$A50.sel
    survey_selectivity$a50$estimated <- TRUE
    survey_selectivity$a50$phase <- 2
    survey_selectivity$a50$min <- 0.0
    survey_selectivity$a50$max <- max(om_input$ages)

    survey_selectivity$slope$value <- 1 / om_input$sel_survey$survey1$slope.sel
    survey_selectivity$slope$estimated <- TRUE
    survey_selectivity$slope$phase <- 2
    survey_selectivity$slope$min <- 0
    survey_selectivity$slope$max <- max(om_input$ages)

    # Index data
    catch_index <- new(r4mas$IndexData)
    catch_index$values <- em_input$L.obs$fleet1
    catch_index$error <- rep(em_input$cv.L$fleet1, times = om_input$nyr)

    survey_index <- new(r4mas$IndexData)
    survey_index$values <- em_input$survey.obs$survey1
    survey_index$error <- rep(em_input$cv.survey$survey1, times = om_input$nyr)


    # Age Comp Data
    catch_comp <- new(r4mas$AgeCompData)
    catch_comp$values <- as.vector(t(em_input$L.age.obs$fleet1))
    catch_comp$sample_size <- rep(em_input$n.L$fleet1, nyears * nseasons)

    survey_comp <- new(r4mas$AgeCompData)
    survey_comp$values <- as.vector(t(em_input$survey.age.obs$survey1))
    survey_comp$sample_size <- rep(em_input$n.survey$survey1, times = om_input$nyr)

    # NLL models
    fleet_index_comp_nll <- new(r4mas$Lognormal)
    fleet_index_comp_nll$use_bias_correction <- FALSE
    fleet_age_comp_nll <- new(r4mas$Multinomial)

    survey_index_comp_nll <- new(r4mas$Lognormal)
    survey_index_comp_nll$use_bias_correction <- FALSE
    survey_age_comp_nll <- new(r4mas$Multinomial)

    # Fleet
    fleet <- new(r4mas$Fleet)
    fleet$AddAgeCompData(catch_comp$id, "undifferentiated")
    fleet$AddIndexData(catch_index$id, "undifferentiated")
    fleet$SetAgeCompNllComponent(fleet_age_comp_nll$id)
    fleet$SetIndexNllComponent(fleet_index_comp_nll$id)
    fleet$AddSelectivity(fleet_selectivity$id, 1, area1$id)
    fleet$AddFishingMortality(fishing_mortality$id, 1, area1$id)


    # Survey
    survey <- new(r4mas$Survey)
    survey$AddAgeCompData(survey_comp$id, "undifferentiated")
    survey$AddIndexData(survey_index$id, "undifferentiated")
    survey$SetIndexNllComponent(survey_index_comp_nll$id)
    survey$SetAgeCompNllComponent(survey_age_comp_nll$id)
    survey$AddSelectivity(survey_selectivity$id, 1, area1$id)
    survey$q$value <- em_input$survey_q$survey1
    survey$q$min <- 0
    survey$q$max <- 10
    survey$q$estimated <- TRUE
    survey$q$phase <- 1

    # Build the MAS model
    mas_model <- new(r4mas$MASModel)
    mas_model$compute_variance_for_derived_quantities<-FALSE
    mas_model$nyears <- nyears
    mas_model$nseasons <- nseasons
    mas_model$nages <- nages
    mas_model$extended_plus_group <- max(om_input$ages)
    mas_model$ages <- ages
    mas_model$AddFleet(fleet$id)
    mas_model$catch_season_offset <- 0.0
    mas_model$spawning_season_offset <- 0.0
    mas_model$survey_season_offset <- 0.0

    #mas_model$AddSurvey(survey$id)
    mas_model$AddPopulation(population$id)

   mas_model$CreateTMBModel()


   mas_model$GetParameterValues()
   parameters <- list(p = mas_model$GetParameterValues())
   obj <- TMB::MakeADFun(data = list(), parameters, DLL = "r4MAS")

   #opt <- with(obj, optim(par, fn, gr,
    #  method = "BFGS",
     # control = list(maxit = 1000000, reltol = 1e-14)
    #))

 opt <- with(obj, nlminb(par, fn, gr,
      control = list(eval.max = 10000,iter.max = 10000, sing.tol = 1e-20,  rel.tol = 1e-15)
   ))
opt

print(obj$gr(opt$par))
