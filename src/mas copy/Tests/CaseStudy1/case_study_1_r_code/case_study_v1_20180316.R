################################################################################################
# The R program file case_study_v1_20180316.R implements data input 
# and initial calculations for case study v1 of the MAS project.
# Jon Brodziak, PIFSC, jon.brodziak@noaa.gov
################################################################################################
# 3-Oct-2017
# Removed 10-year fixed initialization time period and associated deprecated code.
# 6-Oct-2017
# Added function to write input data to output file.
# 18-Dec-2017
# Completed time loop to calculate numbers at age array and quantities of interest.
# 14-Feb-2018
# Added assessment time period results to output file.
# 16-Mar-2018
# Completed scripts to read and echo input data and revised initialization processes
# and implemented the equilibrium and time-varying MaxPlusGroupAge adjustments.
# 
################################################################################################
#
# (1) MODEL INITIALIZATION
#
################################################################################################
# (1.1) INITIALIZE ANALYSIS CLASS
################################################################################################
# NOTE the age groups in each population consist of age=0 to age=9+ 
# and the age group information is stored in array positions a=1 to a=10.
# NOTE that females are indexed by g=1 and males by g=2 in arrays.
# NOTE that fleet 1 fishes in area 1 and fleet 2 fishes in area 2.
# NOTE that survey 1 operates in area 1 and survey 2 operates in area 2.
#-----------------------------------------------------------------------------------------------

# Remove all variable objects from the session memory
rm(list=ls(all.names=F))

# Close any open graphics windows
graphics.off()

# workstation setup for source.folder
source.folder <- '/Users/mattadmin/NetBeansProjects/mas/Tests/CaseStudy1/case_study_1_r_code/'

# laptop setup for source.folder
# source.folder <- 'C:/Users/Jon.Brodziak/Desktop/MAS/case.study/code/'

source(paste(source.folder,'Maturity_Logistic.R',sep=""))

source(paste(source.folder,'Growth_MVB.R',sep=""))

source(paste(source.folder,'WeightAtLength_Allometric.R',sep=""))

source(paste(source.folder,'Recruitment_BH.R',sep=""))

source(paste(source.folder,'Selectivity_Logistic.R',sep=""))

source(paste(source.folder,'Write_Input_Data.R',sep=""))

################################################################################################
# (1.2) SET MODEL DOMAIN PARAMETERS
################################################################################################

source(paste(source.folder,'Read_Input_Data.R',sep=""))

################################################################################################
# (1.3) INITIALIZE POPULATION CLASS
################################################################################################

# Compute maturity probabilities at age by gender, area, and population
#-----------------------------------------------------------------------------------------------
MaturityProbabilityAtAge <- array(rep(1.0,DimPopulationAreaGenderAge),c(NPopulation,NArea,NGender,NAge))

for (p in 1:NPopulation)
  for (d in 1:NArea)
    for (g in 1:NGender)
      {
      for (a in 1:NAge) 
        {
        age <- a-1
        MaturityProbabilityAtAge[p,d,g,a]  <- Maturity.Logistic(age,Maturity.a50[p,d,g],Maturity.slope[p,d,g]) 
        }
      }

# Compute mean length at age at start of year by gender, area, and population
#-----------------------------------------------------------------------------------------------
MeanLengthStartOfYear <- array(rep(0.0,DimPopulationAreaGenderAge),c(NPopulation,NArea,NGender,NAge))

for (p in 1:NPopulation)
  for (d in 1:NArea)
    for (g in 1:NGender)
      for (a in 2:(NAge-1)) {
        age <- a-1
        MeanLengthStartOfYear[p,d,g,a]  <- Growth.MVB(age,Length.Amin[p,d,g],Length.Amax[p,d,g],StartZFraction[p,d],Length.Lmin[p,d,g],Length.Lmax[p,d,g],Length.c[p,d,g]) 
      }

# Use linear interpolation for age-0 fish (index a=1)
for (p in 1:NPopulation)
  for (d in 1:NArea)
    for (g in 1:NGender)
    {
      a <- 1
      MeanLengthStartOfYear[p,d,g,a]  <-  StartZFraction[p,d]*MeanLengthStartOfYear[p,d,g,(a+1)]
    }
