################################################################################################
# The R script Write_Output_Data.R writes the output data and
# results of assessment period calculations for case study v1 
# of the MAS project to user-named OutputFile.
# Jon Brodziak, PIFSC, jon.brodziak@noaa.gov 13-Mar-2018
################################################################################################
print('_____________________________________________________________________________________________________')
print('Writing output data to output file ...')
print('_____________________________________________________________________________________________________')

sink(file=OutputFile,append=TRUE,type="output")

print('_____________________________________________________________________________________________________')
print('CASE STUDY 1 RESULTS')
print('_____________________________________________________________________________________________________')
#-----------------------------------------------------------------------------------------------
# (3.1.1)  FISHERY SELECTIVITY
#-----------------------------------------------------------------------------------------------
print('_____________________________________________________________________________________________________')
print('(3.1.1) FISHERY SELECTIVITY (Population Fleet Gender(1=Females)) AT AGE')
print('_____________________________________________________________________________________________________')
for (p in 1:NPopulation)
  for (v in 1:NFleet)
    for (g in 1:NGender)
    {
      print(c("Population=",p,"Fleet=",v,"Gender=",g,"By Age",FisherySelectivityAtAge[p,FleetArea[v],g,]))
    }
print('_____________________________________________________________________________________________________')

#-----------------------------------------------------------------------------------------------
# (3.1.2)  SURVEY SELECTIVITY
#-----------------------------------------------------------------------------------------------
print('(3.1.2) SURVEY SELECTIVITY (Population Survey Gender(1=Females)) AT AGE')
print('_____________________________________________________________________________________________________')
for (p in 1:NPopulation)
  for (I in 1:NSurvey)
    for (g in 1:NGender)
    {
      print(c("Population=",p,"Survey=",I,"Gender=",g,"By Age",SurveySelectivityAtAge[p,SurveyArea[I],g,]))
    }
print('_____________________________________________________________________________________________________')

#-----------------------------------------------------------------------------------------------
# (3.1.3)  FISHING MORTALITY
#-----------------------------------------------------------------------------------------------
print('(3.1.3) FISHING MORTALITY (Year Fleet Area_A Area_B)')
print('_____________________________________________________________________________________________________')
for (v in 1:NFleet)
  for (y in 1:NYear)
  {
    print(c("Year=",y,"Fleet=",v,"By Area",FishingMortality[y,FleetArea[v],]))
  }
print('_____________________________________________________________________________________________________')

#-----------------------------------------------------------------------------------------------
# (3.1.4)  TOTAL MORTALITY
#-----------------------------------------------------------------------------------------------
print('(3.1.4) TOTAL MORTALITY (Year Population Area/Fleet Gender(1=Females) AT AGE)')
print('_____________________________________________________________________________________________________')
for (p in 1:NPopulation)
  for (d in 1:NArea)
    for (g in 1:NGender)
      for (y in 1:NYear)
      {
        print(c("Year=",y,"Population=",p,"Area=",d,"Gender=",g,"By Age",TotalMortality[y,p,d,g,]))
      }
print('_____________________________________________________________________________________________________')

#-----------------------------------------------------------------------------------------------
# (3.1.5)  FEMALE SPAWNING BIOMASS
#-----------------------------------------------------------------------------------------------
print('(3.1.5) FEMALE SPAWNING BIOMASS (Population Area/Fleet Year')
print('_____________________________________________________________________________________________________')
for (p in 1:NPopulation)
  for (d in 1:NArea)
    for (y in 1:NYear)
    {
      print(c("Population=",p,"Area=",d,"Year=",y,SpawningBiomass[y,p,d,1]*Recruitment.SpawningBiomassUnits/OutputBiomassUnits))
    }
print('_____________________________________________________________________________________________________')

#-----------------------------------------------------------------------------------------------
# (3.1.6)  RECRUITMENT
#-----------------------------------------------------------------------------------------------
print('(3.1.6) RECRUITMENT (Population Area/Fleet Year')
print('_____________________________________________________________________________________________________')
for (p in 1:NPopulation)
  for (d in 1:NArea)
    for (y in 1:NYear)
    {
      print(c("Population=",p,"Area=",d,"Year=",y,(Recruitment[y,p,d,1]+Recruitment[y,p,d,2]/OutputNumbersUnits)))
    }
print('_____________________________________________________________________________________________________')

#-----------------------------------------------------------------------------------------------
# (3.1.7)  TOTAL MORTALITY
#-----------------------------------------------------------------------------------------------
print('(3.1.7) TOTAL MORTALITY (Year Population Area/Fleet Gender(1=Females) AT AGE)')
print('_____________________________________________________________________________________________________')
for (p in 1:NPopulation)
  for (d in 1:NArea)
    for (g in 1:NGender)
      for (y in 1:NYear)
      {
        print(c("Year=",y,"Population=",p,"Area=",d,"Gender=",g,"By Age",TotalMortality[y,p,d,g,]))
      }
