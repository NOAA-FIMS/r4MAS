Selectivity.Logistic <- function(age,a50,slope) {
  selectivity <- 1.0/(1.0+exp(-(age-a50)/slope))
  return(selectivity)
}