# Use MaxPlusGroup age adjustment for the plus group
for (p in 1:NPopulation)
  for (d in 1:NArea)
    for (g in 1:NGender) 
    {
      tmp <- 0.0
      for (k in 0:(MaxPlusGroupAge-NAge-1))
        tmp <- tmp + exp(-k*NaturalMortality[p,d,g,NAge])*Growth.MVB((NAge+k),Length.Amin[p,d,g],Length.Amax[p,d,g],StartZFraction[p,d],Length.Lmin[p,d,g],Length.Lmax[p,d,g],Length.c[p,d,g]) 
      tmp <- tmp*(1.0-exp(-NaturalMortality[p,d,g,NAge]))
      MeanLengthStartOfYear[p,d,g,NAge] <- tmp/(1.0-exp(-NaturalMortality[p,d,g,NAge]*(MaxPlusGroupAge-NAge)))
    }
      
# Compute mean length during spawning season age by gender, area, and population
#-----------------------------------------------------------------------------------------------
MeanLengthSpawning <- array(rep(0.0,DimPopulationAreaGenderAge),c(NPopulation,NArea,NGender,NAge))

for (p in 1:NPopulation)
  for (d in 1:NArea)
    for (g in 1:NGender)
      for (a in 2:(NAge-1)) {
        age <- a-1
        MeanLengthSpawning[p,d,g,a]  <-  Growth.MVB(age,Length.Amin[p,d,g],Length.Amax[p,d,g],SpawningZFraction[p,d],Length.Lmin[p,d,g],Length.Lmax[p,d,g],Length.c[p,d,g]) 
      }
# Use linear interpolation for age-0 fish (index a=1)
for (p in 1:NPopulation)
  for (d in 1:NArea)
    for (g in 1:NGender)
    {
      a <- 1
      MeanLengthSpawning[p,d,g,a]  <-  SpawningZFraction[p,d]*MeanLengthStartOfYear[p,d,g,(a+1)]
    }
# Use MaxPlusGroup age adjustment for the plus group
for (p in 1:NPopulation)
  for (d in 1:NArea)
    for (g in 1:NGender) 
    {
      tmp <- 0.0
      for (k in 0:(MaxPlusGroupAge-NAge-1))
        tmp <- tmp + exp(-k*NaturalMortality[p,d,g,NAge])*Growth.MVB((NAge+k),Length.Amin[p,d,g],Length.Amax[p,d,g],SpawningZFraction[p,d],Length.Lmin[p,d,g],Length.Lmax[p,d,g],Length.c[p,d,g]) 
      tmp <- tmp*(1.0-exp(-NaturalMortality[p,d,g,NAge]))
      MeanLengthSpawning[p,d,g,NAge] <- tmp/(1.0-exp(-NaturalMortality[p,d,g,NAge]*(MaxPlusGroupAge-NAge)))
    }

# Compute mean length of fishery catch at age by gender, fleet, and population
#-----------------------------------------------------------------------------------------------
MeanLengthCatch <- array(rep(0.0,DimPopulationFleetGenderAge),c(NPopulation,NFleet,NGender,NAge))

for (p in 1:NPopulation)
  for (v in 1:NFleet)
    for (g in 1:NGender)
      for (a in 2:(NAge-1)) {
        age <- a-1
        MeanLengthCatch[p,FleetArea[v],g,a]  <-  Growth.MVB(age,Length.Amin[p,FleetArea[v],g],Length.Amax[p,FleetArea[v],g],CatchZFraction[p,FleetArea[v]],Length.Lmin[p,FleetArea[v],g],Length.Lmax[p,FleetArea[v],g],Length.c[p,FleetArea[v],g]) 
      }
# Use linear interpolation for age-0 fish (index a=1)
for (p in 1:NPopulation)
  for (v in 1:NFleet)
    for (g in 1:NGender)
    {
      a <- 1
      MeanLengthCatch[p,FleetArea[v],g,a]  <-  CatchZFraction[p,FleetArea[v]]*MeanLengthStartOfYear[p,FleetArea[v],g,(a+1)]
    }
