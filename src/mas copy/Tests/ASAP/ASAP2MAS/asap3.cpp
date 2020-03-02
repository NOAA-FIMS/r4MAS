#ifdef DEBUG
#ifndef __SUNPRO_C
#include <cfenv>
#include <cstdlib>
#endif
#endif
#include <admodel.h>
#include <time.h>
#include "admb2r.cpp"
time_t start, finish;
long hour, minute, second;
double elapsed_time;
ofstream ageproMCMC("asap3.bsn");
ofstream basicMCMC("asap3MCMC.dat");
ofstream inputlog("asap3input.log");
//--- preprocessor macro from Larry Jacobson NMFS-Woods Hole
#define ICHECK(object) inputlog <<  #object<< " " << object << endl;

#include <admodel.h>
#include <contrib.h>

#include "../mas/third_party/rapidjson/document.h"
#include "../mas/third_party/rapidjson/prettywriter.h"
#include "../mas/third_party/rapidjson/stringbuffer.h"

extern "C" {
    void ad_boundf(int i);
}
#include "asap3.hpp"

model_data::model_data(int argc, char * argv[]) : ad_comm(argc, argv) {
    pi = 3.14159265358979;
    CVfill = 100.0;
    nyears.allocate("nyears");
    ICHECK(nyears);
    year1.allocate("year1");
    ICHECK(year1);
    nages.allocate("nages");
    ICHECK(nages);
    nfleets.allocate("nfleets");
    ICHECK(nfleets);
    nselblocks.allocate("nselblocks");
    ICHECK(nselblocks);
    navailindices.allocate("navailindices");
    ICHECK(navailindices);
    M.allocate(1, nyears, 1, nages, "M");
    ICHECK(M);
    isfecund.allocate("isfecund");
    ICHECK(isfecund);
    fracyearSSB.allocate("fracyearSSB");
    ICHECK(fracyearSSB);
    mature.allocate(1, nyears, 1, nages, "mature");
    ICHECK(mature);
    nWAAmatrices.allocate("nWAAmatrices");
    ICHECK(nWAAmatrices);
    nrowsWAAini = nyears*nWAAmatrices;
    WAA_ini.allocate(1, nrowsWAAini, 1, nages, "WAA_ini");
    ICHECK(WAA_ini);
    nWAApointbio = nfleets * 2 + 2 + 2;
    WAApointbio.allocate(1, nWAApointbio, "WAApointbio");
    ICHECK(WAApointbio);
    fecundity.allocate(1, nyears, 1, nages);
    WAAcatchfleet.allocate(1, nfleets, 1, nyears, 1, nages);
    WAAdiscardfleet.allocate(1, nfleets, 1, nyears, 1, nages);
    WAAcatchall.allocate(1, nyears, 1, nages);
    WAAdiscardall.allocate(1, nyears, 1, nages);
    WAAssb.allocate(1, nyears, 1, nages);
    WAAjan1b.allocate(1, nyears, 1, nages);
    if ((max(WAApointbio) > nWAAmatrices) || (min(WAApointbio) < 1)) {
        cout << "Problem with WAApointbio" << endl;
        ad_exit(1);
    }
    for (ifleet = 1; ifleet <= nfleets; ifleet++) {
        int ipointcatchfleet = (WAApointbio((ifleet * 2) - 1) - 1) * nyears;
        int ipointdiscardfleet = (WAApointbio(ifleet * 2) - 1) * nyears;
        for (iyear = 1; iyear <= nyears; iyear++) {
            WAAcatchfleet(ifleet, iyear) = WAA_ini((ipointcatchfleet + iyear));
            WAAdiscardfleet(ifleet, iyear) = WAA_ini((ipointdiscardfleet + iyear));
        }
    }
    int ipointcatchall = (WAApointbio((nfleets * 2) + 1) - 1) * nyears;
    int ipointdiscardall = (WAApointbio((nfleets * 2) + 2) - 1) * nyears;
    int ipointssb = (WAApointbio((nfleets * 2) + 3) - 1) * nyears;
    int ipointjan1b = (WAApointbio((nfleets * 2) + 4) - 1) * nyears;
    for (iyear = 1; iyear <= nyears; iyear++) {
        WAAcatchall(iyear) = WAA_ini((ipointcatchall + iyear));
        WAAdiscardall(iyear) = WAA_ini((ipointdiscardall + iyear));
        WAAssb(iyear) = WAA_ini((ipointssb + iyear));
        WAAjan1b(iyear) = WAA_ini((ipointjan1b + iyear));
    }
    if (isfecund == 1)
        fecundity = mature;
    else
        fecundity = elem_prod(WAAssb, mature);
    sel_blocks.allocate(1, nfleets, 1, nyears, "sel_blocks");
    ICHECK(sel_blocks);
    nsel_ini = nselblocks * (nages + 6);
    sel_option.allocate(1, nselblocks, "sel_option");
    ICHECK(sel_option);
    sel_ini.allocate(1, nsel_ini, 1, 4, "sel_ini");
    ICHECK(sel_ini);
    nselparm = 0;
    for (i = 1; i <= nselblocks; i++) {
        if (sel_option(i) == 1) nselparm += nages;
        if (sel_option(i) == 2) nselparm += 2;
        if (sel_option(i) == 3) nselparm += 4;
    }
    for (i = 1; i <= nsel_ini; i++) {
        if (sel_ini(i, 4) <= 0.0)
            sel_ini(i, 4) = CVfill;
    }
    sel_initial.allocate(1, nselparm);
    sel_lo.allocate(1, nselparm);
    sel_hi.allocate(1, nselparm);
    sel_phase.allocate(1, nselparm);
    sel_lambda.allocate(1, nselparm);
    sel_CV.allocate(1, nselparm);
    sel_sigma2.allocate(1, nselparm);
    sel_sigma.allocate(1, nselparm);
    sel_like_const.allocate(1, nselparm);
    k = 0;
    for (i = 1; i <= nselblocks; i++) {
        if (sel_option(i) == 1) {
            for (iage = 1; iage <= nages; iage++) {
                k += 1;
                j = (i - 1)*(nages + 6) + iage;
                sel_initial(k) = sel_ini(j, 1);
                sel_lo(k) = 0.0;
                sel_hi(k) = 1.0;
                sel_phase(k) = sel_ini(j, 2);
                sel_lambda(k) = sel_ini(j, 3);
                sel_CV(k) = sel_ini(j, 4);
                sel_sigma2(k) = log(sel_CV(k) * sel_CV(k) + 1.0);
                sel_sigma(k) = sqrt(sel_sigma2(k));
            }
        }
        if (sel_option(i) == 2) {
            for (ia = 1; ia <= 2; ia++) {
                k += 1;
                j = (i - 1)*(nages + 6) + nages + ia;
                sel_initial(k) = sel_ini(j, 1);
                sel_lo(k) = 0.0;
                sel_hi(k) = nages;
                sel_phase(k) = sel_ini(j, 2);
                sel_lambda(k) = sel_ini(j, 3);
                sel_CV(k) = sel_ini(j, 4);
                sel_sigma2(k) = log(sel_CV(k) * sel_CV(k) + 1.0);
                sel_sigma(k) = sqrt(sel_sigma2(k));
            }
        }
        if (sel_option(i) == 3) {
            for (ia = 1; ia <= 4; ia++) {
                k += 1;
                j = (i - 1)*(nages + 6) + nages + 2 + ia;
                sel_initial(k) = sel_ini(j, 1);
                sel_lo(k) = 0.0;
                sel_hi(k) = nages;
                sel_phase(k) = sel_ini(j, 2);
                sel_lambda(k) = sel_ini(j, 3);
                sel_CV(k) = sel_ini(j, 4);
                sel_sigma2(k) = log(sel_CV(k) * sel_CV(k) + 1.0);
                sel_sigma(k) = sqrt(sel_sigma2(k));
            }
        }
    }
    sel_start_age.allocate(1, nfleets, "sel_start_age");
    ICHECK(sel_start_age);
    sel_end_age.allocate(1, nfleets, "sel_end_age");
    ICHECK(sel_end_age);
    Freport_agemin.allocate("Freport_agemin");
    ICHECK(Freport_agemin);
    Freport_agemax.allocate("Freport_agemax");
    ICHECK(Freport_agemax);
    Freport_wtopt.allocate("Freport_wtopt");
    ICHECK(Freport_wtopt);
    use_likelihood_constants.allocate("use_likelihood_constants");
    ICHECK(use_likelihood_constants);
    release_mort.allocate(1, nfleets, "release_mort");
    ICHECK(release_mort);
    CAA_ini.allocate(1, nyears*nfleets, 1, nages + 1, "CAA_ini");
    ICHECK(CAA_ini);
    Discard_ini.allocate(1, nyears*nfleets, 1, nages + 1, "Discard_ini");
    ICHECK(Discard_ini);
    proportion_release_ini.allocate(1, nyears*nfleets, 1, nages, "proportion_release_ini");
    ICHECK(proportion_release_ini);
    CAA_obs.allocate(1, nfleets, 1, nyears, 1, nages);
    Discard_obs.allocate(1, nfleets, 1, nyears, 1, nages);
    proportion_release.allocate(1, nfleets, 1, nyears, 1, nages);
    CAA_prop_obs.allocate(1, nfleets, 1, nyears, sel_start_age, sel_end_age);
    Discard_prop_obs.allocate(1, nfleets, 1, nyears, sel_start_age, sel_end_age);
    Catch_tot_fleet_obs.allocate(1, nfleets, 1, nyears);
    Discard_tot_fleet_obs.allocate(1, nfleets, 1, nyears);
    CAA_prop_obs_sum.allocate(1, nfleets, 1, nyears);
    Discard_prop_obs_sum.allocate(1, nfleets, 1, nyears);
    catch_tot_like_const.allocate(1, nfleets);
    discard_tot_like_const.allocate(1, nfleets);
    for (ifleet = 1; ifleet <= nfleets; ifleet++) {
        catch_tot_like_const(ifleet) = 0.0;
        discard_tot_like_const(ifleet) = 0.0;
        for (iyear = 1; iyear <= nyears; iyear++) {
            CAA_obs(ifleet, iyear)(1, nages) = CAA_ini((ifleet - 1) * nyears + iyear)(1, nages);
            Discard_obs(ifleet, iyear)(1, nages) = Discard_ini((ifleet - 1) * nyears + iyear)(1, nages);
            proportion_release(ifleet, iyear) = proportion_release_ini((ifleet - 1) * nyears + iyear)(1, nages);
            Catch_tot_fleet_obs(ifleet, iyear) = CAA_ini((ifleet - 1) * nyears + iyear, nages + 1);
            Discard_tot_fleet_obs(ifleet, iyear) = Discard_ini((ifleet - 1) * nyears + iyear, nages + 1);
            if (Catch_tot_fleet_obs(ifleet, iyear) > 1.0e-15)
                catch_tot_like_const(ifleet) += 0.5 * log(2.0 * pi) + log(Catch_tot_fleet_obs(ifleet, iyear));
            if (Discard_tot_fleet_obs(ifleet, iyear) > 1.0e-15)
                discard_tot_like_const(ifleet) = 0.5 * log(2.0 * pi) + log(Discard_tot_fleet_obs(ifleet, iyear));
        }
    }
    if (use_likelihood_constants != 1) {
        catch_tot_like_const = 0.0;
        discard_tot_like_const = 0.0;
    }
    CAA_prop_obs = 0.0;
    Discard_prop_obs = 0.0;
    CAA_prop_obs_sum = 0.0;
    Discard_prop_obs_sum = 0.0;
    for (iyear = 1; iyear <= nyears; iyear++) {
        for (ifleet = 1; ifleet <= nfleets; ifleet++) {
            if (Catch_tot_fleet_obs(ifleet, iyear) > 0.0) {
                for (iage = sel_start_age(ifleet); iage <= sel_end_age(ifleet); iage++)
                    CAA_prop_obs_sum(ifleet, iyear) += CAA_obs(ifleet, iyear, iage);
                if (CAA_prop_obs_sum(ifleet, iyear) == 0.0) {
                    CAA_prop_obs(ifleet, iyear) = 0.0;
                } else {
                    CAA_prop_obs(ifleet, iyear) = CAA_obs(ifleet, iyear)(sel_start_age(ifleet), sel_end_age(ifleet)) / CAA_prop_obs_sum(ifleet, iyear);
                }
            }
            if (Discard_tot_fleet_obs(ifleet, iyear) > 0.0) {
                for (iage = sel_start_age(ifleet); iage <= sel_end_age(ifleet); iage++)
                    Discard_prop_obs_sum(ifleet, iyear) += Discard_obs(ifleet, iyear, iage);
                if (Discard_prop_obs_sum(ifleet, iyear) == 0.0) {
                    Discard_prop_obs(ifleet, iyear) = 0.0;
                } else {
                    Discard_prop_obs(ifleet, iyear) = Discard_obs(ifleet, iyear)(sel_start_age(ifleet), sel_end_age(ifleet)) / Discard_prop_obs_sum(ifleet, iyear);
                }
            }
        }
    }
    index_units_aggregate_ini.allocate(1, navailindices, "index_units_aggregate_ini");
    ICHECK(index_units_aggregate_ini);
    index_units_proportions_ini.allocate(1, navailindices, "index_units_proportions_ini");
    ICHECK(index_units_proportions_ini);
    index_WAApoint_ini.allocate(1, navailindices, "index_WAApoint_ini");
    ICHECK(index_WAApoint_ini);
    index_month_ini.allocate(1, navailindices, "index_month_ini");
    ICHECK(index_month_ini);
    index_sel_choice_ini.allocate(1, navailindices, "index_sel_choice_ini");
    ICHECK(index_sel_choice_ini);
    index_sel_option_ini.allocate(1, navailindices, "index_sel_option_ini");
    ICHECK(index_sel_option_ini);
    index_start_age_ini.allocate(1, navailindices, "index_start_age_ini");
    ICHECK(index_start_age_ini);
    index_end_age_ini.allocate(1, navailindices, "index_end_age_ini");
    ICHECK(index_end_age_ini);
    index_estimate_proportions_ini.allocate(1, navailindices, "index_estimate_proportions_ini");
    ICHECK(index_estimate_proportions_ini);
    use_index.allocate(1, navailindices, "use_index");
    ICHECK(use_index);
    nindexsel_ini = navailindices * (nages + 6);
    index_sel_ini.allocate(1, nindexsel_ini, 1, 4, "index_sel_ini");
    ICHECK(index_sel_ini);
    index_ini.allocate(1, nyears*navailindices, 1, 3 + nages + 1, "index_ini");
    ICHECK(index_ini);
    nindices = sum(use_index);
    index_units_aggregate.allocate(1, nindices);
    index_units_proportions.allocate(1, nindices);
    index_WAApoint.allocate(1, nindices);
    index_month.allocate(1, nindices);
    index_sel_option.allocate(1, nindices);
    index_start_age.allocate(1, nindices);
    index_end_age.allocate(1, nindices);
    index_sel_choice.allocate(1, nindices);
    index_nobs.allocate(1, nindices);
    index_estimate_proportions.allocate(1, nindices);
    if ((max(index_WAApoint_ini) > nWAAmatrices) || (min(index_WAApoint_ini) < 1)) {
        cout << "Problem with index_WAApoint_ini" << endl;
        ad_exit(1);
    }
    for (i = 1; i <= nindexsel_ini; i++) {
        if (index_sel_ini(i, 4) <= 0.0)
            index_sel_ini(i, 4) = CVfill;
    }
    for (i = 1; i <= nyears * navailindices; i++) {
        if (index_ini(i, 3) <= 0.0)
            index_ini(i, 3) = CVfill;
    }
    ind = 0;
    nindexselparms = 0;
    for (indavail = 1; indavail <= navailindices; indavail++) {
        if (use_index(indavail) == 1) {
            ind += 1;
            index_units_aggregate(ind) = index_units_aggregate_ini(indavail);
            index_units_proportions(ind) = index_units_proportions_ini(indavail);
            index_WAApoint(ind) = index_WAApoint_ini(indavail);
            index_month(ind) = index_month_ini(indavail);
            index_sel_option(ind) = index_sel_option_ini(indavail);
            if (index_sel_option(ind) == 1) nindexselparms += nages;
            if (index_sel_option(ind) == 2) nindexselparms += 2;
            if (index_sel_option(ind) == 3) nindexselparms += 4;
            index_start_age(ind) = index_start_age_ini(indavail);
            index_end_age(ind) = index_end_age_ini(indavail);
            index_sel_choice(ind) = index_sel_choice_ini(indavail);
            index_estimate_proportions(ind) = index_estimate_proportions_ini(indavail);
            j = 0;
            for (iyear = 1; iyear <= nyears; iyear++) {
                if (index_ini((indavail - 1) * nyears + iyear, 2) > 0.0) // zero or negative value for index means not included
                    j += 1;
            }
            index_nobs(ind) = j;
        }
    }
    index_time.allocate(1, nindices, 1, index_nobs);
    index_year.allocate(1, nindices, 1, index_nobs);
    index_obs.allocate(1, nindices, 1, index_nobs);
    index_cv.allocate(1, nindices, 1, index_nobs);
    index_sigma2.allocate(1, nindices, 1, index_nobs);
    index_sigma.allocate(1, nindices, 1, index_nobs);
    input_eff_samp_size_index.allocate(1, nindices, 1, index_nobs);
    indexsel_initial.allocate(1, nindexselparms);
    indexsel_lo.allocate(1, nindexselparms);
    indexsel_hi.allocate(1, nindexselparms);
    indexsel_phase.allocate(1, nindexselparms);
    indexsel_lambda.allocate(1, nindexselparms);
    indexsel_CV.allocate(1, nindexselparms);
    indexsel_sigma2.allocate(1, nindexselparms);
    indexsel_sigma.allocate(1, nindexselparms);
    indexsel_like_const.allocate(1, nindexselparms);
    index_sel_input.allocate(1, nindices, 1, nyears, 1, nages);
    index_prop_obs.allocate(1, nindices, 1, index_nobs, 1, nages);
    index_WAA.allocate(1, nindices, 1, nyears, 1, nages);
    index_like_const.allocate(1, nindices);
    index_prop_obs = 0.0;
    ind = 0;
    k = 0;
    for (indavail = 1; indavail <= navailindices; indavail++) {
        if (use_index(indavail) == 1) {
            ind += 1;
            if (index_sel_option(ind) == 1) {
                for (iage = 1; iage <= nages; iage++) {
                    k += 1;
                    j = (indavail - 1)*(nages + 6) + iage;
                    indexsel_initial(k) = index_sel_ini(j, 1);
                    indexsel_lo(k) = 0.0;
                    indexsel_hi(k) = 1.0;
                    indexsel_phase(k) = index_sel_ini(j, 2);
                    indexsel_lambda(k) = index_sel_ini(j, 3);
                    indexsel_CV(k) = index_sel_ini(j, 4);
                    indexsel_sigma2(k) = log(indexsel_CV(k) * indexsel_CV(k) + 1.0);
                    indexsel_sigma(k) = sqrt(indexsel_sigma2(k));
                }
            } else if (index_sel_option(ind) == 2) {
                for (ia = 1; ia <= 2; ia++) {
                    k += 1;
                    j = (indavail - 1)*(nages + 6) + nages + ia;
                    indexsel_initial(k) = index_sel_ini(j, 1);
                    indexsel_lo(k) = 0.0;
                    indexsel_hi(k) = nages;
                    indexsel_phase(k) = index_sel_ini(j, 2);
                    indexsel_lambda(k) = index_sel_ini(j, 3);
                    indexsel_CV(k) = index_sel_ini(j, 4);
                    indexsel_sigma2(k) = log(indexsel_CV(k) * indexsel_CV(k) + 1.0);
                    indexsel_sigma(k) = sqrt(indexsel_sigma2(k));
                }
            } else if (index_sel_option(ind) == 3) {
                for (ia = 1; ia <= 4; ia++) {
                    k += 1;
                    j = (indavail - 1)*(nages + 6) + nages + 2 + ia;
                    indexsel_initial(k) = index_sel_ini(j, 1);
                    indexsel_lo(k) = 0.0;
                    indexsel_hi(k) = nages;
                    indexsel_phase(k) = index_sel_ini(j, 2);
                    indexsel_lambda(k) = index_sel_ini(j, 3);
                    indexsel_CV(k) = index_sel_ini(j, 4);
                    indexsel_sigma2(k) = log(indexsel_CV(k) * indexsel_CV(k) + 1.0);
                    indexsel_sigma(k) = sqrt(indexsel_sigma2(k));
                }
            }
            j = 0;
            for (iyear = 1; iyear <= nyears; iyear++) {
                i = (indavail - 1) * nyears + iyear;
                index_sel_input(ind, iyear) = --(--(--index_ini(i)(4, 3 + nages)));
                if (index_ini(i, 2) > 0.0) {
                    j += 1;
                    index_time(ind, j) = index_ini(i, 1) - year1 + 1;
                    index_year(ind, j) = index_ini(i, 1);
                    index_obs(ind, j) = index_ini(i, 2);
                    index_cv(ind, j) = index_ini(i, 3);
                    index_sigma2(ind, j) = log(index_cv(ind, j) * index_cv(ind, j) + 1.0);
                    index_sigma(ind, j) = sqrt(index_sigma2(ind, j));
                    input_eff_samp_size_index(ind, j) = index_ini(i, nages + 4);
                    tempsum = sum(index_sel_input(ind, iyear)(index_start_age(ind), index_end_age(ind)));
                    if (tempsum > 0.0) {
                        for (iage = index_start_age(ind); iage <= index_end_age(ind); iage++) {
                            index_prop_obs(ind, j, iage) = index_sel_input(ind, iyear, iage) / tempsum;
                        }
                    }
                }
            }
        }
    }
    index_like_const = 0.0;
    if (use_likelihood_constants == 1) {
        for (ind = 1; ind <= nindices; ind++) {
            index_like_const(ind) = 0.5 * double(index_nobs(ind)) * log(2.0 * pi) + sum(log(index_obs(ind)));
        }
    }
    index_WAA = 0.0;
    for (ind = 1; ind <= nindices; ind++) {
        if (index_units_aggregate(ind) == 1 || index_units_proportions(ind) == 1) {
            int ipointindex = (index_WAApoint(ind) - 1) * nyears;
            for (iyear = 1; iyear <= nyears; iyear++) {
                index_WAA(ind, iyear) = WAA_ini((ipointindex + iyear));
            }
        }
    }
    phase_Fmult_year1.allocate("phase_Fmult_year1");
    ICHECK(phase_Fmult_year1);
    phase_Fmult_devs.allocate("phase_Fmult_devs");
    ICHECK(phase_Fmult_devs);
    phase_recruit_devs.allocate("phase_recruit_devs");
    ICHECK(phase_recruit_devs);
    phase_N_year1_devs.allocate("phase_N_year1_devs");
    ICHECK(phase_N_year1_devs);
    phase_q_year1.allocate("phase_q_year1");
    ICHECK(phase_q_year1);
    phase_q_devs.allocate("phase_q_devs");
    ICHECK(phase_q_devs);
    phase_SR_scaler.allocate("phase_SR_scaler");
    ICHECK(phase_SR_scaler);
    phase_steepness.allocate("phase_steepness");
    ICHECK(phase_steepness);
    recruit_CV.allocate(1, nyears, "recruit_CV");
    ICHECK(recruit_CV);
    recruit_sigma2.allocate(1, nyears);
    recruit_sigma.allocate(1, nyears);
    for (iyear = 1; iyear <= nyears; iyear++) {
        if (recruit_CV(iyear) <= 0.0)
            recruit_CV(iyear) = CVfill;
        recruit_sigma2(iyear) = log(recruit_CV(iyear) * recruit_CV(iyear) + 1.0);
        recruit_sigma(iyear) = sqrt(recruit_sigma2(iyear));
    }
    SR_like_const = 0.0;
    if (use_likelihood_constants == 1)
        SR_like_const = 0.5 * double(nyears) * log(2.0 * pi);
    lambda_ind_ini.allocate(1, navailindices, "lambda_ind_ini");
    ICHECK(lambda_ind_ini);
    lambda_catch_tot.allocate(1, nfleets, "lambda_catch_tot");
    ICHECK(lambda_catch_tot);
    lambda_Discard_tot.allocate(1, nfleets, "lambda_Discard_tot");
    ICHECK(lambda_Discard_tot);
    catch_tot_CV.allocate(1, nyears, 1, nfleets, "catch_tot_CV");
    ICHECK(catch_tot_CV);
    discard_tot_CV.allocate(1, nyears, 1, nfleets, "discard_tot_CV");
    ICHECK(discard_tot_CV);
    catch_tot_sigma2.allocate(1, nfleets, 1, nyears);
    catch_tot_sigma.allocate(1, nfleets, 1, nyears);
    discard_tot_sigma2.allocate(1, nfleets, 1, nyears);
    discard_tot_sigma.allocate(1, nfleets, 1, nyears);
    input_eff_samp_size_catch_ini.allocate(1, nyears, 1, nfleets, "input_eff_samp_size_catch_ini");
    ICHECK(input_eff_samp_size_catch_ini);
    input_eff_samp_size_discard_ini.allocate(1, nyears, 1, nfleets, "input_eff_samp_size_discard_ini");
    ICHECK(input_eff_samp_size_discard_ini);
    input_eff_samp_size_catch.allocate(1, nfleets, 1, nyears);
    input_eff_samp_size_discard.allocate(1, nfleets, 1, nyears);
    for (iyear = 1; iyear <= nyears; iyear++) {
        for (ifleet = 1; ifleet <= nfleets; ifleet++) {
            if (catch_tot_CV(iyear, ifleet) <= 0.0)
                catch_tot_CV(iyear, ifleet) = CVfill;
            if (discard_tot_CV(iyear, ifleet) <= 0.0)
                discard_tot_CV(iyear, ifleet) = CVfill;

            std::cout << "catch_tot_CV:\n";
            std::cout << catch_tot_CV << "\n";
            catch_tot_sigma2(ifleet, iyear) = log(catch_tot_CV(iyear, ifleet) * catch_tot_CV(iyear, ifleet) + 1.0);
            catch_tot_sigma(ifleet, iyear) = sqrt(catch_tot_sigma2(ifleet, iyear));


            discard_tot_sigma2(ifleet, iyear) = log(discard_tot_CV(iyear, ifleet) * discard_tot_CV(iyear, ifleet) + 1.0);
            discard_tot_sigma(ifleet, iyear) = sqrt(discard_tot_sigma2(ifleet, iyear));
            input_eff_samp_size_catch(ifleet, iyear) = input_eff_samp_size_catch_ini(iyear, ifleet);
            input_eff_samp_size_discard(ifleet, iyear) = input_eff_samp_size_discard_ini(iyear, ifleet);
        }
    }
    std::cout << "catch_tot_sigma2:\n";
    std::cout << catch_tot_sigma2 << "\n";
    //    exit(0);
    lambda_Fmult_year1.allocate(1, nfleets, "lambda_Fmult_year1");
    ICHECK(lambda_Fmult_year1);
    Fmult_year1_CV.allocate(1, nfleets, "Fmult_year1_CV");
    ICHECK(Fmult_year1_CV);
    lambda_Fmult_devs.allocate(1, nfleets, "lambda_Fmult_devs");
    ICHECK(lambda_Fmult_devs);
    Fmult_devs_CV.allocate(1, nfleets, "Fmult_devs_CV");
    ICHECK(Fmult_devs_CV);
    lambda_N_year1_devs.allocate("lambda_N_year1_devs");
    ICHECK(lambda_N_year1_devs);
    N_year1_CV.allocate("N_year1_CV");
    ICHECK(N_year1_CV);
    lambda_recruit_devs.allocate("lambda_recruit_devs");
    ICHECK(lambda_recruit_devs);
    lambda_q_year1_ini.allocate(1, navailindices, "lambda_q_year1_ini");
    ICHECK(lambda_q_year1_ini);
    q_year1_CV_ini.allocate(1, navailindices, "q_year1_CV_ini");
    ICHECK(q_year1_CV_ini);
    lambda_q_devs_ini.allocate(1, navailindices, "lambda_q_devs_ini");
    ICHECK(lambda_q_devs_ini);
    q_devs_CV_ini.allocate(1, navailindices, "q_devs_CV_ini");
    ICHECK(q_devs_CV_ini);
    lambda_steepness.allocate("lambda_steepness");
    ICHECK(lambda_steepness);
    steepness_CV.allocate("steepness_CV");
    ICHECK(steepness_CV);
    lambda_SR_scaler.allocate("lambda_SR_scaler");
    ICHECK(lambda_SR_scaler);
    SR_scaler_CV.allocate("SR_scaler_CV");
    ICHECK(SR_scaler_CV);
    for (i = 1; i <= nfleets; i++) {
        if (Fmult_year1_CV(i) <= 0.0)
            Fmult_year1_CV(i) = CVfill;
        if (Fmult_devs_CV(i) <= 0.0)
            Fmult_devs_CV(i) = CVfill;
    }
    if (N_year1_CV <= 0.0)
        N_year1_CV = CVfill;
    for (i = 1; i <= navailindices; i++) {
        if (q_year1_CV_ini(i) <= 0.0)
            q_year1_CV_ini(i) = CVfill;
        if (q_devs_CV_ini(i) <= 0.0)
            q_devs_CV_ini(i) = CVfill;
    }
    if (steepness_CV <= 0.0)
        steepness_CV = CVfill;
    if (SR_scaler_CV <= 0.0)
        SR_scaler_CV = CVfill;
    Fmult_year1_sigma2.allocate(1, nfleets);
    Fmult_year1_sigma.allocate(1, nfleets);
    Fmult_year1_like_const.allocate(1, nfleets);
    Fmult_devs_sigma2.allocate(1, nfleets);
    Fmult_devs_sigma.allocate(1, nfleets);
    Fmult_devs_like_const.allocate(1, nfleets);
    lambda_ind.allocate(1, nindices);
    lambda_q_year1.allocate(1, nindices);
    q_year1_CV.allocate(1, nindices);
    q_year1_sigma2.allocate(1, nindices);
    q_year1_sigma.allocate(1, nindices);
    q_year1_like_const.allocate(1, nindices);
    lambda_q_devs.allocate(1, nindices);
    q_devs_CV.allocate(1, nindices);
    q_devs_sigma2.allocate(1, nindices);
    q_devs_sigma.allocate(1, nindices);
    q_devs_like_const.allocate(1, nindices);
    NAA_year1_flag.allocate("NAA_year1_flag");
    ICHECK(NAA_year1_flag);
    NAA_year1_ini.allocate(1, nages, "NAA_year1_ini");
    ICHECK(NAA_year1_ini);
    Fmult_year1_ini.allocate(1, nfleets, "Fmult_year1_ini");
    ICHECK(Fmult_year1_ini);
    q_year1_iniavail.allocate(1, navailindices, "q_year1_iniavail");
    ICHECK(q_year1_iniavail);
    q_year1_ini.allocate(1, nindices);
    is_SR_scaler_R.allocate("is_SR_scaler_R");
    ICHECK(is_SR_scaler_R);
    SR_scaler_ini.allocate("SR_scaler_ini");
    ICHECK(SR_scaler_ini);
    SR_steepness_ini.allocate("SR_steepness_ini");
    ICHECK(SR_steepness_ini);
    Fmult_max_value.allocate("Fmult_max_value");
    ICHECK(Fmult_max_value);
    ignore_guesses.allocate("ignore_guesses");
    ICHECK(ignore_guesses);
    do_projections.allocate("do_projections");
    ICHECK(do_projections);
    directed_fleet.allocate(1, nfleets, "directed_fleet");
    ICHECK(directed_fleet);
    nfinalyear.allocate("nfinalyear");
    ICHECK(nfinalyear);
    nprojyears = nfinalyear - year1 - nyears + 1;
    project_ini.allocate(1, nprojyears, 1, 5, "project_ini");
    ICHECK(project_ini);
    proj_recruit.allocate(1, nprojyears);
    proj_what.allocate(1, nprojyears);
    proj_target.allocate(1, nprojyears);
    proj_F_nondir_mult.allocate(1, nprojyears);
    for (iyear = 1; iyear <= nprojyears; iyear++) {
        proj_recruit(iyear) = project_ini(iyear, 2);
        proj_what(iyear) = project_ini(iyear, 3);
        proj_target(iyear) = project_ini(iyear, 4);
        proj_F_nondir_mult(iyear) = project_ini(iyear, 5);
    }
    doMCMC.allocate("doMCMC");
    ICHECK(doMCMC);
    if (doMCMC == 1) {
        basicMCMC << " ";
        for (iyear = 1; iyear <= nyears; iyear++) {
            basicMCMC << "F" << iyear + year1 - 1 << " ";
        }
        for (iyear = 1; iyear <= nyears; iyear++) {
            basicMCMC << "SSB" << iyear + year1 - 1 << " ";
        }
        // Liz added Fmult_in lastyear and totBjan1
        for (iyear = 1; iyear <= nyears; iyear++) {
            basicMCMC << "Fmult_" << iyear + year1 - 1 << " ";
        }
        for (iyear = 1; iyear <= nyears; iyear++) {
            basicMCMC << "totBjan1_" << iyear + year1 - 1 << " ";
        }
        // end stuff Liz added
        basicMCMC << "MSY  SSBmsy  Fmsy  SSBmsy_ratio  Fmsy_ratio  ";
        basicMCMC << endl; // end of header line
    }
    MCMCnyear_opt.allocate("MCMCnyear_opt");
    ICHECK(MCMCnyear_opt)
    MCMCnboot.allocate("MCMCnboot");
    ICHECK(MCMCnboot);
    MCMCnthin.allocate("MCMCnthin");
    ICHECK(MCMCnthin);
    MCMCseed.allocate("MCMCseed");
    ICHECK(MCMCseed);
    fillR_opt.allocate("fillR_opt");
    ICHECK(fillR_opt);
    Ravg_start.allocate("Ravg_start");
    ICHECK(Ravg_start);
    Ravg_end.allocate("Ravg_end");
    ICHECK(Ravg_end);
    make_Rfile.allocate("make_Rfile");
    ICHECK(make_Rfile);
    test_value.allocate("test_value");
    ICHECK(test_value)
    cout << "test value = " << test_value << endl; //CHECK
    cout << "input complete" << endl;
    s_per_r_vec.allocate(1, nyears);
    for (iyear = 1; iyear <= nyears; iyear++) {
        ntemp0 = 1.0;
        s_per_r_vec(iyear) = 0.0;
        for (iage = 1; iage < nages; iage++) {
            s_per_r_vec(iyear) += ntemp0 * fecundity(iyear, iage) * mfexp(-1.0 * fracyearSSB * M(iyear, iage));
            ntemp0 *= mfexp(-M(iyear, iage));
        }
        ntemp0 /= (1.0 - mfexp(-M(iyear, nages)));
        s_per_r_vec(iyear) += ntemp0 * fecundity(iyear, nages) * mfexp(-1.0 * fracyearSSB * M(iyear, nages));
    }
    SR_spawners_per_recruit = s_per_r_vec(nyears); // use last year calculations for SR curve


}

