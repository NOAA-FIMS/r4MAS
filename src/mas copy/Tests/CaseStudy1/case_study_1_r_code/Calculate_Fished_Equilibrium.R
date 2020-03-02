################################################################################################
# The R program file Calculate_Fished_Equilibrium.R calculates
# the fished equilibrium conditions for case study v1 
# of the MAS project.
# Jon Brodziak, PIFSC, jon.brodziak@noaa.gov 13-Mar-2018
################################################################################################
  # Write unfished equilibrium results to output file
  #-----------------------------------------------------------------
  sink(file=OutputFile,append=TRUE,split=TRUE)
  
# Set equilibrium fishing mortality at age 
#-----------------------------------------------------------------
print('_____________________________________________________________________________________________________')
print('Calculating fished equilibrium population numbers at age and spawning biomasses ...')
print('_____________________________________________________________________________________________________')

# Write unfished equilibrium results to output file
#-----------------------------------------------------------------
sink(file=OutputFile,append=TRUE,type="output")

# ITERATION 1

# Compute initial estimates of fished equilibrium numbers at age 
# and equilibrium spawning biomasses by population, area, and 
# gender where iteration i=1 and age-0 index [a=1]
#-----------------------------------------------------------------
i <- 1
a <- 1
for (p in 1:NPopulation)
  for (d in 1:NArea)
    for (g in 1:NGender)
    {        
      tmp <- 0.0
      for (k in 1:NArea)
      {
        tmp <- tmp + Recruitment.GenderFraction[p,k,g]*RecruitmentDistribution[p,k,d]*Recruitment.UnfishedR[p,k]
      }
      EquilibriumNumbersAtAge[i,p,d,g,a] <- tmp
    }

# Compute initial fished numbers at age by population, area, and gender
# For iteration i=1 and true age indexes [a=2:(NAge-1)]
#-----------------------------------------------------------------
for (p in 1:NPopulation)
  for (d in 1:NArea)
    for (g in 1:NGender)
      for (a in 2:(NAge-1))
      {
        EquilibriumNumbersAtAge[i,p,d,g,a] <- EquilibriumNumbersAtAge[i,p,d,g,(a-1)]*exp(-NaturalMortality[p,d,g,(a-1)]-EquilibriumFishingMortalityAtAge[p,d,g,(a-1)])
      }

# Compute initial fished numbers at age by population, area, and gender
# For iteration i=1 and plus-group age index [a=NAge]
#-----------------------------------------------------------------
for (p in 1:NPopulation)
  for (d in 1:NArea)
    for (g in 1:NGender)
      for (a in NAge:NAge)
      {
        EquilibriumNumbersAtAge[i,p,d,g,a] <- EquilibriumNumbersAtAge[i,p,d,g,(a-1)]*exp(-NaturalMortality[p,d,g,(a-1)]-EquilibriumFishingMortalityAtAge[p,d,g,(a-1)])
        EquilibriumNumbersAtAge[i,p,d,g,a] <- EquilibriumNumbersAtAge[i,p,d,g,a]/(1-exp(-NaturalMortality[p,d,g,a]-EquilibriumFishingMortalityAtAge[p,d,g,a]))
      }

# Compute initial fished spawning biomass by population, area, and gender
# For iteration i=1 and age indexes [a=1:NAge]
#-----------------------------------------------------------------
for (p in 1:NPopulation)
  for (d in 1:NArea)
    for (g in 1:NGender)
    {
      for (a in 1:NAge)
      {
        tmp <- EquilibriumNumbersAtAge[i,p,d,g,a]*MeanWeightSpawning[p,d,g,a]*MaturityProbabilityAtAge[p,d,g,a]*exp(-SpawningZFraction[p,d]*(NaturalMortality[p,d,g,a]+EquilibriumFishingMortalityAtAge[p,d,g,a]))
        EquilibriumSpawningBiomass[i,p,d,g] <- EquilibriumSpawningBiomass[i,p,d,g] + tmp
      }
      # Rescale to Spawning Biomass Units
      EquilibriumSpawningBiomass[i,p,d,g] <- EquilibriumSpawningBiomass[i,p,d,g]/Recruitment.SpawningBiomassUnits[p,d]      
    }