# Use MaxPlusGroup age adjustment for the plus group
for (p in 1:NPopulation)
  for (d in 1:NArea)
    for (g in 1:NGender) 
    {
      tmp <- 0.0
      for (k in 0:(MaxPlusGroupAge-NAge-1))
        tmp <- tmp + exp(-k*NaturalMortality[p,d,g,NAge])*Growth.MVB((NAge+k),Length.Amin[p,d,g],Length.Amax[p,d,g],CatchZFraction[p,d],Length.Lmin[p,d,g],Length.Lmax[p,d,g],Length.c[p,d,g]) 
      tmp <- tmp*(1.0-exp(-NaturalMortality[p,d,g,NAge]))
      MeanLengthCatch[p,d,g,NAge] <- tmp/(1.0-exp(-NaturalMortality[p,d,g,NAge]*(MaxPlusGroupAge-NAge)))
    }

# Compute mean length of survey catch at age by gender, survey, and population
#-----------------------------------------------------------------------------------------------
MeanLengthSurvey <- array(rep(0.0,DimPopulationSurveyGenderAge),c(NPopulation,NSurvey,NGender,NAge))

for (p in 1:NPopulation)
  for (I in 1:NSurvey)
    for (g in 1:NGender)
      for (a in 2:(NAge-1)) {
        age <- a-1
        MeanLengthSurvey[p,SurveyArea[I],g,a]  <-  Growth.MVB(age,Length.Amin[p,SurveyArea[I],g],Length.Amax[p,SurveyArea[I],g],SurveyZFraction[p,SurveyArea[I]],Length.Lmin[p,SurveyArea[I],g],Length.Lmax[p,SurveyArea[I],g],Length.c[p,SurveyArea[I],g]) 
      }
# Use linear interpolation for age-0 fish (index a=1)
for (p in 1:NPopulation)
  for (I in 1:NSurvey)
    for (g in 1:NGender)
    {
      a <- 1
      MeanLengthSurvey[p,SurveyArea[I],g,a]  <-  SurveyZFraction[p,SurveyArea[I]]*MeanLengthStartOfYear[p,SurveyArea[I],g,(a+1)]
    }
# Use MaxPlusGroup age adjustment for the plus group
for (p in 1:NPopulation)
  for (d in 1:NArea)
    for (g in 1:NGender) 
    {
      tmp <- 0.0
      for (k in 0:(MaxPlusGroupAge-NAge-1))
        tmp <- tmp + exp(-k*NaturalMortality[p,d,g,NAge])*Growth.MVB((NAge+k),Length.Amin[p,d,g],Length.Amax[p,d,g],SurveyZFraction[p,d],Length.Lmin[p,d,g],Length.Lmax[p,d,g],Length.c[p,d,g]) 
      tmp <- tmp*(1.0-exp(-NaturalMortality[p,d,g,NAge]))
      MeanLengthSurvey[p,d,g,NAge] <- tmp/(1.0-exp(-NaturalMortality[p,d,g,NAge]*(MaxPlusGroupAge-NAge)))
    }

# Compute mean weights at age at start of year, spawning, catch, 
# and survey by gender, area, and population
#-----------------------------------------------------------------------------------------------
MeanWeightStartOfYear <- array(rep(0.0,DimPopulationAreaGenderAge),c(NPopulation,NArea,NGender,NAge))
MeanWeightSpawning <- array(rep(0.0,DimPopulationAreaGenderAge),c(NPopulation,NArea,NGender,NAge))
MeanWeightCatch <- array(rep(0.0,DimPopulationFleetGenderAge),c(NPopulation,NFleet,NGender,NAge))
MeanWeightSurvey <- array(rep(0.0,DimPopulationSurveyGenderAge),c(NPopulation,NSurvey,NGender,NAge))

