make_area <- function(name, id){
  function_env <- sys.frame(sys.parent(0))
  arguments <- ls(function_env)[-1]
  return_list <- vector("list")
  return_list <- lapply(X=arguments, function(X) return_list$X <- get(X, function_env))
  setNames(return_list, arguments)
}