if (VerboseEquilibriumOutput == 1) {
  print('_____________________________________________________________________________________________________')
  print("Iteration:")
  print(i)
  print("Population 1 Female Numbers at Age and Spawning Biomass by Area")
  print(EquilibriumNumbersAtAge[i,1,1,1,]/OutputNumbersUnits)
  print(EquilibriumSpawningBiomass[i,1,1,1]*Recruitment.SpawningBiomassUnits/OutputBiomassUnits)
  print(EquilibriumNumbersAtAge[i,1,2,1,]/OutputNumbersUnits)
  print(EquilibriumSpawningBiomass[i,1,2,1]*Recruitment.SpawningBiomassUnits/OutputBiomassUnits)
  
  print("Population 2 Female Numbers at Age and Spawning Biomass by Area")
  print(EquilibriumNumbersAtAge[i,2,1,1,]/OutputNumbersUnits)
  print(EquilibriumSpawningBiomass[i,2,1,1]*Recruitment.SpawningBiomassUnits/OutputBiomassUnits)
  print(EquilibriumNumbersAtAge[i,2,2,1,]/OutputNumbersUnits)
  print(EquilibriumSpawningBiomass[i,2,2,1]*Recruitment.SpawningBiomassUnits/OutputBiomassUnits)
}

# ITERATION 2

i <- i+1

# Compute recruitment production by population and area for i=2
#-----------------------------------------------------------------
for (p in 1:NPopulation)
  for (d in 1:NArea)
  {
    EquilibriumRecruitmentProduction[p,d] <- Recruitment.BH(EquilibriumSpawningBiomass[(i-1),p,d,1], UnfishedSpawningBiomass[p,d,1], Recruitment.UnfishedR[p,d], Recruitment.h[p,d])
  }

# Compute fished recruitment strength by population, 
# area, and gender for i=2
#-----------------------------------------------------------------
a <- 1
for (p in 1:NPopulation) {
  for (d in 1:NArea)
    for (g in 1:NGender)
    {   
      tmp <- 0.0
      for (k in 1:NArea)
      {
        tmp <- tmp + Recruitment.GenderFraction[p,k,g]*RecruitmentDistribution[p,k,d]*EquilibriumRecruitmentProduction[p,k]
      }
      EquilibriumNumbersAtAge[i,p,d,g,a] <- tmp
    }
}

# Compute fished numbers at age by population, area, and gender
# for i=2 and true age indexes [a=2:(NAge-1)]
#-----------------------------------------------------------------
for (p in 1:NPopulation) {
  for (d in 1:NArea)
    for (g in 1:NGender)
      for (a in 2:(NAge-1))
      {
        tmp <- 0.0
        for (k in 1:NArea)
        {
          tmp <- tmp + MovementProbability[p,k,d]*EquilibriumNumbersAtAge[(i-1),p,k,g,(a-1)]*exp(-NaturalMortality[p,k,g,(a-1)]-EquilibriumFishingMortalityAtAge[p,k,g,(a-1)])
        }
        EquilibriumNumbersAtAge[i,p,d,g,a] <- tmp
      }
}

# Compute fished numbers at age by population, area, and gender
# For iteration i=2 and plus-group age index [a=NAge]
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
        # Age-(A-1) survivors that remained or immigrated to area s
        tmp <- tmp + MovementProbability[p,k,d]*EquilibriumNumbersAtAge[(i-1),p,k,g,(a-1)]*exp(-NaturalMortality[p,k,g,(a-1)]-EquilibriumFishingMortalityAtAge[p,k,g,(a-1)])
        # Age-A survivors that remained or immigrated to area s
        tmp <- tmp + MovementProbability[p,k,d]*EquilibriumNumbersAtAge[(i-1),p,k,g,a]*exp(-NaturalMortality[p,k,g,a]-EquilibriumFishingMortalityAtAge[p,k,g,a])
      }
      EquilibriumNumbersAtAge[i,p,d,g,a] <- tmp
    }
}

# Compute fished spawning biomass by population, area, and gender
# For iteration i=2 and age indexes [a=1:NAge]
#-----------------------------------------------------------------
for (p in 1:NPopulation)
  for (d in 1:NArea)
    for (g in 1:NGender)
    {
      tmp <- 0.0
      for (a in 1:NAge)
      {
        tmp <- tmp + EquilibriumNumbersAtAge[i,p,d,g,a]*MeanWeightSpawning[p,d,g,a]*MaturityProbabilityAtAge[p,d,g,a]*exp(-SpawningZFraction[p,d]*(NaturalMortality[p,d,g,a]+EquilibriumFishingMortalityAtAge[p,d,g,a]))
      }   
      EquilibriumSpawningBiomass[i,p,d,g] <- tmp
      # Rescale to Spawning Biomass Units
      EquilibriumSpawningBiomass[i,p,d,g] <- EquilibriumSpawningBiomass[i,p,d,g]/Recruitment.SpawningBiomassUnits[p,d] 
    }

