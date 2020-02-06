petrale_om <- "C:/Users/Christine.Stawitz/Documents/ReproducibleAssessments/PetraleModel/Petrale2015base/"
devtools::install_github("jeroen/jsonlite")
devtools::load_all(".")
library(jsonlite)
library(dplyr)
petrale_out <- r4ss::SS_output(petrale_om, covar=F)
names(petrale_out)
lookup<- read.csv("./inst/extdata/lookupTable.csv", colClasses="character")

parameter_df <- petrale_out$parameters

names <- rownames(parameter_df)
new_name <- name_lookup(input_format = "r4ss", lookup=lookup, names = names)

out_schema <- vector("list")

jsonlite::write_json(petrale_out, path= "petrale.json", pretty = TRUE, matrix = "rowmajor", force = TRUE)
write_json(parameter_df, path="petrale_params.json")
