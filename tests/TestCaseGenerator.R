# R script to generate inputs for an operating model in RMAS. The script
# populates and runs the operating model and generates the .RData file
# for a test case. Inputs are randomly chosen between min and max values
# for each parameter in the model.

library(RMAS)
library(jsonlite)
require(Rcpp)
require(RMAS)

r4mas <-
  Module("rmas", dyn.load(paste(
    d, "/libs/RMAS", .Platform$dynlib.ext, sep = ""
  )))


#control values for random model construction

number_of_years_min <- 10
number_of_years_max <- 40

number_of_seasons_min <- 1
number_of_seasons_max <- 1

number_of_ages_min <- 7
number_of_ages_max <- 20

number_of_areas_min <- 1
number_of_areas_max <- 5

number_of_populations_min <- 1
number_of_populations_max <- 5

sex_ratio_min <- 0.3
sex_ratio_max <- 0.8

number_of_fleets_min <- 1
number_of_fleets_max <- 5

number_of_surveys_min <- 1
number_of_surveys_max <- 5

# need to add Ricker here
recruitment_type <- c(BevertonHolt = 1,
                      BevertonHoltAlt = 2)

R0_min <- 15000
R0_max <- 200000


bevertonholt_h_min <- 0.2
bevertonholt_h_max <- 1.0

bevertonholt_sigma_r_min <- 0.1
bevertonholt_sigma_r_max <- 0.5

bevertonholt_alt_h_min <- 0.2
bevertonholt_alt_h_max <- 1.0

bevertonholt_alt_sigma_r_min <- 0.1
bevertonholt_alt_sigma_r_max <- 0.5

growth_type <- c(VonBertalanffy = 1,
                 VonBertalanffyModified = 2)


vonbertalanffy_alpha_f_min<-0.000025
vonbertalanffy_alpha_f_max<-0.000025

vonbertalanffy_alpha_m_min<-0.000025
vonbertalanffy_alpha_m_max<-0.000025

vonbertalanffy_beta_f_min<-3.0
vonbertalanffy_beta_f_max<-3.0

vonbertalanffy_beta_m_min<-3.0
vonbertalanffy_beta_m_max<-3.0

vonbertalanffy_lmin_min<-10
vonbertalanffy_lmin_max<-15

vonbertalanffy_lmax_min<-60
vonbertalanffy_lmax_max<-90

vonbertalanffy_l_inf_min<-60
vonbertalanffy_l_inf_max<-100

vonbertalanffy_k_min<-0.2
vonbertalanffy_k_max<-0.7

vonbertalanffymodified_alpha_f_min<-0.000025
vonbertalanffymodified_alpha_f_max<-0.000025

vonbertalanffymodified_alpha_m_min<-0.000025
vonbertalanffymodified_alpha_m_max<-0.000025

vonbertalanffymodified_beta_f_min<-3.0
vonbertalanffymodified_beta_f_max<-3.0

vonbertalanffymodified_beta_m_min<-3.0
vonbertalanffymodified_beta_m_max<-3.0

vonbertalanffymodified_lmin_min<-10
vonbertalanffymodified_lmin_max<-15

vonbertalanffymodified_lmax_min<-60
vonbertalanffymodified_lmax_max<-60

vonbertalanffymodified_c_min<-0.2
vonbertalanffymodified_c_max<-0.7

maturity_a50_min <- 1.9
maturity_a50_max <- 5.0

maturity_slope_min <- 0.05
maturity_slope_max <- 0.5

natural_mortality_age_1_min <- 0.5
natural_mortality_age_1_max <- 0.7

natural_mortality_age_2_min <- 0.2
natural_mortality_age_2_max <- 0.5

natural_mortality_min <- 0.1
natural_mortality_max <- 0.3

movement_coefficient_min <- 0.2
movement_coefficient_max <- 0.5

selectivity_type <- c(Logistic = 1,
                      DoubleLogistic = 2)

fleet_logistic_selectivity_a50_min <- 1.75
fleet_logistic_selectivity_a50_max <- 5.0

fleet_logistic_selectivity_slope_min <- 0.05
fleet_logistic_selectivity_slope_max <- 0.5

fleet_double_logistic_alpha_asc_min <- 1
fleet_double_logistic_alpha_asc_max <- 1

