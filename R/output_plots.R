output_plots <- function(dir, ages, years){
  #setwd("C:/Users/chris/Documents/StockAssessment/MAS")
  setwd(dir)

  #Read in populations.txt and report file
  MAS_pops=scan("populations.txt",
                what="character",
                flush=TRUE,
                blank.lines.skip=FALSE,
                quiet=TRUE,
                sep="\n")
  idx=which(MAS_pops!="")
  non_blanks=MAS_pops[idx]

  #Get scalar parameters
  rec_params <- c("R0\t", "h\t", "SB0\t")
  id_rec <- unlist(lapply(rec_params, grep,non_blanks))
  rec_param_list<- strsplit(non_blanks[id_rec], "\t")

  #Get vector and matrices for each sex
  get_each_sex <- function(sex, non_blanks, id_sex, name_diff, k_start){

    #Identify strings corresponding to sex
  id_sex <- unlist(lapply(sex, grep, non_blanks))
  #Name of the quantity is the string before sex
  names_sex <- paste(non_blanks[id_sex-name_diff], substr(sex,1,1))
  vals_sex <- vector("list")

  #Get values for each quantity
  get_vals <- function(i,k){
    return(as.double(unlist(strsplit(non_blanks[id_sex[i]+k], " "))))
  }

  for(i in 1:length(id_sex)){
    k <- k_start

    tmp <- get_vals(i, k)
    while(!any(is.na(get_vals(i, k+1)))){
      tmp <- rbind(tmp, get_vals(i, k+1))
      k <- k+1
    }
    vals_sex[[names_sex[i]]] <- tmp
  }
    return(vals_sex)
  }

  #List of populations values
  fem_list <- get_each_sex("Female", non_blanks, idx, 1, 1)
  male_list <- get_each_sex("Male", non_blanks, idx, 1, 1)


  if(!dir.exists(file.path(dir,"Figs"))){
    dir.create(file.path(dir,"Figs"))
  }

  each_plot <- function(input_list){
    #Names of each quantity
    if(length(grep("Figs",getwd()))==0){
      setwd("./Figs")
    }
    quants <- names(input_list)

    for(i in 1:length(input_list)){
      curr <- input_list[[quants[i]]]
      if(is.null(nrow(curr))){
        png(paste0(quants[i], ".png"))
        dim_vect <- length(curr)
        if(dim_vect == length(ages)){
          plot(curr~ages, main=quants[i],
               xlab="Ages", type="l",
               ylab=quants[i])
        } else if(dim_vect == length(years)){
          plot(curr~years, main=quants[i],
               xlab="Yrs",
               ylab=quants[i],
               type="l")
        } else{
          print("dimensions do not match!")
        }
        dev.off()
      } else{
        if(nrow(curr)==length(ages)){

          by_age <- apply(curr, 1, sum)
          by_yr <- apply(curr, 2, sum)
          png(paste0(quants[i],"by_age.png"))
          plot(by_age~ages, type="l",
               main=quants[i])
          dev.off()

          png(paste0(quants[i],"by_yr.png"))
          plot(by_yr~years, type="l",
               main=quants[i])
          dev.off()

          pdf(paste0(quants[i], ".pdf"))
          par(mfrow=c(2,2))
          for(j in 1:length(years)){

            plot(curr[,j]~ages, type="l",
                 main=paste(quants[i],years[j]))

          }
          dev.off()
        } else if(nrow(curr)==length(years)){

          pdf(paste0(quants[i], ".pdf"))
          par(mfrow=c(2,2))
          for(j in 1:length(years)){

            plot(curr[j,]~ages, type="l",
                 main=paste(quants[i],years[j]))

          }
          dev.off()
        } else{
          print("Dimensions do not match!")
        }
      }
    }
    setwd(dir)
  }
  each_plot(fem_list)
  each_plot(male_list)



  MAS_rep=scan("mas_report.txt",
                what="character",
                flush=TRUE,
                blank.lines.skip=FALSE,
                quiet=TRUE,
               sep="\n")

  idx=which(MAS_rep!="")
  non_blanks=MAS_rep[idx]

  exp_report <- get_each_sex("Expected", non_blanks, id_sex=idx, 0, 2)
  obs_report <- get_each_sex("Observed", non_blanks, id_sex = idx,0,2)

  pattern_obs <- sub("Observed ","", names(obs_report))
  pattern_obs <- sub(": O", "", pattern_obs)
  ind <- unlist(lapply(paste0(pattern_obs,":"),
                       grep, x=names(exp_report)))

  for(i in 1:length(ind)){
    obs <- obs_report[[names(obs_report)[i]]]
    exp <- exp_report[[names(exp_report)[ind[i]]]]
    if(length(grep("Figs",getwd()))==0){
      setwd("./Figs")
    }
    if(is.null(dim(obs))){
      exp <- apply(exp, 2, sum)
      pdf(paste0("ObsvsExpected", pattern_obs[i],
                 ".pdf"))
      plot(obs~years, pch=19, main=paste(pattern_obs[i]))
      lines(exp~years)
      dev.off()
    } else{
    if(any(dim(obs)!=dim(exp))){
      print("dimensions don't match")
    } else if(dim(obs)[1]==2){
      pdf(paste0("ObsVsExpected", pattern_obs[i],".pdf"))
      par(mfrow=c(2,2))
      for(j in 1:nrow(obs)){
        plot(obs[j,]~years, pch=19, main=paste(pattern_obs[i], years[j]))
        lines(exp[j,]~years)
      }
      dev.off()
    } else{
      pdf(paste0("ObsVsExpected", pattern_obs[i],
                 ".pdf"))
      par(mfrow=c(2,2))
      for(j in 1:ncol(obs)){
        plot(obs[,j]~ages, pch=19, main=paste(pattern_obs[i],j))
      lines(exp[,j]~ages)
    }
    dev.off()
    }
  }


  }

}


output_plots(dir="C:/Users/chris/Documents/StockAssessment/MAS", years = 1966:2017, ages=c(0.01,seq(1,15)))
