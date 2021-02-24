# remotes::install_github("Bai-Li-NOAA/RMAS")
library(RMAS)
library(Rcpp)
library(jsonlite)
r4mas <- Rcpp::Module("rmas", PACKAGE = "RMAS")

# Run two tests -------------------------------------------------------------------------------
for (i in 1:2){
  # r4mas <- Rcpp::Module("rmas", PACKAGE = "RMAS")
  if (i==1) om_data_path <- system.file("extdata",
                                        "externalom_example",
                                        "singlespecies.RData",
                                        package = "RMAS") # test 1: OM without bias correction

  if (i==2) om_data_path <- system.file("extdata",
                                        "externalom_example",
                                        "singlespecies_biascorrection.RData",
                                        package = "RMAS") # test 2: OM with bias correction

  load(om_data_path)

  # General settings
  nyears=om_input$nyr
  nseasons=1
  nages=om_input$nages
  ages = om_input$ages

  #Define area
  area1=new(r4mas$Area)
  area1$name="area1"

  #Recruitment
  recruitment=new(r4mas$BevertonHoltRecruitment)
  recruitment$R0$value=om_input$R0/1000
  recruitment$R0$estimated=TRUE
  recruitment$R0$phase=1
  recruitment$h$value=om_input$h
  recruitment$h$estimated=FALSE
  recruitment$h$phase=3
  recruitment$h$min=0.02001
  recruitment$h$max=1.0
  recruitment$sigma_r$value = exp(om_input$logR_sd)
  recruitment$sigma_r$estimated=TRUE
  recruitment$sigma_r$min=0
  recruitment$sigma_r$max=1.0
  recruitment$sigma_r$phase=2
  recruitment$estimate_deviations=TRUE
  recruitment$constrained_deviations=FALSE
  recruitment$deviations_min=-15.0
  recruitment$deviations_max=15.0
  recruitment$deviation_phase=1
  recruitment$SetDeviations(rep(0.0, times=om_input$nyr))

  #Growth
  growth=new(r4mas$VonBertalanffyModified)
  empirical_weight=rep(om_input$W.kg, times=om_input$nyr)
  survey_empirical_weight=replicate(nages*nyears, 1.0)
  growth$SetUndifferentiatedCatchWeight(empirical_weight)
  growth$SetUndifferentiatedSurveyWeight(survey_empirical_weight)
  growth$SetUndifferentiatedWeightAtSeasonStart(empirical_weight)
  growth$SetUndifferentiatedWeightAtSpawning(empirical_weight)
  growth$a_min$value=0.01
  growth$a_max$value=10.0
  growth$c$value=0.3
  growth$lmin$value=5
  growth$lmax$value=50
  growth$alpha_f$value=0.000025
  growth$alpha_m$value=0.000025
  growth$beta_f$value=2.9624
  growth$beta_m$value=2.9624

  #Maturity
  maturity=new(r4mas$Maturity)
  maturity$values = om_input$mat.age*0.5

  #Natural Mortality
  natural_mortality=new(r4mas$NaturalMortality)
  natural_mortality$SetValues(om_input$M.age)

  #define Movement (only 1 area in this model)
  movement=new(r4mas$Movement)
  movement$connectivity_females=c(1.0)
  movement$connectivity_males=c(1.0)
  movement$connectivity_recruits=c(1.0)

  #Initial Deviations
  initial_deviations=new(r4mas$InitialDeviations)
  initial_deviations$values=rep(0, times=om_input$nages)
  initial_deviations$estimate=FALSE
  initial_deviations$phase=1

  population=new(r4mas$Population)
  for (y in 0:(nyears))
  {
    population$AddMovement(movement$id, y)
  }

  population$AddNaturalMortality(natural_mortality$id,area1$id,"undifferentiated")
  population$AddMaturity(maturity$id,area1$id, "undifferentiated")
  population$AddRecruitment(recruitment$id, 1, area1$id)
  population$SetInitialDeviations(initial_deviations$id, area1$id, "undifferentiated")
  population$SetGrowth(growth$id)
  population$sex_ratio=0.5

  #Fishing Mortality
  fishing_mortality=new(r4mas$FishingMortality)
  fishing_mortality$estimate=TRUE
  fishing_mortality$phase=1
  fishing_mortality$min=0.0
  fishing_mortality$max=4
  fishing_mortality$SetValues(om_output$f)

  #Selectivity Model
  fleet_selectivity=new(r4mas$LogisticSelectivity)
  fleet_selectivity$a50$value=om_input$sel_fleet$fleet1$A50.sel
  fleet_selectivity$a50$estimated=TRUE
  fleet_selectivity$a50$phase=2
  fleet_selectivity$a50$min=0.0
  fleet_selectivity$a50$max=max(om_input$ages)

  fleet_selectivity$slope$value=1/om_input$sel_fleet$fleet1$slope.sel
  fleet_selectivity$slope$estimated=TRUE
  fleet_selectivity$slope$phase=2
  fleet_selectivity$slope$min=0
  fleet_selectivity$slope$max=max(om_input$ages)

  survey_selectivity=new(r4mas$LogisticSelectivity)
  survey_selectivity$a50$value=om_input$sel_survey$survey1$A50.sel
  survey_selectivity$a50$estimated=TRUE
  survey_selectivity$a50$phase=2
  survey_selectivity$a50$min=0.0
  survey_selectivity$a50$max=max(om_input$ages)

  survey_selectivity$slope$value=1/om_input$sel_survey$survey1$slope.sel
  survey_selectivity$slope$estimated=TRUE
  survey_selectivity$slope$phase=2
  survey_selectivity$slope$min=0
  survey_selectivity$slope$max=max(om_input$ages)

  #Index data
  catch_index=new(r4mas$IndexData)
  catch_index$values=em_input$L.obs$fleet1
  #dat.input$cv.L
  catch_index$error=rep(em_input$cv.L$fleet1, times=om_input$nyr)

  survey_index=new(r4mas$IndexData)
  survey_index$values=em_input$survey.obs$survey1
  survey_index$error=rep(em_input$cv.survey$survey1, times=om_input$nyr)


  #Age Comp Data
  catch_comp=new(r4mas$AgeCompData)
  catch_comp$values=as.vector(t(em_input$L.age.obs$fleet1))
  catch_comp$sample_size =rep(em_input$n.L$fleet1, nyears*nseasons)

  survey_comp=new(r4mas$AgeCompData)
  survey_comp$values=as.vector(t(em_input$survey.age.obs$survey1))
  survey_comp$sample_size=rep(em_input$n.survey$survey1, times=om_input$nyr)

  #NLL models
  fleet_index_comp_nll=new(r4mas$Lognormal)
  fleet_age_comp_nll=new(r4mas$Multinomial)

  survey_index_comp_nll=new(r4mas$Lognormal)
  survey_age_comp_nll=new(r4mas$Multinomial)

  #Fleet
  fleet=new(r4mas$Fleet)
  fleet$AddAgeCompData(catch_comp$id, "undifferentiated")
  fleet$AddIndexData(catch_index$id, "undifferentiated")
  fleet$SetAgeCompNllComponent(fleet_age_comp_nll$id)
  fleet$SetIndexNllComponent(fleet_index_comp_nll$id)
  fleet$AddSelectivity(fleet_selectivity$id, 1, area1$id)
  fleet$AddFishingMortality(fishing_mortality$id, 1,area1$id)


  #Survey
  survey=new(r4mas$Survey)
  survey$AddAgeCompData(survey_comp$id,"undifferentiated")
  survey$AddIndexData(survey_index$id,"undifferentiated")
  survey$SetIndexNllComponent(survey_index_comp_nll$id)
  survey$SetAgeCompNllComponent(survey_age_comp_nll$id)
  survey$AddSelectivity(survey_selectivity$id, 1, area1$id)
  survey$q$value=em_input$survey_q$survey1
  survey$q$min=0
  survey$q$max=10
  survey$q$estimated=TRUE
  survey$q$phase=1

  #build the MAS model
  mas_model=new(r4mas$MASModel)
  mas_model$nyears=nyears
  mas_model$nseasons=nseasons
  mas_model$nages=nages
  mas_model$extended_plus_group=max(om_input$ages)
  mas_model$ages=ages
  mas_model$AddFleet(fleet$id)
  mas_model$catch_season_offset=0.0
  mas_model$spawning_season_offset=0.0
  mas_model$survey_season_offset=0.0

  mas_model$AddSurvey(survey$id)
  mas_model$AddPopulation(population$id)
  mas_model$Run()

  output_file <- paste("test", i, "output.json", sep="")
  write(mas_model$GetOutput(),
        file=file.path(getwd(), output_file))
  mas_model$Reset()
}

