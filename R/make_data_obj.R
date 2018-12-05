make_data_obj <- function(data_list, name, units, data_object_type, sex, areas_of_operation, observation_error){

  arguments <- ls(sys.frame(sys.parent(0)))[-1]
  function_env <- sys.frame(sys.parent(0))
    ret <- lapply(arguments[2:6],get,                                      function_env)
    ret[[6]] = seq(1,length(ret[1]))
    ret[[7]]= rep(-999, length(ret[[1]]))
    ret[[8]] = data_list
    ret[[9]] = observation_error

  return(ret)
}
