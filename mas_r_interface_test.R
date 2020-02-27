setwd("/Users/mattadmin/NetBeansProjects/R4MAS/dist/Debug/GNU-MacOSX")
#source("mas_r_interface_test.R")
library(Rcpp)

#load the r4mas module
r4mas <-
  Module("rmas", dyn.load(paste("R4MAS", .Platform$dynlib.ext, sep = "")))

nyears<-20
nseasons<-1
nages<-10
ages<-c(1.0,
        2.0,
        3.0,
        4.0,
        5.0,
        6.0,
        7.0,
        8.0,
        9.0,
        10.0)

#define area
area1<-new(r4mas$Area)
area1$name<-"area1"

#Recruitment
recruitment<-new(r4mas$BevertonHoltRecruitment)
recruitment$R0$value<-20000.0
recruitment$R0$estimated<-TRUE
recruitment$R0$phase<-1
recruitment$R0$min<-1000.0
recruitment$R0$max<-50000.0
recruitment$h$value<-0.7
recruitment$h$estimated<-FALSE
recruitment$h$phase<-2
recruitment$h$min<-0.2
recruitment$h$max<-1.0
recruitment$sigma_r$value<-0.005545130293761911
recruitment$sigma_r$estimated<-FALSE
recruitment$sigma_r$phase<-3
recruitment$estimate_deviations<-FALSE
recruitment$constrained_deviations<-FALSE
recruitment$deviation_phase<-1
recruitment$SetDeviations(c(0.1,
                            0.1,
                            0.1,
                            0.1,
                            0.1,
                            0.1,
                            0.1,
                            0.1,
                            0.1,
                            0.1,
                            0.1,
                            0.1,
                            0.1,
                            0.1,
                            0.1,
                            0.1,
                            0.1,
                            0.1,
                            0.1,
                            0.1))

