make_fleet <- function(names, ids, Fs, selectivity, likelihoods){
  fleet_list <- vector("list")
  for(i in 1:length(names)){
    fleet_list$fleet$name <- names[i]
    fleet_list$fleet$id <- ids[i]
    fleet_list$fleet$fishing_mortality <- Fs[[i]]
    fleet_list$fleet$selectivity <- selectivity[[i]]
    fleet_list$fleet$likelihoods <- likelihoods[[i]]
  }
  return(fleet_list)
}