void model_data::create_mas_data_file(std::string path) {

    rapidjson::Document document;
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    document.SetObject();
    document.AddMember<int>("years", this->nyears, allocator);
    document.AddMember<int>("seasons", 1, allocator);
    document.AddMember<int>("ages", this->nages, allocator);



    //catch biomass
    for (int i = 0; i < this->nfleets; i++) {


        rapidjson::Value dobject(rapidjson::kObjectType);
        dobject.AddMember("id", i + 1, allocator);
        std::stringstream ss;
        ss << "catch_biomass_" << i + 1;
        rapidjson::Value name;
        name.SetString(ss.str().c_str(), ss.str().size(), allocator);
        dobject.AddMember("name", name, allocator);
        dobject.AddMember("data_object_type", "catch_biomass", allocator);
        rapidjson::Value aop(rapidjson::kArrayType);
        rapidjson::Value aop_entry(rapidjson::kObjectType);
        aop_entry.AddMember("area", 1, allocator);
        aop_entry.AddMember("season", 1, allocator);
        aop.PushBack(aop_entry, allocator);
        dobject.AddMember("areas_of_operation", aop, allocator);
        dobject.AddMember("sex", "undifferentiated", allocator);
        dobject.AddMember("units", "MT", allocator);
        dobject.AddMember("missing_values", -999, allocator);
        rapidjson::Value cbvalues(rapidjson::kArrayType);
        rapidjson::Value cberror(rapidjson::kArrayType);
        std::vector<double> cbb;
        std::cout << CAA_prop_obs << "\n";
        for (int y = 1; y <= this->nyears; y++) {
            double cb = CAA_ini(y, this->nages + 1);


            rapidjson::Value cbv(rapidjson::kArrayType);
            cbv.PushBack(cb, allocator);
            cbvalues.PushBack(cbv, allocator);
            rapidjson::Value cbe(rapidjson::kArrayType);
            cbe.PushBack(0.01, allocator);
            cberror.PushBack(cbe, allocator);

        }

        dobject.AddMember("values", cbvalues, allocator);
        dobject.AddMember("observation_error", cberror, allocator);
        document.AddMember("data_object", dobject, allocator);
    }

    //survey biomass
    for (int i = 1; i <= this->navailindices; i++) {


        rapidjson::Value dobject(rapidjson::kObjectType);
        dobject.AddMember("id", i, allocator);
        std::stringstream ss;
        ss << "survey_biomass_" << i + 1;
        rapidjson::Value name;
        name.SetString(ss.str().c_str(), ss.str().size(), allocator);
        dobject.AddMember("name", name, allocator);
        dobject.AddMember("data_object_type", "survey_biomass", allocator);
        rapidjson::Value aop(rapidjson::kArrayType);
        rapidjson::Value aop_entry(rapidjson::kObjectType);
        aop_entry.AddMember("area", 1, allocator);
        aop_entry.AddMember("season", 1, allocator);
        aop.PushBack(aop_entry, allocator);
        dobject.AddMember("areas_of_operation", aop, allocator);
        dobject.AddMember("sex", "undifferentiated", allocator);
        dobject.AddMember("units", "MT", allocator);
        dobject.AddMember("missing_values", 0, allocator);
        rapidjson::Value cbvalues(rapidjson::kArrayType);
        rapidjson::Value cberror(rapidjson::kArrayType);
        std::vector<double> cbb;
        for (int y = 1; y <= this->nyears; y++) {
            double cb = this->index_obs(i, y);


            rapidjson::Value cbv(rapidjson::kArrayType);
            cbv.PushBack(cb, allocator);
            cbvalues.PushBack(cbv, allocator);
            rapidjson::Value cbe(rapidjson::kArrayType);
            cbe.PushBack(0.01, allocator);
            cberror.PushBack(cbe, allocator);

        }

        dobject.AddMember("values", cbvalues, allocator);
        dobject.AddMember("observation_error", cberror, allocator);
        document.AddMember("data_object", dobject, allocator);
    }




    //proportions at age
    for (int i = 0; i < this->nfleets; i++) {
        rapidjson::Value dobject(rapidjson::kObjectType);
        dobject.AddMember("id", i + 1, allocator);
        std::stringstream ss;
        ss << "catch_proportion_at_age_" << i + 1;
        rapidjson::Value name;
        name.SetString(ss.str().c_str(), ss.str().size(), allocator);
        //        dobject.AddMember("name", name, allocator);
        dobject.AddMember("data_object_type", "catch_proportion_at_age", allocator);
        rapidjson::Value aop(rapidjson::kArrayType);
        rapidjson::Value aop_entry(rapidjson::kObjectType);
        aop_entry.AddMember("area", 1, allocator);
        aop_entry.AddMember("season", 1, allocator);
        aop.PushBack(aop_entry, allocator);
        dobject.AddMember("areas_of_operation", aop, allocator);
        dobject.AddMember("sex", "undifferentiated", allocator);
        dobject.AddMember("units", "NA", allocator);
        dobject.AddMember("missing_values", 0, allocator);
        rapidjson::Value values(rapidjson::kArrayType);
        rapidjson::Value eff_sample(rapidjson::kArrayType);
        rapidjson::Value oerror(rapidjson::kArrayType);
        std::cout << CAA_prop_obs << "\n";
        for (int y = 1; y <= this->nyears; y++) {


            rapidjson::Value eff_val(rapidjson::kArrayType);
            eff_val.PushBack(input_eff_samp_size_catch_ini(y, i + 1), allocator);
            eff_sample.PushBack(eff_val, allocator);
            rapidjson::Value yvalues(rapidjson::kArrayType);
            rapidjson::Value yoerror(rapidjson::kArrayType);
            for (int s = 0; s < 1; s++) {
                rapidjson::Value svalues(rapidjson::kArrayType);
                rapidjson::Value soerror(rapidjson::kArrayType);
                for (int a = 1; a <= this->nages; a++) {
                    std::cout << this->CAA_prop_obs(i + 1, y, a) << "\n";
                    svalues.PushBack(this->CAA_prop_obs(i + 1, y, a), allocator);
                    soerror.PushBack(0.001, allocator);
                }

                yvalues.PushBack(svalues, allocator);
                yoerror.PushBack(soerror, allocator);
            }
            values.PushBack(yvalues, allocator);
            oerror.PushBack(yoerror, allocator);
        }

        //input_eff_samp_size_catch_ini

        dobject.AddMember("values", values, allocator);
        dobject.AddMember("observation_error", oerror, allocator);
        dobject.AddMember("sample_size", eff_sample, allocator);
        document.AddMember("data_object", dobject, allocator);
    }

    int index_index = 1;
    for (int i = 0; i < this->navailindices; i++) {
        rapidjson::Value dobject(rapidjson::kObjectType);
        dobject.AddMember("id", i + 1, allocator);
        std::stringstream ss;
        ss << "catch_proportion_at_age_" << i + 1;
        rapidjson::Value name;
        name.SetString(ss.str().c_str(), ss.str().size(), allocator);
        //        dobject.AddMember("name", name, allocator);
        dobject.AddMember("data_object_type", "survey_proportion_at_age", allocator);
        rapidjson::Value aop(rapidjson::kArrayType);
        rapidjson::Value aop_entry(rapidjson::kObjectType);
        aop_entry.AddMember("area", 1, allocator);
        aop_entry.AddMember("season", 1, allocator);
        aop.PushBack(aop_entry, allocator);
        dobject.AddMember("areas_of_operation", aop, allocator);
        dobject.AddMember("sex", "undifferentiated", allocator);
        dobject.AddMember("units", "NA", allocator);
        dobject.AddMember("missing_values", 0, allocator);
        rapidjson::Value values(rapidjson::kArrayType);
        rapidjson::Value eff_sample(rapidjson::kArrayType);
        rapidjson::Value oerror(rapidjson::kArrayType);
        for (int y = 1; y <= this->nyears; y++) {


            rapidjson::Value eff_val(rapidjson::kArrayType);
            eff_val.PushBack(index_ini(index_index, this->nages + 4), allocator);
            eff_sample.PushBack(eff_val, allocator);
            rapidjson::Value yvalues(rapidjson::kArrayType);
            rapidjson::Value yoerror(rapidjson::kArrayType);
            for (int s = 0; s < 1; s++) {
                rapidjson::Value svalues(rapidjson::kArrayType);
                rapidjson::Value soerror(rapidjson::kArrayType);
                for (int a = 1; a <= this->nages; a++) {
                    std::cout << this->index_ini(index_index, a + 3) << "\n";
                    svalues.PushBack(this->index_ini(index_index, a + 3), allocator);
                    soerror.PushBack(0.001, allocator);

                }

                yvalues.PushBack(svalues, allocator);
                yoerror.PushBack(soerror, allocator);
            }
            values.PushBack(yvalues, allocator);
            oerror.PushBack(yoerror, allocator);
            index_index++;
        }

        //input_eff_samp_size_catch_ini

        dobject.AddMember("values", values, allocator);
        dobject.AddMember("observation_error", oerror, allocator);
        dobject.AddMember("sample_size", eff_sample, allocator);
        document.AddMember("data_object", dobject, allocator);
    }

    //empirical catch weight at age
    for (int i = 0; i < this->nfleets; i++) {
        rapidjson::Value dobject(rapidjson::kObjectType);
        dobject.AddMember("id", i + 1, allocator);
        dobject.AddMember("data_object_type", "catch_empirical_weight_at_age", allocator);
        rapidjson::Value aop(rapidjson::kArrayType);
        rapidjson::Value aop_entry(rapidjson::kObjectType);
        aop_entry.AddMember("area", 1, allocator);
        aop_entry.AddMember("season", 1, allocator);
        aop.PushBack(aop_entry, allocator);
        dobject.AddMember("areas_of_operation", aop, allocator);
        dobject.AddMember("sex", "undifferentiated", allocator);
        dobject.AddMember("units", "KG", allocator);
        dobject.AddMember("missing_values", -999, allocator);
        rapidjson::Value values(rapidjson::kArrayType);
        rapidjson::Value oerror(rapidjson::kArrayType);


        for (int y = 1; y <= this->nyears; y++) {
            rapidjson::Value cbvalues(rapidjson::kArrayType);
            rapidjson::Value cberror(rapidjson::kArrayType);
            rapidjson::Value yvalues(rapidjson::kArrayType);
            rapidjson::Value yoerror(rapidjson::kArrayType);
            for (int s = 0; s < 1; s++) {
                rapidjson::Value svalues(rapidjson::kArrayType);
                rapidjson::Value soerror(rapidjson::kArrayType);
                for (int a = 1; a <= this->nages; a++) {
                    std::cout << this->CAA_prop_obs(i + 1, y, a) << "\n";
                    svalues.PushBack(this->WAAcatchfleet(i + 1, y, a), allocator);
                    soerror.PushBack(0.001, allocator);
                }
                yvalues.PushBack(svalues, allocator);
                yoerror.PushBack(soerror, allocator);
            }
            values.PushBack(yvalues, allocator);
            oerror.PushBack(yoerror, allocator);
        }

        dobject.AddMember("values", values, allocator);
        dobject.AddMember("observation_error", oerror, allocator);
        document.AddMember("data_object", dobject, allocator);
    }


    //empirical catch weight at age
    for (int i = 0; i < this->nindices; i++) {
        rapidjson::Value dobject(rapidjson::kObjectType);
        dobject.AddMember("id", i + 1, allocator);
        dobject.AddMember("data_object_type", "survey_empirical_weight_at_age", allocator);
        rapidjson::Value aop(rapidjson::kArrayType);
        rapidjson::Value aop_entry(rapidjson::kObjectType);
        aop_entry.AddMember("area", 1, allocator);
        aop_entry.AddMember("season", 1, allocator);
        aop.PushBack(aop_entry, allocator);
        dobject.AddMember("areas_of_operation", aop, allocator);
        dobject.AddMember("sex", "undifferentiated", allocator);
        dobject.AddMember("units", "KG", allocator);
        dobject.AddMember("missing_values", -999, allocator);
        rapidjson::Value values(rapidjson::kArrayType);
        rapidjson::Value oerror(rapidjson::kArrayType);


        for (int y = 1; y <= this->nyears; y++) {
            rapidjson::Value cbvalues(rapidjson::kArrayType);
            rapidjson::Value cberror(rapidjson::kArrayType);
            rapidjson::Value yvalues(rapidjson::kArrayType);
            rapidjson::Value yoerror(rapidjson::kArrayType);
            for (int s = 0; s < 1; s++) {
                rapidjson::Value svalues(rapidjson::kArrayType);
                rapidjson::Value soerror(rapidjson::kArrayType);
                for (int a = 1; a <= this->nages; a++) {
                    svalues.PushBack(this->WAAcatchfleet(1, y, a), allocator);
                    soerror.PushBack(0.001, allocator);
                }
                yvalues.PushBack(svalues, allocator);
                yoerror.PushBack(soerror, allocator);
            }
            values.PushBack(yvalues, allocator);
            oerror.PushBack(yoerror, allocator);
        }

        dobject.AddMember("values", values, allocator);
        dobject.AddMember("observation_error", oerror, allocator);
        document.AddMember("data_object", dobject, allocator);
    }

    rapidjson::Value ssdobject(rapidjson::kObjectType);
    ssdobject.AddMember("id", 1, allocator);
    ssdobject.AddMember("data_object_type", "empirical_weight_at_age_season_start", allocator);
    ssdobject.AddMember("sex", "undifferentiated", allocator);
    ssdobject.AddMember("units", "KG", allocator);
    ssdobject.AddMember("missing_values", -999, allocator);
    rapidjson::Value ssvalues(rapidjson::kArrayType);
    rapidjson::Value ssoerror(rapidjson::kArrayType);


    for (int y = 1; y <= this->nyears; y++) {

        rapidjson::Value yvalues(rapidjson::kArrayType);
        rapidjson::Value yoerror(rapidjson::kArrayType);
        for (int s = 0; s < 1; s++) {
            rapidjson::Value soerror(rapidjson::kArrayType);
            for (int a = 1; a <= this->nages; a++) {
                yvalues.PushBack(this->WAAcatchfleet(1, y, a), allocator);
                yoerror.PushBack(0.001, allocator);
            }

        }
        ssvalues.PushBack(yvalues, allocator);
        ssoerror.PushBack(yoerror, allocator);
    }


    ssdobject.AddMember("values", ssvalues, allocator);
    ssdobject.AddMember("observation_error", ssoerror, allocator);
    document.AddMember("data_object", ssdobject, allocator);

    rapidjson::Value spdobject(rapidjson::kObjectType);
    spdobject.AddMember("id", 1, allocator);
    spdobject.AddMember("data_object_type", "empirical_weight_at_age_spawning", allocator);
    spdobject.AddMember("sex", "undifferentiated", allocator);
    spdobject.AddMember("units", "KG", allocator);
    spdobject.AddMember("missing_values", -999, allocator);
    rapidjson::Value spvalues(rapidjson::kArrayType);
    rapidjson::Value spoerror(rapidjson::kArrayType);


    for (int y = 1; y <= this->nyears; y++) {

        rapidjson::Value yvalues(rapidjson::kArrayType);
        rapidjson::Value yoerror(rapidjson::kArrayType);
        for (int s = 0; s < 1; s++) {
            rapidjson::Value soerror(rapidjson::kArrayType);
            for (int a = 1; a <= this->nages; a++) {
                yvalues.PushBack(this->WAAcatchfleet(1, y, a), allocator);
                yoerror.PushBack(0.001, allocator);
            }

        }
        spvalues.PushBack(yvalues, allocator);
        spoerror.PushBack(yoerror, allocator);
    }

    spdobject.AddMember("values", spvalues, allocator);
    spdobject.AddMember("observation_error", spoerror, allocator);
    document.AddMember("data_object", spdobject, allocator);



    std::cout << this->WAAcatchfleet << "\n";
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    std::ofstream out;
    out.open(path);
    if (out.good()) {
        out << buffer.GetString();
    }
    out.close();
}

