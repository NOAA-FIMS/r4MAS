
#source("asap3_s2.R")
library(Rcpp)
library(jsonlite)
library(callr)
#Load the RMAS package - assumes you are in the package working directory
devtools::load_all(".")
#load the r4mas module
r4mas <-
  Module("rmas", dyn.load(paste("src/RMAS", .Platform$dynlib.ext, sep = "")))

nyears<-30
nseasons<-1
nages<-12
ages<-c(0.1, seq(1,11))

#define area
area1<-new(r4mas$Area)
area1$name<-"area1"

input_data <- write_test_data(nyears = 30)

#Recruitment
recruitment<-new(r4mas$BevertonHoltRecruitment)

devs_list <- list(TRUE, TRUE, rep(0.0,22))
recruitment <- create_par_section(section_type = "recruitment", section_type_object = recruitment, par_names = c("R0","h","sigma_r","recdevs"),
                   par_lo = c(NA, 0.2001,NA,-15), par_hi = c(NA,1.0,NA,15), par_units = NA, par_phase = c(1,-2,-1,1), par_value = c(1000,0.75,0.55, NA), 
                   rec_devs=devs_list)


#Growth
growth<-new(r4mas$VonBertalanffyModified)
empirical_weight <- input_data$ewaa
growth$SetUndifferentiatedCatchWeight(empirical_weight)
growth$SetUndifferentiatedSurveyWeight(empirical_weight)
growth$SetUndifferentiatedWeightAtSeasonStart(empirical_weight)
growth$SetUndifferentiatedWeightAtSpawning(empirical_weight)
growth <- create_par_section(section_type="growth", section_type_object = growth, 
                             par_names = c("a_min","a_max","c","lmin","lmax","alpha_f","alpha_m","beta_f","beta_m"),
                             par_value=c(0.01,10.0,0.3,5,50,2.5E-5,2.5E-5,2.9624,2.9624))

#Maturity
maturity<-new(r4mas$Maturity)
maturity$values<-c(0.011488685,
                   0.16041065,
                   0.45232527,
                   0.497389935,
                   0.499869405,
                   0.499993495,
                   0.499999675,
                   0.499999985,
                   0.5,
                   0.5,
                   0.5,
                   0.5)

#Natural Mortality
natural_mortality<-new(r4mas$NaturalMortality)
natural_mortality$SetValues(rep(0.2,nages))
#need to add reset button to static variable so you can run again



#define Movement (only 1 area in this model)
movement<-new(r4mas$Movement)
movement$connectivity_females<-c(1.0)
movement$connectivity_males<-c(1.0)
movement$connectivity_recruits<-c(1.0)

#Initial Deviations
initial_deviations<-new(r4mas$InitialDeviations)
initial_deviations$values<-rep(0.0,nages)
initial_deviations$estimate<-FALSE
initial_deviations$phase<-1

population<-new(r4mas$Population)

for (y in 0:(nyears))
{
  population$AddMovement(movement$id, y)
}

population$AddNaturalMortality(natural_mortality$id,area1$id,"undifferentiated")
population$AddMaturity(maturity$id,area1$id, "undifferentiated")
population$AddRecruitment(recruitment$id,area1$id)
population$SetInitialDeviations(initial_deviations$id, area1$id, "undifferentiated")
population$SetGrowth(growth$id)
population$sex_ratio<-0.5

#Fishing Mortality
fishing_mortality<-new(r4mas$FishingMortality)
fishing_mortality$estimate<-TRUE
fishing_mortality$phase<-1
fishing_mortality$min<-1e-8
fishing_mortality$max<-30
fishing_mortality$SetValues(rep(0.3,30))

#Selectivity Model
fleet_selectivity<-new(r4mas$LogisticSelectivity)
fleet_selectivity <- create_par_section("selectivity",fleet_selectivity, par_names = c("a50","slope"),
                                        par_lo = c(0.0,0.0001), par_hi = c(10.0,5.0), par_phase = c(2,0.5),
                                        par_value = c(1.5,1.5))


survey_selectivity<-new(r4mas$LogisticSelectivity)
survey_selectivity <- create_par_section("selectivity",survey_selectivity, par_names = c("a50","slope"),
                                        par_lo = c(0.0,0.0001), par_hi = c(10.0,5.0), par_phase = c(2,2),
                                        par_value = c(1.0,0.3))