fleet_double_logistic_alpha_desc_min <- 1
fleet_double_logistic_alpha_desc_max <- 1

fleet_double_logistic_beta_asc_min <- 1
fleet_double_logistic_beta_asc_max <- 1

fleet_double_logistic_beta_desc_min <- 1
fleet_double_logistic_beta_desc_max <- 1

fishing_mortality_min <- 0.1
fishing_mortality_max <- 0.5

survey_logistic_selectivity_a50_min <- 1.75
survey_logistic_selectivity_a50_max <- 5.0

survey_logistic_selectivity_slope_min <- 0.05
survey_logistic_selectivity_slope_max <- 0.5

survey_double_logistic_alpha_asc_min <- 1
survey_double_logistic_alpha_asc_max <- 1

survey_double_logistic_alpha_desc_min <- 1
survey_double_logistic_alpha_desc_max <- 1

survey_double_logistic_beta_asc_min <- 1
survey_double_logistic_beta_asc_max <- 1

survey_double_logistic_beta_desc_min <- 1
survey_double_logistic_beta_desc_max <- 1

survey_q_min <- 0.001
survey_q_max <- 10.0

#Begin model construction
model <- new(r4mas$MASModel)

nyears <-
  as.integer(runif(1, number_of_years_min, number_of_years_max))
model$nyears <- nyears

nseasons <-
  as.integer(runif(1, number_of_seasons_min, number_of_seasons_max))
model$nseasons <- nseasons

nareas <- as.integer(runif(1, number_of_areas_min, number_of_areas_max))

nages <- as.integer(runif(1, number_of_ages_min, number_of_ages_max))
ages <- c(0.1, seq(1, nages - 1))
model$ages <- ages
model$nages<-nages
model$nyears<-nyears
model$nseasons<-nseasons
model$extended_plus_group<-nages
npopulations <-
  as.integer(runif(1, number_of_populations_min, number_of_populations_max))
nfleets <-
  as.integer(runif(1, number_of_fleets_min, number_of_fleets_max))
nsurveys <-
  as.integer(runif(1, number_of_surveys_min, number_of_surveys_max))

#create area models for the operating model
for (i in 1:nareas) {
  a <- assign(paste0("area_", i), new(r4mas$Area))
}

