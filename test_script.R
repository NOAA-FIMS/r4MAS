devtools::load_all("C:\\Users\\chris\\Documents\\GitHub\\jsonlite")

case_1_config <- read_config("C:\\Users\\chris\\Documents\\GitHub\\MAS\\Tests\\CaseStudy1\\mas_case_1_config.json")
case_2_config <- read_config("C:\\Users\\chris\\Documents\\GitHub\\MAS\\Tests\\CaseStudy2\\mas_case_2_config.json")


case_1_data <- read_data("C:\\Users\\chris\\Documents\\GitHub\\MAS\\Tests\\CaseStudy1\\mas_case_1_data.json")

write_data(case_1_data, "C:\\Users\\chris\\Documents\\GitHub\\MAS\\Tests\\CaseStudy1\\data_test.json")

write_config(case_1_config, "C:\\Users\\chris\\Documents\\GitHub\\MAS\\Tests\\CaseStudy1\\test.json")

hakepars<- r4ss::SS_parlines("C:\\Users\\chris\\Documents\\StockAssessment\\hake_noageerr\\2018hake_control.ss")
lapply(hakepars$Label, name_map, toMAS=TRUE)
growth_ind<- c(2,3,4,7,8)
growth_sub <- hakepars[growth_ind,]
rec_ind<- c(18,19,20)
rec_sub <- hakepars[rec_ind,]
growth <- create_par_section("growth", 1, "von_bertalanffy_modified",
                   par_names = growth_sub$Label, growth_sub=sub$LO,
                   par_hi=growth_sub$HI, par_units=c(rep("cm",2),rep(NA,3)),
                   par_phase=growth_sub$PHASE, par_value = growth_sub$INIT)

rec <- create_par_section("recruitment", 1, "beverton_holt_alt",
                           par_names = rec_sub$Label, par_lo=rec_sub$LO,
                           par_hi=rec_sub$HI, par_units=rep(NA,3),
                           par_phase=rec_sub$PHASE, par_value = rec_sub$INIT)
M_sub <- hakepars[1,]
M <- create_par_section("natural_mortality", 1, "", par_names=M_sub$Label, par_lo=M_sub$LO, par_hi=M_sub$HI, par_units=NA, par_phase=M_sub$PHASE, par_value=M_sub$INIT
)

movement_matrix(1,1,1)

