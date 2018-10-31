devtools::load_all("C:\\Users\\chris\\Documents\\GitHub\\jsonlite")
devtools::install_github("r4ss/r4ss", force=TRUE)
hake_orig <- "C:/Users/chris/Documents/StockAssessment/hake_original/"
hake_om <- "C:/Users/chris/Documents/StockAssessment/hake_om/"

pkg <- getwd()
setwd(hake_om)
system("ss3")
hake_out<- r4ss::SS_output(hake_om, covar=F)
hake_out$current_depletion
setwd(pkg)
r4ss::SS_plots(hake_out, plot=c(1:6,8:24))
#generate recdevs
devs<-exp(-0.5*1.4^2)*exp(rnorm(47+24,0, 1.4))
scaled <- (devs- mean(devs))
sd(scaled)

case_1_config <- read_config("C:\\Users\\chris\\Documents\\GitHub\\MAS\\Tests\\CaseStudy1\\mas_case_1_config.json")
case_2_config <- read_config("C:\\Users\\chris\\Documents\\GitHub\\MAS\\Tests\\CaseStudy2\\mas_case_2_config.json")


case_1_data <- read_data("C:\\Users\\chris\\Documents\\GitHub\\MAS\\Tests\\CaseStudy1\\mas_case_1_data.json")

write_data(case_1_data, "C:\\Users\\chris\\Documents\\GitHub\\MAS\\Tests\\CaseStudy1\\data_test.json")

write_config(case_1_config, "C:\\Users\\chris\\Documents\\GitHub\\MAS\\Tests\\CaseStudy1\\test.json")

hakepars<- r4ss::SS_parlines(paste0(hake_om,"2018hake_control.ss"))
hake_ctl<- r4ss::SS_readctl(paste0(hake_om,"2018hake_control.ss"))
hake_dat <- r4ss::SS_readdat_3.30(paste0(hake_om,"2018hake_data.ss"))
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
                           par_phase=rec_sub$PHASE, par_value = rec_sub$INIT, rec_devs = hake_out$recruit$dev[21:72])
M_sub <- hakepars[1,]
M <- create_par_section("natural_mortality", 1, "", par_names=M_sub$Label, par_lo=M_sub$LO, par_hi=M_sub$HI, par_units=NA, par_phase=M_sub$PHASE, par_value=M_sub$INIT
)

sel_sub1 <-hakepars[24:25,]
sel_sub2 <-hakepars[26:27,]
sel1 <- create_par_section("selectivity", 1, "logistic", par_names=sel_sub1$Label, par_lo=sel_sub1$LO, par_hi=sel_sub1$HI, par_units=NA, par_phase=sel_sub1$PHASE, par_value=sel_sub1$INIT
)
sel2 <- create_par_section("selectivity", 2, "logistic", par_names=sel_sub1$Label, par_lo=sel_sub1$LO, par_hi=sel_sub1$HI, par_units=NA, par_phase=sel_sub1$PHASE, par_value=sel_sub1$INIT
)

F_values= hake_out$derived_quants %>% filter(substr(Label,1,2)=="F_") %>% select(Value)
nyears <- hake_dat$endyr - hake_dat$styr


movement <- movement_matrix(1,1,1)
par <- make_branch(analyst = "Christine Stawitz",
                    study_name = "HakeCaseStudy",
                   movement_type ="spatial",
                   years = nyears,
                   seasons =hake_dat$nseas,
                   ages = c(0.01,1:20),
                   extended_plus_group = hake_ctl$Nages,
                  first_year = hake_dat$styr,
                  last_year = hake_dat$endyr,
                  season = list("name"="season 1",
                                    "id"=1:length(hake_dat$months_per_seas),
                                    "months"= hake_dat$months_per_seas),
                  growth = growth,
                  recruitment = rec,
                  natural_mortality = M,
                  selectivity = list(sel1, sel2),
                  fishing_mortality = list("id" = id,
                                     "parameters" = list(
                                     "estimated" = "true",
                                     "delta_method"= "true",
                                    "phase" = 1,
                                    "values" = F_values[2:52,])),
       area = list("id"=1, "name"="California current"),
    likelihood_component =list("id" = c(1,2,3),
                               "models"=c("lognormal", "multinomial", "multinomial"),
                               "lambdas" = lambdas,
                               "years"=nyears),
    population = list("id"=1, "natal_area"=1,
                      "name"="Population 1",
                      "hcr"="foo",
                      "years" = nyears,
                      "movement" = "spatial",
                      "maturity" = c(0,rep(1,19)),
                      "growth" = growth,
                      "M"= M,
                      "recruitment" = rec),
    fleet = list("name"= "fleet1",
                   "id" = 1,
                 "fishing_mortality" = 1,
                 "selectivity" = 1,
                 "likelihood_components" = list("id" = 1, "component" = "age_comp")),
    survey = list("name"= "survey1",
                    "id" = 1,
                  "fishing_mortality" = 1,
                  "selectivity" = 2,
                  "likelihood_components" = list("id" = 1, "component" = "age_comp")
                  ))

    list("id" = 1, "component" = "biomass_index")



write_config(par, "C:\\Users\\chris\\Documents\\GitHub\\MAS\\Tests\\HakeCaseStudy\\par2config.json")