# 
# #Growth
# growth<-new(r4mas$VonBertalanffyModified)
# empirical_weight<- c(0.134,0.686,1.639,2.719,3.649,4.901,6.329,6.591,7.238,7.491,
#                      0.132,0.724,1.534,2.829,4.092,4.853,5.705,6.143,7.179,8.089,
#                      0.122,0.641,1.533,2.641,3.796,5.054,5.652,6.356,6.95, 8.857,
#                      0.137,0.722,1.606,2.416,3.629,5.027,5.561,6.35, 6.933,7.217,
#                      0.138,0.773,1.645,2.74, 3.711,4.506,5.684,6.929,7.424,7.479,
#                      0.135,0.689,1.683,2.571,3.514,4.777,5.416,6.614,6.998,7.387,
#                      0.148,0.691,1.521,2.506,3.474,4.734,5.422,6.158,7.245,8.554,
#                      0.149,0.753,1.541,2.615,3.697,4.425,5.807,6.27, 7.403,8.172,
#                      0.147,0.725,1.606,2.623,3.73, 4.793,5.9,  6.136,7.203,7.853,
#                      0.157,0.662,1.533,2.803,3.737,4.885,6.338,6.363,8.116,7.306,
#                      0.142,0.733,1.595,2.74, 3.498,4.916,5.604,6.057,7.077,8.432,
#                      0.135,0.707,1.428,2.625,3.639,4.473,5.901,6.982,7.878,7.53, 
#                      0.139,0.733,1.527,2.601,3.702,4.604,6.051,6.455,6.712,7.425,
#                      0.13, 0.72, 1.601,2.884,3.516,4.864,5.531,6.46, 7.331,7.432,
#                      0.143,0.665,1.514,2.706,3.816,4.772,5.364,6.256,7.256,7.534,
#                      0.142,0.654,1.54, 2.757,4.214,5.232,5.652,6.758,6.734,8.103,
#                      0.15, 0.665,1.446,2.751,3.684,4.494,5.346,6.645,7.144,7.219,
#                      0.138,0.672,1.525,2.549,3.266,4.618,5.817,6.341,7.666,8.191,
#                      0.141,0.686,1.391,2.876,3.504,4.855,5.6,  6.252,7.764,7.615,
#                      0.141,0.708,1.474,2.881,3.693,4.844,5.593,6.146,7.654,7.759)
# 
# growth$SetUndifferentiatedCatchWeight(empirical_weight)
# growth$SetUndifferentiatedSurveyWeight(empirical_weight)
# growth$SetUndifferentiatedWeightAtSeasonStart(empirical_weight)
# growth$SetUndifferentiatedWeightAtSpawning(empirical_weight)
# growth$a_min$value<-0.1
# growth$a_max$value<-10.0
# growth$c$value<-0.3
# 
# growth$alpha_f$value<-0.000025
# growth$alpha_m$value<-0.000025
# growth$beta_f$value<-2.9624
# growth$beta_m$value<-2.9624
# 
# #Maturity
# maturity<-new(r4mas$Maturity)
# maturity$values<-c( 0.018000000000000003,
#                     0.11920000000000002,
#                     0.5,
#                     0.8808000000000004,
#                     0.9819999999999997,
#                     0.9975000000000002,
#                     0.9997000000000004,
#                     1.0,
#                     1.0,
#                     1.0)
# 
# #Natural Mortality
# natural_mortality<-new(r4mas$NaturalMortality)
# natural_mortality$SetValues(c(0.3,
#                               0.3,
#                               0.3,
#                               0.3,
#                               0.3,
#                               0.3,
#                               0.3,
#                               0.3,
#                               0.3,
#                               0.3))
# 
# 
# 
# #define Movement (only 1 area in this model)
# movement<-new(r4mas$Movement)
# movement$connectivity_females<-c(1.0)
# movement$connectivity_males<-c(1.0)
# movement$connectivity_recruits<-c(1.0)
# 
# #Initial Deviations
# initial_deviations<-new(r4mas$InitialDeviations)
# initial_deviations$values<-c(0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0)
# initial_deviations$estimate<-FALSE
# initial_deviations$phase<-1
# 
# population<-new(r4mas$Population)
# population$AddMaturity(maturity$id, area1$id,"undifferentiated")
# 
# for (y in 0:(nyears))
# {
#  population$AddMovement(movement$id,as.integer(y))
# }
# 
# population$AddNaturalMortality(natural_mortality$id,area1$id,"undifferentiated")
# population$AddMaturity(maturity$id,area1$id, "undifferentiated")
# population$AddRecruitment(recruitment$id,area1$id)
# population$SetInitialDeviations(initial_deviations$id, area1$id, "undifferentiated")
# population$SetGrowth(growth$id)
# 
# 
# #Fishing Mortality
# fishing_mortality<-new(r4mas$FishingMortality)
# fishing_mortality$estimate<-FALSE
# fishing_mortality$phase<-1
# fishing_mortality$SetValues(c(
#   0.3,
#   0.3,
#   0.3,
#   0.3,
#   0.3,
#   0.3,
#   0.3,
#   0.3,
#   0.3,
#   0.3,
#   0.3,
#   0.3,
#   0.3,
#   0.3,
#   0.3,
#   0.3,
#   0.3,
#   0.3,
#   0.3,
#   0.3
# ))
# 
# #Selectivity Model
# selectivity<-new(r4mas$LogisticSelectivity)
# selectivity$a50$value<-3.0
# selectivity$a50$estimated<-FALSE
# selectivity$a50$phase<-2
# selectivity$a50$min<-0.0
# selectivity$a50$max<-10.0
# selectivity$slope$value<-1.0
# selectivity$slope$estimated<-FALSE
# selectivity$slope$phase<-2
# selectivity$slope$min<-0.0001
# selectivity$slope<-5.0
# print(selectivity$a50$estimated)
# #index data
# catch_index<-new(r4mas$IndexData)
# catch_index$values<-c(82.2,413,543.6,545.4,344.5,469.2,1019.4,705.2,925.4,
#                      894.7,893.3,645.5,431.5,95.2,146.4,103,99,45.6,157.5,119)
# catch_index$error<-c(0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.01,
#                     0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.01, 0.01)
# print(length(catch_index$values))
# survey_index<-new(r4mas$IndexData)
# survey_index$values<-c(728,522,804,454,456,530,218,367,366,176,180,154,
#                        109,145,140,160,142,215,156,198)
# survey_index$error<-c(0.3,0.3,0.3,0.3,0.3,0.3,0.3,0.3,0.3,0.3,
#                       0.3,0.3,0.3,0.3,0.3,0.3,0.3,0.3,0.3)
# #Age Comp Data
# catch_comp<-new(r4mas$AgeCompData)
# catch_comp$values<-c(0.04,0.165,0.095,0.23,0.16,0.1,0.03,0.095,0.03,0.055,
#                      0.02,0.09,0.16,0.1,0.185,0.18,0.08,0.01,0.09,0.085,
#                      0.05,0.105,0.145,0.18,0.09,0.13,0.1,0.085,0.005,0.11,
#                      0.04,0.08,0.13,0.135,0.17,0.07,0.095,0.09,0.085,0.105,
#                      0.02,0.1,0.065,0.185,0.14,0.17,0.05,0.075,0.06,0.135,
#                      0.37,0.025,0.095,0.065,0.115,0.07,0.05,0.035,0.035,0.14,
#                      0.065,0.555,0.045,0.095,0.045,0.045,0.06,0.015,0.02,0.055,
#                      0.055,0.12,0.53,0.03,0.09,0.025,0.025,0.015,0.04,0.07,
#                      0.095,0.08,0.125,0.5,0.025,0.02,0.02,0.035,0.02,0.08,
#                      0.025,0.225,0.105,0.155,0.36,0.005,0.04,0.01,0.01,0.065,
#                      0.2,0.025,0.26,0.08,0.11,0.235,0.02,0.005,0.01,0.055,
#                      0.11,0.38,0.055,0.185,0.06,0.02,0.165,0.015,0.005,0.005,
#                      0.135,0.175,0.445,0.02,0.095,0.03,0.025,0.07,0,0.005,
#                      0.095,0.17,0.255,0.285,0.03,0.09,0.02,0,0.03,0.025,
#                      0.105,0.155,0.2,0.25,0.225,0.01,0.025,0.01,0.005,0.015,
#                      0.045,0.175,0.195,0.21,0.15,0.175,0.005,0.02,0.005,0.02,
#                      0.22,0.045,0.215,0.21,0.115,0.1,0.07,0,0.015,0.01,
#                      0.095,0.235,0.08,0.22,0.125,0.08,0.1,0.05,0,0.015,
#                      0.06,0.085,0.37,0.11,0.185,0.065,0.04,0.03,0.03,0.025,
#                      0.03,0.125,0.125,0.365,0.075,0.1,0.085,0.045,0.025,0.025)
# catch_comp$sample_size<-c(150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150)
# 
# 
# survey_comp<-new(r4mas$AgeCompData)
# survey_comp$values<-c(0,0,0,0,0,0,0,0,0,0,
#                       0,0,0,0,0,0,0,0,0,0,
#                       0,0,0,0,0,0,0,0,0,0,
#                       0,0,0,0,0,0,0,0,0,0,
#                       0,0,0,0,0,0,0,0,0,0,
#                       0,0,0,0,0,0,0,0,0,0,
#                       0,0,0,0,0,0,0,0,0,0,
#                       0,0,0,0,0,0,0,0,0,0,
#                       0,0,0,0,0,0,0,0,0,0,
#                       0,0,0,0,0,0,0,0,0,0,
#                       0,0,0,0,0,0,0,0,0,0,
#                       0.015,0.151,0.052,0.261,0.108,0.095,0.273,0.01,0.014,0.021,
#                       0.017,0.109,0.362,0.065,0.197,0.069,0.056,0.108,0.004,0.013,
#                       0.022,0.11,0.221,0.383,0.039,0.111,0.036,0.023,0.048,0.007,
#                       0.022,0.107,0.196,0.229,0.282,0.027,0.075,0.02,0.013,0.029,
#                       0.01,0.111,0.196,0.209,0.172,0.206,0.019,0.042,0.012,0.023,
#                       0.031,0.046,0.205,0.213,0.166,0.132,0.15,0.011,0.026,0.02,
#                       0.018,0.16,0.084,0.222,0.17,0.128,0.097,0.088,0.007,0.026,
#                       0.013,0.085,0.285,0.089,0.175,0.13,0.093,0.057,0.054,0.019,
#                       0.015,0.076,0.156,0.313,0.071,0.137,0.097,0.056,0.036,0.043)
# survey_comp$sample_size<-c(0,0,0,0,0,0,0,0,0,0,0,200,200,200,200,200,200,200,200,200)
# 
# #NLL models
# fleet_index_comp_nll<-new(r4mas$Lognormal)
# fleet_age_comp_nll<-new(r4mas$MultinomialRobust)
# survey_index_comp_nll<-new(r4mas$Lognormal)
# survey_age_comp_nll<-new(r4mas$MultinomialRobust)
# 
# #Fleet
# fleet<-new(r4mas$Fleet)
# fleet$AddAgeCompData(catch_comp$id, "undifferentiated")
# fleet$AddIndexData(catch_index$id, "undifferentiated")
# fleet$SetIndexNllComponent(fleet_index_comp_nll$id)
# fleet$AddSelectivity(selectivity$id, 1, area1$id)
# fleet$SetAgeCompNllComponent(fleet_age_comp_nll$id)
# fleet$AddFishingMortality(fishing_mortality$id, 1,area1$id)
# 
# 
# #Survey
# # survey<-new(r4mas$Survey)
# # survey$AddAgeCompData(survey_comp$id)
# # survey$AddIndexData(survey_index$id)
# # survey$SetIndexNllComponent(survey_index_comp_nll$id)
# # survey$SetAgeCompNllComponent(survey_age_comp_nll$id)
# 
# #build the MAS model
# mas_model<-new(r4mas$MASModel)
# mas_model$nyears<-nyears
# mas_model$nseasons<-nseasons
# mas_model$nages<-nages
# mas_model$catch_season_offset<-1.0
# mas_model$spawning_season_offset<-0.35
# mas_model$extended_plus_group<-13
# mas_model$ages<-ages
# # mas_model$AddFleet(fleet$id)
# #mas_model$AddSurvey(survey$id)
# mas_model$AddPopulation(population$id)
mas_model$Run()
#print(mas_model$GetOutput())