void model_parameters::create_mas_config_file(std::string path) {
    rapidjson::Document document;
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    document.SetObject();
    document.AddMember("years", this->nyears, allocator);
    document.AddMember("seasons", 1, allocator);
    document.AddMember("first_year", this->year1, allocator);
    document.AddMember("last_year", this->year1 + this->nyears, allocator);
    rapidjson::Value ages(rapidjson::kArrayType);
    double iage;
    for (iage = 1; iage <= nages; iage++) {
        ages.PushBack(iage, allocator);
    }
    document.AddMember("ages", ages, allocator);
    document.AddMember("extended_plus_group", static_cast<int> (iage + 2), allocator);

    rapidjson::Value area_object(rapidjson::kObjectType);
    area_object.AddMember("id", 1, allocator);
    document.AddMember("area", area_object, allocator);

    double sigma = 0.0;
    for (int i = 1; i <= this->nyears; i++) {
        sigma += recruit_sigma(i);
    }
    sigma /= this->nyears;

    rapidjson::Value recruitment_object(rapidjson::kObjectType);
    recruitment_object.AddMember("model", "beverton_holt_alt", allocator);
    recruitment_object.AddMember("id", 1, allocator);

    rapidjson::Value recruitment_parameters(rapidjson::kObjectType);
    rapidjson::Value r0_obj(rapidjson::kObjectType);
    r0_obj.AddMember("value", value(mfexp(log_SR_scaler)), allocator);
    r0_obj.AddMember("min", -1.0, allocator);
    r0_obj.AddMember("max", 200.0, allocator);
    r0_obj.AddMember("estimated", "false", allocator);
    r0_obj.AddMember("phase", 1, allocator);

    recruitment_parameters.AddMember("R0", r0_obj, allocator);

    rapidjson::Value h_obj(rapidjson::kObjectType);
    h_obj.AddMember("value", value(this->SR_steepness), allocator);
    h_obj.AddMember("min", 0.0, allocator);
    h_obj.AddMember("max", 1.0, allocator);
    h_obj.AddMember("estimated", "false", allocator);
    h_obj.AddMember("phase", 1, allocator);
    recruitment_parameters.AddMember("h", h_obj, allocator);

    rapidjson::Value sigma_obj(rapidjson::kObjectType);
    sigma_obj.AddMember("value", sigma, allocator);
    sigma_obj.AddMember("estimated", "false", allocator);
    sigma_obj.AddMember("phase", 1, allocator);
    recruitment_parameters.AddMember("sigma_r", sigma_obj, allocator);

    rapidjson::Value devs_obj(rapidjson::kObjectType);
    rapidjson::Value recruitment_devsa(rapidjson::kArrayType);
    for (int i = 0; i < this->nyears; i++) {
        recruitment_devsa.PushBack(0.0, allocator);
    }
    devs_obj.AddMember("constrained", "true", allocator);
    devs_obj.AddMember("estimated", "false", allocator);
    devs_obj.AddMember("phase", 1, allocator);
    devs_obj.AddMember("values", recruitment_devsa, allocator);

    recruitment_parameters.AddMember("recruitment_deviations", devs_obj, allocator);
    recruitment_object.AddMember("parameters", recruitment_parameters, allocator);
    document.AddMember("recruitment", recruitment_object, allocator);

    rapidjson::Value growth_object(rapidjson::kObjectType);
    growth_object.AddMember("id", 1, allocator);
    growth_object.AddMember("model", "von_bertalanffy_modified", allocator);
    growth_object.AddMember("comment", "this is a dummy model and may require modification.", allocator);

    rapidjson::Value growth_parameters(rapidjson::kObjectType);
    rapidjson::Value amin(rapidjson::kObjectType);
    amin.AddMember("value", 0.01, allocator);
    amin.AddMember("estimated", "false", allocator);
    growth_parameters.AddMember("amin", amin, allocator);

    rapidjson::Value amax(rapidjson::kObjectType);
    amax.AddMember("value", this->nages, allocator);
    amax.AddMember("estimated", "false", allocator);
    growth_parameters.AddMember("amax", amax, allocator);

    rapidjson::Value lmin(rapidjson::kObjectType);
    lmin.AddMember("value", 5, allocator);
    lmin.AddMember("units", "cm", allocator);
    lmin.AddMember("estimated", "false", allocator);
    growth_parameters.AddMember("lmin", lmin, allocator);

    rapidjson::Value lmax(rapidjson::kObjectType);
    lmax.AddMember("value", 50, allocator);
    lmax.AddMember("units", "cm", allocator);
    lmax.AddMember("estimated", "false", allocator);
    growth_parameters.AddMember("lmax", lmax, allocator);

    rapidjson::Value growth_c(rapidjson::kObjectType);
    growth_c.AddMember("value", 0.3, allocator);
    growth_c.AddMember("estimated", "false", allocator);
    growth_parameters.AddMember("growth_c", growth_c, allocator);

    rapidjson::Value alpha_f(rapidjson::kObjectType);
    alpha_f.AddMember("value", 0.000025, allocator);
    alpha_f.AddMember("estimated", "false", allocator);
    growth_parameters.AddMember("alpha_f", alpha_f, allocator);

    rapidjson::Value beta_f(rapidjson::kObjectType);
    beta_f.AddMember("value", 2.9624, allocator);
    beta_f.AddMember("estimated", "false", allocator);
    growth_parameters.AddMember("beta_f", beta_f, allocator);

    rapidjson::Value alpha_m(rapidjson::kObjectType);
    alpha_m.AddMember("value", 0.000025, allocator);
    alpha_m.AddMember("estimated", "false", allocator);
    growth_parameters.AddMember("alpha_f", alpha_m, allocator);

    rapidjson::Value beta_m(rapidjson::kObjectType);
    beta_m.AddMember("value", 2.9624, allocator);
    beta_m.AddMember("estimated", "false", allocator);
    growth_parameters.AddMember("beta_m", beta_m, allocator);

    growth_object.AddMember("parameters", growth_parameters, allocator);
    document.AddMember("growth", growth_object, allocator);

    int selectivity_count = 1;
    //fleet selectivity
    for (int i = 1; i <= this->nselblocks; i++) {
        rapidjson::Value selectivity_object(rapidjson::kObjectType);
        rapidjson::Value selectivity_parameters(rapidjson::kObjectType);

        if (this->sel_option(i) == 1) {
            selectivity_object.AddMember("model", "age_based", allocator);
            rapidjson::Value s(rapidjson::kObjectType);
            rapidjson::Value values(rapidjson::kArrayType);
            rapidjson::Value min_values(rapidjson::kArrayType);
            rapidjson::Value max_values(rapidjson::kArrayType);
            for (int i = 0; i <this->nages; i++) {
                values.PushBack(0.99, allocator);
                min_values.PushBack(0.0, allocator);
                max_values.PushBack(1.0, allocator);
            }
            s.AddMember("values", values, allocator);
            s.AddMember("min", min_values, allocator);
            s.AddMember("max", max_values, allocator);
            s.AddMember("estimated", "false", allocator);
            s.AddMember("min", 0.0, allocator);
            s.AddMember("max", 1.0, allocator);
            s.AddMember("phase", (int) this->sel_ini(this->nages + 1, 2), allocator);
            selectivity_parameters.AddMember("s", s, allocator);
        } else if (this->sel_option(i) == 2) {
            selectivity_object.AddMember("model", "logistic", allocator);
            rapidjson::Value a50(rapidjson::kObjectType);
            a50.AddMember("value", this->sel_ini(this->nages + 1, 1), allocator);
            a50.AddMember("estimated", "false", allocator);
            a50.AddMember("phase", (int) this->sel_ini(this->nages + 1, 2), allocator);
            a50.AddMember("min", this->sel_ini(this->nages + 1, 3), allocator);
            a50.AddMember("max", this->sel_ini(this->nages + 1, 4), allocator);
            selectivity_parameters.AddMember("a50", a50, allocator);

            rapidjson::Value slope(rapidjson::kObjectType);
            slope.AddMember("value", this->sel_ini(this->nages + 2, 1), allocator);
            slope.AddMember("estimated", "false", allocator);
            slope.AddMember("phase", (int) this->sel_ini(this->nages + 2, 2), allocator);
            slope.AddMember("min", this->sel_ini(this->nages + 2, 3), allocator);
            slope.AddMember("max", this->sel_ini(this->nages + 2, 4), allocator);
            selectivity_parameters.AddMember("s", slope, allocator);
        } else if (this->sel_option(1) == 3) {
            selectivity_object.AddMember("model", "double_logistic", allocator);
        }
        selectivity_object.AddMember("id", selectivity_count, allocator);
        selectivity_object.AddMember("parameters", selectivity_parameters, allocator);
        selectivity_count++;
        document.AddMember("selectivity", selectivity_object, allocator);
    }

    //survey selectivity
    for (int i = 1; i <= this->navailindices; i++) {
        rapidjson::Value selectivity_object(rapidjson::kObjectType);
        rapidjson::Value selectivity_parameters(rapidjson::kObjectType);

        if (this->index_sel_option(i) == 1) {
            selectivity_object.AddMember("model", "age_based", allocator);
            rapidjson::Value s(rapidjson::kObjectType);
            rapidjson::Value values(rapidjson::kArrayType);
            rapidjson::Value min_values(rapidjson::kArrayType);
            rapidjson::Value max_values(rapidjson::kArrayType);
            for (int i = 0; i <this->nages; i++) {
                values.PushBack(0.99, allocator);
                min_values.PushBack(0.0, allocator);
                max_values.PushBack(1.0, allocator);
            }
            s.AddMember("values", values, allocator);
            s.AddMember("min", min_values, allocator);
            s.AddMember("max", max_values, allocator);
            s.AddMember("estimated", "false", allocator);
            s.AddMember("min", 0.0, allocator);
            s.AddMember("max", 1.0, allocator);
            s.AddMember("phase", (int) this->sel_ini(this->nages + 1, 2), allocator);
            selectivity_parameters.AddMember("s", s, allocator);
        } else if (this->index_sel_option(i) == 2) {
            selectivity_object.AddMember("model", "logistic", allocator);
            rapidjson::Value a50(rapidjson::kObjectType);
            a50.AddMember("value", this->sel_ini(this->nages + 1, 1), allocator);
            a50.AddMember("estimated", "false", allocator);
            a50.AddMember("phase", (int) this->sel_ini(this->nages + 1, 2), allocator);
            a50.AddMember("min", this->sel_ini(this->nages + 1, 3), allocator);
            a50.AddMember("max", this->sel_ini(this->nages + 1, 4), allocator);
            selectivity_parameters.AddMember("a50", a50, allocator);

            rapidjson::Value slope(rapidjson::kObjectType);
            slope.AddMember("value", this->sel_ini(this->nages + 2, 1), allocator);
            slope.AddMember("estimated", "false", allocator);
            slope.AddMember("phase", (int) this->sel_ini(this->nages + 2, 2), allocator);
            slope.AddMember("min", this->sel_ini(this->nages + 2, 3), allocator);
            slope.AddMember("max", this->sel_ini(this->nages + 2, 4), allocator);
            selectivity_parameters.AddMember("s", slope, allocator);
        } else if (this->sel_option(1) == 3) {
            selectivity_object.AddMember("model", "double_logistic", allocator);
        }
        selectivity_object.AddMember("id", selectivity_count, allocator);
        selectivity_object.AddMember("parameters", selectivity_parameters, allocator);
        selectivity_count++;
        document.AddMember("selectivity", selectivity_object, allocator);
    }
    rapidjson::Value mort_array(rapidjson::kArrayType);
    rapidjson::Value mort_min_array(rapidjson::kArrayType);
    rapidjson::Value mort_max_array(rapidjson::kArrayType);

    std::vector<double> nm(this->nages);
    for (int iyear = 1; iyear <= nyears; iyear++) {
        for (int a = 1; a <= this->nages; a++) {
            nm[a - 1] += M(iyear, a);

        }
    }

    rapidjson::Value mort_obj(rapidjson::kObjectType);
    mort_obj.AddMember("id", 1, allocator);
    rapidjson::Value mort_parameters(rapidjson::kObjectType);
    for (int a = 1; a <= this->nages; a++) {
        mort_array.PushBack(nm[a - 1] / this->nyears, allocator);
        mort_min_array.PushBack(0.0, allocator);
        mort_max_array.PushBack(1.0, allocator);
    }

    mort_parameters.AddMember("comment", "These values are the average across all years from ASAP input", allocator);

    mort_parameters.AddMember("estimated", "false", allocator);
    mort_parameters.AddMember("values", mort_array, allocator);
    mort_parameters.AddMember("min", mort_min_array, allocator);
    mort_parameters.AddMember("max", mort_max_array, allocator);
    mort_obj.AddMember("parameters", mort_parameters, allocator);
    document.AddMember("natural_mortality", mort_obj, allocator);
    //mort_array.PushBack(M(iyear, a), allocator);
    //       mort_array.PushBack(M(iyear),allocaror);
    for (int f = 1; f <= this->nfleets; f++) {
        rapidjson::Value fmort_obj(rapidjson::kObjectType);
        fmort_obj.AddMember("id", f, allocator);
        rapidjson::Value fmort_params(rapidjson::kObjectType);
        fmort_params.AddMember("estimated", "false", allocator);
        rapidjson::Value mort_array(rapidjson::kArrayType);
        for (int y = 1; y <= this->nyears; y++) {
            rapidjson::Value farray(rapidjson::kArrayType);
            farray.PushBack(value(mfexp(log_Fmult(f, y))), allocator);
            mort_array.PushBack(farray, allocator);
        }
        fmort_params.AddMember("values", mort_array, allocator);
        fmort_obj.AddMember("parameters", fmort_params, allocator);
        document.AddMember("fishing_mortality", fmort_obj, allocator);
    }

    //log_Fmult
    rapidjson::Value movement(rapidjson::kObjectType);
    rapidjson::Value yarray(rapidjson::kArrayType);
    rapidjson::Value sarray(rapidjson::kArrayType);
    rapidjson::Value aarray(rapidjson::kArrayType);
    aarray.PushBack(1.0, allocator);
    sarray.PushBack(aarray, allocator);
    yarray.PushBack(sarray, allocator);

    rapidjson::Value yarray_(rapidjson::kArrayType);
    rapidjson::Value sarray_(rapidjson::kArrayType);
    rapidjson::Value aarray_(rapidjson::kArrayType);
    aarray_.PushBack(1.0, allocator);
    sarray_.PushBack(aarray_, allocator);
    yarray_.PushBack(sarray_, allocator);

    rapidjson::Value yarray__(rapidjson::kArrayType);
    rapidjson::Value sarray__(rapidjson::kArrayType);
    rapidjson::Value aarray__(rapidjson::kArrayType);
    aarray__.PushBack(1.0, allocator);
    sarray__.PushBack(aarray__, allocator);
    yarray__.PushBack(sarray__, allocator);

    movement.AddMember("id", 1, allocator);
    movement.AddMember("estimated", "false", allocator);
    movement.AddMember("recruits", yarray, allocator);
    movement.AddMember("male", yarray_, allocator);
    movement.AddMember("female", yarray__, allocator);
    document.AddMember("movement", movement, allocator);

    rapidjson::Value population(rapidjson::kObjectType);
    population.AddMember("id", 1, allocator);
    population.AddMember("sex_ratio", 0.5, allocator);


    rapidjson::Value pop_movement_array(rapidjson::kArrayType);
    for (int i = 1; i <= this->nyears; i++) {
        rapidjson::Value entry(rapidjson::kObjectType);
        entry.AddMember("year", i, allocator);
        entry.AddMember("id", 1, allocator);
        pop_movement_array.PushBack(entry, allocator);
    }
    population.AddMember("movement", pop_movement_array, allocator);

    rapidjson::Value pop_params(rapidjson::kObjectType);


    rapidjson::Value nm_params(rapidjson::kArrayType);
    rapidjson::Value nmm_entry(rapidjson::kObjectType);
    nmm_entry.AddMember("area", 1, allocator);
    nmm_entry.AddMember("id", 1, allocator);
    nmm_entry.AddMember("sex", "male", allocator);
    nm_params.PushBack(nmm_entry, allocator);

    rapidjson::Value nm_entry(rapidjson::kObjectType);
    nm_entry.AddMember("area", 1, allocator);
    nm_entry.AddMember("id", 1, allocator);
    nm_entry.AddMember("sex", "female", allocator);
    nm_params.PushBack(nm_entry, allocator);

    pop_params.AddMember("natural_mortality", nm_params, allocator);

    std::vector<double> maturity(this->nages);
    for (int iyear = 1; iyear <= nyears; iyear++) {
        for (int a = 1; a <= this->nages; a++) {
            maturity[a - 1] += mature(iyear, a);

        }
    }

    rapidjson::Value maturity_array(rapidjson::kArrayType);
    rapidjson::Value maturity_values_m(rapidjson::kArrayType);
    rapidjson::Value maturity_values_f(rapidjson::kArrayType);

    for (int a = 1; a <= this->nages; a++) {
        maturity_values_m.PushBack((maturity[a - 1] /= this->nyears), allocator);
        maturity_values_f.PushBack((maturity[a - 1] /= this->nyears), allocator);
    }

    rapidjson::Value maturity_males_entry(rapidjson::kObjectType);
    maturity_males_entry.AddMember("sex", "males", allocator);
    maturity_males_entry.AddMember("area", 1, allocator);
    maturity_males_entry.AddMember("values", maturity_values_m, allocator);
    maturity_array.PushBack(maturity_males_entry, allocator);

    rapidjson::Value maturity_females_entry(rapidjson::kObjectType);
    maturity_females_entry.AddMember("sex", "females", allocator);
    maturity_females_entry.AddMember("area", 1, allocator);
    maturity_females_entry.AddMember("values", maturity_values_f, allocator);
    maturity_array.PushBack(maturity_females_entry, allocator);
    population.AddMember("maturity", maturity_array, allocator);


    rapidjson::Value rec_params(rapidjson::kArrayType);
    rapidjson::Value rec_entry(rapidjson::kObjectType);
    rec_entry.AddMember("area", 1, allocator);
    rec_entry.AddMember("season", 1, allocator);
    rec_entry.AddMember("id", 1, allocator);
    rec_params.PushBack(rec_entry, allocator);
    pop_params.AddMember("recruitment", rec_params, allocator);


    rapidjson::Value growth_params(rapidjson::kObjectType);
    growth_params.AddMember("id", 1, allocator);
    pop_params.AddMember("growth", growth_params, allocator);


#warning add initial_deviations here


    population.AddMember("parameters", pop_params, allocator);
    document.AddMember("population", population, allocator);


    int nll_count = 1;
    for (int f = 1; f <= this->nfleets; f++) {
        rapidjson::Value nll_comp(rapidjson::kObjectType);
        nll_comp.AddMember("id", nll_count, allocator);
        nll_comp.AddMember("model", "multinomial_robust", allocator);
        document.AddMember("likelihood_component", nll_comp, allocator);
        nll_count++;

        rapidjson::Value nll_comp2(rapidjson::kObjectType);
        nll_comp2.AddMember("id", nll_count, allocator);
        nll_comp2.AddMember("model", "lognormal", allocator);
        document.AddMember("likelihood_component", nll_comp2, allocator);
        nll_count++;
    }

    //    for (int f = 1; f <= this->nfleets; f++) {
    //       
    //    }

    for (int f = 1; f <= this->nindices; f++) {
        rapidjson::Value nll_comp(rapidjson::kObjectType);
        nll_comp.AddMember("id", nll_count, allocator);
        nll_comp.AddMember("model", "multinomial_robust", allocator);
        document.AddMember("likelihood_component", nll_comp, allocator);
        nll_count++;

        rapidjson::Value nll_comp2(rapidjson::kObjectType);
        nll_comp2.AddMember("id", nll_count, allocator);
        nll_comp2.AddMember("model", "lognormal", allocator);
        document.AddMember("likelihood_component", nll_comp2, allocator);
        nll_count++;
    }




    nll_count = 1;
    selectivity_count = 1;
    for (int f = 1; f <= this->nfleets; f++) {
        rapidjson::Value fleet(rapidjson::kObjectType);
        rapidjson::Value fm(rapidjson::kArrayType);
        rapidjson::Value fm_entry(rapidjson::kObjectType);
        fm_entry.AddMember("id", f, allocator);
        fm_entry.AddMember("area", 1, allocator);
        fm_entry.AddMember("season", 1, allocator);
        fm.PushBack(fm_entry, allocator);

        rapidjson::Value selex(rapidjson::kArrayType);
        rapidjson::Value selex_entry(rapidjson::kObjectType);
        selex_entry.AddMember("id", selectivity_count++, allocator);
        selex_entry.AddMember("area", 1, allocator);
        selex_entry.AddMember("season", 1, allocator);
        selex.PushBack(selex_entry, allocator);

        rapidjson::Value nll_(rapidjson::kArrayType);
        rapidjson::Value nll__entry(rapidjson::kObjectType);

        rapidjson::Value age_comp_entry(rapidjson::kObjectType);
        age_comp_entry.AddMember("id", nll_count, allocator);
        age_comp_entry.AddMember("component", "age_comp", allocator);
        nll_.PushBack(age_comp_entry, allocator);
        nll_count++;
        rapidjson::Value biomass_comp_entry(rapidjson::kObjectType);
        biomass_comp_entry.AddMember("id", nll_count, allocator);
        biomass_comp_entry.AddMember("component", "biomass_comp", allocator);
        nll_.PushBack(biomass_comp_entry, allocator);
        nll_count++;
        fleet.AddMember("id", f, allocator);
        fleet.AddMember("fishing_mortality", fm, allocator);
        fleet.AddMember("selectivity", selex, allocator);
        fleet.AddMember("likelihood_components", nll_, allocator);
        document.AddMember("fleet", fleet, allocator);

    }


    for (int i = 1; i <= this->nindices; i++) {
        rapidjson::Value survey(rapidjson::kObjectType);
        rapidjson::Value fm(rapidjson::kArrayType);
        rapidjson::Value fm_entry(rapidjson::kObjectType);
        fm_entry.AddMember("id", i, allocator);
        fm_entry.AddMember("area", 1, allocator);
        fm_entry.AddMember("season", 1, allocator);
        fm.PushBack(fm_entry, allocator);
        rapidjson::Value parameters(rapidjson::kObjectType);

        rapidjson::Value q(rapidjson::kObjectType);
        q.AddMember("value", 1.0, allocator);
        q.AddMember("min", 0.0, allocator);
        q.AddMember("max", 10.0, allocator);
        q.AddMember("estimated", "true", allocator);
        q.AddMember("phase", 2, allocator);
        parameters.AddMember("q", q, allocator);
        survey.AddMember("parameters", parameters, allocator);
        /*
            "q": {
                "value": 1.096,
                "min": 0.0,
                "max": 10.0,
                "units": "something/tonnes",
                "estimated": "false",
                "phase": 2
            }
         */

        rapidjson::Value selex(rapidjson::kArrayType);
        rapidjson::Value selex_entry(rapidjson::kObjectType);
        selex_entry.AddMember("id", selectivity_count++, allocator);
        selex_entry.AddMember("area", 1, allocator);
        selex_entry.AddMember("season", 1, allocator);
        selex.PushBack(selex_entry, allocator);

        rapidjson::Value nll_(rapidjson::kArrayType);
        rapidjson::Value nll__entry(rapidjson::kObjectType);

        rapidjson::Value age_comp_entry(rapidjson::kObjectType);
        age_comp_entry.AddMember("id", nll_count, allocator);
        age_comp_entry.AddMember("component", "age_comp", allocator);
        nll_.PushBack(age_comp_entry, allocator);
        nll_count++;
        rapidjson::Value biomass_comp_entry(rapidjson::kObjectType);
        biomass_comp_entry.AddMember("id", nll_count, allocator);
        biomass_comp_entry.AddMember("component", "biomass_comp", allocator);
        nll_.PushBack(biomass_comp_entry, allocator);
        nll_count++;
        survey.AddMember("id", i, allocator);
        survey.AddMember("selectivity", selex, allocator);
        survey.AddMember("likelihood_components", nll_, allocator);
        document.AddMember("survey", survey, allocator);
    }

    //    for (int s = 1; s< this->n; f++) {
    //        rapidjson::Value nll_comp(rapidjson::kObjectType);
    //        nll_comp.AddMember("id", nll_count, allocator);
    //        nll_comp.AddMember("model", "multinomial_robust", allocator);
    //        document.AddMember("likelihood_component", nll_comp, allocator);
    //        nll_count++;
    //    }



    //
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    std::ofstream out;
    out.open(path);
    if (out.good()) {
        out << buffer.GetString();
    }
    out.close();
    std::cout << "\n\n\n" << sel_params << "\n";

}

