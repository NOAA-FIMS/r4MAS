create_par_schema <- function(parameter_df, recruit_table){
  par_list <-vector("list")
  par_names <- rownames(parameter_df)
  new_names <- unlist(lapply(par_names, name_map, toMAS=TRUE))

  for(x in 1:length(par_names)){
      par_list[[new_names[x]]]$`value` <- par_value[x]
      
      if(!is.na(par_units[x])){
        par_list[[new_names[x]]]$units <- par_units[x]
      }

      if(par_phase[x]<0){
        par_list[[new_names[x]]]$estimated <- "false"
        par_list[[new_names[x]]]$phase <- -par_phase[x]
      } else{
        par_list[[new_names[x]]]$estimated <- "true"
        par_list[[new_names[x]]]$phase <- par_phase[x]
      }
      par_list[[new_names[x]]]$min <- par_lo[x]
      par_list[[new_names[x]]]$max <- par_hi[x]


  }
  if(section_type=="recruitment"){
    par_list$recruitment_deviations$estimated = "false"
    par_list$recruitment_deviations$phase = 4
    par_list$recruitment_deviations$values = rec_devs
  }
  if(section_type=="selectivity"){
    if(grep("Age", par_names)>0){
      par_list$selectivity$base <- "age"
    } else{
      par_list$selectivity$base <- "length"
    }
  }

  return(par_list)
}