if (VerboseEquilibriumOutput == 1) {
  print('_____________________________________________________________________________________________________')
  print("Iteration:")
  print(i)
  print("Population 1 Female Numbers at Age and Spawning Biomass by Area")
  print(EquilibriumNumbersAtAge[i,1,1,1,]/OutputNumbersUnits)
  print(EquilibriumSpawningBiomass[i,1,1,1]*Recruitment.SpawningBiomassUnits/OutputBiomassUnits)
  print(EquilibriumNumbersAtAge[i,1,2,1,]/OutputNumbersUnits)
  print(EquilibriumSpawningBiomass[i,1,2,1]*Recruitment.SpawningBiomassUnits/OutputBiomassUnits)
  
  print("Population 2 Female Numbers at Age and Spawning Biomass by Area")
  print(EquilibriumNumbersAtAge[i,2,1,1,]/OutputNumbersUnits)
  print(EquilibriumSpawningBiomass[i,2,1,1]*Recruitment.SpawningBiomassUnits/OutputBiomassUnits)
  print(EquilibriumNumbersAtAge[i,2,2,1,]/OutputNumbersUnits)
  print(EquilibriumSpawningBiomass[i,2,2,1]*Recruitment.SpawningBiomassUnits/OutputBiomassUnits)
}

# BEGIN WHILE LOOP

HasConverged <- 0

while ((i<MaxIteration) && (HasConverged==0))
{
  i <- i+1
  
  # Compute recruitment production by population and area for i
  #-----------------------------------------------------------------
  for (p in 1:NPopulation)
    for (d in 1:NArea)
    {
      EquilibriumRecruitmentProduction[p,d] <- Recruitment.BH(EquilibriumSpawningBiomass[(i-1),p,d,1], UnfishedSpawningBiomass[p,d,1], Recruitment.UnfishedR[p,d], Recruitment.h[p,d])
    }
  
  # Compute fished recruitment strength by population, 
  # area, and gender for i
  #-----------------------------------------------------------------
  a <- 1
  for (p in 1:NPopulation) {
    for (d in 1:NArea)
      for (g in 1:NGender)
      {   
        tmp <- 0.0
        for (k in 1:NArea)
        {
          tmp <- tmp + Recruitment.GenderFraction[p,k,g]*RecruitmentDistribution[p,k,d]*EquilibriumRecruitmentProduction[p,k]
        }
        EquilibriumNumbersAtAge[i,p,d,g,a] <- tmp
      }
  }
  
  # Compute fished numbers at age by population, area, and gender
  # for i and true age indexes [a=2:(NAge-1)]
  #-----------------------------------------------------------------
  for (p in 1:NPopulation) {
    for (d in 1:NArea)
      for (g in 1:NGender)
        for (a in 2:(NAge-1))
        {
          tmp <- 0.0
          for (k in 1:NArea)
          {
            tmp <- tmp + MovementProbability[p,k,d]*EquilibriumNumbersAtAge[(i-1),p,k,g,(a-1)]*exp(-NaturalMortality[p,k,g,(a-1)]-EquilibriumFishingMortalityAtAge[p,k,g,(a-1)])
          }
          EquilibriumNumbersAtAge[i,p,d,g,a] <- tmp
        }
  }
  
  # Compute fished numbers at age by population, area, and gender
  # For iteration i and plus-group age index [a=NAge]
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
          # Age-(A-1) survivors that remained or immigrated to area s
          tmp <- tmp + MovementProbability[p,k,d]*EquilibriumNumbersAtAge[(i-1),p,k,g,(a-1)]*exp(-NaturalMortality[p,k,g,(a-1)]-EquilibriumFishingMortalityAtAge[p,k,g,(a-1)])
          # Age-A survivors that remained or immigrated to area s
          tmp <- tmp + MovementProbability[p,k,d]*EquilibriumNumbersAtAge[(i-1),p,k,g,a]*exp(-NaturalMortality[p,k,g,a]-EquilibriumFishingMortalityAtAge[p,k,g,a])
        }
        EquilibriumNumbersAtAge[i,p,d,g,a] <- tmp
      }
  }
  
  # Compute fished spawning biomass by population, area, and gender
  # For iteration i and age indexes [a=1:NAge]
  #-----------------------------------------------------------------
  for (p in 1:NPopulation)
    for (d in 1:NArea)
      for (g in 1:NGender)
      {
        tmp <- 0.0
        for (a in 1:NAge)
        {
          tmp <- tmp + EquilibriumNumbersAtAge[i,p,d,g,a]*MeanWeightSpawning[p,d,g,a]*MaturityProbabilityAtAge[p,d,g,a]*exp(-SpawningZFraction[p,d]*(NaturalMortality[p,d,g,a]+EquilibriumFishingMortalityAtAge[p,d,g,a]))
        }   
        EquilibriumSpawningBiomass[i,p,d,g] <- tmp
        # Rescale to Spawning Biomass Units
        EquilibriumSpawningBiomass[i,p,d,g] <- EquilibriumSpawningBiomass[i,p,d,g]/Recruitment.SpawningBiomassUnits[p,d] 
      }
  
  # Compute L1 distance between iterates of unfished spawning
  # biomass by population, area, and gender and check for convergence
  #-----------------------------------------------------------------
  Distance <- 0.0
  
  for (p in 1:NPopulation)
    for (d in 1:NArea)
      for (g in 1:NGender)
      {
        Distance <- Distance + abs(EquilibriumSpawningBiomass[i,p,d,g]-EquilibriumSpawningBiomass[(i-1),p,d,g])
      }
  
  if ((i %% 10 == 0) && (VerboseEquilibriumOutput == 1))
  {
    print('_____________________________________________________________________________________________________')
    print("Iteration:")
    print(i)
    print("Distance:")
    print(Distance)
    print("Population 1 Fished Equilibrium Female Numbers at Age and Spawning Biomass by Area")
    print(EquilibriumNumbersAtAge[i,1,1,1,]/OutputNumbersUnits)
    print(EquilibriumSpawningBiomass[i,1,1,1]*Recruitment.SpawningBiomassUnits/OutputBiomassUnits)
    print(EquilibriumNumbersAtAge[i,1,2,1,]/OutputNumbersUnits)
    print(EquilibriumSpawningBiomass[i,1,2,1]*Recruitment.SpawningBiomassUnits/OutputBiomassUnits)
    
    print("Population 2 Fished Equilibrium /OutputNumbersUnitsFemale Numbers at Age and Spawning Biomass by Area")
    print(EquilibriumNumbersAtAge[i,2,1,1,]/OutputNumbersUnits)
    print(EquilibriumSpawningBiomass[i,2,1,1]*Recruitment.SpawningBiomassUnits/OutputBiomassUnits)
    print(EquilibriumNumbersAtAge[i,2,2,1,]/OutputNumbersUnits)
    print(EquilibriumSpawningBiomass[i,2,2,1]*Recruitment.SpawningBiomassUnits/OutputBiomassUnits)
  }  
  
  if (Distance < EquilibriumConvergenceCriterion)
  {
    HasConverged <- 1
    FinalIteration <- i
    print('_____________________________________________________________________________________________________')
    print('Calculation of fished equilibrium population numbers at age and spawning biomasses has converged at iteration:')
    print(FinalIteration)
  }
  
}

