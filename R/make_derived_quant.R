make_derived_quant <- function(id, type, sex, years, ages, dimensions, values){
  population<- list("id" = id, "type" = type, "years" = years, "ages" = ages, "sex" = sex,
                    "dimensions" = dimensions, "values"=values)
 
  # population$recruitment_type <- "natal"
  # population$maturity$sex <- "male"
  # population$maturity$area <- 1
  # population$maturity$values <- maturity[[1]]
  # population$maturity$sex <- "female"
  # population$maturity$area <- 1
  # population$maturity$values <- maturity[[2]]
  # population$parameters$growth <- growth$id
  # population$parameters$natural_mortality<- lapply(1:2, function(x) [[x]]$id <- 1; [[x]]$area <- 1, sex <- sexes[x]
  #
  #                                                    do_by_sex <- function(i, sex, id, area, values, list__){
  #                                                      list__[[i]] <- generic_assignment(list__[[i]], "sex", sex)
  #                                                      }
  return(ls())

}