for (p in 1:NPopulation)
  for (d in 1:NArea)
    for (g in 1:NGender)
      for (a in 1:NAge) {
        MeanWeightStartOfYear[p,d,g,a]  <-  WeightAtLength.Allometric(MeanLengthStartOfYear[p,d,g,a],WeightAtLength.A[p,d,g],WeightAtLength.B[p,d,g])
        MeanWeightSpawning[p,d,g,a] <- WeightAtLength.Allometric(MeanLengthSpawning[p,d,g,a],WeightAtLength.A[p,d,g],WeightAtLength.B[p,d,g])
      }
for (p in 1:NPopulation)
  for (v in 1:NFleet)
    for (g in 1:NGender)
      for (a in 1:NAge) {
        MeanWeightCatch[p,FleetArea[v],g,a] <-  WeightAtLength.Allometric(MeanLengthCatch[p,FleetArea[v],g,a],WeightAtLength.A[p,FleetArea[v],g],WeightAtLength.B[p,FleetArea[v],g])
      }
for (p in 1:NPopulation)
  for (I in 1:NSurvey)
    for (g in 1:NGender)
      for (a in 1:NAge)
        MeanWeightSurvey[p,SurveyArea[I],g,a] <-  WeightAtLength.Allometric(MeanLengthSurvey[p,SurveyArea[I],g,a],WeightAtLength.A[p,SurveyArea[I],g],WeightAtLength.B[p,SurveyArea[I],g])

# Store unfished equilibrium mean lengths and mean weights
#-----------------------------------------------------------------------------------------------
UnfishedMeanLengthStartOfYear <- array(rep(0.0,DimPopulationAreaGenderAge),c(NPopulation,NArea,NGender,NAge))
UnfishedMeanLengthSpawning <- array(rep(0.0,DimPopulationAreaGenderAge),c(NPopulation,NArea,NGender,NAge))
UnfishedMeanLengthCatch <- array(rep(0.0,DimPopulationAreaGenderAge),c(NPopulation,NArea,NGender,NAge))
UnfishedMeanLengthSurvey <- array(rep(0.0,DimPopulationAreaGenderAge),c(NPopulation,NArea,NGender,NAge))
UnfishedMeanWeightStartOfYear <- array(rep(0.0,DimPopulationAreaGenderAge),c(NPopulation,NArea,NGender,NAge))
UnfishedMeanWeightSpawning <- array(rep(0.0,DimPopulationAreaGenderAge),c(NPopulation,NArea,NGender,NAge))
UnfishedMeanWeightCatch <- array(rep(0.0,DimPopulationFleetGenderAge),c(NPopulation,NFleet,NGender,NAge))
UnfishedMeanWeightSurvey <- array(rep(0.0,DimPopulationSurveyGenderAge),c(NPopulation,NSurvey,NGender,NAge))

UnfishedMeanLengthStartOfYear <- MeanLengthStartOfYear
UnfishedMeanLengthSpawning <- MeanLengthSpawning
UnfishedMeanLengthCatch <- MeanLengthCatch
UnfishedMeanLengthSurvey <- MeanLengthSurvey
UnfishedMeanWeightStartOfYear <- MeanWeightStartOfYear
UnfishedMeanWeightSpawning <- MeanWeightSpawning
UnfishedMeanWeightCatch <- MeanWeightCatch
UnfishedMeanWeightSurvey <- MeanWeightSurvey

# Set stock-recruitment submodel parameters by gender, area, and population
#-----------------------------------------------------------------------------------------------
Recruitment.LogUnfishedR <- log(Recruitment.UnfishedR)

################################################################################################
# (1.4) INITIALIZE OBSERVATION CLASS
################################################################################################

# Set fishery selectivity submodel parameters by gender, area (fleet), and population
#-----------------------------------------------------------------------------------------------
FisherySelectivityAtAge <- array(rep(1.0,DimPopulationFleetGenderAge),c(NPopulation,NFleet,NGender,NAge))

# Compute equilibrium fishery selectivity at age by gender, area (fleet), and population
#-----------------------------------------------------------------------------------------------
EquilibriumFisherySelectivityAtAge <- array(rep(1.0,DimPopulationFleetGenderAge),c(NPopulation,NFleet,NGender,NAge))