# END WHILE LOOP

# Dimension arrays
#-----------------------------------------------------------------
FishedEquilibriumNumbersAtAge <- array(rep(0.0,DimPopulationAreaGenderAge),c(NPopulation,NArea,NGender,NAge)) 
FishedEquilibriumSpawningBiomass <- array(rep(0.0,DimPopulationAreaGender),c(NPopulation,NArea,NGender))

# Set fished equilibrium population numbers and spawning
# biomass by population, area, and gender
#-----------------------------------------------------------------
FishedEquilibriumNumbersAtAge <- EquilibriumNumbersAtAge[FinalIteration,,,,]
FishedEquilibriumSpawningBiomass <- EquilibriumSpawningBiomass[FinalIteration,,,]

print('_____________________________________________________________________________________________________')
print('Fished Equilibrium Results')
print('_____________________________________________________________________________________________________')
print('Population 1 Fished Equilibrium Female Numbers at Age in Area 1')
print(FishedEquilibriumNumbersAtAge[1,1,1,]/OutputNumbersUnits)
print('Population 1 Fished Equilibrium Female Spawning Biomass in Area 1')
print(FishedEquilibriumSpawningBiomass[1,1,1]*Recruitment.SpawningBiomassUnits/OutputBiomassUnits)
print('_____________________________________________________________________________________________________')
print('Population 1 Fished Equilibrium Female Numbers at Age in Area 2')
print(FishedEquilibriumNumbersAtAge[1,2,1,]/OutputNumbersUnits)
print('Population 1 Fished Equilibrium Female Spawning Biomass in Area 2')
print(FishedEquilibriumSpawningBiomass[1,2,1]*Recruitment.SpawningBiomassUnits/OutputBiomassUnits)
print('_____________________________________________________________________________________________________')
print('Population 2 Fished Equilibrium Female Numbers at Age in Area 1')
print(FishedEquilibriumNumbersAtAge[2,1,1,]/OutputNumbersUnits)
print('Population 2 Fished Equilibrium Female Spawning Biomass in Area 1')
print(FishedEquilibriumSpawningBiomass[2,1,1]*Recruitment.SpawningBiomassUnits/OutputBiomassUnits)
print('_____________________________________________________________________________________________________')
print('Population 2 Fished Equilibrium Female Numbers at Age in Area 2')
print(FishedEquilibriumNumbersAtAge[2,2,1,]/OutputNumbersUnits)
print('Population 2 Fished Equilibrium Female Spawning Biomass in Area 2')
print(FishedEquilibriumSpawningBiomass[2,2,1]*Recruitment.SpawningBiomassUnits/OutputBiomassUnits)
print('_____________________________________________________________________________________________________')

sink()