print('_____________________________________________________________________________________________________')


#-----------------------------------------------------------------------------------------------
# (3.1.8)  FISHERY OBSERVATIONS
#-----------------------------------------------------------------------------------------------
print('(3.1.8) FISHERY CATCH NUMBERS AT AGE (Year Population Fleet Gender(1=Females))')
print('_____________________________________________________________________________________________________')
for (p in 1:NPopulation)
  for (v in 1:NFleet)
    for (d in 1:NArea)
      if (FleetArea[v]==d)
        for (g in 1:NGender)
          for (y in 1:NYear)
          {
            print(c("Year=",y,"Population=",p,"Fleet=",v,"Gender=",g,"By Age",FisheryCatchNumbersAtAgeByPopulation[y,p,FleetArea[v],d,g,]/OutputNumbersUnits))
          }
print('_____________________________________________________________________________________________________')
print('(3.1.8) FISHERY CATCH PROPORTION AT AGE (Year Population Area/Fleet Gender(1=Females))')
print('_____________________________________________________________________________________________________')
for (p in 1:NPopulation)
  for (v in 1:NFleet)
    for (d in 1:NArea)
      if (FleetArea[v]==d)
        for (g in 1:NGender)
          for (y in 1:NYear)
          {
            print(c("Year=",y,"Population=",p,"Fleet=",v,"Gender=",g,"By Age",FisheryCatchProportionAtAgeByPopulation[y,p,FleetArea[v],d,g,]))
          }
print('_____________________________________________________________________________________________________')
print('(3.1.8) FISHERY CATCH BIOMASS AT AGE (Year Population Area/Fleet Gender(1=Females))')
print('_____________________________________________________________________________________________________')
for (p in 1:NPopulation)
  for (v in 1:NFleet)
    for (d in 1:NArea)
      if (FleetArea[v]==d)
        for (g in 1:NGender)
          for (y in 1:NYear)
          {
            print(c("Year=",y,"Population=",p,"Fleet=",v,"Gender=",g,"By Age",FisheryCatchBiomassAtAgeByPopulation[y,p,FleetArea[v],d,g,]/OutputBiomassUnits))
          }
print('_____________________________________________________________________________________________________')


#-----------------------------------------------------------------------------------------------
# (3.1.9)  SURVEY OBSERVATIONS
#-----------------------------------------------------------------------------------------------
print('(3.1.9) SURVEY CATCH NUMBERS AT AGE (Year Population Area Gender(1=Females))')
print('_____________________________________________________________________________________________________')
for (p in 1:NPopulation)
  for (I in 1:NSurvey)
    for (g in 1:NGender)
      for (y in 1:NYear)
      {
        print(c("Year=",y,"Population=",p,"Survey=",I,"Gender=",g,"By Age",SurveyCatchNumbersAtAgeByPopulation[y,p,SurveyArea[I],g,]))
      }
print('_____________________________________________________________________________________________________')
print('(3.1.9) SURVEY CATCH PROPORTION AT AGE (Year Population Area Gender(1=Females))')
print('_____________________________________________________________________________________________________')
for (p in 1:NPopulation)
  for (I in 1:NSurvey)
    for (g in 1:NGender)
      for (y in 1:NYear)
      {
        print(c("Year=",y,"Population=",p,"Survey=",I,"Gender=",g,"By Age",SurveyCatchProportionAtAgeByPopulation[y,p,SurveyArea[I],g,]))
      }
print('_____________________________________________________________________________________________________')
print('(3.1.9) SURVEY CATCH BIOMASS AT AGE (Year Population Area Gender(1=Females))')
print('_____________________________________________________________________________________________________')
for (p in 1:NPopulation)
  for (I in 1:NSurvey)
    for (g in 1:NGender)
      for (y in 1:NYear)
      {
        print(c("Year=",y,"Population=",p,"Survey=",I,"Gender=",g,"By Age",SurveyCatchBiomassAtAgeByPopulation[y,p,SurveyArea[I],g,]))
      }
print('_____________________________________________________________________________________________________')

#-----------------------------------------------------------------------------------------------
# (3.1.10)  QUANTITIES OF INTEREST
#-----------------------------------------------------------------------------------------------
print('(3.1.10) FISHERY CATCH NUMBERS AT AGE (Year Area/Fleet Gender(1=Females))')
print('_____________________________________________________________________________________________________')
for (v in 1:NFleet)
  for (d in 1:NArea)
    if (FleetArea[v]==d)
      for (g in 1:NGender)
        for (y in 1:NYear)
        {
          print(c("Year=",y,"Fleet=",v,"Gender=",g,"By Age",FisheryCatchNumbersAtAge[y,FleetArea[v],d,g,]/OutputNumbersUnits))
        }