survey2_selectivity<-new(r4mas$AgeBasedSelectivity)
survey2_selectivity$estimated<-FALSE
survey2_selectivity$values<-c(1.0,rep(0.0,nages-1))

#Index data
catch_index<-new(r4mas$IndexData)
catch_index$values <- input_data$catch_index$values
catch_index$error <- input_data$catch_index$error

survey_index<-new(r4mas$IndexData)
survey_index$values <- input_data$survey_index$values
survey_index$error <- input_data$survey_index$error


#Age Comp Data
catch_comp<-new(r4mas$AgeCompData)
catch_comp$values <- input_data$catch_comp$values
catch_comp$sample_size <-input_data$catch_comp$sample_size



survey_comp<-new(r4mas$AgeCompData)
survey_comp$values <- input_data$survey_comp$values
survey_comp$sample_size <-input_data$survey_comp$sample_size


#NLL models
fleet_index_comp_nll<-survey_index_comp_nll <- survey2_index_comp_nll <- new(r4mas$Lognormal)
fleet_age_comp_nll<-survey_age_comp_nll <- new(r4mas$MultinomialRobust)


#Fleet
fleet<- make_fleet(r4mas, catch_comp, catch_index, fleet_selectivity, area1, fishing_mortality)

#Survey
survey<-new(r4mas$Survey)
survey$AddAgeCompData(survey_comp$id,"undifferentiated")
survey$AddIndexData(survey_index$id,"undifferentiated")
survey$SetIndexNllComponent(survey_index_comp_nll$id)
survey$SetAgeCompNllComponent(survey_age_comp_nll$id)
survey$AddSelectivity(survey_selectivity$id, 1, area1$id)
survey$q$value<-0.0001
survey$q$min<-0
survey$q$max<-10
survey$q$estimated<-TRUE
survey$q$phase<-1



#build the MAS model
mas_model<-new(r4mas$MASModel)
mas_model$nyears<-nyears
mas_model$nseasons<-nseasons
mas_model$nages<-nages
mas_model$extended_plus_group<-15
mas_model$ages<-ages
mas_model$AddFleet(fleet$id)
mas_model$catch_season_offset<-0.5
mas_model$spawning_season_offset<-0.5

mas_model$AddSurvey(survey$id)
mas_model$AddPopulation(population$id)


#########################################################
# Run the model
############################################################

mas_model$Run()
mas_model

write(mas_model$GetOutput(), file="mas_s2_output.json")


##############################################################
# Read in the output
#############################################################

args <- commandArgs(trailingOnly = TRUE)
args <- "./inst/extdata/mas_s2_output.json"
json_output <- jsonlite::read_json(args[1])
years <- json_output$nyears
ages <- json_output$nages
popdy <- json_output$population_dynamics
observed_catch_data <- popdy$fleet$observed
expected_catch_data <- popdy$fleet$undifferentiated
expected_surv_data <- popdy$survey$undifferentiated
observed_surv_data <- popdy$survey$observed


ind_surv <- names(expected_surv_data[which(names(expected_surv_data) %in% names(observed_surv_data))])
names(observed_catch_data) <- sub("fleet","catch", names(observed_catch_data))
ind_catch <- names(expected_catch_data[which(names(expected_catch_data) %in% names(observed_catch_data))])


make_mat <- function(data_list) {
  if(length(unlist(data_list))==years*ages){
    return(matrix(unlist(data_list), nrow=ages, ncol=years, byrow=FALSE))}
  else{
    return(unlist(data_list))
  }
}



 e_catch <- lapply((1:length(ind_catch)), function(x) make_mat(expected_catch_data[[ind_catch[x]]]$values))
# o_catch <- lapply((1:length(ind_catch)), function(x) make_mat(observed_catch_data[[ind_catch[x]]]$values))
# 
# e_surv <- lapply((1:length(ind_surv)), 
#                  function(x) make_mat(expected_surv_data[[ind_surv[x]]]$values))
# o_surv <- lapply((1:length(ind_surv)), 
#                  function(x) make_mat(observed_surv_data[[ind_surv[x]]]$values))
# 
# 
