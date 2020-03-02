Maturity.Logistic <- function(age,a50,slope) {
  fraction.mature <- 1.0/(1.0+exp(-(age-a50)/slope))
  return(fraction.mature)
}