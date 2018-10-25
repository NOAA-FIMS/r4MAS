name_map <-function(toMAS = TRUE, in_name){
  if(toMAS){
    new_name <- switch(in_name, "L_at_Amin_Fem_GP_1" = "lmin",
           "L_at_Amax_Fem_GP_1" = "lmax",
           "L_at_Amin__GP_1" = "lmin",
           "Wtlen_1_Fem" = "alpha_f",
           "Wtlen_2_Fem" = "beta_f",
           "NatM_p_1_Fem_GP_1" = "M",
           "SR_LN(R0)" = "R0",
           "SR_BH_steep" = "h",
           "SR_sigmaR" = "sigma_r",
           "SR_autocorr" = "rho",
           "VonBert_K_Fem_GP_1" = "c",
           "AgeSel_P1_Fishery(1)" = "a50",
           "AgeSel_P2_Fishery(1)" = "s",
           "AgeSel_P1_Acoustic_Survey(2)" = "a50",
           "AgeSel_P2_Acoustic_Survey(2)" = "s"
           )
  }
  return(new_name)
}