#create populations for the operating model
for (i in 1:npopulations) {
  p <- assign(paste0("population_", i), new(r4mas$Population))
  
  #sex ratio
  p$sex_ratio<-runif(1,sex_ratio_min, sex_ratio_max)
  
  #Recruitment
  recruitment_t <- sample(recruitment_type, 1)
  if (recruitment_t[1] == 1) {
    show(r4mas$Population)
    for (j in 1:nareas) {
      a <- get(paste0("area_", j))
      r <-
        assign(paste0(paste0(
          paste0("beverton-holt_recruitment_", i), "_"
        ), j), new(r4mas$BevertonHoltRecruitment))
      r$R0$value <- runif(1, R0_min, R0_max)
      r$h$value <- runif(1, bevertonholt_h_min, bevertonholt_h_max)
      r$sigma_r$value <-
        runif(1, bevertonholt_sigma_r_min, bevertonholt_sigma_r_max)
      p$AddRecruitment(r$id, 1, a$id)
    }
    
  } else if (recruitment_t[1] == 2) {
    for (j in 1:nareas) {
      a <- get(paste0("area_", j))
      r <-
        assign(paste0(paste0(
          paste0("beverton-holt_alt_recruitment_", i), "_"
        ), j),
        new(r4mas$BevertonHoltRecruitmentAlt))
      r$R0$value <- runif(1, R0_min, R0_max)
      r$h$value <-
        runif(1, bevertonholt_alt_h_min, bevertonholt_alt_h_max)
      r$sigma_r$value <-
        runif(1,
              bevertonholt_alt_sigma_r_min,
              bevertonholt_alt_sigma_r_max)
      p$AddRecruitment(r$id, 1, a$id)
    }
  }
  
  #Growth
  growth_t <- sample(growth_type, 1)
  if (growth_t[1] == 1) {
    
    g<-assign((paste0("VonBertalanffy_",i)), new(r4mas$VonBertalanffy))
    
    g$a_min$value<-ages[1]
    g$a_max$value<-ages[nages]
    
    g$alpha_f$value<-runif(1,vonbertalanffy_alpha_f_min,vonbertalanffy_alpha_f_max)
    g$alpha_m$value<-runif(1,vonbertalanffy_alpha_m_min,vonbertalanffy_alpha_m_max)
    
    g$beta_f$value<-runif(1,vonbertalanffy_beta_f_min,vonbertalanffy_beta_f_max)
    g$beta_m$value<-runif(1,vonbertalanffy_beta_m_min,vonbertalanffy_beta_m_max)
    
    g$k$value<-runif(1,vonbertalanffy_k_min,vonbertalanffy_k_max)
    
    g$l_inf$value<-runif(1,vonbertalanffy_l_inf_min,vonbertalanffy_l_inf_max)
    
    p$SetGrowth(g$id)
    
  } else if (growth_t[1] == 2) {
    g<-assign((paste0("VonBertalanffyModified_",i)), new(r4mas$VonBertalanffyModified))
    
    g$a_min$value<-ages[1]
    g$a_max$value<-ages[nages]
    
    g$alpha_f$value<-runif(1,vonbertalanffymodified_alpha_f_min,vonbertalanffymodified_alpha_f_max)
    g$alpha_m$value<-runif(1,vonbertalanffymodified_alpha_m_min,vonbertalanffymodified_alpha_m_max)
    
    g$beta_f$value<-runif(1,vonbertalanffymodified_beta_f_min,vonbertalanffymodified_beta_f_max)
    g$beta_m$value<-runif(1,vonbertalanffymodified_beta_m_min,vonbertalanffymodified_beta_m_max)
    
    g$c$value<-runif(1,vonbertalanffymodified_c_min,vonbertalanffymodified_c_max)
    
    p$SetGrowth(g$id)
    
  }
 
  #Maturity
  m<-assign(paste0("maturity_",i), new(r4mas$Maturity))
  t<-c(1:nages)
  
  m_a50<-runif(1,maturity_a50_min, maturity_a50_max)
  m_slope<-runif(1,maturity_slope_min, maturity_slope_max)
  
  for(j in 1:nages){
    t[j] <- 1.0/(1.0 + exp(-1.0*(ages[j]-m_a50)/m_slope))
  }
  
  m$values<-t*p$sex_ratio
  
  for( j in nareas){
    a <- get(paste0("area_", j))
    p$AddMaturity(m$id, a$id, "undifferentiated")
  }


  #Natural Mortality
  nm<-assign(paste0("natural_mortality_",i), new(r4mas$NaturalMortality))
  nmv<-c(1:nages)
  nmv[1]<-runif(1,natural_mortality_age_1_min,natural_mortality_age_1_max)
  nmv[2]<-runif(1,natural_mortality_age_2_min,natural_mortality_age_2_max)
  
  nnn<-runif(1,natural_mortality_min, natural_mortality_max)
  for(j in 3:nages){
    nmv[j]<- nnn
  }
  nm$values<-nmv
  for( j in nareas){
    a <- get(paste0("area_", j))
    p$AddNaturalMortality(nm$id, a$id, "undifferentiated")
  }
  
  
  #Movement
  coefficients<-c(nareas*nareas)
  for(j in 1:(nareas*nareas)){
    coefficients[j]<-runif(1,movement_coefficient_min,movement_coefficient_max)
  }
  
  mdat <- matrix( coefficients, nrow = nareas, ncol = nareas, byrow = TRUE)
  for(j in 1:nareas){
    mdat[j,]<-mdat[j,]/sum(mdat[j,])
  }
  
  seasonal_movement<-c(rep(as.vector(mdat), nseasons))
  
  mm<-assign(paste0("movement_",i), new(r4mas$Movement))
  mm$connectivity_females<-seasonal_movement
  mm$connectivity_males<-seasonal_movement
  mm$connectivity_recruits<-seasonal_movement
  
  for(y in 1:nyears){
    p$AddMovement(mm$id, y)
  }
  
  model$AddPopulation(p$id)
}

