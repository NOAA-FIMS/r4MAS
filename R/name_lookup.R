#'@author Christine Stawitz
#'@description Function to map names between different stock assessment models
#'@param input_format string; the model the data or output is coming from
#'@param names string or vector of strings; names to map
#'@return new_name - the mapped name
#'
#'@example name_lookup(input_format = "SS", names = "SR_LN(R0)")
name_lookup <-function(input_format, names, lookup){
  
  #Identify strings to look for
  search_strings<- lookup %>% filter(type %in% c("parameter", "derived_quantity")) %>% select(input_format)
  
  #Get row index corresponding to sex names
  female <- which(lookup$name == "female")
  male <- which(lookup$name == "male")
  
  #Remove blanks
  search_strings<-search_strings[!search_strings==""]
  
  #Define objects needed
  ind <- rep("",length(names))
  new_name <- data.frame(matrix(NA, nrow=length(names), ncol=4))
  names(new_name) <- c("name", "sex", "fleet", "year")
  
  get_name <-function(lookup_name) return(lookup[lookup_name,"output"])
  
  #Define cases
  #case 1: 1 parameter for whole population
  #Case 2: 1 parameter for females only
  #Case 3: 2 parameters total: 1 parameter each for males and females
  #Case 4: multiple parameters: year-specific parameters
  #Case 5: multiple parameters: 1 parameter per fleet
  
  for(i in 1:length(search_strings)){
    
    matches <- grep(search_strings[i],names, fixed = TRUE)
    lookup_index <- grep(search_strings[i],lookup[,input_format], fixed=TRUE)
    
    #Indices for female- and male- specific parameter value
    fem_par <- grep(lookup[female,input_format], names[matches])
    male_par <-  grep(lookup[male,input_format], names[matches])
    year_par <- grep("_\\d{4}$" , names[matches])
    
    case1_bool <- length(matches)==1
    case2_bool <- (length(matches)==1)&&(length(fem_par)==1)&&(length(male_par)==0)
    case3_bool <- (length(fem_par)==1)&&(length(male_par)==1)
    case4_bool <- (length(year_par)>0)
    
    
    if((length(fem_par)==1) && (length(male_par)==0)){
    #If only one parameter matches, put in 'name' column
    new_name[matches,1] <- get_name(lookup_index)
    new_name[matches,2] <- get_name(female)
    
    new_names_rows <- new_names_rows + 1
    } else if((length(fem_par)==1) && (length(male_par)==1)){
        sex_index <- c(matches[fem_par], matches[male_par])
        
        #Double-check sex index and matches correspond
        assertthat::assert_that((sex_index==matches)||sex_index==rev(matches), msg = "Sex index doesn't match parameter names")
        
        #If so, there is one version of this parameter for each sex
        new_name[sex_index,1] <- get_name(lookup_index)
        new_name[sex_index,2] <- get_name(c(female,male))
        new_name_rows <- new_names_rows + 2
    } else if((length(fem_par)>1) || (length(male_par)>1)){
        
        browser()
    } else if(length(fem_par)==0){
    }
  }
  return(new_name)
}