for (p in 1:NPopulation)
  for (v in 1:NFleet)
    for (g in 1:NGender) {
      tmp <- 0.0
      for (a in 1:NAge) {
        age <- a-1
        EquilibriumFisherySelectivityAtAge[p,FleetArea[v],g,a]  <- Selectivity.Logistic(age,FisherySelectivity.a50[p,FleetArea[v],g],FisherySelectivity.slope[p,FleetArea[v],g])
      }
# Rescale to set maximum selectivity at age to be 1      
      tmp <- max(EquilibriumFisherySelectivityAtAge[p,FleetArea[v],g,])
      for (a in 1:NAge) {
        EquilibriumFisherySelectivityAtAge[p,FleetArea[v],g,a] <- EquilibriumFisherySelectivityAtAge[p,FleetArea[v],g,a]/tmp
        }
    }

# Set fishery selectivity during assessment time horizon to equal equilibrium fishery selectivity
FisherySelectivityAtAge <- EquilibriumFisherySelectivityAtAge

# Set survey selectivity submodel parameters by gender, area, and population
#-----------------------------------------------------------------------------------------------
SurveySelectivityAtAge <- array(rep(1.0,DimPopulationSurveyGenderAge),c(NPopulation,NSurvey,NGender,NAge))

# Compute survey selectivity at age by gender, area, and population
#-----------------------------------------------------------------------------------------------
for (p in 1:NPopulation)
  for (I in 1:NSurvey)
    for (g in 1:NGender) {
      tmp <- 0.0
      for (a in 1:NAge) {
        age <- a-1
        SurveySelectivityAtAge[p,SurveyArea[I],g,a]  <- Selectivity.Logistic(age,SurveySelectivity.a50[p,SurveyArea[I],g],SurveySelectivity.slope[p,SurveyArea[I],g])
      }
      # Rescale to set maximum selectivity at age to be 1      
      tmp <- max(SurveySelectivityAtAge[p,SurveyArea[I],g,])
      for (a in 1:NAge) {
        SurveySelectivityAtAge[p,SurveyArea[I],g,a] <- SurveySelectivityAtAge[p,SurveyArea[I],g,a]/tmp
        }
      }

################################################################################################
# (1.5) INITIALIZE ENVIRONMENT CLASS
################################################################################################
# Placeholder
#-----------------------------------------------------------------------------------------------

################################################################################################
# WRITE INPUT DATA TO FILE
################################################################################################

Write_Input_Data(OutputFile)

################################################################################################
#
# (2) POPULATION LOOP OVER TIME
#
################################################################################################
#
# (2.1) CALCULATE UNFISHED EQUILIBRIUM
#       POPULATION NUMBERS AND SPAWNING BIOMASS BY POPULATION, AREA, AND GENDER
#
################################################################################################
#
# Dimension arrays
#-----------------------------------------------------------------------------------------------
 EquilibriumNumbersAtAge <- array(rep(0.0,DimIterationPopulationAreaGenderAge),c(MaxIteration,NPopulation,NArea,NGender,NAge)) 
 EquilibriumSpawningBiomass <- array(rep(0.0,DimIterationPopulationAreaGender),c(MaxIteration,NPopulation,NArea,NGender))

# Calculate unfished equilibrium spawning biomasses
#-----------------------------------------------------------------------------------------------

source(paste(source.folder,'Calculate_Unfished_Equilibrium.R',sep=""))
 
################################################################################################
# (2.1) END
################################################################################################

################################################################################################
#
# (2.2) CALCULATE FISHED EQUILIBRIUM
#       POPULATION NUMBERS AND SPAWNING BIOMASS BY POPULATION, AREA, AND GENDER
#
################################################################################################

