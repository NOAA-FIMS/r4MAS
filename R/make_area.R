make_area <- function(area_names){
  area <- area_list <- vector("list")
  for(i in 1:length(area_names)){
    area$id <- i
    area$name <- area_names[i]
    area_list$area <- area
  }
  return(area_list)
}
