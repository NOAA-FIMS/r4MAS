Recruitment.BH <- function(SB, SB0, R0, h) {
  R <- 4*h*R0*SB/(SB0*(1-h)+SB*(5*h-1))
  return(R)
}