# Dimension arrays
#-----------------------------------------------------------------
EquilibriumNumbersAtAge <- array(rep(0.0,DimIterationPopulationAreaGenderAge),c(MaxIteration,NPopulation,NArea,NGender,NAge)) 
EquilibriumSpawningBiomass <- array(rep(0.0,DimIterationPopulationAreaGender),c(MaxIteration,NPopulation,NArea,NGender))
EquilibriumFishingMortalityAtAge <- array(rep(0.0,DimPopulationAreaGenderAge),c(NPopulation,NArea,NGender,NAge))
EquilibriumTotalMortalityAtAge <- array(rep(0.0,DimPopulationAreaGenderAge),c(NPopulation,NArea,NGender,NAge))
EquilibriumRecruitmentProduction <- array(rep(0.0,DimPopulationArea),c(NPopulation,NArea))

# Calculate equilibrium fishing mortality at age
#-----------------------------------------------------------------
for (p in 1:NPopulation)
  for (d in 1:NArea)
    for (g in 1:NGender)
      for (a in 1:(NAge)) {
        EquilibriumFishingMortalityAtAge[p,d,g,a] <- EquilibriumFisherySelectivityAtAge[p,d,g,a]*EquilibriumFishingMortality[d,d]
        EquilibriumTotalMortalityAtAge[p,d,g,a] <- NaturalMortality[p,k,g,a]+EquilibriumFishingMortalityAtAge[p,k,g,a]
      }

# Calculate fished equilibrium lengths and weights at age 
# for the plus group using the MaxPlusGroupAge adjustment
#-----------------------------------------------------------------
# Use MaxPlusGroup age adjustment for the MeanLengthStartOfYear
for (p in 1:NPopulation)
  for (d in 1:NArea)
    for (g in 1:NGender) 
    {
      tmp <- 0.0
      for (k in 0:(MaxPlusGroupAge-NAge-1))
        tmp <- tmp + exp(-k*EquilibriumTotalMortalityAtAge[p,d,g,NAge])*Growth.MVB((NAge+k),Length.Amin[p,d,g],Length.Amax[p,d,g],StartZFraction[p,d],Length.Lmin[p,d,g],Length.Lmax[p,d,g],Length.c[p,d,g]) 
      tmp <- tmp*(1.0-exp(-EquilibriumTotalMortalityAtAge[p,d,g,NAge]))
      MeanLengthStartOfYear[p,d,g,NAge] <- tmp/(1.0-exp(-EquilibriumTotalMortalityAtAge[p,d,g,NAge]*(MaxPlusGroupAge-NAge)))
    }
# Use MaxPlusGroup age adjustment for the MeanLengthSpawning
for (p in 1:NPopulation)
  for (d in 1:NArea)
    for (g in 1:NGender) 
    {
      tmp <- 0.0
      for (k in 0:(MaxPlusGroupAge-NAge-1))
        tmp <- tmp + exp(-k*EquilibriumTotalMortalityAtAge[p,d,g,NAge])*Growth.MVB((NAge+k),Length.Amin[p,d,g],Length.Amax[p,d,g],SpawningZFraction[p,d],Length.Lmin[p,d,g],Length.Lmax[p,d,g],Length.c[p,d,g]) 
      tmp <- tmp*(1.0-exp(-EquilibriumTotalMortalityAtAge[p,d,g,NAge]))
      MeanLengthSpawning[p,d,g,NAge] <- tmp/(1.0-exp(-EquilibriumTotalMortalityAtAge[p,d,g,NAge]*(MaxPlusGroupAge-NAge)))
    }
# Use MaxPlusGroup age adjustment for the MeanLengthCatch
for (p in 1:NPopulation)
  for (d in 1:NArea)
    for (g in 1:NGender) 
    {
      tmp <- 0.0
      for (k in 0:(MaxPlusGroupAge-NAge-1))
        tmp <- tmp + exp(-k*EquilibriumTotalMortalityAtAge[p,d,g,NAge])*Growth.MVB((NAge+k),Length.Amin[p,d,g],Length.Amax[p,d,g],CatchZFraction[p,d],Length.Lmin[p,d,g],Length.Lmax[p,d,g],Length.c[p,d,g]) 
      tmp <- tmp*(1.0-exp(-EquilibriumTotalMortalityAtAge[p,d,g,NAge]))
      MeanLengthCatch[p,d,g,NAge] <- tmp/(1.0-exp(-EquilibriumTotalMortalityAtAge[p,d,g,NAge]*(MaxPlusGroupAge-NAge)))
    }