# Aggregate output data -------------------------------------------------------------------------
om <- list()
mas <- list()
for (i in 1:2) {

  ## Aggregate OM output
  if (i==1) om_data_path <- system.file("extdata",
                                        "externalom_example",
                                        "singlespecies.RData",
                                        package = "RMAS") # test 1: OM without bias correction

  if (i==2) om_data_path <- system.file("extdata",
                                        "externalom_example",
                                        "singlespecies_biascorrect.RData",
                                        package = "RMAS") # test 2: OM with bias correction

  load(om_data_path)
  om[[i]] <- list(
    "biomass" = om_output$biomass.mt,
    "abundance" = om_output$abundance/1000,
    "ssb" = om_output$SSB,
    "recruit" = om_output$N.age[,1]/1000,
    "Ftot" = apply(om_output$FAA, 1, max),
    "landing" = om_output$L.mt$fleet1,
    "survey" = om_output$survey_index$survey1,
    "msy" = om_output$msy$msy,
    "fmsy" = round(om_output$msy$Fmsy, digits = 3),
    "ssbmsy" = om_output$msy$SSBmsy,
    "fratio" = apply(om_output$FAA, 1, max)/round(om_output$msy$Fmsy, digits = 3),
    "ssbratio" = om_output$SSB/om_output$msy$SSBmsy,
    "agecomp" = apply(om_output$N.age/1000, 1, function(x) x/sum(x))
    # "geomR0" = om_input$median_R0/1000,
    # "arimR0" = om_input$mean_R0/1000,
    # "geomS0" = om_input$median_R0*om_input$Phi.0,
    # "arimS0" = om_input$mean_R0*om_input$Phi.0,
    # "q" = em_input$survey_q$survey1
  )

  ## Aggregate MAS output
  args <- commandArgs(trailingOnly = TRUE)
  args <- paste("test", i, "output.json", sep="")
  json_output <- jsonlite::read_json(file.path(getwd(), args[1]))
  popdy<-json_output$population_dynamics
  pop <- popdy$populations[[1]]
  flt<-popdy$fleets[[1]]
  srvy<-popdy$surveys[[1]]

  mas[[i]] <- list(
    "biomass" = unlist(pop$undifferentiated$biomass$values),
    "abundance" = unlist(pop$undifferentiated$abundance$values),
    "ssb" = unlist(pop$undifferentiated$spawning_stock_biomass$values),
    "recruit" = unlist(pop$undifferentiated$recruits$values),
    "Ftot" = unlist(pop$undifferentiated$fishing_mortality$values),
    "landing" = unlist(flt$undifferentiated$catch_biomass$values),
    "survey" = unlist(srvy$undifferentiated$survey_biomass$values),
    "msy" = pop$MSY$B_msy, # Include MSY in MAS output
    "fmsy" = round(pop$MSY$F_msy, digits = 3),
    "ssbmsy" = pop$females$MSY$SSB_msy, #pop$MSY$SSB_msy is the sum of female and male ssbmsy
    "fratio" = unlist(pop$undifferentiated$fishing_mortality$values)/round(pop$MSY$F_msy, digits = 3),
    "ssbratio" = unlist(pop$undifferentiated$spawning_stock_biomass$values)/pop$females$MSY$SSB_msy,
    "agecomp" = apply(matrix(unlist(pop$undifferentiated$numbers_at_age$values), nrow=popdy$nyears, ncol=popdy$nages, byrow = T), 1, function(x) x/sum(x))

  )
}
# Compare output data -------------------------------------------------------------------------

var_id <- c(1:7, 11:12)
for (i in var_id){
    figure_data <- cbind(om[[1]][[i]], mas[[1]][[i]],
                         om[[2]][[i]], mas[[2]][[i]])

    ylim=range(figure_data)

    par(mfrow=c(1,2))
    plot(om[[1]][[i]], xlab="Year", ylab=names(mas[[1]])[i],
         ylim=ylim)
    lines(mas[[1]][[i]], col="blue")
    legend("topright", "test1: no bias correction", bty="n")

    plot(om[[2]][[i]], xlab="Year", ylab=names(mas[[2]])[i],
         ylim=ylim)
    lines(mas[[2]][[i]], col="blue")
    legend("topright", "test2: bias correction", bty="n")
}


