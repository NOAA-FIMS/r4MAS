assign_values <- function(X, function_env, return_list){
  return_list$X <- get(X,                                                function_env)
  return(return_list)
}