# Use MaxPlusGroup age adjustment for the MeanLengthSurvey
for (p in 1:NPopulation)
  for (d in 1:NArea)
    for (g in 1:NGender) 
    {
      tmp <- 0.0
      for (k in 0:(MaxPlusGroupAge-NAge-1))
        tmp <- tmp + exp(-k*EquilibriumTotalMortalityAtAge[p,d,g,NAge])*Growth.MVB((NAge+k),Length.Amin[p,d,g],Length.Amax[p,d,g],SurveyZFraction[p,d],Length.Lmin[p,d,g],Length.Lmax[p,d,g],Length.c[p,d,g]) 
      tmp <- tmp*(1.0-exp(-EquilibriumTotalMortalityAtAge[p,d,g,NAge]))
      MeanLengthSurvey[p,d,g,NAge] <- tmp/(1.0-exp(-EquilibriumTotalMortalityAtAge[p,d,g,NAge]*(MaxPlusGroupAge-NAge)))
    }

# Compute fished equilibrium mean weights of the plus group for the start of year, 
# spawning, catch and survey by gender, area and population
#-----------------------------------------------------------------------------------------------
for (p in 1:NPopulation)
  for (d in 1:NArea)
    for (g in 1:NGender) {
        MeanWeightStartOfYear[p,d,g,NAge]  <-  WeightAtLength.Allometric(MeanLengthStartOfYear[p,d,g,NAge],WeightAtLength.A[p,d,g],WeightAtLength.B[p,d,g])
        MeanWeightSpawning[p,d,g,NAge] <- WeightAtLength.Allometric(MeanLengthSpawning[p,d,g,NAge],WeightAtLength.A[p,d,g],WeightAtLength.B[p,d,g])
      }
for (p in 1:NPopulation)
  for (v in 1:NFleet)
    for (g in 1:NGender) {
        MeanWeightCatch[p,FleetArea[v],g,NAge] <-  WeightAtLength.Allometric(MeanLengthCatch[p,FleetArea[v],g,NAge],WeightAtLength.A[p,FleetArea[v],g],WeightAtLength.B[p,FleetArea[v],g])
      }
for (p in 1:NPopulation)
  for (I in 1:NSurvey)
    for (g in 1:NGender)
        MeanWeightSurvey[p,SurveyArea[I],g,NAge] <-  WeightAtLength.Allometric(MeanLengthSurvey[p,SurveyArea[I],g,NAge],WeightAtLength.A[p,SurveyArea[I],g],WeightAtLength.B[p,SurveyArea[I],g])

# Calculate fished equilibrium numbers at age 
#-----------------------------------------------------------------
source(paste(source.folder,'Calculate_Fished_Equilibrium.R',sep=""))

################################################################################################
#
# (2.3) CALCULATE ASSESSMENT TIME PERIOD
#       POPULATION NUMBERS, SPAWNING BIOMASS, AND RECRUITMENT BY POPULATION, AREA, AND GENDER
#
################################################################################################
# Dimension Arrays
#-----------------------------------------------------------------------------------------------
NumbersAtAge <- array(rep(0.0,DimYearPlusOnePopulationAreaGenderAge),c(NYearPlusOne,NPopulation,NArea,NGender,NAge))
AssessmentMeanLengthStartOfYear <- array(rep(0.0,DimYearPopulationAreaGenderAge),c(NYear,NPopulation,NArea,NGender,NAge))
AssessmentMeanLengthSpawning <- array(rep(0.0,DimYearPopulationAreaGenderAge),c(NYear,NPopulation,NArea,NGender,NAge))
AssessmentMeanLengthCatch <- array(rep(0.0,DimYearPopulationAreaGenderAge),c(NYear,NPopulation,NArea,NGender,NAge))
AssessmentMeanLengthSurvey <- array(rep(0.0,DimYearPopulationAreaGenderAge),c(NYear,NPopulation,NArea,NGender,NAge))
AssessmentMeanWeightStartOfYear <- array(rep(0.0,DimYearPopulationAreaGenderAge),c(NYear,NPopulation,NArea,NGender,NAge))
AssessmentMeanWeightSpawning <- array(rep(0.0,DimYearPopulationAreaGenderAge),c(NYear,NPopulation,NArea,NGender,NAge))
AssessmentMeanWeightCatch <- array(rep(0.0,DimYearPopulationAreaGenderAge),c(NYear,NPopulation,NArea,NGender,NAge))
AssessmentMeanWeightSurvey <- array(rep(0.0,DimYearPopulationAreaGenderAge),c(NYear,NPopulation,NArea,NGender,NAge))

