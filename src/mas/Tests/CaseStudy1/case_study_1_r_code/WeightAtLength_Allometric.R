WeightAtLength.Allometric <- function(length,A,B) {
  weight <- A*(length^B)
  return(weight)
}