for(i in 1:nfleets){
  f<-assign(paste0("fleet_",i), new(r4mas$Fleet))
  
  #randomly choose fishing areas
  na<-as.integer(runif(1,1, nareas))
  fa <- as.integer(runif(na, 1, nareas))
  
  selectivity_t<-sample(selectivity_type,1)
  if(selectivity_t[1] == 1){
 
    for (j in 1:length(fa)) {
      a<-get(paste0("area_",fa[j]))
      selx <-
        assign(paste0(paste0(
          paste0("logistic_selectivity_", i), "_"
        ), fa[j]), new(r4mas$LogisticSelectivity))
      
      selx$a50$value<-runif(1,fleet_logistic_selectivity_a50_min, fleet_logistic_selectivity_a50_max)
      selx$slope$value<-runif(1,fleet_logistic_selectivity_slope_min, fleet_logistic_selectivity_slope_max)
      f$AddSelectivity(selx$id,1, a$id)
      
    }
    
  }else if(selectivity_t[1] == 2){
    
    for (j in 1:length(fa)) {
      a<-get(paste0("area_",fa[j]))
      selx <-
        assign(paste0(paste0(
          paste0("double_logistic_selectivity_", i), "_"
        ), fa[j]), new(r4mas$DoubleLogisticSelectivity))
      
      selx$alpha_asc<-runif(1,fleet_double_logistic_alpha_asc_min, fleet_double_logistic_alpha_asc_max)
      selx$alpha_desc<-runif(1,fleet_double_logistic_alpha_desc_min, fleet_double_logistic_alpha_desc_max)
      selx$beta_asc<-runif(1,fleet_double_logistic_beta_asc_min, fleet_double_logistic_beta_asc_max)
      selx$beta_desc<-runif(1,fleet_double_logistic_beta_desc_min, fleet_double_logistic_beta_desc_max)
      f$AddSelectivity(selx$id, 1,a$id)
    }
  }
  
  for(j in 1:nseasons){
    for (k in 1:length(fa)) {
      a<-get(paste0("area_",fa[k]))
      
      fm<-assign(paste0(paste0(paste0(paste0(paste0("fishing_mortality_",i)," "),j),""),k), new(r4mas$FishingMortality))
      fm$values<-c(rep(runif(fishing_mortality_min, fishing_mortality_max),1:nyears*nseasons))
      f$AddFishingMortality(fm$id,1,a$id)
    }
  }
  model$AddFleet(f$id)
}


for(i in 1:nsurveys){
  survey<-assign(paste0("survey_", i), new(r4mas$Survey))
  
  survey$q$value<-runif(1, survey_q_min, survey_q_max)
  
  na<-as.integer(runif(1,1, nareas))
  fa <- as.integer(runif(na, 1, nareas))
  
  selectivity_t<-sample(selectivity_type,1)
  if(selectivity_t[1] == 1){
    
    for (j in 1:length(fa)) {
      a<-get(paste0("area_",fa[j]))
      selx <-
        assign(paste0(paste0(
          paste0("logistic_selectivity_", i), "_"
        ), fa[j]), new(r4mas$LogisticSelectivity))
      
      selx$a50$value<-runif(1,survey_logistic_selectivity_a50_min, survey_logistic_selectivity_a50_max)
      selx$slope$value<-runif(1,survey_logistic_selectivity_slope_min, survey_logistic_selectivity_slope_max)
      survey$AddSelectivity(selx$id, 1,a$id)
      
    }
    
  }else if(selectivity_t[1] == 2){
    
    for (j in 1:length(fa)) {
      a<-get(paste0("area_",fa[j]))
      selx <-
        assign(paste0(paste0(
          paste0("double_logistic_selectivity_", i), "_"
        ), fa[j]), new(r4mas$DoubleLogisticSelectivity))
      
      selx$alpha_asc<-runif(1,survey_double_logistic_alpha_asc_min, survey_double_logistic_alpha_asc_max)
      selx$alpha_desc<-runif(1,survey_double_logistic_alpha_desc_min, survey_double_logistic_alpha_desc_max)
      selx$beta_asc<-runif(1,survey_double_logistic_beta_asc_min, survey_double_logistic_beta_asc_max)
      selx$beta_desc<-runif(1,survey_double_logistic_beta_desc_min, survey_double_logistic_beta_desc_max)
      survey$AddSelectivity(selx$id, 1,a$id)
    }
    
    model$AddSurvey(survey$id)
  }
  
  # model$Run()
  
}