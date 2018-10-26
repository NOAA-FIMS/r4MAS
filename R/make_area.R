make_area <- function(area_names){
  area <- area_list <- vector("list")
  for(i in 1:length(area_names)){
    area[[i]]$id <- i
    area[[i]]$name <- area_names[i]
    area_list <- c(area_list, area[[i]])
  }
  return(area_list)
}
