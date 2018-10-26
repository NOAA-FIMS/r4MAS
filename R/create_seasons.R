create_seasons <- function(season_names, number_of_seasons, months_per_season){
  season_tmp <- season_list <- vector("list")
  for(i in 1:number_of_seasons){
    season_tmp[[i]]$id <- i
    season_tmp[[i]]$name <- season_names[i]
    season_tmp[[i]]$months <- months_per_season[i]
    season_list <- c(season_list, season_tmp[[i]])
  }
  return(season_list)
}
