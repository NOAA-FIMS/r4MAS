devtools::load_all("C:\\Users\\chris\\Documents\\GitHub\\jsonlite")
hake_om <- "C:/Users/chris/Documents/StockAssessment/hake_om/"
hake_out<- r4ss::SS_output(hake_om)
r4ss::SS_plots(hake_out, plot=c(1:6,8:24))




case_1_config <- read_config("C:\\Users\\chris\\Documents\\GitHub\\MAS\\Tests\\CaseStudy1\\mas_case_1_config.json")
case_2_config <- read_config("C:\\Users\\chris\\Documents\\GitHub\\MAS\\Tests\\CaseStudy2\\mas_case_2_config.json")


case_1_data <- read_data("C:\\Users\\chris\\Documents\\GitHub\\MAS\\Tests\\CaseStudy1\\mas_case_1_data.json")

write_data(case_1_data, "C:\\Users\\chris\\Documents\\GitHub\\MAS\\Tests\\CaseStudy1\\data_test.json")

write_config(case_1_config, "C:\\Users\\chris\\Documents\\GitHub\\MAS\\Tests\\CaseStudy1\\test.json")

hakepars<- r4ss::SS_parlines(paste0(hake_om,"2018hake_control.ss"))
lapply(hakepars$Label, name_map, toMAS=TRUE)
growth_ind<- c(2,3,4,7,8)
growth_sub <- hakepars[growth_ind,]
rec_ind<- c(18,19,20)
rec_sub <- hakepars[rec_ind,]
growth <- create_par_section("growth", 1, "von_bertalanffy_modified",
                   par_names = growth_sub$Label, par_lo=growth_sub$LO,
                   par_hi=growth_sub$HI, par_units=c(rep("cm",2),rep(NA,3)),
                   par_phase=growth_sub$PHASE, par_value = growth_sub$INIT)

rec <- create_par_section("recruitment", 1, "beverton_holt_alt",
                           par_names = rec_sub$Label, par_lo=rec_sub$LO,
                           par_hi=rec_sub$HI, par_units=rep(NA,3),
                           par_phase=rec_sub$PHASE, par_value = rec_sub$INIT)
M_sub <- hakepars[1,]
M <- create_par_section("natural_mortality", 1, "", par_names=M_sub$Label, par_lo=M_sub$LO, par_hi=M_sub$HI, par_units=NA, par_phase=M_sub$PHASE, par_value=M_sub$INIT
)

sel_sub1 <-hakepars[24:25,]
sel_sub2 <-hakepars[26:27,]
sel1 <- create_par_section("selectivity", 1, "logistic", par_names=sel_sub1$Label, par_lo=sel_sub1$LO, par_hi=sel_sub1$HI, par_units=NA, par_phase=sel_sub1$PHASE, par_value=sel_sub1$INIT
)
sel2 <- create_par_section("selectivity", 2, "logistic", par_names=sel_sub1$Label, par_lo=sel_sub1$LO, par_hi=sel_sub1$HI, par_units=NA, par_phase=sel_sub1$PHASE, par_value=sel_sub1$INIT
)
hake_ctl<- r4ss::SS_readctl_3.24(paste0(hake_om,"2018hake_control.ss"))
hake_dat <- r4ss::SS_readdat_3.24(paste0(hake_om,"2018hake_data.ss"))

movement <- movement_matrix(1,1,1)
par <- vector("list")
par$analyst <- "Christine Stawitz"
par$study_name <- "HakeCaseStudy"
par$movement_type <-
par$years <- 2017-1966
par$seasons <- 1
par$ages <- c(0.01,1:20)
par$extended_plus_group <- 20
par$first_year <- 1966
par$last_year <- 2017
par$season$id <- 1
par$season$name <- "season 1"
par$season$months <- 12
par$growth <- growth
par$recruitment <- rec
par$natural_mortality <- M
par$selectivity <- sel1
par$selectivity <- sel2
par$fishing_mortality
par$area
par$likelihood_component
par$population
par$fleet
par$survey

write_config(par, "C:\\Users\\chris\\Documents\\GitHub\\MAS\\Tests\\HakeCaseStudy\\parconfig.json")