print('_____________________________________________________________________________________________________')
print('(3.1.10) FISHERY CATCH PROPORTION AT AGE (Year Fleet Gender(1=Females))')
print('_____________________________________________________________________________________________________')
print('_____________________________________________________________________________________________________')
for (v in 1:NFleet)
  for (d in 1:NArea)
    if (FleetArea[v]==d)
      for (g in 1:NGender)
        for (y in 1:NYear)
        {
          print(c("Year=",y,"Area/Fleet=",v,"Gender=",g,"By Age",FisheryCatchProportionAtAge[y,FleetArea[v],d,g,]))
        }
print('_____________________________________________________________________________________________________')
print('(3.1.10) FISHERY CATCH BIOMASS (Year Area/Fleet)')
print('_____________________________________________________________________________________________________')
for (v in 1:NFleet)
  if (FleetArea[v]==d)
    for (d in 1:NArea)
      for (y in 1:NYear)
      {
        print(c("Year=",y,"Fleet=",v,FisheryCatchBiomass[y,FleetArea[v],d]/OutputBiomassUnits))
      }
print('_____________________________________________________________________________________________________')
print('(3.1.10) SURVEY CATCH NUMBERS AT AGE (Year Area Gender(1=Females))')
print('_____________________________________________________________________________________________________')
for (I in 1:NSurvey)
  for (g in 1:NGender)
    for (y in 1:NYear)
    {
      print(c("Year=",y,"Survey=",I,"Gender=",g,"By Age",SurveyCatchNumbersAtAge[y,SurveyArea[I],g,]))
    }
print('_____________________________________________________________________________________________________')
print('(3.1.10) SURVEY CATCH PROPORTION AT AGE (Year Area Gender(1=Females))')
print('_____________________________________________________________________________________________________')
for (I in 1:NSurvey)
  for (g in 1:NGender)
    for (y in 1:NYear)
    {
      print(c("Year=",y,"Survey=",I,"Gender=",g,"By Age",SurveyCatchProportionAtAge[y,SurveyArea[I],g,]))
    }
print('_____________________________________________________________________________________________________')
print('(3.1.10) SURVEY CATCH BIOMASS (Year Area)')
print('_____________________________________________________________________________________________________')
for (I in 1:NSurvey)
  for (y in 1:NYear)
  {
    print(c("Year=",y,"Survey=",I,SurveyCatchBiomass[y,SurveyArea[I]]))
  }
print('_____________________________________________________________________________________________________')

#-----------------------------------------------------------------------------------------------
# (3.1.11)  POPULATION NUMBERS AT AGE ON JANUARY 1ST BY YEAR
#-----------------------------------------------------------------------------------------------
print('(3.1.11) POPULATION NUMBERS AT AGE  ON JANUARY 1ST BY YEAR (Year Population Area Gender(1=Females))')
print('_____________________________________________________________________________________________________')
for (p in 1:NPopulation)
  for (d in 1:NArea)
    for (g in 1:NGender)
      for (y in 1:NYear)
      {
        print(c("Year=",y,"Population=",p,"Area=",d,"Gender=",g,"By Age",NumbersAtAge[y,p,d,g,]/OutputNumbersUnits))
      }
print('_____________________________________________________________________________________________________')


#-----------------------------------------------------------------------------------------------
# (3.1.12)  POPULATION MEAN LENGTHS AT AGE ON JANUARY 1ST BY YEAR
#-----------------------------------------------------------------------------------------------
print('(3.1.12) POPULATION MEAN LENGTHS AT AGE ON JANUARY 1ST BY YEAR (Year Population Area Gender(1=Females))')
print('_____________________________________________________________________________________________________')
for (p in 1:NPopulation)
  for (d in 1:NArea)
    for (g in 1:NGender)
      for (y in 1:NYear)
      {
        print(c("Year=",y,"Population=",p,"Area=",d,"Gender=",g,"By Age",AssessmentMeanLengthStartOfYear[y,p,d,g,]))
      }
print('_____________________________________________________________________________________________________')

#-----------------------------------------------------------------------------------------------
# (3.1.13)  POPULATION MEAN WEIGHTS AT AGE ON JANUARY 1ST BY YEAR
#-----------------------------------------------------------------------------------------------
print('(3.1.12) POPULATION MEAN WEIGHTS AT AGE ON JANUARY 1ST BY YEAR (Year Population Area Gender(1=Females))')
print('_____________________________________________________________________________________________________')
for (p in 1:NPopulation)
  for (d in 1:NArea)
    for (g in 1:NGender)
      for (y in 1:NYear)
      {
        print(c("Year=",y,"Population=",p,"Area=",d,"Gender=",g,"By Age",AssessmentMeanWeightStartOfYear[y,p,d,g,]))
      }
print('_____________________________________________________________________________________________________')

sink()

print('_____________________________________________________________________________________________________')
print('Calculations are complete for MAS Case Study 1')
print('_____________________________________________________________________________________________________')