FisheryCatchNumbersAtAgeByPopulation <- array(rep(0.0,DimYearPopulationFleetAreaGenderAge),c(NYear,NPopulation,NFleet,NArea,NGender,NAge))
FisheryCatchProportionAtAgeByPopulation <- array(rep(0.0,DimYearPopulationFleetAreaGenderAge),c(NYear,NPopulation,NFleet,NArea,NGender,NAge))
FisheryCatchNumbersAtAge <- array(rep(0.0,DimYearFleetAreaGenderAge),c(NYear,NFleet,NArea,NGender,NAge))
FisheryCatchProportionAtAge <- array(rep(0.0,DimYearFleetAreaGenderAge),c(NYear,NFleet,NArea,NGender,NAge))
FisheryCatchBiomassAtAgeByPopulation <- array(rep(0.0,DimYearPopulationFleetAreaGenderAge),c(NYear,NPopulation,NFleet,NArea,NGender,NAge))
FisheryCatchBiomass <- array(rep(0.0,DimYearFleetArea),c(NYear,NFleet,NArea))

TotalMortality  <- array(rep(0.0,DimYearPopulationAreaGenderAge),c(NYear,NPopulation,NArea,NGender,NAge))

SpawningNumbersAtAge <- array(rep(0.0,DimPopulationAreaGenderAge),c(NPopulation,NArea,NGender,NAge))
SpawningBiomass <- array(rep(0.0,DimYearPopulationAreaGender),c(NYear,NPopulation,NArea,NGender))

RecruitmentProduction <- array(rep(0.0,DimYearPopulationArea),c(NYear,NPopulation,NArea))
Recruitment <- array(rep(0.0,DimYearPopulationAreaGender),c(NYear,NPopulation,NArea,NGender))

SurveyCatchability <- array(rep(0.00001,DimPopulationSurvey),c(NPopulation,NSurvey))
SurveyCatchNumbersAtAgeByPopulation <- array(rep(0.0,DimYearPopulationSurveyGenderAge),c(NYear,NPopulation,NSurvey,NGender,NAge))
SurveyCatchProportionAtAgeByPopulation <- array(rep(0.0,DimYearPopulationSurveyGenderAge),c(NYear,NPopulation,NSurvey,NGender,NAge))
SurveyCatchNumbersAtAge <- array(rep(0.0,DimYearSurveyGenderAge),c(NYear,NSurvey,NGender,NAge))
SurveyCatchProportionAtAge <- array(rep(0.0,DimYearSurveyGenderAge),c(NYear,NSurvey,NGender,NAge))
SurveyCatchBiomassAtAgeByPopulation <- array(rep(0.0,DimYearPopulationSurveyGenderAge),c(NYear,NPopulation,NSurvey,NGender,NAge))
SurveyCatchBiomass <- array(rep(0.0,DimYearSurvey),c(NYear,NSurvey))

# Calculate numbers at age and QOI during assessment period 
#-----------------------------------------------------------------
source(paste(source.folder,'Calculate_Assessment_Period.R',sep=""))

################################################################################################
#
# (3)	WRITE MODEL RESULTS TO OUTPUT FILE
#  
################################################################################################

# Output numbers at age and QOI during assessment period 
#-----------------------------------------------------------------
source(paste(source.folder,'Write_Output_Data.R',sep=""))

################################################################################################
# END 
################################################################################################
