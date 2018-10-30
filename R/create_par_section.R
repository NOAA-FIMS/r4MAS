create_par_section <- function(section_type, id = 1,
                               model, par_names, par_lo,
                               par_hi, par_units, par_phase,
                               par_value, rec_devs = NA){
  output_list <- vector("list")
  par_list <-vector("list")

  output_list$model <- model
  output_list$id <- id
  new_names <- unlist(lapply(par_names, name_map, toMAS=TRUE))

  for(x in 1:length(new_names)){
      if(new_names[[x]]=='R0'){
        par_list[[new_names[x]]]$`value` <- exp(par_value[x])
      } else{
      par_list[[new_names[x]]]$`value` <- par_value[x]
      }
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

  output_list$parameters <- par_list
  return(output_list)
}
