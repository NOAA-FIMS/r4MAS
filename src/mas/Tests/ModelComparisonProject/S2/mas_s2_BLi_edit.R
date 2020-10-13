setwd("/Users/mattadmin/NetBeansProjects/R4MAS/dist/Release/GNU-MacOSX")
#source("mas_s2_BLi_edit.R")

library(Rcpp)
library(jsonlite)

# par(mfrow=c(2,1))
# par(mar=c(5.1, 4.1, 4.1, 8.1), xpd=TRUE)
# for(i in 1:160){
#  str<- paste(c("OM Outputs/OM",i), collapse = "")
#  str2<- paste(c(str,".RData"), collapse = "")
#   print(str2)
# 
# 
# load(str2)
# 
# }
 # load("OM Outputs/OM23.RData")



for(i in 1:1){
 
  #load the r4mas module
  r4mas <-
    Module("rmas", dyn.load(paste("R4MAS", .Platform$dynlib.ext, sep = "")))
  
 str<- paste(c("OM Outputs/OM",i), collapse = "")
 str2<- paste(c(str,".RData"), collapse = "")
 load(str2)

 ostr<- paste(c("EM Outputs/mas_s2_om",i), collapse = "")
 output_file<- paste(c(ostr,".json"), collapse = "")

# nyears<-30
nyears<-par.sim1$nyr #
nseasons<-1
# nages<-12
nages<-length(par.sim1$ages) #
#ages<-c(0.1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11)
ages <- par.sim1$ages # from 1 to 12 
#How does the SSB and R been modeled in MAS? If it is similar with SS [Rt = f(SSBt)], then extra work may needed for MAS to set up the ages. If it is a standard relationship [Rt+1 = f(SSBt)], then ages could be from 1 to 12.

#define area
area1<-new(r4mas$Area)
area1$name<-"area1"

#Recruitment
recruitment<-new(r4mas$BevertonHoltRecruitment)
# recruitment$R0$value<-1000
recruitment$R0$value<-par.sim1$R0/1000 #
recruitment$R0$estimated<-TRUE
recruitment$R0$phase<-1
# recruitment$h$value<-0.75
recruitment$h$value<-par.sim1$h #
recruitment$h$estimated<-FALSE
# recruitment$h$phase<-2
recruitment$h$phase<-3 # changed from 2 to 3 to match setting in other models, but it doesn't affect the results because h is not estimated
recruitment$h$min<-0.2001
recruitment$h$max<-1.0
recruitment$sigma_r$value<-0.55
# recruitment$sigma_r$value <- par.sim1$logR.sd # if simgma_r is the SD of recruitment in log space
recruitment$sigma_r$estimated<-FALSE

recruitment$estimate_deviations<-TRUE
recruitment$constrained_deviations<-TRUE
recruitment$deviations_min<--15.0
recruitment$deviations_max<-15.0
recruitment$deviation_phase<-1
# recruitment$SetDeviations(c( 0.0,
#                              0.0,
#                              0.0,
#                              0.0,
#                              0.0,
#                              0.0,
#                              0.0,
#                              0.0,
#                              0.0,
#                              0.0,
#                              0.0,
#                              0.0,
#                              0.0,
#                              0.0,
#                              0.0,
#                              0.0,
#                              0.0,
#                              0.0,
#                              0.0,
#                              0.0,
#                              0.0,
#                              0.0))
recruitment$SetDeviations(rep(0.0, times=par.sim1$nyr)) #year 1 to 30?


#Growth
#Growth
growth<-new(r4mas$VonBertalanffyModified)
# empirical_weight<- c(0.5306555, 1.1963283, 2.0582654, 3.0349873, 4.0552124, 5.0646975, 6.0262262, 6.9169206, 7.7248909, 8.4461128, 9.0818532, 9.636695,
#                      0.5306555, 1.1963283, 2.0582654, 3.0349873, 4.0552124, 5.0646975, 6.0262262, 6.9169206, 7.7248909, 8.4461128, 9.0818532, 9.636695,
#                      0.5306555, 1.1963283, 2.0582654, 3.0349873, 4.0552124, 5.0646975, 6.0262262, 6.9169206, 7.7248909, 8.4461128, 9.0818532, 9.636695,
#                      0.5306555, 1.1963283, 2.0582654, 3.0349873, 4.0552124, 5.0646975, 6.0262262, 6.9169206, 7.7248909, 8.4461128, 9.0818532, 9.636695,
#                      0.5306555, 1.1963283, 2.0582654, 3.0349873, 4.0552124, 5.0646975, 6.0262262, 6.9169206, 7.7248909, 8.4461128, 9.0818532, 9.636695,
#                      0.5306555, 1.1963283, 2.0582654, 3.0349873, 4.0552124, 5.0646975, 6.0262262, 6.9169206, 7.7248909, 8.4461128, 9.0818532, 9.636695,
#                      0.5306555, 1.1963283, 2.0582654, 3.0349873, 4.0552124, 5.0646975, 6.0262262, 6.9169206, 7.7248909, 8.4461128, 9.0818532, 9.636695,
#                      0.5306555, 1.1963283, 2.0582654, 3.0349873, 4.0552124, 5.0646975, 6.0262262, 6.9169206, 7.7248909, 8.4461128, 9.0818532, 9.636695,
#                      0.5306555, 1.1963283, 2.0582654, 3.0349873, 4.0552124, 5.0646975, 6.0262262, 6.9169206, 7.7248909, 8.4461128, 9.0818532, 9.636695,
#                      0.5306555, 1.1963283, 2.0582654, 3.0349873, 4.0552124, 5.0646975, 6.0262262, 6.9169206, 7.7248909, 8.4461128, 9.0818532, 9.636695,
#                      0.5306555, 1.1963283, 2.0582654, 3.0349873, 4.0552124, 5.0646975, 6.0262262, 6.9169206, 7.7248909, 8.4461128, 9.0818532, 9.636695,
#                      0.5306555, 1.1963283, 2.0582654, 3.0349873, 4.0552124, 5.0646975, 6.0262262, 6.9169206, 7.7248909, 8.4461128, 9.0818532, 9.636695,
#                      0.5306555, 1.1963283, 2.0582654, 3.0349873, 4.0552124, 5.0646975, 6.0262262, 6.9169206, 7.7248909, 8.4461128, 9.0818532, 9.636695,
#                      0.5306555, 1.1963283, 2.0582654, 3.0349873, 4.0552124, 5.0646975, 6.0262262, 6.9169206, 7.7248909, 8.4461128, 9.0818532, 9.636695,
#                      0.5306555, 1.1963283, 2.0582654, 3.0349873, 4.0552124, 5.0646975, 6.0262262, 6.9169206, 7.7248909, 8.4461128, 9.0818532, 9.636695,
#                      0.5306555, 1.1963283, 2.0582654, 3.0349873, 4.0552124, 5.0646975, 6.0262262, 6.9169206, 7.7248909, 8.4461128, 9.0818532, 9.636695,
#                      0.5306555, 1.1963283, 2.0582654, 3.0349873, 4.0552124, 5.0646975, 6.0262262, 6.9169206, 7.7248909, 8.4461128, 9.0818532, 9.636695,
#                      0.5306555, 1.1963283, 2.0582654, 3.0349873, 4.0552124, 5.0646975, 6.0262262, 6.9169206, 7.7248909, 8.4461128, 9.0818532, 9.636695,
#                      0.5306555, 1.1963283, 2.0582654, 3.0349873, 4.0552124, 5.0646975, 6.0262262, 6.9169206, 7.7248909, 8.4461128, 9.0818532, 9.636695,
#                      0.5306555, 1.1963283, 2.0582654, 3.0349873, 4.0552124, 5.0646975, 6.0262262, 6.9169206, 7.7248909, 8.4461128, 9.0818532, 9.636695,
#                      0.5306555, 1.1963283, 2.0582654, 3.0349873, 4.0552124, 5.0646975, 6.0262262, 6.9169206, 7.7248909, 8.4461128, 9.0818532, 9.636695,
#                      0.5306555, 1.1963283, 2.0582654, 3.0349873, 4.0552124, 5.0646975, 6.0262262, 6.9169206, 7.7248909, 8.4461128, 9.0818532, 9.636695,
#                      0.5306555, 1.1963283, 2.0582654, 3.0349873, 4.0552124, 5.0646975, 6.0262262, 6.9169206, 7.7248909, 8.4461128, 9.0818532, 9.636695,
#                      0.5306555, 1.1963283, 2.0582654, 3.0349873, 4.0552124, 5.0646975, 6.0262262, 6.9169206, 7.7248909, 8.4461128, 9.0818532, 9.636695,
#                      0.5306555, 1.1963283, 2.0582654, 3.0349873, 4.0552124, 5.0646975, 6.0262262, 6.9169206, 7.7248909, 8.4461128, 9.0818532, 9.636695,
#                      0.5306555, 1.1963283, 2.0582654, 3.0349873, 4.0552124, 5.0646975, 6.0262262, 6.9169206, 7.7248909, 8.4461128, 9.0818532, 9.636695,
#                      0.5306555, 1.1963283, 2.0582654, 3.0349873, 4.0552124, 5.0646975, 6.0262262, 6.9169206, 7.7248909, 8.4461128, 9.0818532, 9.636695,
#                      0.5306555, 1.1963283, 2.0582654, 3.0349873, 4.0552124, 5.0646975, 6.0262262, 6.9169206, 7.7248909, 8.4461128, 9.0818532, 9.636695,
#                      0.5306555, 1.1963283, 2.0582654, 3.0349873, 4.0552124, 5.0646975, 6.0262262, 6.9169206, 7.7248909, 8.4461128, 9.0818532, 9.636695,
#                      0.5306555, 1.1963283, 2.0582654, 3.0349873, 4.0552124, 5.0646975, 6.0262262, 6.9169206, 7.7248909, 8.4461128, 9.0818532, 9.636695)
empirical_weight<-rep(par.sim1$W.kg, times=par.sim1$nyr) #

growth$SetUndifferentiatedCatchWeight(empirical_weight)
growth$SetUndifferentiatedSurveyWeight(empirical_weight)
growth$SetUndifferentiatedWeightAtSeasonStart(empirical_weight)
growth$SetUndifferentiatedWeightAtSpawning(empirical_weight)
growth$a_min$value<-0.01
growth$a_max$value<-10.0
growth$c$value<-0.3
growth$lmin$value<-5
growth$lmax$value<-50
growth$alpha_f$value<-0.000025
growth$alpha_m$value<-0.000025
growth$beta_f$value<-2.9624
growth$beta_m$value<-2.9624
#I assume that growth is not used in MAS and empirical?weight-at-age data are used in this case, so I did not modify the growth part. The true growth and weight-at-length in the OM is La=Linf*(1-exp(-K(a-t0)) and Wa=a*La^b/1000. Linf=800 mm, K=0.18, t0=-1.36, a=2.5e-8, b=3.

#Maturity
maturity<-new(r4mas$Maturity)
# maturity$values<-c(0.011488685,
#                    0.16041065,
#                    0.45232527,
#                    0.497389935,
#                    0.499869405,
#                    0.499993495,
#                    0.499999675,
#                    0.499999985,
#                    0.5,
#                    0.5,
#                    0.5,
#                    0.5)
maturity$values <- par.sim1$mat.age # If sex ratio is defined somewhere. If not, then maturity$values <- par.sim1$mat.age/2

#Natural Mortality
natural_mortality<-new(r4mas$NaturalMortality) 
# natural_mortality$SetValues(c(0.2,
#                               0.2,
#                               0.2,
#                               0.2,
#                               0.2,
#                               0.2,
#                               0.2,
#                               0.2,
#                               0.2,
#                               0.2,
#                               0.2,
#                               0.2))
natural_mortality$SetValues(par.sim1$M.age) #

#define Movement (only 1 area in this model)
movement<-new(r4mas$Movement)
movement$connectivity_females<-c(1.0)
movement$connectivity_males<-c(1.0)
movement$connectivity_recruits<-c(1.0)

#Initial Deviations
initial_deviations<-new(r4mas$InitialDeviations)
# initial_deviations$values<-c(0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0)
initial_deviations$values<-rep(0, times=length(par.sim1$ages)) #
#I assume initial deviations are the number at age in year 1 deviations. For ASAP and BAM, they estimate deviations from age 2 to 12 in year 1. For AMAK and SS, they use the ramp function to estimate deviations from age 1 to 12. What about MAS?
# initial_deviations$estimate<-FALSE
initial_deviations$estimate<-FALSE #
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
# fishing_mortality$max<-30
fishing_mortality$max<-4 # For matching settings in other models
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
fishing_mortality$SetValues(sim1$F) #For the model comparison project, true values are used for initial guess

#Selectivity Model
fleet_selectivity<-new(r4mas$LogisticSelectivity)
# fleet_selectivity$a50$value<-1.5
# fleet_selectivity$a50$estimated<-FALSE
# fleet_selectivity$a50$phase<-2
# fleet_selectivity$a50$min<-0.0
# fleet_selectivity$a50$max<-10.0


fleet_selectivity$a50$value<-par.sim1$A50.sel #
fleet_selectivity$a50$estimated<-TRUE # S2 is selected as base case scenario, and the only difference between S1 and S2 are estimating selectivity parameters or not
fleet_selectivity$a50$phase<-2 #
fleet_selectivity$a50$min<-0.0 #
fleet_selectivity$a50$max<-max(par.sim1$ages) #

# fleet_selectivity$slope$value<-1.5
# fleet_selectivity$slope$estimated<-FALSE
# fleet_selectivity$slope$phase<-.5
# fleet_selectivity$slope$min<-0.0001
# fleet_selectivity$slope$max<-5.0

fleet_selectivity$slope$value<-1/par.sim1$slope.sel #
fleet_selectivity$slope$estimated<-TRUE #
fleet_selectivity$slope$phase<-2 #
fleet_selectivity$slope$min<-0 #
fleet_selectivity$slope$max<-max(par.sim1$ages) #

survey_selectivity<-new(r4mas$LogisticSelectivity)
# survey_selectivity$a50$value<-1.0
# survey_selectivity$a50$estimated<-FALSE
# survey_selectivity$a50$phase<-2
# survey_selectivity$a50$min<-0.0
# survey_selectivity$a50$max<-10.0

survey_selectivity$a50$value<-par.sim1$A50.sel.survey #
survey_selectivity$a50$estimated<-TRUE #
survey_selectivity$a50$phase<-2 #
survey_selectivity$a50$min<-0.0 #
survey_selectivity$a50$max<-max(par.sim1$ages) #

# survey_selectivity$slope$value<-0.3
# survey_selectivity$slope$estimated<-FALSE
# survey_selectivity$slope$phase<-2
# survey_selectivity$slope$min<-0.0001
# survey_selectivity$slope$max<-5.0

survey_selectivity$slope$value<-1/par.sim1$slope.sel.survey #
survey_selectivity$slope$estimated<-TRUE #
survey_selectivity$slope$phase<-2 #
survey_selectivity$slope$min<-0 #
survey_selectivity$slope$max<-max(par.sim1$ages) #

survey2_selectivity<-new(r4mas$AgeBasedSelectivity)
survey2_selectivity$estimated<-FALSE
survey2_selectivity$values<-c(1.0,
                              0.0,
                              0.0,
                              0.0,
                              0.0,
                              0.0,
                              0.0,
                              0.0,
                              0.0,
                              0.0,
                              0.0,
                              0.0)
# The OM only has one survey, do you need to keep survey2_selectivity information here?

#Index data
#Index data
catch_index<-new(r4mas$IndexData)
# catch_index$values<-c(160.9729922, 469.039834, 767.2392242, 995.4491466, 760.1287713, 1330.812956, 
#                       1293.228597, 2476.815314, 1325.554509, 1523.691282, 1621.524464, 1611.573564, 
#                       1113.866103, 1538.117519, 1508.500687, 1266.046142, 2238.86148, 1607.917363, 
#                       1453.868581, 1295.528013, 1613.812022, 1072.966713, 1636.167979, 1210.822294, 
#                       1129.045115, 964.3269465, 923.3521247, 1218.20288, 881.6275361, 1270.847518)
# catch_index$error<-c(0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.01,
#                     0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.01, 0.01,
#                     0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.01, 0.01 )
catch_index$values<-dat.input$L.obs #
catch_index$error<-rep(dat.input$cv.L, times=par.sim1$nyr) #

survey_index<-new(r4mas$IndexData)
# survey_index$values<-c(1.769444064, 1.507772568, 1.533016092, 1.616248667, 1.383168775, 1.616386391, 1.412438781, 1.312749108, 
#                        1.196086699, 1.100803192, 1.237393383, 1.059375707, 0.960531835, 0.975227232, 1.029677793, 0.859372577, 
#                        0.920958391, 0.702236849, 0.910970335, 0.671124274, 0.771789588, 0.636336668,  0.602957579, 0.66842653, 
#                        0.501908084, 0.462440308, 0.537562886, 0.564409646, 0.566285357, 0.495778669)
# survey_index$error<-c(0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 
#                       0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2)
survey_index$values<-dat.input$survey.obs #
survey_index$error<-rep(dat.input$cv.survey, times=par.sim1$nyr) #

#Age Comp Data
catch_comp<-new(r4mas$AgeCompData)
# catch_comp$values<-c(0.075, 0.085, 0.105, 0.11, 0.1, 0.08, 0.1, 0.065, 0.065, 0.005, 0.055, 0.155,
#                      0.07, 0.15, 0.125, 0.13, 0.095, 0.04, 0.07, 0.075, 0.025, 0.055, 0.05, 0.115,
#                      0.075, 0.155, 0.11, 0.11, 0.135, 0.045, 0.075, 0.065, 0.04, 0.035, 0.025, 0.13,
#                      0.075, 0.09, 0.12, 0.165, 0.095, 0.07, 0.09, 0.06, 0.06, 0.03, 0.02, 0.125,
#                      0.115, 0.05, 0.11, 0.175, 0.1, 0.04, 0.085, 0.05, 0.045, 0.03, 0.035, 0.165,
#                      0.075, 0.14, 0.12, 0.085, 0.085, 0.105, 0.085, 0.06, 0.055, 0.04, 0.025, 0.125,
#                      0.09, 0.125, 0.195, 0.06, 0.105, 0.075, 0.055, 0.095, 0.055, 0.035, 0.015, 0.095,
#                      0.065, 0.11, 0.135, 0.195, 0.075, 0.07, 0.095, 0.035, 0.055, 0.03, 0.015, 0.12,
#                      0.105, 0.165, 0.155, 0.155, 0.12, 0.065, 0.05, 0.05, 0.02, 0.015, 0.015, 0.085,
#                      0.07, 0.115, 0.145, 0.125, 0.17, 0.105, 0.035, 0.04, 0.045, 0.03, 0.02, 0.1,
#                      0.085, 0.18, 0.115, 0.14, 0.095, 0.07, 0.075, 0.055, 0.02, 0.025, 0.045, 0.095,
#                      0.105, 0.13, 0.16, 0.095, 0.13, 0.06, 0.085, 0.085, 0.03, 0.015, 0.025, 0.08,
#                      0.1, 0.135, 0.16, 0.135, 0.09, 0.1, 0.065, 0.07, 0.055, 0.01, 0.01, 0.07,
#                      0.085, 0.12, 0.18, 0.17, 0.11, 0.06, 0.075, 0.035, 0.055, 0.03, 0.01, 0.07,
#                      0.075, 0.145, 0.215, 0.09, 0.135, 0.085, 0.06, 0.025, 0.055, 0.015, 0.02, 0.08,
#                      0.125, 0.105, 0.205, 0.125, 0.095, 0.09, 0.05, 0.035, 0.045, 0.035, 0.02, 0.07,
#                      0.12, 0.14, 0.155, 0.145, 0.125, 0.075, 0.075, 0.03, 0.045, 0.025, 0.01, 0.055,
#                      0.085, 0.215, 0.16, 0.12, 0.12, 0.04, 0.075, 0.075, 0.04, 0.015, 0.03, 0.025,
#                      0.105, 0.135, 0.22, 0.18, 0.095, 0.085, 0.065, 0.05, 0.02, 0.015, 0.005, 0.025,
#                      0.12, 0.14, 0.16, 0.215, 0.135, 0.035, 0.065, 0.04, 0.015, 0.015, 0.01, 0.05,
#                      0.16, 0.14, 0.145, 0.14, 0.125, 0.085, 0.06, 0.045, 0.02, 0.025, 0.01, 0.045,
#                      0.165, 0.265, 0.135, 0.135, 0.06, 0.07, 0.055, 0.02, 0.01, 0.03, 0.015, 0.04,
#                      0.11, 0.245, 0.23, 0.085, 0.11, 0.09, 0.055, 0.025, 0, 0.01, 0, 0.04,
#                      0.165, 0.235, 0.23, 0.17, 0.055, 0.06, 0.03, 0.02, 0.005, 0.005, 0.01, 0.015,
#                      0.165, 0.155, 0.205, 0.18, 0.13, 0.06, 0.035, 0.02, 0.02, 0.005, 0.005, 0.02,
#                      0.12, 0.185, 0.19, 0.14, 0.185, 0.085, 0.02, 0.015, 0.03, 0.015, 0.005, 0.01,
#                      0.165, 0.185, 0.19, 0.13, 0.11, 0.085, 0.05, 0.03, 0.01, 0.025, 0, 0.02,
#                      0.19, 0.2, 0.135, 0.175, 0.09, 0.07, 0.07, 0.045, 0.01, 0, 0, 0.015,
#                      0.18, 0.22, 0.155, 0.135, 0.125, 0.065, 0.07, 0.025, 0.015, 0.005, 0.005, 0,
#                      0.145, 0.255, 0.215, 0.135, 0.07, 0.08, 0.02, 0.035, 0.02, 0.005, 0.005, 0.015)
# catch_comp$sample_size<-c(200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
#                           200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200)
catch_comp$values<-as.vector(t(dat.input$L.age.obs)) #
catch_comp$sample_size<-rep(par.sim1$n.L, times=par.sim1$nyr) #

survey_comp<-new(r4mas$AgeCompData)
# survey_comp$values<-c(0.04, 0.12, 0.155, 0.15, 0.15, 0.06, 0.085, 0.03, 0.035, 0.03, 0.01, 0.135,
#                       0.07, 0.135, 0.135, 0.095, 0.11, 0.07, 0.105, 0.045, 0.045, 0.025, 0.04, 0.125,
#                       0.085, 0.165, 0.12, 0.13, 0.115, 0.065, 0.045, 0.045, 0.04, 0.045, 0.01, 0.135,
#                       0.05, 0.12, 0.195, 0.15, 0.125, 0.085, 0.06, 0.03, 0.035, 0.035, 0.025, 0.09,
#                       0.04, 0.17, 0.12, 0.17, 0.105, 0.07, 0.045, 0.065, 0.035, 0.045, 0.01, 0.125,
#                       0.085, 0.155, 0.13, 0.155, 0.105, 0.055, 0.06, 0.055, 0.045, 0.045, 0.01, 0.1,
#                       0.085, 0.17, 0.235, 0.08, 0.13, 0.065, 0.055, 0.045, 0.025, 0.01, 0.04, 0.06,
#                       0.07, 0.125, 0.2, 0.18, 0.07, 0.075, 0.065, 0.03, 0.045, 0.02, 0.02, 0.1,
#                       0.07, 0.165, 0.145, 0.135, 0.15, 0.07, 0.06, 0.05, 0.035, 0.015, 0.025, 0.08,
#                       0.065, 0.16, 0.16, 0.12, 0.14, 0.085, 0.055, 0.03, 0.035, 0.05, 0.01, 0.09,
#                       0.105, 0.205, 0.14, 0.12, 0.08, 0.105, 0.055, 0.04, 0.02, 0.025, 0.02, 0.085,
#                       0.09, 0.21, 0.155, 0.095, 0.12, 0.07, 0.07, 0.05, 0.04, 0.02, 0.02, 0.06,
#                       0.115, 0.1, 0.19, 0.175, 0.09, 0.075, 0.055, 0.045, 0.025, 0.005, 0.02, 0.105,
#                       0.065, 0.16, 0.18, 0.155, 0.095, 0.055, 0.075, 0.025, 0.025, 0.04, 0.025, 0.1,
#                       0.09, 0.205, 0.15, 0.095, 0.15, 0.1, 0.07, 0.045, 0.01, 0.03, 0.02, 0.035,
#                       0.09, 0.16, 0.18, 0.165, 0.095, 0.11, 0.035, 0.045, 0.025, 0.02, 0.025, 0.05,
#                       0.14, 0.19, 0.17, 0.145, 0.065, 0.06, 0.07, 0.045, 0.03, 0.015, 0.005, 0.065,
#                       0.055, 0.225, 0.28, 0.115, 0.105, 0.07, 0.045, 0.035, 0.015, 0.01, 0.015, 0.03,
#                       0.08, 0.155, 0.255, 0.2, 0.085, 0.055, 0.025, 0.02, 0.035, 0.015, 0.025, 0.05,
#                       0.07, 0.23, 0.175, 0.145, 0.135, 0.045, 0.06, 0.04, 0.035, 0.02, 0.015, 0.03,
#                       0.16, 0.205, 0.21, 0.105, 0.1, 0.045, 0.05, 0.035, 0.015, 0.02, 0.005, 0.05,
#                       0.12, 0.255, 0.16, 0.09, 0.095, 0.105, 0.065, 0.025, 0.025, 0.015, 0.005, 0.04,
#                       0.115, 0.225, 0.29, 0.125, 0.065, 0.055, 0.03, 0.03, 0.015, 0.01, 0.015, 0.025,
#                       0.095, 0.23, 0.24, 0.2, 0.085, 0.025, 0.04, 0.02, 0.01, 0.025, 0.005, 0.025,
#                       0.11, 0.23, 0.185, 0.19, 0.125, 0.055, 0.025, 0.025, 0.02, 0.01, 0, 0.025,
#                       0.08, 0.265, 0.165, 0.13, 0.185, 0.075, 0.055, 0.015, 0.01, 0.015, 0, 0.005,
#                       0.125, 0.225, 0.275, 0.135, 0.1, 0.06, 0.045, 0.02, 0.005, 0, 0.005, 0.005,
#                       0.13, 0.285, 0.185, 0.195, 0.1, 0.025, 0.045, 0.02, 0, 0, 0.005, 0.01,
#                       0.135, 0.3, 0.245, 0.1, 0.1, 0.035, 0.02, 0.025, 0.025, 0.01, 0.005, 0,
#                       0.11, 0.285, 0.225, 0.17, 0.05, 0.065, 0.02, 0.01, 0.035, 0.01, 0.005, 0.015)
# 
# survey_comp$sample_size<-c(200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 
#                            200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200)

survey_comp$values<-as.vector(t(dat.input$survey.age.obs)) #
survey_comp$sample_size<-rep(par.sim1$n.survey, times=par.sim1$nyr) #

#NLL models
fleet_index_comp_nll<-new(r4mas$Lognormal)
fleet_age_comp_nll<-new(r4mas$MultinomialRobust)
#The other four models uses standard multinomial for age comp nll
survey_index_comp_nll<-new(r4mas$Lognormal)
survey_age_comp_nll<-new(r4mas$MultinomialRobust)
#The other four models uses standard multinomial for age comp nll
survey2_index_comp_nll<-new(r4mas$Lognormal)
#The OM currently only have one survey

#Fleet
fleet<-new(r4mas$Fleet)
fleet$AddAgeCompData(catch_comp$id, "undifferentiated")
fleet$AddIndexData(catch_index$id, "undifferentiated")
fleet$SetAgeCompNllComponent(fleet_age_comp_nll$id)
fleet$SetIndexNllComponent(fleet_index_comp_nll$id)
fleet$AddSelectivity(fleet_selectivity$id, 1, area1$id)
fleet$AddFishingMortality(fishing_mortality$id, 1,area1$id)


#Survey
survey<-new(r4mas$Survey)
survey$AddAgeCompData(survey_comp$id,"undifferentiated")
survey$AddIndexData(survey_index$id,"undifferentiated")
survey$SetIndexNllComponent(survey_index_comp_nll$id)
survey$SetAgeCompNllComponent(survey_age_comp_nll$id)
survey$AddSelectivity(survey_selectivity$id, 1, area1$id)
# survey$q$value<-0.0001
survey$q$value<-dat.input$q #
survey$q$min<-0.0
survey$q$max<-1.0
# survey$q$max<-10.0 #
survey$q$estimated<-TRUE
survey$q$phase<-1



#build the MAS model
mas_model<-new(r4mas$MASModel)
mas_model$nyears<-nyears
mas_model$nseasons<-nseasons
mas_model$nages<-nages
#mas_model$extended_plus_group<-15
mas_model$extended_plus_group<-12 #not sure about this, 12+ is the plus group in the OM
mas_model$ages<-ages
mas_model$AddFleet(fleet$id)
mas_model$catch_season_offset<-0.5
mas_model$survey_season_offset<-0.5
mas_model$spawning_season_offset<-0.0 #Jan-1

#survey time is also Jan-1, I don't know if it needs to be defined in MAS

mas_model$AddSurvey(survey$id)
mas_model$AddPopulation(population$id)
mas_model$Run()
write(mas_model$GetOutput(), file=toString(output_file))
mas_model$Reset()
# 
# #read output
# args <- commandArgs(trailingOnly = TRUE)
args <- toString(output_file)
json_output <- read_json(args[1])
popdy<-json_output$population_dynamics
years <- popdy$nyears
seasons<-popdy$nseasons
areas<-popdy$nareas
ages <- popdy$nages

#population
pop<-popdy$populations[[1]]
pop_recruits<-pop$undifferentiated$recruits
pop_abundance<-pop$undifferentiated$abundance
pop_ssb<-pop$undifferentiated$spawning_stock_biomass
pop_biomass<-pop$undifferentiated$biomass
pop_F<-pop$undifferentiated$fishing_mortality

print(unlist(pop_biomass$values))
#fleet
fleet_<-popdy$fleets[[1]]
fleet_obs_index<-unlist(fleet_$observed$fleet_biomass$values)
fleet_expectd_index <-unlist(fleet_$undifferentiated$catch_biomass)

#survey
survey_<-popdy$surveys[[1]]
survey_obs_index<-unlist(survey_$observed$survey_biomass$values)
survey_expectd_index <-unlist(survey_$undifferentiated$survey_biomass)
}



