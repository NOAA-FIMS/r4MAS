create_par_section <- function(section_type, id = 1,
                               model, par_names, par_lo,
                               par_hi, par_units, par_phase,
                               par_value){
  output_list <- vector("list")
  par_list <-vector("list")

  output_list$model <- model
  output_list$id <- id
  new_names <- unlist(lapply(par_names, name_map, toMAS=TRUE))

  create_par <-  function(x){
      par_list[[new_names[x]]]$`value` <- par_value[x]
      par_list[[new_names[x]]]$estimated <- ifelse(par_phase[x]<0, "false", "true")
      par_list[[new_names[x]]]$min <- par_lo[x]
      par_list[[new_names[x]]]$max <- par_hi[x]
      par_list[[new_names[x]]]$phase <- par_phase[x]
      return(par_list)

  }
   par_list <- lapply(1:length(par_names), create_par)


  output_list$parameters <- par_list
  return(output_list)
}
