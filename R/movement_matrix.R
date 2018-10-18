movement_matrix <- function(recruits, males, females){
  move_list <- vector("list")
  move_list$`id` <- 1
  move_list$estimated <- "false"
  move_list$recruits <- vector("list")
  move_list$recruits[[1]] <- recruits
  move_list$males <- vector("list")
  move_list$males[[1]] <- males
  move_list$females <-vector("list")
  move_list$females[[1]] <- females
  return(move_list)
}