# 
# print(fleet_obs_index)
# #plot results
# x  <- seq(1, (nyears*nseasons), 1)
# CATCH_INDEX<-unlist(catch_index$values)
# OBSERVED_CATCH_INDEX<-unlist(fleet$observed$fleet_biomass$values)
# SURVEY_INDEX<-unlist(survey_index$values)
# OBSERVED_SURVEY_INDEX<-unlist(popdy$survey$observed$survey_biomass$values)
# print(OBSERVED_CATCH_INDEX)
# 
# 
# plot( x, CATCH_INDEX, type="l", col="red",main="Observed vs Expected Catch Index Stage 2"
#       ,ylab="Index", xlab="Year")
# par(new=TRUE)
# plot( x, OBSERVED_CATCH_INDEX, type="p", col="black",ylab="", xlab="" )
# legend("topright", inset=c(-0.34,0),pch=c(1,3),
#        c("Observed","Expected"),fill=c("white","red"))
# 
# par(mar=c(5.1, 4.1, 4.1, 8.1), xpd=TRUE)
# plot( x, SURVEY_INDEX, type="l", col="red",main="Observed vs Expected Survey Index Stage 2"
#       ,ylab="Index", xlab="Year")
# par(new=TRUE)
# plot( x, OBSERVED_SURVEY_INDEX, type="p", col="black",ylab="", xlab="" )
# legend("topright", inset=c(-0.34,0),pch=c(1,3),
#        c("Observed","Expected"),fill=c("white","red"))