model_parameters::model_parameters(int sz, int argc, char * argv[]) :
model_data(argc, argv), function_minimizer(sz) {
    initializationfunction();
    sel_params.allocate(1, nselparm, sel_lo, sel_hi, sel_phase, "sel_params");
    log_Fmult_year1.allocate(1, nfleets, -15., 2., phase_Fmult_year1, "log_Fmult_year1");
    log_Fmult_devs.allocate(1, nfleets, 2, nyears, -15., 15., phase_Fmult_devs, "log_Fmult_devs");
    log_recruit_devs.allocate(1, nyears, -15., 15., phase_recruit_devs, "log_recruit_devs");
    log_N_year1_devs.allocate(2, nages, -15., 15., phase_N_year1_devs, "log_N_year1_devs");
    log_q_year1.allocate(1, nindices, -30, 5, phase_q_year1, "log_q_year1");
    log_q_devs.allocate(1, nindices, 2, index_nobs, -15., 15., phase_q_devs, "log_q_devs");
    index_sel_params.allocate(1, nindexselparms, indexsel_lo, indexsel_hi, indexsel_phase, "index_sel_params");
    log_SR_scaler.allocate(-1.0, 200, phase_SR_scaler, "log_SR_scaler");
    SR_steepness.allocate(0.20001, 1.0, phase_steepness, "SR_steepness");
    sel_likely.allocate(1, nselparm, "sel_likely");
#ifndef NO_AD_INITIALIZE
    sel_likely.initialize();
#endif
    sel_stdresid.allocate(1, nselparm, "sel_stdresid");
#ifndef NO_AD_INITIALIZE
    sel_stdresid.initialize();
#endif
    sel_rmse.allocate("sel_rmse");
#ifndef NO_AD_INITIALIZE
    sel_rmse.initialize();
#endif
    sel_rmse_nobs.allocate("sel_rmse_nobs");
#ifndef NO_AD_INITIALIZE
    sel_rmse_nobs.initialize();
#endif
    sum_sel_lambda.allocate("sum_sel_lambda");
#ifndef NO_AD_INITIALIZE
    sum_sel_lambda.initialize();
#endif
    sum_sel_lambda_likely.allocate("sum_sel_lambda_likely");
#ifndef NO_AD_INITIALIZE
    sum_sel_lambda_likely.initialize();
#endif
    indexsel.allocate(1, nindices, 1, nages, "indexsel");
#ifndef NO_AD_INITIALIZE
    indexsel.initialize();
#endif
    indexsel_likely.allocate(1, nindexselparms, "indexsel_likely");
#ifndef NO_AD_INITIALIZE
    indexsel_likely.initialize();
#endif
    indexsel_stdresid.allocate(1, nindexselparms, "indexsel_stdresid");
#ifndef NO_AD_INITIALIZE
    indexsel_stdresid.initialize();
#endif
    indexsel_rmse.allocate("indexsel_rmse");
#ifndef NO_AD_INITIALIZE
    indexsel_rmse.initialize();
#endif
    indexsel_rmse_nobs.allocate("indexsel_rmse_nobs");
#ifndef NO_AD_INITIALIZE
    indexsel_rmse_nobs.initialize();
#endif
    sum_indexsel_lambda.allocate("sum_indexsel_lambda");
#ifndef NO_AD_INITIALIZE
    sum_indexsel_lambda.initialize();
#endif
    sum_indexsel_lambda_likely.allocate("sum_indexsel_lambda_likely");
#ifndef NO_AD_INITIALIZE
    sum_indexsel_lambda_likely.initialize();
#endif
    log_Fmult.allocate(1, nfleets, 1, nyears, "log_Fmult");
#ifndef NO_AD_INITIALIZE
    log_Fmult.initialize();
#endif
    Fmult.allocate(1, nfleets, 1, nyears, "Fmult");
#ifndef NO_AD_INITIALIZE
    Fmult.initialize();
#endif
    NAA.allocate(1, nyears, 1, nages, "NAA");
#ifndef NO_AD_INITIALIZE
    NAA.initialize();
#endif
    temp_NAA.allocate(1, nyears, 1, nages, "temp_NAA");
#ifndef NO_AD_INITIALIZE
    temp_NAA.initialize();
#endif
    temp_BAA.allocate(1, nyears, 1, nages, "temp_BAA");
#ifndef NO_AD_INITIALIZE
    temp_BAA.initialize();
#endif
    temp_PAA.allocate(1, nyears, 1, nages, "temp_PAA");
#ifndef NO_AD_INITIALIZE
    temp_PAA.initialize();
#endif
    FAA_tot.allocate(1, nyears, 1, nages, "FAA_tot");
#ifndef NO_AD_INITIALIZE
    FAA_tot.initialize();
#endif
    Z.allocate(1, nyears, 1, nages, "Z");
#ifndef NO_AD_INITIALIZE
    Z.initialize();
#endif
    S.allocate(1, nyears, 1, nages, "S");
#ifndef NO_AD_INITIALIZE
    S.initialize();
#endif
    Catch_stdresid.allocate(1, nfleets, 1, nyears, "Catch_stdresid");
#ifndef NO_AD_INITIALIZE
    Catch_stdresid.initialize();
#endif
    Discard_stdresid.allocate(1, nfleets, 1, nyears, "Discard_stdresid");
#ifndef NO_AD_INITIALIZE
    Discard_stdresid.initialize();
#endif
    Catch_tot_fleet_pred.allocate(1, nfleets, 1, nyears, "Catch_tot_fleet_pred");
#ifndef NO_AD_INITIALIZE
    Catch_tot_fleet_pred.initialize();
#endif
    Discard_tot_fleet_pred.allocate(1, nfleets, 1, nyears, "Discard_tot_fleet_pred");
#ifndef NO_AD_INITIALIZE
    Discard_tot_fleet_pred.initialize();
#endif
    CAA_pred.allocate(1, nfleets, 1, nyears, 1, nages, "CAA_pred");
#ifndef NO_AD_INITIALIZE
    CAA_pred.initialize();
#endif
    Discard_pred.allocate(1, nfleets, 1, nyears, 1, nages, "Discard_pred");
#ifndef NO_AD_INITIALIZE
    Discard_pred.initialize();
#endif
    CAA_prop_pred.allocate(1, nfleets, 1, nyears, sel_start_age, sel_end_age, "CAA_prop_pred");
#ifndef NO_AD_INITIALIZE
    CAA_prop_pred.initialize();
#endif
    Discard_prop_pred.allocate(1, nfleets, 1, nyears, sel_start_age, sel_end_age, "Discard_prop_pred");
#ifndef NO_AD_INITIALIZE
    Discard_prop_pred.initialize();
#endif
    FAA_by_fleet_dir.allocate(1, nfleets, 1, nyears, 1, nages, "FAA_by_fleet_dir");
#ifndef NO_AD_INITIALIZE
    FAA_by_fleet_dir.initialize();
#endif
    FAA_by_fleet_Discard.allocate(1, nfleets, 1, nyears, 1, nages, "FAA_by_fleet_Discard");
#ifndef NO_AD_INITIALIZE
    FAA_by_fleet_Discard.initialize();
#endif
    sel_by_block.allocate(1, nselblocks, 1, nages, "sel_by_block");
#ifndef NO_AD_INITIALIZE
    sel_by_block.initialize();
#endif
    sel_by_fleet.allocate(1, nfleets, 1, nyears, 1, nages, "sel_by_fleet");
#ifndef NO_AD_INITIALIZE
    sel_by_fleet.initialize();
#endif
    temp_sel_over_time.allocate(1, nyears, "temp_sel_over_time");
#ifndef NO_AD_INITIALIZE
    temp_sel_over_time.initialize();
#endif
    temp_sel_fix.allocate("temp_sel_fix");
#ifndef NO_AD_INITIALIZE
    temp_sel_fix.initialize();
#endif
    temp_Fmult_max.allocate("temp_Fmult_max");
#ifndef NO_AD_INITIALIZE
    temp_Fmult_max.initialize();
#endif
    Fmult_max_pen.allocate("Fmult_max_pen");
#ifndef NO_AD_INITIALIZE
    Fmult_max_pen.initialize();
#endif
    q_by_index.allocate(1, nindices, 1, index_nobs, "q_by_index");
#ifndef NO_AD_INITIALIZE
    q_by_index.initialize();
#endif
    temp_sel.allocate(1, nyears, 1, nages, "temp_sel");
#ifndef NO_AD_INITIALIZE
    temp_sel.initialize();
#endif
    temp_sel2.allocate(1, nages, "temp_sel2");
#ifndef NO_AD_INITIALIZE
    temp_sel2.initialize();
#endif
    index_pred.allocate(1, nindices, 1, index_nobs, "index_pred");
#ifndef NO_AD_INITIALIZE
    index_pred.initialize();
#endif
    output_index_prop_obs.allocate(1, nindices, 1, nyears, 1, nages, "output_index_prop_obs");
#ifndef NO_AD_INITIALIZE
    output_index_prop_obs.initialize();
#endif
    output_index_prop_pred.allocate(1, nindices, 1, nyears, 1, nages, "output_index_prop_pred");
#ifndef NO_AD_INITIALIZE
    output_index_prop_pred.initialize();
#endif
    index_Neff_init.allocate(1, nindices, 1, nyears, "index_Neff_init");
#ifndef NO_AD_INITIALIZE
    index_Neff_init.initialize();
#endif
    index_Neff_est.allocate(1, nindices, 1, nyears, "index_Neff_est");
#ifndef NO_AD_INITIALIZE
    index_Neff_est.initialize();
#endif
    index_prop_pred.allocate(1, nindices, 1, index_nobs, 1, nages, "index_prop_pred");
#ifndef NO_AD_INITIALIZE
    index_prop_pred.initialize();
#endif
    new_Neff_catch.allocate("new_Neff_catch");
#ifndef NO_AD_INITIALIZE
    new_Neff_catch.initialize();
#endif
    new_Neff_discard.allocate("new_Neff_discard");
#ifndef NO_AD_INITIALIZE
    new_Neff_discard.initialize();
#endif
    ntemp.allocate("ntemp");
#ifndef NO_AD_INITIALIZE
    ntemp.initialize();
#endif
    SR_S0.allocate("SR_S0");
#ifndef NO_AD_INITIALIZE
    SR_S0.initialize();
#endif
    SR_R0.allocate("SR_R0");
#ifndef NO_AD_INITIALIZE
    SR_R0.initialize();
#endif
    SR_alpha.allocate("SR_alpha");
#ifndef NO_AD_INITIALIZE
    SR_alpha.initialize();
#endif
    SR_beta.allocate("SR_beta");
#ifndef NO_AD_INITIALIZE
    SR_beta.initialize();
#endif
    S0_vec.allocate(1, nyears, "S0_vec");
#ifndef NO_AD_INITIALIZE
    S0_vec.initialize();
#endif
    R0_vec.allocate(1, nyears, "R0_vec");
#ifndef NO_AD_INITIALIZE
    R0_vec.initialize();
#endif
    steepness_vec.allocate(1, nyears, "steepness_vec");
#ifndef NO_AD_INITIALIZE
    steepness_vec.initialize();
#endif
    SR_pred_recruits.allocate(1, nyears + 1, "SR_pred_recruits");
#ifndef NO_AD_INITIALIZE
    SR_pred_recruits.initialize();
#endif
    likely_SR_sigma.allocate("likely_SR_sigma");
#ifndef NO_AD_INITIALIZE
    likely_SR_sigma.initialize();
#endif
    SR_stdresid.allocate(1, nyears, "SR_stdresid");
#ifndef NO_AD_INITIALIZE
    SR_stdresid.initialize();
#endif
    SR_rmse.allocate("SR_rmse");
#ifndef NO_AD_INITIALIZE
    SR_rmse.initialize();
#endif
    SR_rmse_nobs.allocate("SR_rmse_nobs");
#ifndef NO_AD_INITIALIZE
    SR_rmse_nobs.initialize();
#endif
    RSS_sel_devs.allocate(1, nfleets, "RSS_sel_devs");
#ifndef NO_AD_INITIALIZE
    RSS_sel_devs.initialize();
#endif
    RSS_catch_tot_fleet.allocate(1, nfleets, "RSS_catch_tot_fleet");
#ifndef NO_AD_INITIALIZE
    RSS_catch_tot_fleet.initialize();
#endif
    RSS_Discard_tot_fleet.allocate(1, nfleets, "RSS_Discard_tot_fleet");
#ifndef NO_AD_INITIALIZE
    RSS_Discard_tot_fleet.initialize();
#endif
    catch_tot_likely.allocate(1, nfleets, "catch_tot_likely");
#ifndef NO_AD_INITIALIZE
    catch_tot_likely.initialize();
#endif
    discard_tot_likely.allocate(1, nfleets, "discard_tot_likely");
#ifndef NO_AD_INITIALIZE
    discard_tot_likely.initialize();
#endif
    likely_catch.allocate("likely_catch");
#ifndef NO_AD_INITIALIZE
    likely_catch.initialize();
#endif
    likely_Discard.allocate("likely_Discard");
#ifndef NO_AD_INITIALIZE
    likely_Discard.initialize();
#endif
    RSS_ind.allocate(1, nindices, "RSS_ind");
#ifndef NO_AD_INITIALIZE
    RSS_ind.initialize();
#endif
    RSS_ind_sigma.allocate(1, nindices, "RSS_ind_sigma");
#ifndef NO_AD_INITIALIZE
    RSS_ind_sigma.initialize();
#endif
    likely_ind.allocate(1, nindices, "likely_ind");
#ifndef NO_AD_INITIALIZE
    likely_ind.initialize();
#endif
    index_stdresid.allocate(1, nindices, 1, index_nobs, "index_stdresid");
#ifndef NO_AD_INITIALIZE
    index_stdresid.initialize();
#endif
    likely_index_age_comp.allocate("likely_index_age_comp");
#ifndef NO_AD_INITIALIZE
    likely_index_age_comp.initialize();
#endif
    fpenalty.allocate("fpenalty");
#ifndef NO_AD_INITIALIZE
    fpenalty.initialize();
#endif
    fpenalty_lambda.allocate("fpenalty_lambda");
#ifndef NO_AD_INITIALIZE
    fpenalty_lambda.initialize();
#endif
    Fmult_year1_stdresid.allocate(1, nfleets, "Fmult_year1_stdresid");
#ifndef NO_AD_INITIALIZE
    Fmult_year1_stdresid.initialize();
#endif
    Fmult_year1_rmse.allocate("Fmult_year1_rmse");
#ifndef NO_AD_INITIALIZE
    Fmult_year1_rmse.initialize();
#endif
    Fmult_year1_rmse_nobs.allocate("Fmult_year1_rmse_nobs");
#ifndef NO_AD_INITIALIZE
    Fmult_year1_rmse_nobs.initialize();
#endif
    Fmult_year1_likely.allocate(1, nfleets, "Fmult_year1_likely");
#ifndef NO_AD_INITIALIZE
    Fmult_year1_likely.initialize();
#endif
    Fmult_devs_likely.allocate(1, nfleets, "Fmult_devs_likely");
#ifndef NO_AD_INITIALIZE
    Fmult_devs_likely.initialize();
#endif
    Fmult_devs_stdresid.allocate(1, nfleets, 1, nyears, "Fmult_devs_stdresid");
#ifndef NO_AD_INITIALIZE
    Fmult_devs_stdresid.initialize();
#endif
    Fmult_devs_fleet_rmse.allocate(1, nfleets, "Fmult_devs_fleet_rmse");
#ifndef NO_AD_INITIALIZE
    Fmult_devs_fleet_rmse.initialize();
#endif
    Fmult_devs_fleet_rmse_nobs.allocate(1, nfleets, "Fmult_devs_fleet_rmse_nobs");
#ifndef NO_AD_INITIALIZE
    Fmult_devs_fleet_rmse_nobs.initialize();
#endif
    Fmult_devs_rmse.allocate("Fmult_devs_rmse");
#ifndef NO_AD_INITIALIZE
    Fmult_devs_rmse.initialize();
#endif
    Fmult_devs_rmse_nobs.allocate("Fmult_devs_rmse_nobs");
#ifndef NO_AD_INITIALIZE
    Fmult_devs_rmse_nobs.initialize();
#endif
    N_year1_likely.allocate("N_year1_likely");
#ifndef NO_AD_INITIALIZE
    N_year1_likely.initialize();
#endif
    N_year1_stdresid.allocate(2, nages, "N_year1_stdresid");
#ifndef NO_AD_INITIALIZE
    N_year1_stdresid.initialize();
#endif
    N_year1_rmse.allocate("N_year1_rmse");
#ifndef NO_AD_INITIALIZE
    N_year1_rmse.initialize();
#endif
    N_year1_rmse_nobs.allocate("N_year1_rmse_nobs");
#ifndef NO_AD_INITIALIZE
    N_year1_rmse_nobs.initialize();
#endif
    nyear1temp.allocate(1, nages, "nyear1temp");
#ifndef NO_AD_INITIALIZE
    nyear1temp.initialize();
#endif
    q_year1_likely.allocate(1, nindices, "q_year1_likely");
#ifndef NO_AD_INITIALIZE
    q_year1_likely.initialize();
#endif
    q_year1_stdresid.allocate(1, nindices, "q_year1_stdresid");
#ifndef NO_AD_INITIALIZE
    q_year1_stdresid.initialize();
#endif
    q_year1_rmse.allocate("q_year1_rmse");
#ifndef NO_AD_INITIALIZE
    q_year1_rmse.initialize();
#endif
    q_year1_rmse_nobs.allocate("q_year1_rmse_nobs");
#ifndef NO_AD_INITIALIZE
    q_year1_rmse_nobs.initialize();
#endif
    q_devs_likely.allocate(1, nindices, "q_devs_likely");
#ifndef NO_AD_INITIALIZE
    q_devs_likely.initialize();
#endif
    q_devs_stdresid.allocate(1, nindices, 1, index_nobs, "q_devs_stdresid");
#ifndef NO_AD_INITIALIZE
    q_devs_stdresid.initialize();
#endif
    q_devs_rmse.allocate("q_devs_rmse");
#ifndef NO_AD_INITIALIZE
    q_devs_rmse.initialize();
#endif
    q_devs_rmse_nobs.allocate("q_devs_rmse_nobs");
#ifndef NO_AD_INITIALIZE
    q_devs_rmse_nobs.initialize();
#endif
    steepness_likely.allocate("steepness_likely");
#ifndef NO_AD_INITIALIZE
    steepness_likely.initialize();
#endif
    steepness_stdresid.allocate("steepness_stdresid");
#ifndef NO_AD_INITIALIZE
    steepness_stdresid.initialize();
#endif
    steepness_rmse.allocate("steepness_rmse");
#ifndef NO_AD_INITIALIZE
    steepness_rmse.initialize();
#endif
    steepness_rmse_nobs.allocate("steepness_rmse_nobs");
#ifndef NO_AD_INITIALIZE
    steepness_rmse_nobs.initialize();
#endif
    SR_scaler_likely.allocate("SR_scaler_likely");
#ifndef NO_AD_INITIALIZE
    SR_scaler_likely.initialize();
#endif
    SR_scaler_stdresid.allocate("SR_scaler_stdresid");
#ifndef NO_AD_INITIALIZE
    SR_scaler_stdresid.initialize();
#endif
    SR_scaler_rmse.allocate("SR_scaler_rmse");
#ifndef NO_AD_INITIALIZE
    SR_scaler_rmse.initialize();
#endif
    SR_scaler_rmse_nobs.allocate("SR_scaler_rmse_nobs");
#ifndef NO_AD_INITIALIZE
    SR_scaler_rmse_nobs.initialize();
#endif
    effective_sample_size.allocate(1, nfleets, 1, nyears, "effective_sample_size");
#ifndef NO_AD_INITIALIZE
    effective_sample_size.initialize();
#endif
    effective_Discard_sample_size.allocate(1, nfleets, 1, nyears, "effective_Discard_sample_size");
#ifndef NO_AD_INITIALIZE
    effective_Discard_sample_size.initialize();
#endif
    Neff_stage2_mult_catch.allocate(1, nfleets, "Neff_stage2_mult_catch");
#ifndef NO_AD_INITIALIZE
    Neff_stage2_mult_catch.initialize();
#endif
    Neff_stage2_mult_discard.allocate(1, nfleets, "Neff_stage2_mult_discard");
#ifndef NO_AD_INITIALIZE
    Neff_stage2_mult_discard.initialize();
#endif
    Neff_stage2_mult_index.allocate(1, nindices, "Neff_stage2_mult_index");
#ifndef NO_AD_INITIALIZE
    Neff_stage2_mult_index.initialize();
#endif
    mean_age_obs.allocate(1, nyears, "mean_age_obs");
#ifndef NO_AD_INITIALIZE
    mean_age_obs.initialize();
#endif
    mean_age_pred.allocate(1, nyears, "mean_age_pred");
#ifndef NO_AD_INITIALIZE
    mean_age_pred.initialize();
#endif
    mean_age_pred2.allocate(1, nyears, "mean_age_pred2");
#ifndef NO_AD_INITIALIZE
    mean_age_pred2.initialize();
#endif
    mean_age_resid.allocate(1, nyears, "mean_age_resid");
#ifndef NO_AD_INITIALIZE
    mean_age_resid.initialize();
#endif
    mean_age_sigma.allocate(1, nyears, "mean_age_sigma");
#ifndef NO_AD_INITIALIZE
    mean_age_sigma.initialize();
#endif
    mean_age_x.allocate("mean_age_x");
#ifndef NO_AD_INITIALIZE
    mean_age_x.initialize();
#endif
    mean_age_n.allocate("mean_age_n");
#ifndef NO_AD_INITIALIZE
    mean_age_n.initialize();
#endif
    mean_age_delta.allocate("mean_age_delta");
#ifndef NO_AD_INITIALIZE
    mean_age_delta.initialize();
#endif
    mean_age_mean.allocate("mean_age_mean");
#ifndef NO_AD_INITIALIZE
    mean_age_mean.initialize();
#endif
    mean_age_m2.allocate("mean_age_m2");
#ifndef NO_AD_INITIALIZE
    mean_age_m2.initialize();
#endif
    temp_Fmult.allocate(1, nfleets, "temp_Fmult");
#ifndef NO_AD_INITIALIZE
    temp_Fmult.initialize();
#endif
    tempU.allocate("tempU");
#ifndef NO_AD_INITIALIZE
    tempU.initialize();
#endif
    tempN.allocate("tempN");
#ifndef NO_AD_INITIALIZE
    tempN.initialize();
#endif
    tempB.allocate("tempB");
#ifndef NO_AD_INITIALIZE
    tempB.initialize();
#endif
    tempUd.allocate("tempUd");
#ifndef NO_AD_INITIALIZE
    tempUd.initialize();
#endif
    tempNd.allocate("tempNd");
#ifndef NO_AD_INITIALIZE
    tempNd.initialize();
#endif
    tempBd.allocate("tempBd");
#ifndef NO_AD_INITIALIZE
    tempBd.initialize();
#endif
    trefU.allocate("trefU");
#ifndef NO_AD_INITIALIZE
    trefU.initialize();
#endif
    trefN.allocate("trefN");
#ifndef NO_AD_INITIALIZE
    trefN.initialize();
#endif
    trefB.allocate("trefB");
#ifndef NO_AD_INITIALIZE
    trefB.initialize();
#endif
    trefUd.allocate("trefUd");
#ifndef NO_AD_INITIALIZE
    trefUd.initialize();
#endif
    trefNd.allocate("trefNd");
#ifndef NO_AD_INITIALIZE
    trefNd.initialize();
#endif
    trefBd.allocate("trefBd");
#ifndef NO_AD_INITIALIZE
    trefBd.initialize();
#endif
    Fref_report.allocate("Fref_report");
#ifndef NO_AD_INITIALIZE
    Fref_report.initialize();
#endif
    Fref.allocate("Fref");
#ifndef NO_AD_INITIALIZE
    Fref.initialize();
#endif
    freftemp.allocate(1, nages, "freftemp");
#ifndef NO_AD_INITIALIZE
    freftemp.initialize();
#endif
    nreftemp.allocate(1, nages, "nreftemp");
#ifndef NO_AD_INITIALIZE
    nreftemp.initialize();
#endif
    Freport_U.allocate(1, nyears, "Freport_U");
#ifndef NO_AD_INITIALIZE
    Freport_U.initialize();
#endif
    Freport_N.allocate(1, nyears, "Freport_N");
#ifndef NO_AD_INITIALIZE
    Freport_N.initialize();
#endif
    Freport_B.allocate(1, nyears, "Freport_B");
#ifndef NO_AD_INITIALIZE
    Freport_B.initialize();
#endif
    Freport.allocate(1, nyears, "Freport");
    TotJan1B.allocate(1, nyears, "TotJan1B");
    SSB.allocate(1, nyears, "SSB");
    ExploitableB.allocate(1, nyears, "ExploitableB");
    recruits.allocate(1, nyears, "recruits");
    SSBfracZ.allocate(1, nyears, 1, nages, "SSBfracZ");
#ifndef NO_AD_INITIALIZE
    SSBfracZ.initialize();
#endif
    final_year_total_sel.allocate(1, nages, "final_year_total_sel");
#ifndef NO_AD_INITIALIZE
    final_year_total_sel.initialize();
#endif
    dir_F.allocate(1, nages, "dir_F");
#ifndef NO_AD_INITIALIZE
    dir_F.initialize();
#endif
    Discard_F.allocate(1, nages, "Discard_F");
#ifndef NO_AD_INITIALIZE
    Discard_F.initialize();
#endif
    proj_nondir_F.allocate(1, nages, "proj_nondir_F");
#ifndef NO_AD_INITIALIZE
    proj_nondir_F.initialize();
#endif
    proj_dir_sel.allocate(1, nages, "proj_dir_sel");
#ifndef NO_AD_INITIALIZE
    proj_dir_sel.initialize();
#endif
    proj_Discard_sel.allocate(1, nages, "proj_Discard_sel");
#ifndef NO_AD_INITIALIZE
    proj_Discard_sel.initialize();
#endif
    proj_NAA.allocate(1, nprojyears, 1, nages, "proj_NAA");
#ifndef NO_AD_INITIALIZE
    proj_NAA.initialize();
#endif
    proj_Fmult.allocate(1, nprojyears, "proj_Fmult");
#ifndef NO_AD_INITIALIZE
    proj_Fmult.initialize();
#endif
    Ftemp.allocate(1, nages, "Ftemp");
#ifndef NO_AD_INITIALIZE
    Ftemp.initialize();
#endif
    Ztemp.allocate(1, nages, "Ztemp");
#ifndef NO_AD_INITIALIZE
    Ztemp.initialize();
#endif
    proj_TotJan1B.allocate(1, nprojyears, "proj_TotJan1B");
#ifndef NO_AD_INITIALIZE
    proj_TotJan1B.initialize();
#endif
    proj_SSB.allocate(1, nprojyears, "proj_SSB");
#ifndef NO_AD_INITIALIZE
    proj_SSB.initialize();
#endif
    SSBtemp.allocate("SSBtemp");
#ifndef NO_AD_INITIALIZE
    SSBtemp.initialize();
#endif
    denom.allocate("denom");
#ifndef NO_AD_INITIALIZE
    denom.initialize();
#endif
    proj_F_dir.allocate(1, nprojyears, 1, nages, "proj_F_dir");
#ifndef NO_AD_INITIALIZE
    proj_F_dir.initialize();
#endif
    proj_F_Discard.allocate(1, nprojyears, 1, nages, "proj_F_Discard");
#ifndef NO_AD_INITIALIZE
    proj_F_Discard.initialize();
#endif
    proj_F_nondir.allocate(1, nprojyears, 1, nages, "proj_F_nondir");
#ifndef NO_AD_INITIALIZE
    proj_F_nondir.initialize();
#endif
    proj_Z.allocate(1, nprojyears, 1, nages, "proj_Z");
#ifndef NO_AD_INITIALIZE
    proj_Z.initialize();
#endif
    proj_SSBfracZ.allocate(1, nprojyears, 1, nages, "proj_SSBfracZ");
#ifndef NO_AD_INITIALIZE
    proj_SSBfracZ.initialize();
#endif
    proj_catch.allocate(1, nprojyears, 1, nages, "proj_catch");
#ifndef NO_AD_INITIALIZE
    proj_catch.initialize();
#endif
    proj_Discard.allocate(1, nprojyears, 1, nages, "proj_Discard");
#ifndef NO_AD_INITIALIZE
    proj_Discard.initialize();
#endif
    proj_yield.allocate(1, nprojyears, 1, nages, "proj_yield");
#ifndef NO_AD_INITIALIZE
    proj_yield.initialize();
#endif
    proj_total_yield.allocate(1, nprojyears, "proj_total_yield");
#ifndef NO_AD_INITIALIZE
    proj_total_yield.initialize();
#endif
    proj_total_Discard.allocate(1, nprojyears, "proj_total_Discard");
#ifndef NO_AD_INITIALIZE
    proj_total_Discard.initialize();
#endif
    output_prop_obs.allocate(1, nages, "output_prop_obs");
#ifndef NO_AD_INITIALIZE
    output_prop_obs.initialize();
#endif
    output_prop_pred.allocate(1, nages, "output_prop_pred");
#ifndef NO_AD_INITIALIZE
    output_prop_pred.initialize();
#endif
    output_Discard_prop_obs.allocate(1, nages, "output_Discard_prop_obs");
#ifndef NO_AD_INITIALIZE
    output_Discard_prop_obs.initialize();
#endif
    output_Discard_prop_pred.allocate(1, nages, "output_Discard_prop_pred");
#ifndef NO_AD_INITIALIZE
    output_Discard_prop_pred.initialize();
#endif
    NAAbsn.allocate(1, nages, "NAAbsn");
#ifndef NO_AD_INITIALIZE
    NAAbsn.initialize();
#endif
    temp_sum.allocate("temp_sum");
#ifndef NO_AD_INITIALIZE
    temp_sum.initialize();
#endif
    temp_sum2.allocate("temp_sum2");
#ifndef NO_AD_INITIALIZE
    temp_sum2.initialize();
#endif
    A.allocate("A");
#ifndef NO_AD_INITIALIZE
    A.initialize();
#endif
    B.allocate("B");
#ifndef NO_AD_INITIALIZE
    B.initialize();
#endif
    C.allocate("C");
#ifndef NO_AD_INITIALIZE
    C.initialize();
#endif
    f.allocate("f");
#ifndef NO_AD_INITIALIZE
    f.initialize();
#endif
    z.allocate("z");
#ifndef NO_AD_INITIALIZE
    z.initialize();
#endif
    SPR_Fmult.allocate("SPR_Fmult");
#ifndef NO_AD_INITIALIZE
    SPR_Fmult.initialize();
#endif
    YPR_Fmult.allocate("YPR_Fmult");
#ifndef NO_AD_INITIALIZE
    YPR_Fmult.initialize();
#endif
    SPR.allocate("SPR");
#ifndef NO_AD_INITIALIZE
    SPR.initialize();
#endif
    SPRatio.allocate("SPRatio");
#ifndef NO_AD_INITIALIZE
    SPRatio.initialize();
#endif
    YPR.allocate("YPR");
#ifndef NO_AD_INITIALIZE
    YPR.initialize();
#endif
    S_F.allocate("S_F");
#ifndef NO_AD_INITIALIZE
    S_F.initialize();
#endif
    R_F.allocate("R_F");
#ifndef NO_AD_INITIALIZE
    R_F.initialize();
#endif
    slope_origin.allocate("slope_origin");
#ifndef NO_AD_INITIALIZE
    slope_origin.initialize();
#endif
    slope.allocate("slope");
#ifndef NO_AD_INITIALIZE
    slope.initialize();
#endif
    F30SPR.allocate("F30SPR");
#ifndef NO_AD_INITIALIZE
    F30SPR.initialize();
#endif
    F40SPR.allocate("F40SPR");
#ifndef NO_AD_INITIALIZE
    F40SPR.initialize();
#endif
    Fmsy.allocate("Fmsy");
#ifndef NO_AD_INITIALIZE
    Fmsy.initialize();
#endif
    F01.allocate("F01");
#ifndef NO_AD_INITIALIZE
    F01.initialize();
#endif
    Fmax.allocate("Fmax");
#ifndef NO_AD_INITIALIZE
    Fmax.initialize();
#endif
    F30SPR_report.allocate("F30SPR_report");
#ifndef NO_AD_INITIALIZE
    F30SPR_report.initialize();
#endif
    F40SPR_report.allocate("F40SPR_report");
#ifndef NO_AD_INITIALIZE
    F40SPR_report.initialize();
#endif
    F01_report.allocate("F01_report");
#ifndef NO_AD_INITIALIZE
    F01_report.initialize();
#endif
    Fmax_report.allocate("Fmax_report");
#ifndef NO_AD_INITIALIZE
    Fmax_report.initialize();
#endif
    Fcurrent.allocate("Fcurrent");
#ifndef NO_AD_INITIALIZE
    Fcurrent.initialize();
#endif
    F30SPR_slope.allocate("F30SPR_slope");
#ifndef NO_AD_INITIALIZE
    F30SPR_slope.initialize();
#endif
    F40SPR_slope.allocate("F40SPR_slope");
#ifndef NO_AD_INITIALIZE
    F40SPR_slope.initialize();
#endif
    Fmsy_slope.allocate("Fmsy_slope");
#ifndef NO_AD_INITIALIZE
    Fmsy_slope.initialize();
#endif
    F01_slope.allocate("F01_slope");
#ifndef NO_AD_INITIALIZE
    F01_slope.initialize();
#endif
    Fmax_slope.allocate("Fmax_slope");
#ifndef NO_AD_INITIALIZE
    Fmax_slope.initialize();
#endif
    Fcurrent_slope.allocate("Fcurrent_slope");
#ifndef NO_AD_INITIALIZE
    Fcurrent_slope.initialize();
#endif
    SSmsy.allocate("SSmsy");
#ifndef NO_AD_INITIALIZE
    SSmsy.initialize();
#endif
    tempR.allocate("tempR");
#ifndef NO_AD_INITIALIZE
    tempR.initialize();
#endif
    tempFmult.allocate(1, nyears, "tempFmult");
#ifndef NO_AD_INITIALIZE
    tempFmult.initialize();
#endif
    MSY.allocate("MSY");
    SSBmsy_report.allocate("SSBmsy_report");
    Fmsy_report.allocate("Fmsy_report");
    SSBmsy_ratio.allocate("SSBmsy_ratio");
    Fmsy_ratio.allocate("Fmsy_ratio");
    obj_fun.allocate("obj_fun");
    prior_function_value.allocate("prior_function_value");
    likelihood_function_value.allocate("likelihood_function_value");
}

