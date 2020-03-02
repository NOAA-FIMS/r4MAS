################################################################################################
# The R program file Write_Input_Data.R writes the input data and
# results of initialization calculations for case study v1 
# of the MAS project to user-named OutputFile.
# Jon Brodziak, PIFSC, jon.brodziak@noaa.gov 13-Mar-2018
################################################################################################
Write_Input_Data<-function(filename)
{
  print('_____________________________________________________________________________________________________')
  print('Writing input data to output file ...')
  print('_____________________________________________________________________________________________________')

  sink(filename)

  print('#################################################################################################')
  print('# INITIIALIZING ANALYSIS CLASS')
  print('#################################################################################################')
  print('#')
  
  print('#################################################################################################')
  print('# SETTING MODEL DOMAIN PARAMETERS')
  print('#################################################################################################')
  
  print('# Number of populations (1:NPopulation)')
  print(NPopulation)
  
  print('# Number of age classes (1:NAge)')
  print(NAge)
  
  print('# Maximum plus group age')
  print(MaxPlusGroupAge)
  
  print('# Number of genders (1:NGender)')
  print(NGender)
  
  print('# Number of areas (1:NArea)')
  print(NArea)
  
  print('# Number of fleets (1:NFleet)')
  print(NFleet)
  
  for (I in 1:NFleet)
  print(c("# Fleet =",I,"operates in Area=",FleetArea[I]))
  
  print('# Number of surveys (1:NSurvey)')
  print(NSurvey)
  
  for (I in 1:NSurvey)
  print(c("# Survey=",I,"operates in Area=",SurveyArea[I]))
  
  print('# Number of years in assessment time horizon (1:NYear)')
  print(NYear)
  
  print('# Number of seasons in a year (1:NSeason)')
  print(NSeason)
  
  print('# Maximum number of iterations for computing initial equilibrium (1:MaxIteration)')
  print(MaxIteration)
  
  print('# Convergence criterion for computing initial equilibrium: EquilibriumConvergenceCriterion')
  print(EquilibriumConvergenceCriterion)
  print('#')
  
  print('# Population numbers units for output (numbers of fish)')
  print(OutputNumbersUnits)
  
  print('# Population biomass units for output (kilograms of fish)')
  print(OutputBiomassUnits)
  
  print('#################################################################################################')
  print('# INITIIALIZING POPULATION CLASS')
  print('#################################################################################################')
  
  print('# Timing of start of year within year: StartZFraction[NPopulation,NArea]')
  print(StartZFraction)
  
  print('# Timing of spawning within year: SpawningZFraction[NPopulation,NArea]')
  print(SpawningZFraction)
  
  print('# Timing of catch within year: CatchZFraction[NPopulation,NFleet]')
  print(CatchZFraction)
  
  print('# Timing of survey within year: SurveyZFraction[NPopulation,NSurvey]')
  print(SurveyZFraction)
  
  print('# Population movement probability arrays: MovementProbability[NPopulation,NArea,NArea]')
  for (i in 1:NPopulation) {
    print('# Movement array for population:')
    print(i)
    print(MovementProbability[i,,])
  }
  
  print('# Population recruitment distribution arrays: RecruitmentDistribution[NPopulation,NArea,NArea]')
  for (i in 1:NPopulation) {
    print('# Recruitment distribution array for population:')
    print(i)
    print(RecruitmentDistribution[i,,])
  }
  
  print('# Population natural mortality arrays: NaturalMortality[NPopulation,NArea,NGender,NAge]')
  for (i in 1:NPopulation) {
    print('# Natural mortality at age array for population:')
    print(i)
    for (j in 1:NArea)
      for (k in 1:NGender) 
        print(NaturalMortality[i,j,k,])
  }
  
  print('# Population maturity parameters: Maturity.a50[NPopulation,NArea,NGender]')
  for (i in 1:NPopulation) {
    print('# Age at 50% maturity array for population:')
    print(i)
    for (j in 1:NArea)
        print(Maturity.a50[i,j,])
  }
  
  print('# Population maturity parameters: Maturity.slope[NPopulation,NArea,NGender]')
  for (i in 1:NPopulation) {
    print('# Maturity slope parameter array for population:')
    print(i)
    for (j in 1:NArea)
      print(Maturity.slope[i,j,])
  }
  
  print('# Population maturity probability at age arrays: MaturityProbabilityAtAge[NPopulation,NArea,NGender,NAge]')
  for (i in 1:NPopulation) {
    print('# Maturity proabability at age array for population:')
    print(i)
    for (j in 1:NArea)
      for (k in 1:NGender) 
        print(MaturityProbabilityAtAge[i,j,k,])  
  }
  
  print('# Population growth parameters: Minimum reference age Length.Amin[NPopulation,NArea,NGender]:')
  for (i in 1:NPopulation) {
    print('# Age at amin (yr) array for population:')
    print(i)
    for (j in 1:NArea)
      print(Length.Amin[i,j,])
  }
  
  print('# Population growth parameters: Maximum reference age Length.Amax[NPopulation,NArea,NGender]:')
  for (i in 1:NPopulation) {
    print('# Age at amax (yr) array for population:')
    print(i)
    for (j in 1:NArea)
      print(Length.Amax[i,j,])
  }
  
  print('# Population growth parameters: Maximum reference age amax:')
  print('# Population growth parameters: Length.Lmin[NPopulation,NArea,NGender]')
  for (i in 1:NPopulation) {
    print('# Length at age amin (cm) array for population:')
    print(i)
    for (j in 1:NArea)
      print(Length.Lmin[i,j,])
  }
  
  print('# Population growth parameters: Length.Lmax[NPopulation,NArea,NGender]')
  for (i in 1:NPopulation) {
    print('# Length at age amax (cm) array for population:')
    print(i)
    for (j in 1:NArea)
      print(Length.Lmax[i,j,])
  }
  
  print('# Population growth parameters: Length.c[NPopulation,NArea,NGender]')
  for (i in 1:NPopulation) {
    print('# Length at age curvature parameter array for population:')
    print(i)
    for (j in 1:NArea)
      print(Length.c[i,j,])
  }
  
  print('# Unfished population mean length at age on January 1st arrays: MeanLengthStartOfYear[NPopulation,NArea,NGender,NAge]')
  for (i in 1:NPopulation) {
    print('# Mean length at age (cm) on January 1st array for population:')
    print(i)
    for (j in 1:NArea)
      for (k in 1:NGender) 
        print(MeanLengthStartOfYear[i,j,k,]) 
  }
  
  print('# Unfished population mean length at age during spawning season arrays: MeanLengthSpawning[NPopulation,NArea,NGender,NAge]')
  for (i in 1:NPopulation) {
    print('# Mean length at age (cm) during spawning season array for population:')
    print(i)
    for (j in 1:NArea)
      for (k in 1:NGender) 
        print(MeanLengthSpawning[i,j,k,]) 
  }
  
  print('# Unfished population mean length at age of fishery catch arrays: MeanLengthCatch[NPopulation,Nfleet,NGender,NAge]')
  for (i in 1:NPopulation) {
    print('# Mean length at age (cm) of fishery catch array for population:')
    print(i)
    for (j in 1:NFleet)
      for (k in 1:NGender) 
        print(MeanLengthCatch[i,j,k,]) 
  }
  
  print('# Unfished population mean length at age during survey arrays: MeanLengthSurvey[NPopulation,NSurvey,NGender,NAge]')
  for (i in 1:NPopulation) {
    print('# Mean length at age (cm) during survey array for population:')
    print(i)
    for (j in 1:NSurvey)
      for (k in 1:NGender) 
        print(MeanLengthSurvey[i,j,k,]) 
  }
  
  print('# Population length-weight parameters: WeightAtLength.A[NPopulation,NArea,NGender]')
  for (i in 1:NPopulation) {
    print('# Length-weight scalar parameter A array for population:')
    print(i)
    for (j in 1:NArea)
      print(WeightAtLength.A[i,j,])
  }
  
  print('# Population length-weight parameters: WeightAtLength.B[NPopulation,NArea,NGender]')
  for (i in 1:NPopulation) {
    print('# Length-weight exponent parameter B array for population:')
    print(i)
    for (j in 1:NArea)
      print(WeightAtLength.B[i,j,])
  }
  
  print('# Unfished population mean weight at age on January 1st arrays: MeanWeightStartOfYear[NPopulation,NArea,NGender,NAge]')
  for (i in 1:NPopulation) {
    print('# Mean weight at age (kg) on January 1st array for population:')
    print(i)
    for (j in 1:NArea)
      for (k in 1:NGender) 
        print(MeanWeightStartOfYear[i,j,k,]) 
  }
  
  print('# Unfished population mean weight at age during spawning season arrays: MeanWeightSpawning[NPopulation,NArea,NGender,NAge]')
  for (i in 1:NPopulation) {
    print('# Mean weight at age (kg) during spawning season array for population:')
    print(i)
    for (j in 1:NArea)
      for (k in 1:NGender) 
        print(MeanWeightSpawning[i,j,k,]) 
  }
  
  print('# Unfished population mean weight at age of fishery catch arrays: MeanWeightCatch[NPopulation,NFleet,NGender,NAge]')
  for (i in 1:NPopulation) {
    print('# Mean weight at age (kg) of fishery catch array for population:')
    print(i)
    for (j in 1:NFleet)
      for (k in 1:NGender) 
        print(MeanWeightCatch[i,j,k,]) 
  }
  
  print('# Unfished population mean weight at age during survey arrays: MeanWeightSurvey[NPopulation,NSurvey,NGender,NAge]')
  for (i in 1:NPopulation) {
    print('# Mean weight at age (kg) during survey array for population:')
    print(i)
    for (j in 1:NSurvey)
      for (k in 1:NGender) 
        print(MeanWeightSurvey[i,j,k,]) 
  }
  
  print('# Units of female spawning biomass (kg) for recruitment process models: Recruitment.SpawningBiomassUnits[NPopulation,NArea]')
  for (i in 1:NPopulation) {
    print('# Units of female spawning biomass (kg) by area array for population:')
    print(i)
    print(Recruitment.SpawningBiomassUnits[i,])
  }

  print('# Fraction of recruits by gender parameters for recruitment process models: Recruitment.GenderFraction[NPopulation,NArea]')
  for (i in 1:NPopulation) {
    print('# Fraction of recruits by gender array for population:')
    print(i)
    for (j in 1:NArea)
      print(Recruitment.GenderFraction[i,j,])
  }
  
  print('# Steepness parameter h for recruitment process models: Recruitment.h[NPopulation,NArea]')
  for (i in 1:NPopulation) {
    print('# Steepness parameter h by area array for population:')
    print(i)
    print(Recruitment.h[i,])
  }
  
  print('# Unfished recruitment parameter for recruitment process models: Recruitment.UnfishedR[NPopulation,NArea]')
  for (i in 1:NPopulation) {
  print('# Unfished recruitment parameter by area array for population:')
  print(i)
  print(Recruitment.UnfishedR[i,])
  }
  
  print('# Logarithm of unfished recruitment parameter for recruitment process models: Recruitment.LogUnfishedR[NPopulation,NArea]')
  for (i in 1:NPopulation) {
    print('# Logarithm of unfished recruitment parameter by area array for population:')
    print(i)
    print(Recruitment.LogUnfishedR[i,])
  }
  
  print('# Standard deviation of log-scale R for recruitment process models: Recruitment.sigma.R[NPopulation,NArea]')
  for (i in 1:NPopulation) {
    print('# Standard deviation of log-scale R parameter by area array for population:')
    print(i)
    print(Recruitment.sigmaR[i,])
  }
  print('#')
  
  print('#################################################################################################')
  print('# INITIIALIZING OBSERVATION CLASS')
  print('#################################################################################################')
  
  print('# Equilibrium fishing mortality by fleet: EquilibriumFishingMortality[NFleet,NArea]')
  for (i in 1:NFleet) {
    print('# Equilibrium fishing mortality by area array for fleet:')
    print(i)
    print(EquilibriumFishingMortality[i,])
  }  
  
  print('# Fishing mortality by year and fleet: FishingMortality[NYear,NFleet,NArea]')
  for (i in 1:NFleet) {
    print('# Fishing mortality by year and area arrays for fleet:')
    print(i)
    for (j in 1:NArea)
      print(FishingMortality[,i,j])
  }   
  
  print('# Fishery selectivity parameters for populations by fleet/area: FisherySelectivity.a50[NPopulation,NFleet,NGender]')
  for (i in 1:NPopulation) {
    print('# Fishery age at 50% selectivity by fleet/area and gender arrays for population:')
    print(i)
    for (j in 1:NFleet)
      print(FisherySelectivity.a50[i,j,])  
  }
  
  print('# Fishery selectivity parameters for populations by fleet/area: FisherySelectivity.slope[NPopulation,Nfleet,NGender]')
  for (i in 1:NPopulation) {
    print('# Fishery selectivity slope parameter by area and gender arrays for population:')
    print(i)
    print(FisherySelectivity.slope[i,,])  
  }
  
  print('# Population fishery selectivity at age arrays: FisherySelectivityAtAge[NPopulation,NFleet,NGender,NAge]')
  for (i in 1:NPopulation) {
    print('# Initial fishery selectivity at age by area and gender arrays for population:')
    print(i)
    for (j in 1:NFleet)
      for (k in 1:NGender) 
        print(FisherySelectivityAtAge[i,j,k,])  
  }
  
  print('# Survey selectivity parameters for populations by fleet/area: SurveySelectivity.a50[NPopulation,NSurvey,NGender]')
  for (i in 1:NPopulation) {
    print('# Survey age at 50% selectivity by fleet/area and gender arrays for population:')
    print(i)
    for (j in 1:NSurvey)
      print(SurveySelectivity.a50[i,j,]) 
  }
  
  print('# Survey selectivity parameters for populations by fleet/area: SurveySelectivity.slope[NPopulation,NSurvey,NGender]')
  for (i in 1:NPopulation) {
    print('# Survey selectivity slope parameter by area and gender arrays for population:')
    print(i)
    print(SurveySelectivity.slope[i,,])  
  }
  
  print('# Population survey selectivity at age arrays: SurveySelectivityAtAge[NPopulation,NSurvey,NGender,NAge]')
  for (i in 1:NPopulation) {
    print('# Initial survey selectivity at age by area and gender arrays for population:')
    print(i)
    for (j in 1:NSurvey)
      for (k in 1:NGender) 
        print(SurveySelectivityAtAge[i,j,k,])  
  }
  print('#')
  
  print('#################################################################################################')
  print('# INITIIALIZING ENVIRONMENT CLASS')
  print('#################################################################################################')
  print('#')
  
  sink()

  return()
}






