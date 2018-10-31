make_likelihood_component <- function(ids, models, lambdas, parameters=NA, nyears){
  likelihood_list <- vector("list")
  for(i in 1:length(ids)){
    likelihood_list[[i]] <- vector("list")
    likelihood_list[[i]]$id <- ids[i]
    likelihood_list[[i]]$model <- models[i]
    dimensions <- length(lambdas[[i]])
    if(dimensions==nyears){
      likelihood_list[[i]]$lambda$years <- nyears
      likelihood_list[[i]]$lambda$values <- lambdas[[i]]
    } else{
      break("Error: weighting beyond per-year is not implemented yet.")
    }

    if(length(grep("dirichlet",models[i]))>0){
      likelihood_list$parameters <- parameters
    }
  }
  return(likelihood_list)
}