void model_parameters::preliminary_calculations(void) {

#if defined(USE_ADPVM)

    admaster_slave_variable_interface(*this);

#endif
    ind = 0;
    for (indavail = 1; indavail <= navailindices; indavail++) {
        if (use_index(indavail) == 1) {
            ind += 1;
            lambda_ind(ind) = lambda_ind_ini(indavail);
            lambda_q_year1(ind) = lambda_q_year1_ini(indavail);
            q_year1_CV(ind) = q_year1_CV_ini(indavail);
            lambda_q_devs(ind) = lambda_q_devs_ini(indavail);
            q_devs_CV(ind) = q_devs_CV_ini(indavail);
            q_year1_ini(ind) = q_year1_iniavail(indavail);
        }
    }

    if (ignore_guesses == 0) {
        NAA(1) = NAA_year1_ini;
        log_Fmult_year1 = log(Fmult_year1_ini);
        log_q_year1 = log(q_year1_ini);
        log_SR_scaler = log(SR_scaler_ini);
        SR_steepness = SR_steepness_ini;
        for (k = 1; k <= nselparm; k++) {
            sel_params(k) = sel_initial(k);
        }
        for (k = 1; k <= nindexselparms; k++) {
            index_sel_params(k) = indexsel_initial(k);
        }
    }

    delta = 0.00001;
    Fmult_year1_sigma2 = log(elem_prod(Fmult_year1_CV, Fmult_year1_CV) + 1.0);
    Fmult_year1_sigma = sqrt(Fmult_year1_sigma2);
    Fmult_devs_sigma2 = log(elem_prod(Fmult_devs_CV, Fmult_devs_CV) + 1.0);
    Fmult_devs_sigma = sqrt(Fmult_devs_sigma2);
    N_year1_sigma2 = log(N_year1_CV * N_year1_CV + 1.0);
    N_year1_sigma = sqrt(N_year1_sigma2);
    q_year1_sigma2 = log(elem_prod(q_year1_CV, q_year1_CV) + 1.0);
    q_year1_sigma = sqrt(q_year1_sigma2);
    q_devs_sigma2 = log(elem_prod(q_devs_CV, q_devs_CV) + 1.0);
    q_devs_sigma = sqrt(q_devs_sigma2);
    steepness_sigma2 = log(steepness_CV * steepness_CV + 1.0);
    steepness_sigma = sqrt(steepness_sigma2);
    SR_scaler_sigma2 = log(SR_scaler_CV * SR_scaler_CV + 1.0);
    SR_scaler_sigma = sqrt(SR_scaler_sigma2);

    catch_prop_like_const = 0.0;
    discard_prop_like_const = 0.0;
    if (use_likelihood_constants == 1) {
        for (ifleet = 1; ifleet <= nfleets; ifleet++) {
            for (iyear = 1; iyear <= nyears; iyear++) {
                if (input_eff_samp_size_catch(ifleet, iyear) > 0) {
                    nfact_in = input_eff_samp_size_catch(ifleet, iyear);
                    get_log_factorial();
                    catch_prop_like_const += -1.0 * nfact_out; // negative for the total
                    for (iage = sel_start_age(ifleet); iage <= sel_end_age(ifleet); iage++) {
                        nfact_in = double(input_eff_samp_size_catch(ifleet, iyear)) * CAA_prop_obs(ifleet, iyear, iage) + 0.5; // +0.5 to round instead of truncate nfact_in
                        get_log_factorial();
                        catch_prop_like_const += nfact_out; // positive for the parts
                    }
                }
                if (input_eff_samp_size_discard(ifleet, iyear) > 0) {
                    nfact_in = input_eff_samp_size_discard(ifleet, iyear);
                    get_log_factorial();
                    discard_prop_like_const += -1.0 * nfact_out; // negative for the total
                    for (iage = sel_start_age(ifleet); iage <= sel_end_age(ifleet); iage++) {
                        nfact_in = double(input_eff_samp_size_discard(ifleet, iyear)) * Discard_prop_obs(ifleet, iyear, iage) + 0.5;
                        get_log_factorial();
                        discard_prop_like_const += nfact_out; // positive for the parts
                    }
                }
            }
        }
    }
    index_prop_like_const = 0.0;
    if (use_likelihood_constants == 1) {
        for (ind = 1; ind <= nindices; ind++) {
            if (index_estimate_proportions(ind) == 1) {
                for (i = 1; i <= index_nobs(ind); i++) {
                    if (input_eff_samp_size_index(ind, i) > 0) {
                        nfact_in = input_eff_samp_size_index(ind, i);
                        get_log_factorial();
                        index_prop_like_const += -1.0 * nfact_out; // negative for total
                        for (iage = index_start_age(ind); iage <= index_end_age(ind); iage++) {
                            nfact_in = double(input_eff_samp_size_index(ind, i)) * index_prop_obs(ind, i, iage) + 0.5;
                            get_log_factorial();
                            index_prop_like_const += nfact_out; // positive for the parts
                        }
                    }
                }
            }
        }
    }
    sel_like_const = 0.0;
    if (use_likelihood_constants == 1) {
        for (k = 1; k <= nselparm; k++) {
            if (sel_phase(k) >= 1) {
                sel_like_const(k) = 0.5 * log(2.0 * pi) + log(sel_initial(k));
            }
        }
    }

    indexsel_like_const = 0.0;
    if (use_likelihood_constants == 1) {
        for (k = 1; k <= nindexselparms; k++) {
            if (indexsel_phase(k) >= 1) {
                indexsel_like_const(k) = 0.5 * log(2.0 * pi) + log(indexsel_initial(k));
            }
        }
    }
    if (use_likelihood_constants == 1) {
        Fmult_year1_like_const = 0.5 * log(2.0 * pi) + log(Fmult_year1_ini);
        Fmult_devs_like_const = 0.5 * log(2.0 * pi);
        N_year1_like_const = 0.5 * log(2.0 * pi);
        q_year1_like_const = 0.5 * log(2.0 * pi) + log(q_year1_ini);
        q_devs_like_const = 0.5 * log(2.0 * pi);
        steepness_like_const = 0.5 * log(2.0 * pi) + log(SR_steepness_ini);
        SR_scaler_like_const = 0.5 * log(2.0 * pi) + log(SR_scaler_ini);
    } else {
        Fmult_year1_like_const = 0.0;
        Fmult_devs_like_const = 0.0;
        N_year1_like_const = 0.0;
        q_year1_like_const = 0.0;
        q_devs_like_const = 0.0;
        steepness_like_const = 0.0;
        SR_scaler_like_const = 0.0;
    }

    log_Fmult_devs.initialize();
    log_recruit_devs.initialize();
    log_N_year1_devs.initialize();
    log_q_devs.initialize();

    MSY.initialize();
    SSBmsy_report.initialize();
    Fmsy_report.initialize();
    SSBmsy_ratio.initialize();
    Fmsy_ratio.initialize();

    debug = 0; // debug checks commented out to speed calculations
}

void model_parameters::userfunction(void) {
    obj_fun = 0.0;
    //  if (debug==1) cout << "starting procedure section" << endl;
    get_SR(); //  if (debug==1) cout << "got SR" << endl;
    get_selectivity(); //  if (debug==1) cout << "got selectivity" << endl;
    get_mortality_rates(); //  if (debug==1) cout << "got mortality rates" << endl;
    get_numbers_at_age(); //  if (debug==1) cout << "got numbers at age" << endl;
    get_Freport(); //  if (debug==1) cout << "got Freport" << endl;
    get_predicted_catch(); //  if (debug==1) cout << "got predicted catch" << endl;
    get_q(); //  if (debug==1) cout << "got q" << endl;
    get_predicted_indices(); //  if (debug==1) cout << "got predicted indices" << endl;
    compute_the_objective_function(); //  if (debug==1) cout << "computed objective function" << endl;
    if (last_phase() || mceval_phase()) {
        get_proj_sel(); //  if (debug==1) cout <<"got proj sel" << endl;
        get_Fref(); //  if (debug==1) cout <<"got Fref" << endl;
        get_multinomial_multiplier(); //  if (debug==1) cout <<"got multinomial multiplier" << endl;
    }
    if (mceval_phase()) {
        write_MCMC();
    } //  if (debug==1) cout << "  . . . end of procedure section" << endl;
    //    write_MCMC();
}

void model_parameters::get_SR(void) {
    if (is_SR_scaler_R == 1) {
        SR_R0 = mfexp(log_SR_scaler);
        SR_S0 = SR_spawners_per_recruit*SR_R0;
    } else {
        SR_S0 = mfexp(log_SR_scaler);
        SR_R0 = SR_S0 / SR_spawners_per_recruit;
    }
    SR_alpha = 4.0 * SR_steepness * SR_R0 / (5.0 * SR_steepness - 1.0);
    SR_beta = SR_S0 * (1.0 - SR_steepness) / (5.0 * SR_steepness - 1.0);
    // now compute year specific vectors of R0, S0, and steepness
    for (iyear = 1; iyear <= nyears; iyear++) {
        steepness_vec(iyear) = 0.2 * SR_alpha * s_per_r_vec(iyear) / (0.8 * SR_beta + 0.2 * SR_alpha * s_per_r_vec(iyear));
        R0_vec(iyear) = (SR_alpha * s_per_r_vec(iyear) - SR_beta) / s_per_r_vec(iyear);
        S0_vec(iyear) = s_per_r_vec(iyear) * R0_vec(iyear);
    }
}

void model_parameters::get_selectivity(void) {
    dvariable sel_alpha1;
    dvariable sel_beta1;
    dvariable sel_alpha2;
    dvariable sel_beta2;
    dvariable sel_temp;
    dvariable sel1;
    dvariable sel2;
    k = 0;
    for (i = 1; i <= nselblocks; i++) {
        if (sel_option(i) == 1) {
            for (iage = 1; iage <= nages; iage++) {
                k += 1;
                sel_by_block(i, iage) = sel_params(k);
            }
        }
        if (sel_option(i) == 2) {
            sel_alpha1 = sel_params(k + 1);
            sel_beta1 = sel_params(k + 2);
            k += 2;
            for (iage = 1; iage <= nages; iage++) {
                sel_by_block(i, iage) = 1.0 / (1.0 + mfexp((sel_alpha1 - double(iage)) / sel_beta1));
            }
            sel_temp = max(sel_by_block(i));
            sel_by_block(i) /= sel_temp;
        }
        if (sel_option(i) == 3) {
            sel_alpha1 = sel_params(k + 1);
            sel_beta1 = sel_params(k + 2);
            sel_alpha2 = sel_params(k + 3);
            sel_beta2 = sel_params(k + 4);
            k += 4;
            for (iage = 1; iage <= nages; iage++) {
                sel1 = 1.0 / (1.0 + mfexp((sel_alpha1 - double(iage)) / sel_beta1));
                sel2 = 1.0 - 1.0 / (1.0 + mfexp((sel_alpha2 - double(iage)) / sel_beta2));
                sel_by_block(i, iage) = sel1*sel2;
            }
            sel_temp = max(sel_by_block(i));
            sel_by_block(i) /= sel_temp;
        }
    }
    for (ifleet = 1; ifleet <= nfleets; ifleet++) {
        for (iyear = 1; iyear <= nyears; iyear++) {
            sel_by_fleet(ifleet, iyear) = sel_by_block(sel_blocks(ifleet, iyear));
        }
    }
}

void model_parameters::get_mortality_rates(void) {
    for (ifleet = 1; ifleet <= nfleets; ifleet++) {
        log_Fmult(ifleet, 1) = log_Fmult_year1(ifleet);
        if (active(log_Fmult_devs)) {
            for (iyear = 2; iyear <= nyears; iyear++)
                log_Fmult(ifleet, iyear) = log_Fmult(ifleet, iyear - 1) + log_Fmult_devs(ifleet, iyear);
        } else {
            for (iyear = 2; iyear <= nyears; iyear++)
                log_Fmult(ifleet, iyear) = log_Fmult_year1(ifleet);
        }
    }
    FAA_tot = 0.0;
    for (ifleet = 1; ifleet <= nfleets; ifleet++) {
        for (iyear = 1; iyear <= nyears; iyear++) {
            for (iage = 1; iage <= nages; iage++) {
                FAA_by_fleet_dir(ifleet, iyear, iage) = (mfexp(log_Fmult(ifleet, iyear)) * sel_by_fleet(ifleet, iyear, iage))*(1.0 - proportion_release(ifleet, iyear, iage));
                FAA_by_fleet_Discard(ifleet, iyear, iage) = (mfexp(log_Fmult(ifleet, iyear)) * sel_by_fleet(ifleet, iyear, iage))*(proportion_release(ifleet, iyear, iage) * release_mort(ifleet));
            }
        }
        FAA_tot += FAA_by_fleet_dir(ifleet) + FAA_by_fleet_Discard(ifleet);
    }
    for (iyear = 1; iyear <= nyears; iyear++)
        Z(iyear) = FAA_tot(iyear) + M(iyear);
    S = mfexp(-1.0 * Z);
    SSBfracZ = mfexp(-1.0 * fracyearSSB * Z); // for use in SSB calcuations
}

void model_parameters::get_numbers_at_age(void) {
    if (phase_N_year1_devs > 0) {
        for (iage = 2; iage <= nages; iage++) {
            NAA(1, iage) = NAA_year1_ini(iage) * mfexp(log_N_year1_devs(iage));
        }
    }
    SSB(1) = 0.0;
    for (iage = 2; iage <= nages; iage++) {
        SSB(1) += NAA(1, iage) * SSBfracZ(1, iage) * fecundity(1, iage); // note SSB in year 1 does not include age 1 to estimate pred_R in year 1
    }
    SR_pred_recruits(1) = SR_alpha * SSB(1) / (SR_beta + SSB(1));
    NAA(1, 1) = SR_pred_recruits(1) * mfexp(log_recruit_devs(1));
    SSB(1) += NAA(1, 1) * SSBfracZ(1, 1) * fecundity(1, 1); // now SSB in year 1 is complete and can be used for pred_R in year 2
    for (iyear = 2; iyear <= nyears; iyear++) {
        SR_pred_recruits(iyear) = SR_alpha * SSB(iyear - 1) / (SR_beta + SSB(iyear - 1));
        NAA(iyear, 1) = SR_pred_recruits(iyear) * mfexp(log_recruit_devs(iyear));
        for (iage = 2; iage <= nages; iage++)
            NAA(iyear, iage) = NAA(iyear - 1, iage - 1) * S(iyear - 1, iage - 1);
        NAA(iyear, nages) += NAA(iyear - 1, nages) * S(iyear - 1, nages);
        SSB(iyear) = elem_prod(NAA(iyear), SSBfracZ(iyear)) * fecundity(iyear);
    }
    SR_pred_recruits(nyears + 1) = SR_alpha * SSB(nyears) / (SR_beta + SSB(nyears));
    for (iyear = 1; iyear <= nyears; iyear++) {
        recruits(iyear) = NAA(iyear, 1);
    }
    for (iyear = 1; iyear <= nyears; iyear++) {
        TotJan1B(iyear) = NAA(iyear) * WAAjan1b(iyear);
        ExploitableB(iyear) = elem_prod(NAA(iyear), FAA_tot(iyear)) * WAAcatchall(iyear) / max(FAA_tot(iyear));
    }
}

void model_parameters::get_Freport(void) {
    for (iyear = 1; iyear <= nyears; iyear++) {
        tempU = 0.0;
        tempN = 0.0;
        tempB = 0.0;
        tempUd = 0.0;
        tempNd = 0.0;
        tempBd = 0.0;
        for (iage = Freport_agemin; iage <= Freport_agemax; iage++) {
            tempU += FAA_tot(iyear, iage);
            tempN += FAA_tot(iyear, iage) * NAA(iyear, iage);
            tempB += FAA_tot(iyear, iage) * NAA(iyear, iage) * WAAjan1b(iyear, iage);
            tempUd += 1.0;
            tempNd += NAA(iyear, iage);
            tempBd += NAA(iyear, iage) * WAAjan1b(iyear, iage);
        }
        // April 2012 error trap addition
        if (tempUd <= 0.) Freport_U(iyear) = 0.0;
        else Freport_U(iyear) = tempU / tempUd;
        if (tempNd <= 0.) Freport_N(iyear) = Freport_U(iyear);
        else Freport_N(iyear) = tempN / tempNd;
        if (tempBd <= 0.) Freport_B(iyear) = Freport_U(iyear);
        else Freport_B(iyear) = tempB / tempBd;
    }
    if (Freport_wtopt == 1) Freport = Freport_U;
    if (Freport_wtopt == 2) Freport = Freport_N;
    if (Freport_wtopt == 3) Freport = Freport_B;

}

void model_parameters::get_predicted_catch(void) {
    for (ifleet = 1; ifleet <= nfleets; ifleet++) {
        CAA_pred(ifleet) = elem_prod(elem_div(FAA_by_fleet_dir(ifleet), Z), elem_prod(1.0 - S, NAA));
        Discard_pred(ifleet) = elem_prod(elem_div(FAA_by_fleet_Discard(ifleet), Z), elem_prod(1.0 - S, NAA));
    }
    for (iyear = 1; iyear <= nyears; iyear++) {
        for (ifleet = 1; ifleet <= nfleets; ifleet++) {
            CAA_prop_pred(ifleet, iyear) = 0.0;
            Discard_prop_pred(ifleet, iyear) = 0.0;
            Catch_tot_fleet_pred(ifleet, iyear) = sum(CAA_pred(ifleet, iyear)(sel_start_age(ifleet), sel_end_age(ifleet)));
            Discard_tot_fleet_pred(ifleet, iyear) = sum(Discard_pred(ifleet, iyear)(sel_start_age(ifleet), sel_end_age(ifleet)));
            if (Catch_tot_fleet_pred(ifleet, iyear) > 0.0)
                CAA_prop_pred(ifleet, iyear) = CAA_pred(ifleet, iyear)(sel_start_age(ifleet), sel_end_age(ifleet)) / Catch_tot_fleet_pred(ifleet, iyear);
            if (Discard_tot_fleet_pred(ifleet, iyear) > 0.0)
                Discard_prop_pred(ifleet, iyear) = Discard_pred(ifleet, iyear)(sel_start_age(ifleet), sel_end_age(ifleet)) / Discard_tot_fleet_pred(ifleet, iyear);
            Catch_tot_fleet_pred(ifleet, iyear) = CAA_pred(ifleet, iyear)(sel_start_age(ifleet), sel_end_age(ifleet)) * WAAcatchfleet(ifleet, iyear)(sel_start_age(ifleet), sel_end_age(ifleet));
            Discard_tot_fleet_pred(ifleet, iyear) = Discard_pred(ifleet, iyear)(sel_start_age(ifleet), sel_end_age(ifleet)) * WAAdiscardfleet(ifleet, iyear)(sel_start_age(ifleet), sel_end_age(ifleet));
            for (iage = 1; iage <= nages; iage++) {
                if (CAA_prop_pred(ifleet, iyear, iage) < 1.e-15)
                    CAA_prop_pred(ifleet, iyear, iage) = 1.0e-15;
                if (Discard_prop_pred(ifleet, iyear, iage) < 1.e-15)
                    Discard_prop_pred(ifleet, iyear, iage) = 1.0e-15;
            }
        }
    }
}

