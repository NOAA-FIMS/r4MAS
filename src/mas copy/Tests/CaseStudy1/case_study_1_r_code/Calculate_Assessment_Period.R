################################################################################################
# The R program file Calculate_Assessment_Period.R calculates
# the fishery system conditions during the assessment time horizon 
# for case study v1 of the MAS project.
# Jon Brodziak, PIFSC, jon.brodziak@noaa.gov 13-Mar-2018
################################################################################################

# (2.3.1) SET INITIAL POPULATION NUMBERS AT AGE BY POPULATION, AREA, AND GENDER ON JANUARY 1ST
#-----------------------------------------------------------------------------------------------
NumbersAtAge[1,,,,] <- FishedEquilibriumNumbersAtAge

################################################################################################
# LOOP OVER ASSESSMENT TIME HORIZON TO COMPUTE NUMBERS AT AGE
# BY POPULATION, AREA, AND GENDER, AND OTHER QOI
#-----------------------------------------------------------------------------------------------
print('_____________________________________________________________________________________________________')
print('Calculating population numbers at age and spawning biomasses by area through assessment time horizon ...')
print('_____________________________________________________________________________________________________')
for (y in 1:NYear)
{
  
  # Store mean lengths and weights for each year in the assessment time horizon
  #-----------------------------------------------------------------------------------------------
  AssessmentMeanLengthStartOfYear[y,,,,] <- MeanLengthStartOfYear
  AssessmentMeanLengthSpawning[y,,,,] <- MeanLengthSpawning
  AssessmentMeanLengthCatch[y,,,,] <- MeanLengthCatch
  AssessmentMeanLengthSurvey[y,,,,] <- MeanLengthSurvey
  AssessmentMeanWeightStartOfYear[y,,,,] <- MeanWeightStartOfYear 
  AssessmentMeanWeightSpawning[y,,,,] <- MeanWeightSpawning
  AssessmentMeanWeightCatch[y,,,,] <- MeanWeightCatch
  AssessmentMeanWeightSurvey[y,,,,] <- MeanWeightSurvey
  
  # (2.3.2) CALCULATE FISHERY SELECTIVITY FOR YEAR Y
  #-----------------------------------------------------------------------------------------------
  FisherySelectivityAtAge <- EquilibriumFisherySelectivityAtAge
  
  # (2.3.3) CALCULATE SURVEY SELECTIVITY FOR YEAR Y
  #-----------------------------------------------------------------------------------------------
  SurveySelectivityAtAge <- SurveySelectivityAtAge
  
  # (2.3.4) CALCULATE TOTAL MORTALITY FOR YEAR y
  # ----------------------------------------------------------------------------------------------
  for (p in 1:NPopulation)
    for (d in 1:NArea)
      for (g in 1:NGender)
        for (a in 1:NAge)
        {          
          TotalMortality[y,p,d,g,a] <- NaturalMortality[p,d,g,a]
          for (v in 1:NFleet)
            TotalMortality[y,p,FleetArea[v],g,a] <- TotalMortality[y,p,FleetArea[v],g,a]+FishingMortality[y,FleetArea[v],d]*FisherySelectivityAtAge[p,FleetArea[v],g,a]
        }
  
  # (2.3.5) CALCULATE SPAWNING NUMBERS AT AGE AND SPAWNING BIOMASS FOR YEAR y
  # ----------------------------------------------------------------------------------------------
  for (p in 1:NPopulation)
    for (d in 1:NArea)
      for (g in 1:NGender)
        for (a in 1:NAge)
          SpawningNumbersAtAge[p,d,g,a] <- exp(-SpawningZFraction[p,d]*TotalMortality[y,p,d,g,a])*NumbersAtAge[y,p,d,g,a]
  
  for (p in 1:NPopulation)
    for (d in 1:NArea)
      for (g in 1:NGender)
      {
        tmp <- 0.0
        for (a in 1:NAge)
        {
          tmp <- tmp + SpawningNumbersAtAge[p,d,g,a]*MeanWeightSpawning[p,d,g,a]*MaturityProbabilityAtAge[p,d,g,a]
        }
        SpawningBiomass[y,p,d,g] <- tmp
        
        # Rescale to Spawning Biomass Units
        SpawningBiomass[y,p,d,g] <- SpawningBiomass[y,p,d,g]/Recruitment.SpawningBiomassUnits[p,d] 
      }
  
  # (2.3.6) CALCULATE RECRUITMENT BY POPULATION, AREA, AND GENDER FOR YEAR y
  # ----------------------------------------------------------------------------------------------
  # Compute recruitment production by population and area for year=y
  #-----------------------------------------------------------------
  for (p in 1:NPopulation)
    for (d in 1:NArea)
    {
      RecruitmentProduction[y,p,d] <- Recruitment.BH(SpawningBiomass[y,p,d,1], UnfishedSpawningBiomass[p,d,1], Recruitment.UnfishedR[p,d], Recruitment.h[p,d])
    }
  
  # Compute annual recruitment strength by population, 
  # area, and gender for year=y and age-0 index a=1
  #-----------------------------------------------------------------
  a <- 1
  for (p in 1:NPopulation) {
    for (d in 1:NArea)
      for (g in 1:NGender)
      {   
        tmp <- 0.0
        for (k in 1:NArea)
        {
          tmp <- tmp + Recruitment.GenderFraction[p,k,g]*RecruitmentDistribution[p,k,d]*RecruitmentProduction[y,p,k]
        }
        Recruitment[y,p,d,g] <- tmp
      }
  }
  
  # Update NumbersAtAge at age-0 index (a=1)
  # by population, area, and gender for year=y
  #-----------------------------------------------------------------
  for (p in 1:NPopulation)
    for (d in 1:NArea)
      for (g in 1:NGender)
        for (a in 1:NAge)
          NumbersAtAge[y,p,d,g,1] <- Recruitment[y,p,d,g]
  
  # (2.3.7) CALCULATE FISHERY OBSERVATIONS
  # CALCULATE FISHERY CATCH NUMBERS AT AGE FOR YEAR y
  # BY POPULATION, FLEET, AREA, AND GENDER
  # ----------------------------------------------------------------------------------------------
  for (p in 1:NPopulation)
    for (v in 1:NFleet)
      for (d in 1:NArea)
        if (FleetArea[v]==d)
          for (g in 1:NGender)
            for (a in 1:NAge)
            {
              FisheryCatchNumbersAtAgeByPopulation[y,p,FleetArea[v],d,g,a] <- NumbersAtAge[y,p,d,g,a]*FishingMortality[y,FleetArea[v],d]*FisherySelectivityAtAge[p,FleetArea[v],g,a]
              FisheryCatchNumbersAtAgeByPopulation[y,p,v,d,g,a] <-  FisheryCatchNumbersAtAgeByPopulation[y,p,FleetArea[v],d,g,a]*(1.0-exp(-TotalMortality[y,p,d,g,a]))/TotalMortality[y,p,d,g,a]
            }
  
  # CALCULATE FISHERY CATCH PROPORTIONS AT AGE FOR YEAR y
  # BY POPULATION, FLEET, AREA, AND GENDER
  # ----------------------------------------------------------------------------------------------
  for (p in 1:NPopulation)
    for (v in 1:NFleet)
      for (d in 1:NArea)
        if (FleetArea[v]==d)
          for (g in 1:NGender)
          {
            tmp <- sum(FisheryCatchNumbersAtAgeByPopulation[y,p,FleetArea[v],d,g,])
            for (a in 1:NAge)
            {
              FisheryCatchProportionAtAgeByPopulation[y,p,FleetArea[v],d,g,a] <- FisheryCatchNumbersAtAgeByPopulation[y,p,FleetArea[v],d,g,a]/tmp
            }
          }
  
  # CALCULATE FISHERY CATCH BIOMASS AT AGE FOR YEAR y
  # BY POPULATION, FLEET, AREA, AND GENDER
  # ----------------------------------------------------------------------------------------------
  for (p in 1:NPopulation)
    for (v in 1:NFleet)
      for (d in 1:NArea)
        if (FleetArea[v]==d)
          for (g in 1:NGender)
            for (a in 1:NAge)
            {
              tmp <- 0.0
              FisheryCatchBiomassAtAgeByPopulation[y,p,FleetArea[v],d,g,a] <- FisheryCatchNumbersAtAgeByPopulation[y,p,FleetArea[v],d,g,a]*MeanWeightCatch[p,FleetArea[v],g,a]
            }
  
  # (2.3.8) CALCULATE SURVEY OBSERVATIONS
  # CALCULATE SURVEY CATCH NUMBERS AT AGE
  # BY YEAR, POPULATION, AREA, AND GENDER
  # ----------------------------------------------------------------------------------------------
  for (p in 1:NPopulation)
    for (I in 1:NSurvey)
      for (g in 1:NGender)
        for (a in 1:NAge)
          SurveyCatchNumbersAtAgeByPopulation[y,p,SurveyArea[I],g,a] <- SurveyCatchability[p,SurveyArea[I]]*SurveySelectivityAtAge[p,SurveyArea[I],g,a]*exp(-SurveyZFraction[p,SurveyArea[I]]*TotalMortality[y,p,SurveyArea[I],g,a])*NumbersAtAge[y,p,SurveyArea[I],g,a]
  
  # CALCULATE SURVEY CATCH PROPORTIONS AT AGE FOR YEAR y
  # BY AREA, AND GENDER
  # ----------------------------------------------------------------------------------------------
  for (p in 1:NPopulation)
    for (I in 1:NSurvey)
      for (g in 1:NGender)
      {
        tmp <- sum(SurveyCatchNumbersAtAgeByPopulation[y,p,SurveyArea[I],g,])
        for (a in 1:NAge)
        {
          SurveyCatchProportionAtAgeByPopulation[y,p,SurveyArea[I],g,a] <- SurveyCatchNumbersAtAgeByPopulation[y,p,SurveyArea[I],g,a]/tmp
        }
      }
  
  # CALCULATE SURVEY CATCH BIOMASS AT AGE FOR YEAR y
  # BY POPULATION, AREA, AND GENDER
  # ----------------------------------------------------------------------------------------------
  for (p in 1:NPopulation)
    for (I in 1:NSurvey)
      for (g in 1:NGender)
        for (a in 1:NAge)
        {
          tmp <- 0.0
          SurveyCatchBiomassAtAgeByPopulation[y,p,SurveyArea[I],g,a] <- SurveyCatchNumbersAtAgeByPopulation[y,p,SurveyArea[I],g,a]*MeanWeightSurvey[p,SurveyArea[I],g,a]
        }
  
  # (2.3.9) CALCULATE QUANTITIES OF INTEREST
  # CALCULATE FISHERY CATCH NUMBERS AT AGE FOR YEAR y
  # BY FLEET, AREA, AND GENDER
  # ----------------------------------------------------------------------------------------------
  for (v in 1:NFleet)
    for (d in 1:NArea)
      if (FleetArea[v]==d)
        for (g in 1:NGender)
          for (a in 1:NAge)
          {
            FisheryCatchNumbersAtAge[y,FleetArea[v],d,g,a] <-  sum(FisheryCatchNumbersAtAgeByPopulation[y,,FleetArea[v],d,g,a])
          }
  
  # CALCULATE FISHERY CATCH PROPORTIONS AT AGE FOR YEAR y
  # BY FLEET, AREA, AND GENDER
  # ----------------------------------------------------------------------------------------------
  for (v in 1:NFleet)
    for (d in 1:NArea)
      if (FleetArea[v]==d)
        for (g in 1:NGender)
        {
          tmp <- sum(FisheryCatchNumbersAtAge[y,FleetArea[v],d,g,])
          for (a in 1:NAge)
          {
            FisheryCatchProportionAtAge[y,FleetArea[v],d,g,a] <- FisheryCatchNumbersAtAge[y,FleetArea[v],d,g,a]/tmp
          }
        }
  
  # CALCULATE FISHERY CATCH BIOMASS FOR YEAR y
  # BY FLEET AND AREA
  # ----------------------------------------------------------------------------------------------
  for (v in 1:NFleet)
    for (d in 1:NArea)
      if (FleetArea[v]==d)
        FisheryCatchBiomass[y,FleetArea[v],d] <- sum(FisheryCatchBiomassAtAgeByPopulation[y,,FleetArea[v],d,,])
  
  # CALCULATE SURVEY CATCH NUMBERS AT AGE FOR YEAR y
  # BY AREA, AND GENDER
  # ----------------------------------------------------------------------------------------------
  for (I in 1:NSurvey)
    for (g in 1:NGender)
      for (a in 1:NAge)
      {
        SurveyCatchNumbersAtAge[y,SurveyArea[I],g,a] <-  sum(SurveyCatchNumbersAtAgeByPopulation[y,,SurveyArea[I],g,a])
      }
  
  # CALCULATE SURVEY CATCH PROPORTIONS AT AGE FOR YEAR y
  # BY AREA, AND GENDER
  # ----------------------------------------------------------------------------------------------
  for (I in 1:NSurvey)
    for (g in 1:NGender)
    {
      tmp <- sum(SurveyCatchNumbersAtAge[y,SurveyArea[I],g,])
      for (a in 1:NAge)
      {
        SurveyCatchProportionAtAge[y,SurveyArea[I],g,a] <- SurveyCatchNumbersAtAge[y,SurveyArea[I],g,a]/tmp
      }
    }
  
  # CALCULATE SURVEY CATCH BIOMASS FOR YEAR y
  # BY AREA
  # ----------------------------------------------------------------------------------------------
  for (I in 1:NSurvey)
    SurveyCatchBiomass[y,SurveyArea[I]] <- sum(SurveyCatchBiomassAtAgeByPopulation[y,,SurveyArea[I],,])
  
  # (2.3.10) CALCULATE NUMBERS AT AGE IN YEAR Y+1
  # Compute numbers at age by population, area, and gender
  # for true ages [a=2:(NAge-1)]
  #-----------------------------------------------------------------
  for (p in 1:NPopulation) {
    for (d in 1:NArea)
      for (g in 1:NGender)
        for (a in 2:(NAge-1))
        {
          tmp <- 0.0
          for (k in 1:NArea)
          {
            tmp <- tmp + MovementProbability[p,k,d]*NumbersAtAge[y,p,k,g,(a-1)]*exp(-TotalMortality[y,p,k,g,(a-1)])
          }
          NumbersAtAge[(y+1),p,d,g,a] <- tmp
        }
  }
  
  # Compute fished numbers at age by population, area, and gender
  # For the plus-group age  [a=NAge]
  #-----------------------------------------------------------------
  a <- NAge
  
  for (p in 1:NPopulation) 
  {
    for (d in 1:NArea)
      for (g in 1:NGender)
      { 
        tmp <- 0.0
        for (k in 1:NArea)
        {
          # Age-(A-1) survivors that remained or immigrated to area d
          tmp <- tmp + MovementProbability[p,k,d]*NumbersAtAge[y,p,k,g,(a-1)]*exp(-TotalMortality[y,p,k,g,(a-1)])
          # Age-A survivors that remained or immigrated to area d
          tmp <- tmp + MovementProbability[p,k,d]*NumbersAtAge[y,p,k,g,a]*exp(-TotalMortality[y,p,k,g,a])
        }
        NumbersAtAge[(y+1),p,d,g,a] <- tmp
      }
  }
 
  # Update mean lengths and weights of the plus group
  #-----------------------------------------------------------------
  if (y > 1) 
  {
    # Calculate time-varying mean lengths at age 
    # for the plus group using the MaxPlusGroupAge adjustment
    #-----------------------------------------------------------------
    for (p in 1:NPopulation)
      for (d in 1:NArea)
        for (g in 1:NGender) 
        {
          tmp1 <- NumbersAtAge[(y-1),p,d,g,(NAge-1)]*exp(-TotalMortality[(y-1),p,d,g,(NAge-1)])
          tmp2 <- NumbersAtAge[(y-1),p,d,g,NAge]*exp(-TotalMortality[(y-1),p,d,g,NAge])
          tmp3 <- tmp1*UnfishedMeanLengthStartOfYear[p,d,g,NAge]+tmp2*MeanLengthStartOfYear[p,d,g,NAge]
          MeanLengthStartOfYear[p,d,g,NAge] <- tmp3/(tmp1+tmp2)
          tmp3 <- tmp1*UnfishedMeanLengthSpawning[p,d,g,NAge]+tmp2*MeanLengthSpawning[p,d,g,NAge]
          MeanLengthSpawning[p,d,g,NAge] <- tmp3/(tmp1+tmp2)
          tmp3 <- tmp1*UnfishedMeanLengthSpawning[p,d,g,NAge]+tmp2*MeanLengthSpawning[p,d,g,NAge]
          MeanLengthSpawning[p,d,g,NAge] <- tmp3/(tmp1+tmp2)
          tmp3 <- tmp1*UnfishedMeanLengthCatch[p,d,g,NAge]+tmp2*MeanLengthCatch[p,d,g,NAge]
          MeanLengthCatch[p,d,g,NAge] <- tmp3/(tmp1+tmp2) 
          tmp3 <- tmp1*UnfishedMeanLengthSurvey[p,d,g,NAge]+tmp2*MeanLengthSurvey[p,d,g,NAge]
          MeanLengthSurvey[p,d,g,NAge] <- tmp3/(tmp1+tmp2) 
       }

    # Calculate time-varying mean weights at age 
    # for the plus group using the MaxPlusGroupAge adjustment
    #-----------------------------------------------------------------
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
  }

}
# END OF FOR LOOP OVER TIME

