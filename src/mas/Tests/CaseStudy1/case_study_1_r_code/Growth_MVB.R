Growth.MVB <- function(age,amin,amax,offset,Lmin,Lmax,c) {
  tmp <- 0.0
  tmp <- (1.0-c^(age+offset-amin))/(1.0-c^(amax-amin))
  length  <- Lmin+tmp*(Lmax-Lmin)
  return(length)
}