void model_parameters::get_q(void) {
    for (ind = 1; ind <= nindices; ind++) {
        q_by_index(ind, 1) = mfexp(log_q_year1(ind));
        if (active(log_q_devs)) {
            for (i = 2; i <= index_nobs(ind); i++)
                q_by_index(ind, i) = q_by_index(ind, i - 1) * mfexp(log_q_devs(ind, i));
        } else {
            for (i = 2; i <= index_nobs(ind); i++)
                q_by_index(ind, i) = q_by_index(ind, 1);
        }
    }
}

void model_parameters::get_predicted_indices(void) {
    dvariable sel_alpha1;
    dvariable sel_beta1;
    dvariable sel_alpha2;
    dvariable sel_beta2;
    dvariable sel_temp;
    dvariable sel1;
    dvariable sel2;
    k = 0;
    for (ind = 1; ind <= nindices; ind++) {
        if (index_sel_choice(ind) > 0) {
            temp_sel = sel_by_fleet(index_sel_choice(ind));
            if (index_sel_option(ind) == 1) k += nages;
            if (index_sel_option(ind) == 2) k += 2;
            if (index_sel_option(ind) == 3) k += 4;

        } else {
            if (index_sel_option(ind) == 1) {
                for (iage = 1; iage <= nages; iage++) {
                    k += 1;
                    temp_sel2(iage) = index_sel_params(k);
                }
            }
            if (index_sel_option(ind) == 2) {
                sel_alpha1 = index_sel_params(k + 1);
                sel_beta1 = index_sel_params(k + 2);
                k += 2;
                for (iage = 1; iage <= nages; iage++) {
                    temp_sel2(iage) = 1.0 / (1.0 + mfexp((sel_alpha1 - double(iage)) / sel_beta1));
                }
                sel_temp = max(temp_sel2);
                temp_sel2 /= sel_temp;
            }
            if (index_sel_option(ind) == 3) {
                sel_alpha1 = index_sel_params(k + 1);
                sel_beta1 = index_sel_params(k + 2);
                sel_alpha2 = index_sel_params(k + 3);
                sel_beta2 = index_sel_params(k + 4);
                k += 4;
                for (iage = 1; iage <= nages; iage++) {
                    sel1 = 1.0 / (1.0 + mfexp((sel_alpha1 - double(iage)) / sel_beta1));
                    sel2 = 1.0 - 1.0 / (1.0 + mfexp((sel_alpha2 - double(iage)) / sel_beta2));
                    temp_sel2(iage) = sel1*sel2;
                }
                sel_temp = max(temp_sel2);
                temp_sel2 /= sel_temp;
            }
            for (iyear = 1; iyear <= nyears; iyear++) {
                temp_sel(iyear) = temp_sel2;
            }
        }
        indexsel(ind) = temp_sel(1);
        if (index_month(ind) == -1) {
            temp_NAA = elem_prod(NAA, elem_div(1.0 - S, Z));
        } else {
            temp_NAA = elem_prod(NAA, mfexp(-1.0 * ((index_month(ind) - 1.0) / 12.0) * Z));
        }
        temp_BAA = elem_prod(temp_NAA, index_WAA(ind));
        if (index_units_aggregate(ind) == 1) {
            temp_PAA = temp_BAA;
        } else {
            temp_PAA = temp_NAA;
        }
        for (i = 1; i <= index_nobs(ind); i++) {
            j = index_time(ind, i);
            index_pred(ind, i) = q_by_index(ind, i) * sum(elem_prod(
                    temp_PAA(j)(index_start_age(ind), index_end_age(ind)),
                    temp_sel(j)(index_start_age(ind), index_end_age(ind))));
        }
        if (index_units_proportions(ind) == 1) {
            temp_PAA = temp_BAA;
        } else {
            temp_PAA = temp_NAA;
        }
        index_prop_pred(ind) = 0.0;
        if (index_estimate_proportions(ind) == 1) {
            for (i = 1; i <= index_nobs(ind); i++) {
                j = index_time(ind, i);
                if (index_pred(ind, i) > 0.0) {
                    for (iage = index_start_age(ind); iage <= index_end_age(ind); iage++) {
                        index_prop_pred(ind, i, iage) = q_by_index(ind, i) * temp_PAA(j, iage) * temp_sel(j, iage);
                    }
                    if (sum(index_prop_pred(ind, i)) > 0)
                        index_prop_pred(ind, i) /= sum(index_prop_pred(ind, i));
                    for (iage = index_start_age(ind); iage <= index_end_age(ind); iage++) {
                        if (index_prop_pred(ind, i, iage) < 1.e-15)
                            index_prop_pred(ind, i, iage) = 1.e-15;
                    }
                }
            }
        }
    }
}

void model_parameters::get_proj_sel(void) {
    dir_F = 0.0;
    Discard_F = 0.0;
    proj_nondir_F = 0.0;
    for (ifleet = 1; ifleet <= nfleets; ifleet++) {
        if (directed_fleet(ifleet) == 1) {
            dir_F += FAA_by_fleet_dir(ifleet, nyears);
            Discard_F += FAA_by_fleet_Discard(ifleet, nyears);
        } else {
            proj_nondir_F += FAA_by_fleet_dir(ifleet, nyears);
        }
    }
    proj_dir_sel = dir_F / max(dir_F);
    proj_Discard_sel = Discard_F / max(dir_F);
}

void model_parameters::get_Fref(void) {
    A = 0.0;
    B = 5.0;
    for (iloop = 1; iloop <= 20; iloop++) {
        C = (A + B) / 2.0;
        SPR_Fmult = C;
        get_SPR();
        if (SPR / SR_spawners_per_recruit < 0.30) {
            B = C;
        } else {
            A = C;
        }
    }
    F30SPR = C;
    Fref = F30SPR;
    get_Freport_ref();
    F30SPR_report = Fref_report;
    F30SPR_slope = 1.0 / SPR;
    A = 0.0;
    B = 5.0;
    for (iloop = 1; iloop <= 20; iloop++) {
        C = (A + B) / 2.0;
        SPR_Fmult = C;
        get_SPR();
        if (SPR / SR_spawners_per_recruit < 0.40) {
            B = C;
        } else {
            A = C;
        }
    }
    F40SPR = C;
    Fref = F40SPR;
    get_Freport_ref();
    F40SPR_report = Fref_report;
    F40SPR_slope = 1.0 / SPR;
    A = 0.0;
    B = 3.0;
    for (iloop = 1; iloop <= 20; iloop++) {
        C = (A + B) / 2.0;
        SPR_Fmult = C + delta;
        get_SPR();
        S_F = SR_alpha * SPR - SR_beta;
        R_F = S_F / SPR;
        YPR_Fmult = C + delta;
        get_YPR();
        slope = R_F*YPR;
        SPR_Fmult = C;
        get_SPR();
        S_F = SR_alpha * SPR - SR_beta;
        R_F = S_F / SPR;
        YPR_Fmult = C;
        get_YPR();
        slope -= R_F*YPR;
        if (slope > 0.0) {
            A = C;
        } else {
            B = C;
        }
    }
    Fmsy = C;
    Fref = Fmsy;
    get_Freport_ref();
    Fmsy_report = Fref_report;
    SSmsy = S_F;
    SSBmsy_report = SSmsy;
    if (SSmsy > 0.0)
        SSBmsy_ratio = SSB(nyears) / SSmsy;
    MSY = YPR*R_F;
    SPR_Fmult = Fmsy;
    get_SPR();
    Fmsy_slope = 1.0 / SPR;
    YPR_Fmult = delta;
    get_YPR();
    slope_origin = YPR / delta;
    A = 0.0;
    B = 5.0;
    for (iloop = 1; iloop <= 20; iloop++) {
        C = (A + B) / 2.0;
        YPR_Fmult = C + delta;
        get_YPR();
        slope = YPR;
        YPR_Fmult = C;
        get_YPR();
        slope -= YPR;
        slope /= delta;
        if (slope < 0.10 * slope_origin) {
            B = C;
        } else {
            A = C;
        }
    }
    F01 = C;
    Fref = F01;
    get_Freport_ref();
    F01_report = Fref_report;
    SPR_Fmult = F01;
    get_SPR();
    F01_slope = 1.0 / SPR;
    A = 0.0;
    B = 10.0;
    for (iloop = 1; iloop <= 20; iloop++) {
        C = (A + B) / 2.0;
        YPR_Fmult = C + delta;
        get_YPR();
        slope = YPR;
        YPR_Fmult = C;
        get_YPR();
        slope -= YPR;
        slope /= delta;
        if (slope < 0.0) {
            B = C;
        } else {
            A = C;
        }
    }
    Fmax = C;
    Fref = Fmax;
    get_Freport_ref();
    Fmax_report = Fref_report;
    SPR_Fmult = Fmax;
    get_SPR();
    Fmax_slope = 1.0 / SPR;
    Fcurrent = max(FAA_tot(nyears) - proj_nondir_F - Discard_F);
    SPR_Fmult = Fcurrent;
    get_SPR();
    Fcurrent_slope = 1.0 / SPR;
    if (Fmsy > 0.0)
        Fmsy_ratio = Fcurrent / Fmsy;
}

void model_parameters::get_Freport_ref(void) {
    trefU = 0.0;
    trefN = 0.0;
    trefB = 0.0;
    trefUd = 0.0;
    trefNd = 0.0;
    trefBd = 0.0;
    nreftemp(1) = 1.0;
    for (iage = 1; iage < nages; iage++) {
        freftemp(iage) = Fref * (proj_dir_sel(iage) + proj_Discard_sel(iage)) + proj_nondir_F(iage);
        nreftemp(iage + 1) = mfexp(-1.0 * (M(nyears, iage) + freftemp(iage)));
    }
    freftemp(nages) = Fref * (proj_dir_sel(nages) + proj_Discard_sel(nages)) + proj_nondir_F(nages);
    nreftemp(nages) /= (1.0 - mfexp(-1.0 * (M(nyears, nages) + freftemp(nages))));
    for (iage = Freport_agemin; iage <= Freport_agemax; iage++) {
        trefU += freftemp(iage);
        trefN += freftemp(iage) * nreftemp(iage);
        trefB += freftemp(iage) * nreftemp(iage) * WAAjan1b(nyears, iage);
        trefUd += 1.0;
        trefNd += nreftemp(iage);
        trefBd += nreftemp(iage) * WAAjan1b(nyears, iage);
    }
    if (Freport_wtopt == 1) Fref_report = trefU / trefUd;
    if (Freport_wtopt == 2) Fref_report = trefN / trefNd;
    if (Freport_wtopt == 3) Fref_report = trefB / trefBd;

}

void model_parameters::get_YPR(void) {
    YPR = 0.0;
    ntemp = 1.0;
    for (iage = 1; iage < nages; iage++) {
        f = YPR_Fmult * proj_dir_sel(iage);
        z = M(nyears, iage) + f + proj_nondir_F(iage) + YPR_Fmult * proj_Discard_sel(iage);
        YPR += ntemp * f * WAAcatchall(nyears, iage)*(1.0 - mfexp(-1.0 * z)) / z;
        ntemp *= mfexp(-1.0 * z);
    }
    f = YPR_Fmult * proj_dir_sel(nages);
    z = M(nyears, nages) + f + proj_nondir_F(nages) + YPR_Fmult * proj_Discard_sel(nages);
    ntemp /= (1.0 - mfexp(-1.0 * z));
    YPR += ntemp * f * WAAcatchall(nyears, nages)*(1.0 - mfexp(-1.0 * z)) / z;
}

void model_parameters::project_into_future(void) {
    for (iyear = 1; iyear <= nprojyears; iyear++) {
        proj_F_nondir(iyear) = proj_nondir_F * proj_F_nondir_mult(iyear);
        if (proj_recruit(iyear) < 0.0) // use stock-recruit relationship
        {
            if (iyear == 1) {
                proj_NAA(iyear, 1) = SR_alpha * SSB(nyears) / (SR_beta + SSB(nyears));
            } else {
                proj_NAA(iyear, 1) = SR_alpha * proj_SSB(iyear - 1) / (SR_beta + proj_SSB(iyear - 1));
            }
        } else {
            proj_NAA(iyear, 1) = proj_recruit(iyear);
        }
        if (iyear == 1) {
            for (iage = 2; iage <= nages; iage++)
                proj_NAA(1, iage) = NAA(nyears, iage - 1) * S(nyears, iage - 1);
            proj_NAA(1, nages) += NAA(nyears, nages) * S(nyears, nages);
        } else {
            for (iage = 2; iage <= nages; iage++)
                proj_NAA(iyear, iage) = proj_NAA(iyear - 1, iage - 1) * mfexp(-1.0 * proj_Z(iyear - 1, iage - 1));
            proj_NAA(iyear, nages) += proj_NAA(iyear - 1, nages) * mfexp(-1.0 * proj_Z(iyear - 1, nages));
        }
        if (proj_what(iyear) == 1) // match directed yield
        {
            proj_Fmult(iyear) = 3.0; // first see if catch possible
            proj_F_dir(iyear) = proj_Fmult(iyear) * proj_dir_sel;
            proj_F_Discard(iyear) = proj_Fmult(iyear) * proj_Discard_sel;
            proj_Z(iyear) = M(nyears) + proj_F_nondir(iyear) + proj_F_dir(iyear) + proj_F_Discard(iyear);
            proj_catch(iyear) = elem_prod(elem_div(proj_F_dir(iyear), proj_Z(iyear)), elem_prod(1.0 - mfexp(-1.0 * proj_Z(iyear)), proj_NAA(iyear)));
            proj_Discard(iyear) = elem_prod(elem_div(proj_F_Discard(iyear), proj_Z(iyear)), elem_prod(1.0 - mfexp(-1.0 * proj_Z(iyear)), proj_NAA(iyear)));
            proj_yield(iyear) = elem_prod(proj_catch(iyear), WAAcatchall(nyears));
            proj_total_yield(iyear) = sum(proj_yield(iyear));
            proj_total_Discard(iyear) = sum(elem_prod(proj_Discard(iyear), WAAdiscardall(nyears)));
            if (proj_total_yield(iyear) > proj_target(iyear)) // if catch possible, what F needed
            {
                proj_Fmult(iyear) = 0.0;
                for (iloop = 1; iloop <= 20; iloop++) {
                    Ftemp = proj_Fmult(iyear) * proj_dir_sel;
                    denom = 0.0;
                    for (iage = 1; iage <= nages; iage++) {
                        Ztemp(iage) = M(nyears, iage) + proj_F_nondir(iyear, iage) + proj_Fmult(iyear) * proj_Discard_sel(iage) + Ftemp(iage);
                        denom += proj_NAA(iyear, iage) * WAAcatchall(nyears, iage) * proj_dir_sel(iage)*(1.0 - mfexp(-1.0 * Ztemp(iage))) / Ztemp(iage);
                    }
                    proj_Fmult(iyear) = proj_target(iyear) / denom;
                }
            }
        } else if (proj_what(iyear) == 2) // match F%SPR
        {
            A = 0.0;
            B = 5.0;
            for (iloop = 1; iloop <= 20; iloop++) {
                C = (A + B) / 2.0;
                SPR_Fmult = C;
                get_SPR();
                SPRatio = SPR / SR_spawners_per_recruit;
                if (SPRatio < proj_target(iyear)) {
                    B = C;
                } else {
                    A = C;
                }
            }
            proj_Fmult(iyear) = C;
        } else if (proj_what(iyear) == 3) // project Fmsy
        {
            proj_Fmult = Fmsy;
        } else if (proj_what(iyear) == 4) // project Fcurrent
        {
            proj_Fmult = Fcurrent;
        } else if (proj_what(iyear) == 5) // project input F
        {
            proj_Fmult = proj_target(iyear);
        }
        proj_F_dir(iyear) = proj_Fmult(iyear) * proj_dir_sel;
        proj_F_Discard(iyear) = proj_Fmult(iyear) * proj_Discard_sel;
        proj_Z(iyear) = M(nyears) + proj_F_nondir(iyear) + proj_F_dir(iyear) + proj_F_Discard(iyear);
        proj_SSBfracZ(iyear) = mfexp(-1.0 * fracyearSSB * proj_Z(iyear));
        proj_catch(iyear) = elem_prod(elem_div(proj_F_dir(iyear), proj_Z(iyear)), elem_prod(1.0 - mfexp(-1.0 * proj_Z(iyear)), proj_NAA(iyear)));
        proj_Discard(iyear) = elem_prod(elem_div(proj_F_Discard(iyear), proj_Z(iyear)), elem_prod(1.0 - mfexp(-1.0 * proj_Z(iyear)), proj_NAA(iyear)));
        proj_yield(iyear) = elem_prod(proj_catch(iyear), WAAcatchall(nyears));
        proj_total_yield(iyear) = sum(proj_yield(iyear));
        proj_total_Discard(iyear) = sum(elem_prod(proj_Discard(iyear), WAAdiscardall(nyears)));
        proj_TotJan1B(iyear) = sum(elem_prod(proj_NAA(iyear), WAAjan1b(nyears)));
        proj_SSB(iyear) = elem_prod(proj_NAA(iyear), proj_SSBfracZ(iyear)) * fecundity(nyears);
    }
}

void model_parameters::get_SPR(void) {
    ntemp = 1.0;
    SPR = 0.0;
    for (iage = 1; iage < nages; iage++) {
        z = M(nyears, iage) + proj_nondir_F(iage) + SPR_Fmult * proj_dir_sel(iage) + SPR_Fmult * proj_Discard_sel(iage);
        SPR += ntemp * fecundity(nyears, iage) * mfexp(-1.0 * fracyearSSB * z);
        ntemp *= mfexp(-1.0 * z);
    }
    z = M(nyears, nages) + proj_nondir_F(nages) + SPR_Fmult * proj_dir_sel(nages) + SPR_Fmult * proj_Discard_sel(nages);
    ntemp /= (1.0 - mfexp(-1.0 * z));
    SPR += ntemp * fecundity(nyears, nages) * mfexp(-1.0 * fracyearSSB * z);
}

void model_parameters::get_multinomial_multiplier(void) {
    Neff_stage2_mult_catch = 1;
    Neff_stage2_mult_discard = 1;
    Neff_stage2_mult_index = 1;
    // Catch
    for (ifleet = 1; ifleet <= nfleets; ifleet++) {
        mean_age_obs = 0.0;
        mean_age_pred = 0.0;
        mean_age_pred2 = 0.0;
        mean_age_resid = 0.0;
        for (iyear = 1; iyear <= nyears; iyear++) {
            for (iage = sel_start_age(ifleet); iage <= sel_end_age(ifleet); iage++) {
                mean_age_obs(iyear) += CAA_prop_obs(ifleet, iyear, iage) * iage;
                mean_age_pred(iyear) += CAA_prop_pred(ifleet, iyear, iage) * iage;
                mean_age_pred2(iyear) += CAA_prop_pred(ifleet, iyear, iage) * iage*iage;
            }
        }
        mean_age_resid = mean_age_obs - mean_age_pred;
        mean_age_sigma = sqrt(mean_age_pred2 - elem_prod(mean_age_pred, mean_age_pred));
        mean_age_n = 0.0;
        mean_age_mean = 0.0;
        mean_age_m2 = 0.0;
        for (iyear = 1; iyear <= nyears; iyear++) {
            if (input_eff_samp_size_catch(ifleet, iyear) > 0) {
                mean_age_x = mean_age_resid(iyear) * sqrt(input_eff_samp_size_catch(ifleet, iyear)) / mean_age_sigma(iyear);
                mean_age_n += 1.0;
                mean_age_delta = mean_age_x - mean_age_mean;
                mean_age_mean += mean_age_delta / mean_age_n;
                mean_age_m2 += mean_age_delta * (mean_age_x - mean_age_mean);
            }
        }
        if ((mean_age_n > 0) && (mean_age_m2 > 0)) Neff_stage2_mult_catch(ifleet) = 1.0 / (mean_age_m2 / (mean_age_n - 1.0));
    }
    // Discards
    for (ifleet = 1; ifleet <= nfleets; ifleet++) {
        mean_age_obs = 0.0;
        mean_age_pred = 0.0;
        mean_age_pred2 = 0.0;
        mean_age_resid = 0.0;
        for (iyear = 1; iyear <= nyears; iyear++) {
            for (iage = sel_start_age(ifleet); iage <= sel_end_age(ifleet); iage++) {
                mean_age_obs(iyear) += Discard_prop_obs(ifleet, iyear, iage) * iage;
                mean_age_pred(iyear) += Discard_prop_pred(ifleet, iyear, iage) * iage;
                mean_age_pred2(iyear) += Discard_prop_pred(ifleet, iyear, iage) * iage*iage;
            }
        }
        mean_age_resid = mean_age_obs - mean_age_pred;
        mean_age_sigma = sqrt(mean_age_pred2 - elem_prod(mean_age_pred, mean_age_pred));
        mean_age_n = 0.0;
        mean_age_mean = 0.0;
        mean_age_m2 = 0.0;
        for (iyear = 1; iyear <= nyears; iyear++) {
            if (input_eff_samp_size_discard(ifleet, iyear) > 0) {
                mean_age_x = mean_age_resid(iyear) * sqrt(input_eff_samp_size_discard(ifleet, iyear)) / mean_age_sigma(iyear);
                mean_age_n += 1.0;
                mean_age_delta = mean_age_x - mean_age_mean;
                mean_age_mean += mean_age_delta / mean_age_n;
                mean_age_m2 += mean_age_delta * (mean_age_x - mean_age_mean);
            }
        }
        if ((mean_age_n > 0) && (mean_age_m2 > 0)) Neff_stage2_mult_discard(ifleet) = 1.0 / (mean_age_m2 / (mean_age_n - 1.0));
    }
    // Indices
    for (ind = 1; ind <= nindices; ind++) {
        mean_age_obs = 0.0;
        mean_age_pred = 0.0;
        mean_age_pred2 = 0.0;
        mean_age_resid = 0.0;
        for (i = 1; i <= index_nobs(ind); i++) {
            j = index_time(ind, i);
            for (iage = index_start_age(ind); iage <= index_end_age(ind); iage++) {
                mean_age_obs(j) += index_prop_obs(ind, i, iage) * iage;
                mean_age_pred(j) += index_prop_pred(ind, i, iage) * iage;
                mean_age_pred2(j) += index_prop_pred(ind, i, iage) * iage*iage;
            }
        }
        mean_age_resid = mean_age_obs - mean_age_pred;
        mean_age_sigma = sqrt(mean_age_pred2 - elem_prod(mean_age_pred, mean_age_pred));
        mean_age_n = 0.0;
        mean_age_mean = 0.0;
        mean_age_m2 = 0.0;
        for (iyear = 1; iyear <= nyears; iyear++) {
            if (index_Neff_init(ind, iyear) > 0) {
                mean_age_x = mean_age_resid(iyear) * sqrt(index_Neff_init(ind, iyear)) / mean_age_sigma(iyear);
                mean_age_n += 1.0;
                mean_age_delta = mean_age_x - mean_age_mean;
                mean_age_mean += mean_age_delta / mean_age_n;
                mean_age_m2 += mean_age_delta * (mean_age_x - mean_age_mean);
            }
        }
        if ((mean_age_n > 0) && (mean_age_m2 > 0)) Neff_stage2_mult_index(ind) = 1.0 / (mean_age_m2 / (mean_age_n - 1.0));
    }

}

void model_parameters::get_log_factorial(void) {
    nfact_out = 0.0;
    if (nfact_in >= 2) {
        for (int ilogfact = 2; ilogfact <= nfact_in; ilogfact++) {
            nfact_out += log(ilogfact);
        }
    }
}

