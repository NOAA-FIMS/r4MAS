devtools::load_all("C:\\Users\\chris\\Documents\\GitHub\\jsonlite")

case_1_config <- read_config("C:\\Users\\chris\\Documents\\GitHub\\MAS\\Tests\\CaseStudy1\\mas_case_1_config.json")

case_1_data <- read_data("C:\\Users\\chris\\Documents\\GitHub\\MAS\\Tests\\CaseStudy1\\mas_case_1_data.json")

write_data(case_1_data, "C:\\Users\\chris\\Documents\\GitHub\\MAS\\Tests\\CaseStudy1\\data_test.json")

write_config(case_1_config, "C:\\Users\\chris\\Documents\\GitHub\\MAS\\Tests\\CaseStudy1\\test.json")

hakepars<- r4ss::SS_parlines("C:\\Users\\chris\\Documents\\StockAssessment\\hake_noageerr\\2018hake_control.ss")
lapply(hakepars$Label, name_map, toMAS=TRUE)
growth_ind<- c(2,3,4,7,8)
sub <- hakepars[growth_ind,]
create_par_section("growth", 1, "von_bertalanffy_modified",
                   par_names = sub$Label, par_lo=sub$LO,
                   par_hi=sub$HI, par_units=rep("cm",5),
                   par_phase=sub$PHASE, par_value = sub$INIT)
