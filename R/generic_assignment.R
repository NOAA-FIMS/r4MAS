generic_assignment <- function(list__, element_type, element_value){
  list__[[get(element_type, parent.frame)]] <- element_value
  return(list__)
}