void model_parameters::compute_the_objective_function(void) {
    obj_fun = 0.0;
    io = 0; // io if statements commented out to speed up program
    for (ind = 1; ind <= nindices; ind++) {
        likely_ind(ind) = index_like_const(ind);
        RSS_ind(ind) = norm2(log(index_obs(ind)) - log(index_pred(ind)));
        for (i = 1; i <= index_nobs(ind); i++) {
            likely_ind(ind) += log(index_sigma(ind, i));
            likely_ind(ind) += 0.5 * square(log(index_obs(ind, i)) - log(index_pred(ind, i))) / index_sigma2(ind, i);
         
            index_stdresid(ind, i) = (log(index_obs(ind, i)) - log(index_pred(ind, i))) / index_sigma(ind, i);
        }
        obj_fun += lambda_ind(ind) * likely_ind(ind);
    }
    // if (io==1) cout << "likely_ind " << likely_ind << endl;

    likely_index_age_comp = index_prop_like_const;
    for (ind = 1; ind <= nindices; ind++) {
        std::cout << "inedx " << ind << "\n";
        if (index_estimate_proportions(ind) == 1) {
            for (i = 1; i <= index_nobs(ind); i++) {
                temp_sum = 0.0;
                for (iage = index_start_age(ind); iage <= index_end_age(ind); iage++) {
                    temp_sum += index_prop_obs(ind, i, iage) * log(index_prop_pred(ind, i, iage));
                    std::cout << index_prop_obs(ind, i, iage) << " ";
                }
                std::cout << "\n";
                likely_index_age_comp += -1.0 * input_eff_samp_size_index(ind, i) * temp_sum;
            }
        }
    }
//    std::cout<<index_sel_params<<"\n\n"<<indexsel_lo<<"\n\n"<<indexsel_hi<<"\n\n"<<indexsel_phase;
//    exit(0);
    obj_fun += likely_index_age_comp;
    // if (io==1) cout << "likely_index_age_comp " << likely_index_age_comp << endl;

    for (ifleet = 1; ifleet <= nfleets; ifleet++) {
        catch_tot_likely(ifleet) = catch_tot_like_const(ifleet);
        discard_tot_likely(ifleet) = discard_tot_like_const(ifleet);
        RSS_catch_tot_fleet(ifleet) = norm2(log(Catch_tot_fleet_obs(ifleet) + 0.00001) - log(Catch_tot_fleet_pred(ifleet) + 0.00001));
        RSS_Discard_tot_fleet(ifleet) = norm2(log(Discard_tot_fleet_obs(ifleet) + 0.00001) - log(Discard_tot_fleet_pred(ifleet) + 0.00001));
        for (iyear = 1; iyear <= nyears; iyear++) {
            catch_tot_likely(ifleet) += log(catch_tot_sigma(ifleet, iyear));
            catch_tot_likely(ifleet) += 0.5 * square(log(Catch_tot_fleet_obs(ifleet, iyear) + 0.00001) - log(Catch_tot_fleet_pred(ifleet, iyear) + 0.00001)) / catch_tot_sigma2(ifleet, iyear);
            discard_tot_likely(ifleet) += log(discard_tot_sigma(ifleet, iyear));
            discard_tot_likely(ifleet) += 0.5 * square(log(Discard_tot_fleet_obs(ifleet, iyear) + 0.00001) - log(Discard_tot_fleet_pred(ifleet, iyear) + 0.00001)) / discard_tot_sigma2(ifleet, iyear);
        }
        obj_fun += lambda_catch_tot(ifleet) * catch_tot_likely(ifleet);
        obj_fun += lambda_Discard_tot(ifleet) * discard_tot_likely(ifleet);
    }
    // if (io==1) cout << "catch_tot_likely " << catch_tot_likely << endl;

    likely_catch = catch_prop_like_const;
    likely_Discard = discard_prop_like_const;
    for (ifleet = 1; ifleet <= nfleets; ifleet++) {
        for (iyear = 1; iyear <= nyears; iyear++) {
            temp_sum = 0.0;
            temp_sum2 = 0.0;
            for (iage = sel_start_age(ifleet); iage <= sel_end_age(ifleet); iage++) {
                temp_sum += CAA_prop_obs(ifleet, iyear, iage) * log(CAA_prop_pred(ifleet, iyear, iage));
                if (proportion_release(ifleet, iyear, iage) > 0.0)
                    temp_sum2 += Discard_prop_obs(ifleet, iyear, iage) * log(Discard_prop_pred(ifleet, iyear, iage));
            }
            likely_catch += -1.0 * input_eff_samp_size_catch(ifleet, iyear) * temp_sum;
            likely_Discard += -1.0 * input_eff_samp_size_discard(ifleet, iyear) * temp_sum2;
        }
    }
    obj_fun += likely_catch;
    obj_fun += likely_Discard;
    // if (io==1) cout << "likely_catch " << likely_catch << endl;

    likely_SR_sigma = SR_like_const;
    if (use_likelihood_constants == 1) {
        likely_SR_sigma += sum(log(SR_pred_recruits));
        likely_SR_sigma -= log(SR_pred_recruits(nyears + 1)); // pred R in terminal year plus one does not have a deviation
    }
    SR_stdresid = 0.0;
    if (active(log_recruit_devs)) {
        for (iyear = 1; iyear <= nyears; iyear++) {
            likely_SR_sigma += log(recruit_sigma(iyear));
            likely_SR_sigma += 0.5 * square(log(recruits(iyear)) - log(SR_pred_recruits(iyear))) / recruit_sigma2(iyear);
            SR_stdresid(iyear) = (log(recruits(iyear)) - log(SR_pred_recruits(iyear))) / recruit_sigma(iyear);
        }
        obj_fun += lambda_recruit_devs*likely_SR_sigma;
    }
    // if (io==1) cout << "likely_SR_sigma " << likely_SR_sigma << endl;

    sel_likely = 0.0;
    sel_stdresid = 0.0;
    for (k = 1; k <= nselparm; k++) {
        if (active(sel_params(k))) {
            sel_likely(k) += sel_like_const(k);
            sel_likely(k) += log(sel_sigma(k)) + 0.5 * square(log(sel_initial(k)) - log(sel_params(k))) / sel_sigma2(k);
            std::cout<<sel_sigma(k)<<" ----- "<<sel_sigma2(k)<<"\n";
            sel_stdresid(k) = (log(sel_initial(k)) - log(sel_params(k))) / sel_sigma(k);
            obj_fun += sel_lambda(k) * sel_likely(k);
        }
    }
    // if (io==1) cout << "sel_likely " << sel_likely << endl;
//    exit(0);
    indexsel_likely = 0.0;
    indexsel_stdresid = 0.0;
    for (k = 1; k <= nindexselparms; k++) {
        if (active(index_sel_params(k))) {
            indexsel_likely(k) += indexsel_like_const(k);
            std::cout<<indexsel_sigma(k)<<"   "<<indexsel_sigma2(k)<<"\n";
            indexsel_likely(k) += log(indexsel_sigma(k)) + 0.5 * square(log(indexsel_initial(k)) - log(index_sel_params(k))) / indexsel_sigma2(k);
            indexsel_stdresid(k) = (log(indexsel_initial(k)) - log(index_sel_params(k))) / indexsel_sigma(k);
            obj_fun += indexsel_lambda(k) * indexsel_likely(k);
        }
    }
    // if (io==1) cout << "indexsel_likely " << indexsel_likely << endl;

    steepness_likely = 0.0;
    steepness_stdresid = 0.0;
    if (active(SR_steepness)) {
        steepness_likely = steepness_like_const;
        steepness_likely += log(steepness_sigma) + 0.5 * square(log(SR_steepness_ini) - log(SR_steepness)) / steepness_sigma2;
        steepness_stdresid = (log(SR_steepness_ini) - log(SR_steepness)) / steepness_sigma;
        obj_fun += lambda_steepness*steepness_likely;
    }
    // if (io==1) cout << "steepness_likely " << steepness_likely << endl;
    SR_scaler_likely = 0.0;
    SR_scaler_stdresid = 0.0;
    if (active(log_SR_scaler)) {
        SR_scaler_likely = SR_scaler_like_const;
        SR_scaler_likely += log(SR_scaler_sigma) + 0.5 * (square(log(SR_scaler_ini) - log_SR_scaler)) / SR_scaler_sigma2;
        SR_scaler_stdresid = (log(SR_scaler_ini) - log_SR_scaler) / SR_scaler_sigma;
        obj_fun += lambda_SR_scaler*SR_scaler_likely;
    }
    // if (io==1) cout << "SR_scaler_likely " << SR_scaler_likely << endl;
    Fmult_year1_stdresid = 0.0;
    if (active(log_Fmult_year1)) {
        for (ifleet = 1; ifleet <= nfleets; ifleet++) {
            Fmult_year1_likely(ifleet) = Fmult_year1_like_const(ifleet);
            Fmult_year1_likely(ifleet) += log(Fmult_year1_sigma(ifleet)) + 0.5 * square(log_Fmult_year1(ifleet) - log(Fmult_year1_ini(ifleet))) / Fmult_year1_sigma2(ifleet);
            Fmult_year1_stdresid(ifleet) = (log_Fmult_year1(ifleet) - log(Fmult_year1_ini(ifleet))) / Fmult_year1_sigma(ifleet);
        }
        obj_fun += lambda_Fmult_year1*Fmult_year1_likely;
    }
    // if (io==1) cout << "Fmult_year1_likely " << Fmult_year1_likely << endl;

    Fmult_devs_stdresid = 0.0;
    if (active(log_Fmult_devs)) {
        for (ifleet = 1; ifleet <= nfleets; ifleet++) {
            Fmult_devs_likely(ifleet) = Fmult_devs_like_const(ifleet);
            Fmult_devs_likely(ifleet) += log(Fmult_devs_sigma(ifleet)) + 0.5 * norm2(log_Fmult_devs(ifleet)) / Fmult_devs_sigma2(ifleet);
            for (iyear = 2; iyear <= nyears; iyear++)
                Fmult_devs_stdresid(ifleet, iyear) = log_Fmult_devs(ifleet, iyear) / Fmult_devs_sigma(ifleet);
        }
        obj_fun += lambda_Fmult_devs*Fmult_devs_likely;
    }
    // if (io==1) cout << "Fmult_devs_likely " << Fmult_devs_likely << endl;

    q_year1_stdresid = 0.0;
    if (active(log_q_year1)) {
        for (ind = 1; ind <= nindices; ind++) {
            q_year1_likely(ind) = q_year1_like_const(ind);
            q_year1_likely(ind) += log(q_year1_sigma(ind)) + 0.5 * square(log_q_year1(ind) - log(q_year1_ini(ind))) / q_year1_sigma2(ind);
            q_year1_stdresid(ind) = (log_q_year1(ind) - log(q_year1_ini(ind))) / q_year1_sigma(ind);
        }
        obj_fun += lambda_q_year1*q_year1_likely;
    }
    // if (io==1) cout << "q_year1_likely " << q_year1_likely << endl;

    q_devs_stdresid = 0.0;
    if (active(log_q_devs)) {
        for (ind = 1; ind <= nindices; ind++) {
            q_devs_likely(ind) = q_devs_like_const(ind);
            q_devs_likely(ind) += log(q_devs_sigma(ind)) + 0.5 * norm2(log_q_devs(ind)) / q_devs_sigma2(ind);
            for (i = 2; i <= index_nobs(ind); i++)
                q_devs_stdresid(ind, i) = log_q_devs(ind, i) / q_devs_sigma(ind);
        }
        obj_fun += lambda_q_devs*q_devs_likely;
    }
    // if (io==1) cout << "q_devs_likely " << q_devs_likely << endl;

    if (NAA_year1_flag == 1) {
        nyear1temp(1) = SR_pred_recruits(1);
        N_year1_stdresid = 0.0;
        for (iage = 2; iage <= nages; iage++) {
            nyear1temp(iage) = nyear1temp(iage - 1) * S(1, iage - 1);
        }
        nyear1temp(nages) /= (1.0 - S(1, nages));
    } else if (NAA_year1_flag == 2) {
        nyear1temp = NAA_year1_ini;
    }
    if (active(log_N_year1_devs)) {
        if (N_year1_sigma > 0.0) {
            for (iage = 2; iage <= nages; iage++)
                N_year1_stdresid(iage) = (log(NAA(1, iage)) - log(nyear1temp(iage))) / N_year1_sigma;
        }
        N_year1_likely = N_year1_like_const + sum(log(nyear1temp));
        N_year1_likely += log(N_year1_sigma) + 0.5 * norm2(log(NAA(1)) - log(nyear1temp)) / N_year1_sigma2;
        obj_fun += lambda_N_year1_devs*N_year1_likely;
    }
    // if (io==1) cout << "N_year1_likely " << N_year1_likely << endl;

    Fmult_max_pen = 0.0;
    for (ifleet = 1; ifleet <= nfleets; ifleet++) {
        for (iyear = 1; iyear <= nyears; iyear++) {
            temp_Fmult_max = mfexp(log_Fmult(ifleet, iyear)) * max(sel_by_fleet(ifleet, iyear));
            if (temp_Fmult_max > Fmult_max_value)
                Fmult_max_pen += 1000. * (temp_Fmult_max - Fmult_max_value)*(temp_Fmult_max - Fmult_max_value);
        }
    }
    obj_fun += Fmult_max_pen;
    // if (io==1) cout << "Fmult_max_pen " << Fmult_max_pen << endl;
    fpenalty_lambda = 100.0 * pow(10.0, (-1.0 * current_phase())); // decrease emphasis on F near M as phases increase
    if (last_phase()) // no penalty in final solution
        fpenalty_lambda = 0.0;
    fpenalty = fpenalty_lambda * square(log(mean(FAA_tot)) - log(mean(M)));
    obj_fun += fpenalty;
    // if (io==1) cout << "fpenalty " << fpenalty << endl;
}

void model_parameters::write_MCMC(void) {
    if (MCMCnyear_opt == 0) // use final year
    {
        if (fillR_opt == 0) {
            NAAbsn(1) = NAA(nyears, 1);
        } else if (fillR_opt == 1) {
            NAAbsn(1) = SR_pred_recruits(nyears);
        } else if (fillR_opt == 2) {
            tempR = 0.0;
            for (i = Ravg_start; i <= Ravg_end; i++) {
                iyear = i - year1 + 1;
                tempR += log(NAA(iyear, 1));
            }
            NAAbsn(1) = mfexp(tempR / (Ravg_end - Ravg_start + 1.0));
        }
        for (iage = 2; iage <= nages; iage++) {
            NAAbsn(iage) = NAA(nyears, iage);
        }
    } else // use final year + 1
    {
        if (fillR_opt == 1) {
            NAAbsn(1) = SR_pred_recruits(nyears + 1);
        } else if (fillR_opt == 2) {
            tempR = 0.0;
            for (i = Ravg_start; i <= Ravg_end; i++) {
                iyear = i - year1 + 1;
                tempR += log(NAA(iyear, 1));
            }
            NAAbsn(1) = mfexp(tempR / (Ravg_end - Ravg_start + 1.0));
        }
        for (iage = 2; iage <= nages; iage++) {
            NAAbsn(iage) = NAA(nyears, iage - 1) * S(nyears, iage - 1);
        }
        NAAbsn(nages) += NAA(nyears, nages) * S(nyears, nages);


    }

    // Liz added
    for (iyear = 1; iyear <= nyears; iyear++) {
        tempFmult(iyear) = max(extract_row(FAA_tot, iyear));
    }
    // end stuff Liz added
    ageproMCMC << NAAbsn << endl;

    basicMCMC << Freport << " " <<
            SSB << " " <<
            ///  Liz added

            tempFmult << " " <<

            rowsum(elem_prod(WAAjan1b, NAA)) << " " <<

            /// end stuff Liz added               

            MSY << " " <<
            SSmsy << " " <<
            Fmsy << " " <<
            SSBmsy_ratio << " " <<
            Fmsy_ratio << " " <<
            endl;

}

