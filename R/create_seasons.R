create_seasons <- function(season_names, number_of_seasons, months_per_season){
  season_tmp <- season_list <- vector("list")
  for(i in 1:number_of_seasons){
    season_tmp$id <- i
    season_tmp$name <- season_names[i]
    season_tmp$months <- months_per_season[i]
    season_list$season <- season_tmp
  }
  return(season_list)
}
