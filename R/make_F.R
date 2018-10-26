make_F <- function(nyears, id, estimated, delta_method, phase, values = NA){
  F_list <- vector("list")
  F_list$id <- id
  F_list$parameters$estimated <- estimated
  F_list$parameters$delta_method <- delta_method
  F_list$parameters$phase <- phase
  if(!is.na(values)){
    F_list$parameters$values <- values
  } else{
    F_list$parameters$values <- rep(0.01, nyears)
  }
  return(F_list)
}
