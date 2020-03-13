create_par_section <- function(section_type, section_type_object, id = 1,
                               par_names, par_lo=NULL,
                               par_hi=NULL, par_units=NULL, par_phase=NULL,
                               par_value, rec_devs = NA){
  

  check_if_null <- function(x, ind){

    bool <- FALSE
    if(is.null(x)){
      bool <- TRUE
    } else if(is.na(x[ind])){
      bool <- TRUE
    }
    return(bool)
  }
  
  for(x in 1:length(par_names)){
    if(par_names[x]=="recdevs"){
      section_type_object$estimate_deviations = rec_devs[[1]]
      section_type_object$constrained_deviations = rec_devs[[2]]
      if(rec_devs[[2]]){
        section_type_object$deviations_min = par_lo[x]
        section_type_object$deviations_max = par_hi[x]
      }
      section_type_object$deviation_phase = par_phase[x]
      recruitment$SetDeviations(rec_devs[[3]])
      
    } else{
      section_type_object[[par_names[x]]]$value <- par_value[x]
      if(!check_if_null(par_units,x)){
        section_type_object[[par_names[x]]]$units <- par_units[x]
      }

      if(!check_if_null(par_phase,x)){
      if(par_phase[x]<0){
        section_type_object[[par_names[x]]]$estimated <- FALSE
      } else{
        section_type_object[[par_names[x]]]$estimated <- TRUE
        section_type_object[[par_names[x]]]$phase <- par_phase[x]

      }
      }
      if(!check_if_null(par_lo,x)){
          section_type_object[[par_names[x]]]$min <- par_lo[x]
      }
      if(!check_if_null(par_hi,x)){
          section_type_object[[par_names[x]]]$max <- par_hi[x]}
      }
  }


  return(section_type_object)
}