void model_parameters::report(const dvector& gradients) {
    adstring ad_tmp = initial_params::get_reportfile_name();
    ofstream report((char*) (adprogram_name + ad_tmp));
    if (!report) {
        cerr << "error trying to open report file" << adprogram_name << ".rep";
        return;
    }
    report << "Age Structured Assessment Program (ASAP) Version 3.0" << endl;
    report << "Start time for run: " << ctime(&start) << endl;
    report << "obj_fun        = " << obj_fun << endl << endl;
    report << "Component                Lambda          obj_fun" << endl;
    for (ifleet = 1; ifleet <= nfleets; ifleet++)
        report << "__Catch_Fleet_" << ifleet << "               " << lambda_catch_tot(ifleet) << "          " << lambda_catch_tot(ifleet) * catch_tot_likely(ifleet) << endl;
    report << "Catch_Fleet_Total             " << sum(lambda_catch_tot) << "          " << lambda_catch_tot * catch_tot_likely << endl;
    if (lambda_Discard_tot * discard_tot_likely > 0.0) {
        for (ifleet = 1; ifleet <= nfleets; ifleet++)
            report << "__Discard_Fleet_" << ifleet << "             " << lambda_Discard_tot(ifleet) << "          " << lambda_Discard_tot(ifleet) * discard_tot_likely(ifleet) << endl;
    }
    report << "Discard_Fleet_Total           " << sum(lambda_Discard_tot) << "          " << lambda_Discard_tot * discard_tot_likely << endl;
    for (ind = 1; ind <= nindices; ind++)
        report << "__Index_Fit_" << ind << "                 " << lambda_ind(ind) << "          " << lambda_ind(ind) * likely_ind(ind) << endl;
    report << "Index_Fit_Total               " << sum(lambda_ind) << "          " << lambda_ind * likely_ind << endl;
    report << "Catch_Age_Comps       see_below          " << likely_catch << endl;
    report << "Discard_Age_Comps     see_below          " << likely_Discard << endl;
    report << "Index_Age_Comps       see_below          " << likely_index_age_comp << endl;
    sum_sel_lambda = 0;
    sum_sel_lambda_likely = 0.0;
    for (k = 1; k <= nselparm; k++) {
        if (sel_phase(k) >= 1) {
            if (k < 10) report << "__Sel_Param_" << k << "                 " << sel_lambda(k) << "          " << sel_lambda(k) * sel_likely(k) << endl;
            else if (k < 100) report << "__Sel_Param_" << k << "                " << sel_lambda(k) << "          " << sel_lambda(k) * sel_likely(k) << endl;
            else if (k < 1000) report << "__Sel_Param_" << k << "               " << sel_lambda(k) << "          " << sel_lambda(k) * sel_likely(k) << endl;
            sum_sel_lambda += sel_lambda(k);
            sum_sel_lambda_likely += sel_lambda(k) * sel_likely(k);
        }
    }
    report << "Sel_Params_Total              " << sum_sel_lambda << "          " << sum_sel_lambda_likely << endl;
    sum_indexsel_lambda = 0;
    sum_indexsel_lambda_likely = 0.0;
    for (k = 1; k <= nindexselparms; k++) {
        if (indexsel_phase(k) >= 1) {
            if (k < 10) report << "__Index_Sel_Param_" << k << "           " << indexsel_lambda(k) << "          " << indexsel_lambda(k) * indexsel_likely(k) << endl;
            else if (k < 100) report << "__Index_Sel_Param_" << k << "          " << indexsel_lambda(k) << "          " << indexsel_lambda(k) * indexsel_likely(k) << endl;
            else if (k < 1000) report << "__Index_Sel_Param_" << k << "         " << indexsel_lambda(k) << "          " << indexsel_lambda(k) * indexsel_likely(k) << endl;
            sum_indexsel_lambda += indexsel_lambda(k);
            sum_indexsel_lambda_likely += indexsel_lambda(k) * indexsel_likely(k);
        }
    }
    report << "Index_Sel_Params_Total        " << sum_indexsel_lambda << "          " << sum_indexsel_lambda_likely << endl;
    if (lambda_q_year1 * q_year1_likely > 0.0) {
        for (ind = 1; ind <= nindices; ind++)
            report << "__q_year1_index_" << ind << "             " << lambda_q_year1(ind) << "          " << lambda_q_year1(ind) * q_year1_likely(ind) << endl;
    }
    report << "q_year1_Total                 " << sum(lambda_q_year1) << "          " << lambda_q_year1 * q_year1_likely << endl;

    if (lambda_q_devs * q_devs_likely > 0.0) {
        for (ind = 1; ind <= nindices; ind++)
            report << "__q_devs_index_" << ind << "              " << lambda_q_devs(ind) << "          " << lambda_q_devs(ind) * q_devs_likely(ind) << endl;
    }
    report << "q_devs_Total                  " << sum(lambda_q_devs) << "          " << lambda_q_devs * q_devs_likely << endl;
    if (lambda_Fmult_year1 * Fmult_year1_likely > 0.0) {
        for (ifleet = 1; ifleet <= nfleets; ifleet++)
            report << "__Fmult_year1_fleet_" << ifleet << "         " << lambda_Fmult_year1(ifleet) << "          " << lambda_Fmult_year1(ifleet) * Fmult_year1_likely(ifleet) << endl;
    }
    report << "Fmult_year1_fleet_Total       " << sum(lambda_Fmult_year1) << "          " << lambda_Fmult_year1 * Fmult_year1_likely << endl;
    if (lambda_Fmult_devs * Fmult_devs_likely > 0.0) {
        for (ifleet = 1; ifleet <= nfleets; ifleet++)
            report << "__Fmult_devs_fleet_" << ifleet << "          " << lambda_Fmult_devs(ifleet) << "          " << lambda_Fmult_devs(ifleet) * Fmult_devs_likely(ifleet) << endl;
    }
    report << "Fmult_devs_fleet_Total        " << sum(lambda_Fmult_devs) << "          " << lambda_Fmult_devs * Fmult_devs_likely << endl;
    report << "N_year_1                      " << lambda_N_year1_devs << "          " << lambda_N_year1_devs * N_year1_likely << endl;
    report << "Recruit_devs                  " << lambda_recruit_devs << "          " << lambda_recruit_devs * likely_SR_sigma << endl;
    report << "SR_steepness                  " << lambda_steepness << "          " << lambda_steepness * steepness_likely << endl;
    report << "SR_scaler                     " << lambda_SR_scaler << "          " << lambda_SR_scaler * SR_scaler_likely << endl;
    report << "Fmult_Max_penalty          1000          " << Fmult_max_pen << endl;
    report << "F_penalty                     " << fpenalty_lambda << "          " << fpenalty << endl;
    report << endl;
    for (ifleet = 1; ifleet <= nfleets; ifleet++) {
        for (iyear = 1; iyear <= nyears; iyear++) {
            if (input_eff_samp_size_catch(ifleet, iyear) == 0) {
                effective_sample_size(ifleet, iyear) = 0;
            } else {
                effective_sample_size(ifleet, iyear) = CAA_prop_pred(ifleet, iyear)*(1.0 - CAA_prop_pred(ifleet, iyear)) / norm2(CAA_prop_obs(ifleet, iyear) - CAA_prop_pred(ifleet, iyear));
            }
            if (input_eff_samp_size_discard(ifleet, iyear) == 0) {
                effective_Discard_sample_size(ifleet, iyear) = 0;
            } else {
                effective_Discard_sample_size(ifleet, iyear) = Discard_prop_pred(ifleet, iyear)*(1.0 - Discard_prop_pred(ifleet, iyear)) / norm2(Discard_prop_obs(ifleet, iyear) - Discard_prop_pred(ifleet, iyear));
            }
        }
    }
    for (ifleet = 1; ifleet <= nfleets; ifleet++) {
        report << " Input and Estimated effective sample sizes for fleet " << ifleet << endl;
        for (iyear = 1; iyear <= nyears; iyear++)
            report << iyear + year1 - 1 << "  " << input_eff_samp_size_catch(ifleet, iyear) << "  " << effective_sample_size(ifleet, iyear) << endl;
        report << " Total  " << sum(input_eff_samp_size_catch(ifleet)) << "  " << sum(effective_sample_size(ifleet)) << endl;
    }
    report << endl;
    for (ifleet = 1; ifleet <= nfleets; ifleet++) {
        report << " Input and Estimated effective Discard sample sizes for fleet " << ifleet << endl;
        for (iyear = 1; iyear <= nyears; iyear++)
            report << iyear + year1 - 1 << "  " << input_eff_samp_size_discard(ifleet, iyear) << "  " << effective_Discard_sample_size(ifleet, iyear) << endl;
        report << " Total  " << sum(input_eff_samp_size_discard(ifleet)) << "  " << sum(effective_Discard_sample_size(ifleet)) << endl;
    }
    report << endl;
    report << "Observed and predicted total fleet catch by year and standardized residual" << endl;
    for (ifleet = 1; ifleet <= nfleets; ifleet++) {
        report << " fleet " << ifleet << " total catches" << endl;
        for (iyear = 1; iyear <= nyears; iyear++) {
            Catch_stdresid(ifleet, iyear) = (log(Catch_tot_fleet_obs(ifleet, iyear) + 0.00001) - log(Catch_tot_fleet_pred(ifleet, iyear) + 0.00001)) / catch_tot_sigma(ifleet, iyear);
            report << iyear + year1 - 1 << "  " << Catch_tot_fleet_obs(ifleet, iyear) << "  " << Catch_tot_fleet_pred(ifleet, iyear) << "  " << Catch_stdresid(ifleet, iyear) << endl;
        }
    }
    report << "Observed and predicted total fleet Discards by year and standardized residual" << endl;
    for (ifleet = 1; ifleet <= nfleets; ifleet++) {
        report << " fleet " << ifleet << " total Discards" << endl;
        for (iyear = 1; iyear <= nyears; iyear++) {
            Discard_stdresid(ifleet, iyear) = (log(Discard_tot_fleet_obs(ifleet, iyear) + 0.00001) - log(Discard_tot_fleet_pred(ifleet, iyear) + 0.00001)) / discard_tot_sigma(ifleet, iyear);
            report << iyear + year1 - 1 << "  " << Discard_tot_fleet_obs(ifleet, iyear) << "  " << Discard_tot_fleet_pred(ifleet, iyear) << "  " << Discard_stdresid(ifleet, iyear) << endl;
        }
    }
    report << endl << "Index data" << endl;
    for (ind = 1; ind <= nindices; ind++) {
        report << "index number " << ind << endl;
        report << "aggregate units = " << index_units_aggregate(ind) << endl;
        report << "proportions units = " << index_units_proportions(ind) << endl;
        report << "month = " << index_month(ind) << endl;
        report << "starting and ending ages for selectivity = " << index_start_age(ind) << "  " << index_end_age(ind) << endl;
        report << "selectivity choice = " << index_sel_choice(ind) << endl;
        report << " year, obs index, pred index, standardized residual" << endl;
        for (j = 1; j <= index_nobs(ind); j++)
            report << index_year(ind, j) << "  " << index_obs(ind, j) << "  " << index_pred(ind, j) << "  " << index_stdresid(ind, j) << endl;
    }
    report << endl;
    index_Neff_init = 0.0;
    index_Neff_est = 0.0;
    for (ind = 1; ind <= nindices; ind++) {
        for (iyear = 1; iyear <= nyears; iyear++) {
            for (i = 1; i <= index_nobs(ind); i++) {
                if (index_time(ind, i) == iyear) {
                    index_Neff_init(ind, iyear) = input_eff_samp_size_index(ind, i);
                    if (input_eff_samp_size_index(ind, i) == 0) {
                        index_Neff_est(ind, iyear) = 0.0;
                    } else {
                        index_Neff_est(ind, iyear) = index_prop_pred(ind, i)*(1.0 - index_prop_pred(ind, i)) / norm2(index_prop_obs(ind, i) - index_prop_pred(ind, i));
                    }
                }
            }
        }
    }
    report << "Input effective sample sizes by index (row=index, column=year)" << endl;
    report << index_Neff_init << endl;
    report << "Estimated effective sample sizes by index (row=index, column=year)" << endl;
    report << index_Neff_est << endl;
    report << endl;
    report << "Index proportions at age by index" << endl;
    for (ind = 1; ind <= nindices; ind++) {
        output_index_prop_obs(ind) = 0.0;
        output_index_prop_pred(ind) = 0.0;
        if (index_estimate_proportions(ind) == 1) {
            report << " Index number " << ind << endl;
            for (iyear = 1; iyear <= nyears; iyear++) {
                for (i = 1; i <= index_nobs(ind); i++) {
                    if (index_time(ind, i) == iyear) {
                        for (iage = index_start_age(ind); iage <= index_end_age(ind); iage++) {
                            output_index_prop_obs(ind, iyear, iage) = index_prop_obs(ind, i, iage);
                            output_index_prop_pred(ind, iyear, iage) = index_prop_pred(ind, i, iage);
                        }
                    }
                }
                report << "Year " << iyear + year1 - 1 << " Obs  = " << output_index_prop_obs(ind, iyear) << endl;
                report << "Year " << iyear + year1 - 1 << " Pred = " << output_index_prop_pred(ind, iyear) << endl;
            }
        }
    }
    report << endl;
    report << "Index Selectivity at Age" << endl;
    report << indexsel << endl;
    report << endl;

    report << "Deviations section: only applicable if associated lambda > 0" << endl;
    report << "Nyear1 observed, expected, standardized residual" << endl;
    if (lambda_N_year1_devs > 0.0) {
        for (iage = 2; iage <= nages; iage++) {
            report << iage << "  " << NAA(1, iage) << "  " << nyear1temp(iage) << "  " << N_year1_stdresid(iage) << endl;
        }
    } else {
        report << "N/A" << endl;
    }
    report << endl;
    report << "Fleet Obs, Initial, and Stadardized Residual for Fmult" << endl;
    if (sum(lambda_Fmult_year1) > 0.0) {
        for (ifleet = 1; ifleet <= nfleets; ifleet++)
            report << ifleet << "  " << mfexp(log_Fmult_year1(ifleet)) << "  " << Fmult_year1_ini(ifleet) << "  " << Fmult_year1_stdresid(ifleet) << endl;
    } else {
        report << "N/A" << endl;
    }
    report << endl;
    report << "Standardized Residuals for Fmult_devs by fleet and year" << endl;
    if (sum(lambda_Fmult_devs) > 0.0) {
        for (ifleet = 1; ifleet <= nfleets; ifleet++) {
            report << " fleet " << ifleet << " Fmult_devs standardized residuals" << endl;
            for (iyear = 2; iyear <= nyears; iyear++)
                report << iyear << "  " << Fmult_devs_stdresid(ifleet, iyear) << endl;
        }
    } else {
        report << "N/A" << endl;
    }
    report << endl;
    report << "Index Obs, Initial, and Standardized Residual for q_year1" << endl;
    if (sum(lambda_q_year1) > 0.0) {
        for (ind = 1; ind <= nindices; ind++)
            report << ind << "  " << mfexp(log_q_year1(ind)) << "  " << q_year1_ini(ind) << "  " <<
            (log_q_year1(ind) - log(q_year1_ini(ind))) / q_year1_sigma(ind) << endl;
    } else {
        report << "N/A" << endl;
    }
    report << endl;
    report << "Standardized Residuals for catchability deviations by index and year" << endl;
    if (sum(lambda_q_devs) > 0.0) {
        for (ind = 1; ind <= nindices; ind++) {
            report << " index " << ind << " q_devs standardized residuals" << endl;
            for (i = 2; i <= index_nobs(ind); i++)
                report << index_year(ind, i) << "  " << log_q_devs(ind, i) / q_devs_sigma(ind) << endl;
        }
    } else {
        report << "N/A" << endl;
    }
    report << endl;
    report << "Obs, Initial, and Stadardized Residual for SR steepness" << endl;
    if (lambda_steepness > 0.0) {
        report << SR_steepness << "  " << SR_steepness_ini << "  " << (log(SR_steepness) - log(SR_steepness_ini)) / steepness_sigma << endl;
    } else {
        report << "N/A" << endl;
    }
    report << endl;
    report << "Obs, Initial, and Stadardized Residual for SR scaler" << endl;
    if (lambda_SR_scaler > 0.0) {
        report << mfexp(log_SR_scaler) << "  " << SR_scaler_ini << "  " << (log_SR_scaler - log(SR_scaler_ini)) / SR_scaler_sigma << endl;
    } else {
        report << "N/A" << endl;
    }
    report << endl;
    report << "End of Deviations Section" << endl << endl;

    report << "Selectivity by age and year for each fleet" << endl;
    for (ifleet = 1; ifleet <= nfleets; ifleet++) {
        report << " fleet " << ifleet << " selectivity at age" << endl;
        for (iyear = 1; iyear <= nyears; iyear++)
            report << sel_by_fleet(ifleet, iyear) << endl;
    }
    report << endl;
    report << "Fmult by year for each fleet" << endl;
    Fmult = mfexp(log_Fmult);
    for (iyear = 1; iyear <= nyears; iyear++) {
        for (ifleet = 1; ifleet <= nfleets; ifleet++) {
            temp_Fmult(ifleet) = Fmult(ifleet, iyear);
        }
        report << iyear + year1 - 1 << "  " << temp_Fmult << endl;
    }
    report << endl;
    report << "Directed F by age and year for each fleet" << endl;
    for (ifleet = 1; ifleet <= nfleets; ifleet++) {
        report << " fleet " << ifleet << " directed F at age" << endl;
        for (iyear = 1; iyear <= nyears; iyear++)
            report << FAA_by_fleet_dir(ifleet, iyear) << endl;
    }
    report << "Discard F by age and year for each fleet" << endl;
    for (ifleet = 1; ifleet <= nfleets; ifleet++) {
        report << " fleet " << ifleet << " Discard F at age" << endl;
        for (iyear = 1; iyear <= nyears; iyear++)
            report << FAA_by_fleet_Discard(ifleet, iyear) << endl;
    }
    report << "Total F" << endl;
    for (iyear = 1; iyear <= nyears; iyear++)
        report << FAA_tot(iyear) << endl;
    report << endl;
    report << "Average F for ages " << Freport_agemin << " to " << Freport_agemax << endl;
    if (Freport_wtopt == 1) report << "Freport unweighted in .std and MCMC files" << endl;
    if (Freport_wtopt == 2) report << "Freport N weighted in .std and MCMC files" << endl;
    if (Freport_wtopt == 3) report << "Freport B weighted in .std and MCMC files" << endl;
    report << "year    unweighted   Nweighted    Bweighted" << endl;
    for (iyear = 1; iyear <= nyears; iyear++) {
        report << iyear + year1 - 1 << "  " << Freport_U(iyear) << "  " << Freport_N(iyear) << "  " << Freport_B(iyear) << endl;
    }
    report << endl;
    report << "Population Numbers at the Start of the Year" << endl;
    for (iyear = 1; iyear <= nyears; iyear++)
        report << NAA(iyear) << endl;
    report << endl;
    report << "Biomass Time Series" << endl;
    report << "Year, TotJan1B, SSB, ExploitableB" << endl;
    for (iyear = 1; iyear <= nyears; iyear++) {
        report << iyear + year1 - 1 << "  " << TotJan1B(iyear) << "  " << SSB(iyear) << "  " << ExploitableB(iyear) << endl;
    }
    report << endl;
    report << "q by index" << endl;
    for (ind = 1; ind <= nindices; ind++) {
        report << " index " << ind << " q over time" << endl;
        for (i = 1; i <= index_nobs(ind); i++) {
            report << index_year(ind, i) << "  " << q_by_index(ind, i) << endl;
        }
    }
    report << endl;
    report << "Proportions of catch at age by fleet" << endl;
    for (ifleet = 1; ifleet <= nfleets; ifleet++) {
        report << " fleet " << ifleet << endl;
        for (iyear = 1; iyear <= nyears; iyear++) {
            output_prop_obs = 0.0;
            output_prop_pred = 0.0;
            output_prop_obs(sel_start_age(ifleet), sel_end_age(ifleet)) = CAA_prop_obs(ifleet, iyear);
            output_prop_pred(sel_start_age(ifleet), sel_end_age(ifleet)) = CAA_prop_pred(ifleet, iyear);
            report << "Year " << iyear << " Obs  = " << output_prop_obs << endl;
            report << "Year " << iyear << " Pred = " << output_prop_pred << endl;
        }
    }
    report << endl;
    report << "Proportions of Discards at age by fleet" << endl;
    for (ifleet = 1; ifleet <= nfleets; ifleet++) {
        report << " fleet " << ifleet << endl;
        for (iyear = 1; iyear <= nyears; iyear++) {
            output_Discard_prop_obs = 0.0;
            output_Discard_prop_pred = 0.0;
            output_Discard_prop_obs(sel_start_age(ifleet), sel_end_age(ifleet)) = Discard_prop_obs(ifleet, iyear);
            output_Discard_prop_pred(sel_start_age(ifleet), sel_end_age(ifleet)) = Discard_prop_pred(ifleet, iyear);
            report << "Year " << iyear << " Obs  = " << output_Discard_prop_obs << endl;
            report << "Year " << iyear << " Pred = " << output_Discard_prop_pred << endl;
        }
    }
    report << endl;
    report << "F Reference Points Using Final Year Selectivity and Freport options" << endl;
    report << " refpt           F       slope to plot on SR" << endl;
    report << "  F0.1     " << F01_report << "     " << F01_slope << endl;
    report << "  Fmax     " << Fmax_report << "     " << Fmax_slope << endl;
    report << "  F30%SPR  " << F30SPR_report << "     " << F30SPR_slope << endl;
    report << "  F40%SPR  " << F40SPR_report << "     " << F40SPR_slope << endl;
    report << "  Fmsy     " << Fmsy_report << "     " << Fmsy_slope << "    SSBmsy    " << SSBmsy_report << "     MSY   " << MSY << endl;
    report << "  Fcurrent " << Freport(nyears) << "     " << Fcurrent_slope << endl;
    report << endl;
    report << "Stock-Recruitment Relationship Parameters" << endl;
    report << " alpha     = " << SR_alpha << endl;
    report << " beta      = " << SR_beta << endl;
    report << " R0        = " << SR_R0 << endl;
    report << " S0        = " << SR_S0 << endl;
    report << " steepness = " << SR_steepness << endl;
    report << "Spawning Stock, Obs Recruits(year+1), Pred Recruits(year+1), standardized residual" << endl;
    report << "init  xxxx  " << recruits(1) << "  " << SR_pred_recruits(1) << "  " <<
            (log(recruits(1)) - log(SR_pred_recruits(1))) / recruit_sigma(1) << endl;
    for (iyear = 1; iyear < nyears; iyear++)
        report << iyear + year1 - 1 << "  " << SSB(iyear) << "  " << recruits(iyear + 1) << "  " << SR_pred_recruits(iyear + 1) << "  " <<
        (log(recruits(iyear + 1)) - log(SR_pred_recruits(iyear + 1))) / recruit_sigma(iyear + 1) << endl;
    report << nyears + year1 - 1 << "  " << SSB(nyears) << "       xxxx   " << SR_pred_recruits(nyears + 1) << endl;
    report << endl;

    report << "Annual stock recruitment parameters" << endl;
    report << "Year, S0_vec, R0_vec, steepness_vec, s_per_r_vec" << endl;
    for (iyear = 1; iyear <= nyears; iyear++)
        report << iyear + year1 - 1 << "  " << S0_vec(iyear) << "  " << R0_vec(iyear) << "  " << steepness_vec(iyear) << "  " << s_per_r_vec(iyear) << endl;
    report << endl;
    report << "Root Mean Square Error computed from Standardized Residuals" << endl;
    report << "Component                 #resids         RMSE" << endl;
    for (ifleet = 1; ifleet <= nfleets; ifleet++) {
        report << "_Catch_Fleet_" << ifleet << "               " << nyears << "           " << sqrt(mean(square(Catch_stdresid(ifleet)))) << endl;
    }
    report << "Catch_Fleet_Total            " << nyears * nfleets << "           " << sqrt(mean(square(Catch_stdresid))) << endl;
    for (ifleet = 1; ifleet <= nfleets; ifleet++) {
        if (norm2(Discard_stdresid(ifleet)) > 0.0) {
            report << "_Discard_Fleet_" << ifleet << "             " << nyears << "           " << sqrt(mean(square(Discard_stdresid(ifleet)))) << endl;
        } else {
            report << "_Discard_Fleet_" << ifleet << "             " << "0" << "            " << "0" << endl;
        }
    }
    if (norm2(Discard_stdresid) > 0.0) {
        report << "Discard_Fleet_Total          " << nyears * nfleets << "           " << sqrt(mean(square(Discard_stdresid))) << endl;
    } else {
        report << "Discard_Fleet_Total          " << "0" << "            " << "0" << endl;
    }
    for (ind = 1; ind <= nindices; ind++) {
        report << "_Index_" << ind << "                     " << index_nobs(ind) << "           " << sqrt(mean(square(index_stdresid(ind)))) << endl;
    }
    report << "Index_Total                  " << sum(index_nobs) << "           " << sqrt(mean(square(index_stdresid))) << endl;
    N_year1_rmse = 0.0;
    N_year1_rmse_nobs = 0;
    if (lambda_N_year1_devs > 0.0 && norm2(N_year1_stdresid) > 0.0) {
        N_year1_rmse = sqrt(mean(square(N_year1_stdresid)));
        N_year1_rmse_nobs = nages - 1;
    }
    report << "Nyear1                       " << N_year1_rmse_nobs << "           " << N_year1_rmse << endl;
    Fmult_year1_rmse = 0.0;
    Fmult_year1_rmse_nobs = 0;
    if (sum(lambda_Fmult_year1) > 0.0 && norm2(Fmult_year1_stdresid) > 0.0) {
        Fmult_year1_rmse = sqrt(mean(square(Fmult_year1_stdresid)));
        Fmult_year1_rmse_nobs = nfleets;
    }
    report << "Fmult_Year1                  " << Fmult_year1_rmse_nobs << "           " << Fmult_year1_rmse << endl;
    Fmult_devs_fleet_rmse = 0.0;
    Fmult_devs_fleet_rmse_nobs = 0;
    Fmult_devs_rmse = 0.0;
    Fmult_devs_rmse_nobs = 0;
    for (ifleet = 1; ifleet <= nfleets; ifleet++) {
        if (sum(lambda_Fmult_devs) > 0.0 && norm2(Fmult_devs_stdresid(ifleet)) > 0.0) {
            Fmult_devs_fleet_rmse(ifleet) = sqrt(mean(square(Fmult_devs_stdresid(ifleet))));
            Fmult_devs_fleet_rmse_nobs(ifleet) = nyears - 1;
        }
        report << "_Fmult_devs_Fleet_" << ifleet << "          " << Fmult_devs_fleet_rmse_nobs(ifleet) << "           " << Fmult_devs_fleet_rmse(ifleet) << endl;
    }
    if (sum(lambda_Fmult_devs) > 0.0 && norm2(Fmult_devs_stdresid) > 0.0) {
        Fmult_devs_rmse = sqrt(mean(square(Fmult_devs_stdresid)));
        Fmult_devs_rmse_nobs = nfleets * (nyears - 1);
    }
    report << "Fmult_devs_Total             " << Fmult_devs_rmse_nobs << "           " << Fmult_devs_rmse << endl;
    SR_rmse = 0.0;
    SR_rmse_nobs = 0;
    if (lambda_recruit_devs > 0.0 && norm2(SR_stdresid) > 0.0) {
        SR_rmse = sqrt(mean(square(SR_stdresid)));
        SR_rmse_nobs = nyears;
    }
    report << "Recruit_devs                 " << SR_rmse_nobs << "           " << SR_rmse << endl;
    sel_rmse = 0.0;
    sel_rmse_nobs = 0;
    if (sum(sel_lambda) > 0.0 && norm2(sel_stdresid) > 0.0) {
        sel_rmse = sqrt(mean(square(sel_stdresid)));
        for (k = 1; k <= nselparm; k++) {
            if (sel_lambda(k) > 0.0)
                sel_rmse_nobs += 1;
        }
    }
    report << "Fleet_Sel_params             " << sel_rmse_nobs << "           " << sel_rmse << endl;
    indexsel_rmse = 0.0;
    indexsel_rmse_nobs = 0;
    if (sum(indexsel_lambda) > 0.0 && norm2(indexsel_stdresid) > 0.0) {
        indexsel_rmse = sqrt(mean(square(indexsel_stdresid)));
        for (k = 1; k <= nindexselparms; k++) {
            if (indexsel_lambda(k) > 0.0)
                indexsel_rmse_nobs += 1;
        }
    }
    report << "Index_Sel_params             " << indexsel_rmse_nobs << "           " << indexsel_rmse << endl;
    q_year1_rmse = 0.0;
    q_year1_rmse_nobs = 0;
    if (sum(lambda_q_year1) > 0.0 && norm2(q_year1_stdresid) > 0.0) {
        q_year1_rmse = sqrt(mean(square(q_year1_stdresid)));
        for (ind = 1; ind <= nindices; ind++) {
            if (lambda_q_year1(ind) > 0.0)
                q_year1_rmse_nobs += 1;
        }
    }
    report << "q_year1                      " << q_year1_rmse_nobs << "           " << q_year1_rmse << endl;
    q_devs_rmse = 0.0;
    q_devs_rmse_nobs = 0;
    if (sum(lambda_q_devs) > 0.0 && norm2(q_devs_stdresid) > 0.0) {
        q_devs_rmse = sqrt(mean(square(q_devs_stdresid)));
        for (ind = 1; ind <= nindices; ind++) {
            if (lambda_q_year1(ind) > 0.0)
                q_devs_rmse_nobs += index_nobs(ind) - 1;
        }
    }
    report << "q_devs                       " << q_devs_rmse_nobs << "           " << q_devs_rmse << endl;
    steepness_rmse = 0.0;
    steepness_rmse_nobs = 0;
    if (lambda_steepness > 0.0) {
        steepness_rmse = sfabs(steepness_stdresid);
        steepness_rmse_nobs = 1;
    }
    report << "SR_steepness                 " << steepness_rmse_nobs << "           " << steepness_rmse << endl;
    SR_scaler_rmse = 0.0;
    SR_scaler_rmse_nobs = 0;
    if (lambda_SR_scaler > 0.0) {
        SR_scaler_rmse = sfabs(SR_scaler_stdresid);
        SR_scaler_rmse_nobs = 1;
    }
    report << "SR_scaler                    " << SR_scaler_rmse_nobs << "           " << SR_scaler_rmse << endl;
    report << endl;

    report << "Stage2 Multipliers for Multinomials (Francis 2011)" << endl;
    report << "Catch by Fleet" << endl;
    report << Neff_stage2_mult_catch << endl;
    report << "Discards by Fleet" << endl;
    report << Neff_stage2_mult_discard << endl;
    report << "Indices" << endl;
    report << Neff_stage2_mult_index << endl;
    report << endl;
    report << "New Input ESS based on applying stage2 multipliers" << endl;
    report << "Catch (rows are fleets, columns are years)" << endl;
    for (ifleet = 1; ifleet <= nfleets; ifleet++) {
        report << input_eff_samp_size_catch(ifleet) * Neff_stage2_mult_catch(ifleet) << endl;
    }
    report << "Discards (rows are fleets, columns are years)" << endl;
    for (ifleet = 1; ifleet <= nfleets; ifleet++) {
        report << input_eff_samp_size_discard(ifleet) * Neff_stage2_mult_discard(ifleet) << endl;
    }
    report << "Indices (rows are indices, columns are years)" << endl;
    for (ind = 1; ind <= nindices; ind++) {
        report << index_Neff_init(ind) * Neff_stage2_mult_index(ind) << endl;
    }
    report << endl;

    if (do_projections == 1 && last_phase()) {
        project_into_future();
        report << "Projection into Future" << endl;
        report << "Projected NAA" << endl;
        report << proj_NAA << endl;
        report << "Projected Directed FAA" << endl;
        report << proj_F_dir << endl;
        report << "Projected Discard FAA" << endl;
        report << proj_F_Discard << endl;
        report << "Projected Nondirected FAA" << endl;
        report << proj_F_nondir << endl;
        report << "Projected Catch at Age" << endl;
        report << proj_catch << endl;
        report << "Projected Discards at Age (in numbers)" << endl;
        report << proj_Discard << endl;
        report << "Projected Yield at Age" << endl;
        report << proj_yield << endl;
        report << "Year, Total Yield (in weight), Total Discards (in weight), TotJan1B, SSB, proj_what, SS/SSmsy" << endl;
        for (iyear = 1; iyear <= nprojyears; iyear++)
            report << year1 + nyears - 1 + iyear << "  " << proj_total_yield(iyear) << "  " << proj_total_Discard(iyear) << "  " << proj_TotJan1B(iyear) << "  " << proj_SSB(iyear) << "  " << proj_what(iyear) << "  " << proj_SSB(iyear) / SSmsy << endl;
        report << endl;
    } else {
        report << "Projections not requested" << endl;
        report << endl;
    }
    report << "that's all" << endl;

    if (make_Rfile == 1 && last_phase()) {
        //    #include "make-Rfile_asap3.cxx"  // ADMB2R code in this file
    }
}

void model_parameters::set_runtime(void) {
    dvector temp("{1.0e-4}");
    convergence_criteria.allocate(temp.indexmin(), temp.indexmax());
    convergence_criteria = temp;
    dvector temp1("{1000,1600,10000}");
    maximum_function_evaluations.allocate(temp1.indexmin(), temp1.indexmax());
    maximum_function_evaluations = temp1;
}

void model_parameters::final_calcs() {
    //Calculates how long is taking to run
    // this code is based on the Widow Rockfish model (from Erik H. Williams, NMFS-Santa Cruz, now Beaufort)  
    time(&finish);
    elapsed_time = difftime(finish, start);
    hour = long(elapsed_time) / 3600;
    minute = long(elapsed_time) % 3600 / 60;
    second = (long(elapsed_time) % 3600) % 60;
    cout << endl << endl << "starting time: " << ctime(&start);
    cout << "finishing time: " << ctime(&finish);
    cout << "This run took: ";
    cout << hour << " hours, " << minute << " minutes, " << second << " seconds." << endl << endl << endl;
}

model_data::~model_data() {
}

model_parameters::~model_parameters() {
}



#ifdef _BORLANDC_
extern unsigned _stklen = 10000U;
#endif


#ifdef __ZTC__
extern unsigned int _stack = 10000U;
#endif

long int arrmblsize = 0;

int main(int argc, char * argv[]) {
    ad_set_new_handler();
    ad_exit = &ad_boundf;
    arrmblsize = 5000000;
    gradient_structure::set_GRADSTACK_BUFFER_SIZE(10000000);
    gradient_structure::set_MAX_NVAR_OFFSET(50000);
    gradient_structure::set_NUM_DEPENDENT_VARIABLES(10000);
    time(&start); //this is to see how long it takes to run
    cout << endl << "Start time : " << ctime(&start) << endl;
    gradient_structure::set_NO_DERIVATIVES();
#ifdef DEBUG
#ifndef __SUNPRO_C
    std::feclearexcept(FE_ALL_EXCEPT);
#endif
#endif
    gradient_structure::set_YES_SAVE_VARIABLES_VALUES();
    if (!arrmblsize) arrmblsize = 15000000;
    model_parameters mp(arrmblsize, argc, argv);
    mp.iprint = 10;
    mp.preliminary_calculations();
    mp.create_mas_data_file("asap2mas_data.json");
    mp.create_mas_config_file("asap2mas_config.json");
//    mp.computations(argc, argv);
//    mp.write_MCMC();
#ifdef DEBUG
#ifndef __SUNPRO_C
    bool failedtest = false;
    if (std::fetestexcept(FE_DIVBYZERO)) {
        cerr << "Error: Detected division by zero." << endl;
        failedtest = true;
    }
    if (std::fetestexcept(FE_INVALID)) {
        cerr << "Error: Detected invalid argument." << endl;
        failedtest = true;
    }
    if (std::fetestexcept(FE_OVERFLOW)) {
        cerr << "Error: Detected overflow." << endl;
        failedtest = true;
    }
    if (std::fetestexcept(FE_UNDERFLOW)) {
        cerr << "Error: Detected underflow." << endl;
    }
    if (failedtest) {
        std::abort();
    }
#endif
#endif
    return 0;
}

extern "C" {

    void ad_boundf(int i) {
        /* so we can stop here */
        exit(i);
    }
}
