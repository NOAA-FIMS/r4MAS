/*
 * File:   HarvestControlRule.hpp
 *
 * Author: ZTA
 * National Oceanic and Atmospheric Administration
 * National Marine Fisheries Service
 * Office of Science and Technology
 * Silver Spring, MD
 *
 * Created on August 3, 2017
 *
 * This File is part of the NOAA, National Marine Fisheries Service
 * Metapopulation Assessment System project.
 *
 * This software is a "United States Government Work" under the terms of the
 * United States Copyright Act.  It was written as part of the author's official
 * duties as a United States Government employee and thus cannot be copyrighted.
 * This software is freely available to the public for use. The National Oceanic
 * And Atmospheric Administration and the U.S. Government have not placed any
 * restriction on its use or reproduction.  Although all reasonable efforts have
 * been taken to ensure the accuracy and reliability of the software and data,
 * the National Oceanic And Atmospheric Administration and the U.S. Government
 * do not and cannot warrant the performance or results that may be obtained by
 * using this  software or data. The National Oceanic And Atmospheric
 * Administration and the U.S. Government disclaim all warranties, express or
 * implied, including warranties of performance, merchantability or fitness
 * for any particular purpose.
 *
 * Please cite the author(s) in any work or product based on this material.
 *
 */

#ifndef MAS_HARVEST_CONTROL_RULE_HPP
#define MAS_HARVEST_CONTROL_RULE_HPP

#include "Common.hpp"
#include "Fleet.hpp"
// given the properties of the AreaPopulationInfo object, calculate the associated biological reference points
// (BRPs) for the specified harvest control rule (HCR), for this population and area for the specified year
// NOTE:  currently these calculations assume NO MOVEMENT, ONE season per year, and ONE fishery per area

namespace mas
{
    // forward declaration
    template<class REAL_T>
    struct Subpopulation;


    template<typename REAL_T>
    struct HCRBase : mas::ModelObject<REAL_T>
    {
        typedef typename VariableTrait<REAL_T>::variable variable;

        virtual const std::tuple<variable, variable, variable, variable> Evaluate(const int& year, const mas::Subpopulation<REAL_T>& api_males, const mas::Subpopulation<REAL_T>& api_females) = 0;

        virtual const std::string Name()
        {
            return "HCRBase";
        }

    };


    // AFSC - Alaska HCR
    template<typename REAL_T>
    struct NPFMC_Tier3_HCR : HCRBase<REAL_T>
    {
        typedef typename VariableTrait<REAL_T>::variable variable;

        std::pair<variable, variable> BRP100, BRP40, BRP35, BRP20, BRP05;
        variable F_ABC, ABC, F_OFL, OFL;


        // find the fishing mortality F such that the ratio of spawning biomass at F to the unfished
        // equilibrium spawning biomass is spr_fraction
        inline std::pair<variable, variable> CalculateFatSPRfraction(const variable& spr_fraction, const std::vector<variable>& fsh_sel, const mas::Subpopulation<REAL_T>& api)
        {
            variable Fval  = 0.0;
            variable SBval = 0.0;

            variable df = static_cast<REAL_T> (1.0e-3);
            variable Flow, Fhigh, Yrate, Ylow, Yhigh, deltaY, deltaYprime;

            variable SB0 = CalculateSBatF(0.0, fsh_sel, api);

            Fval = 0.2; // initial value

            // Newton-Raphson
            for (int i = 0; i < 10 && Fval > 0.0; ++i)
            {
                Fhigh = Fval + df;
                Flow  = Fval - df;

                Yrate = static_cast<REAL_T> (-1000.0) * mas::pow(mas::log( spr_fraction / (CalculateSBatF(Fval,  fsh_sel, api) / SB0) ), 2.0);
                Yhigh = static_cast<REAL_T> (-1000.0) * mas::pow(mas::log( spr_fraction / (CalculateSBatF(Fhigh, fsh_sel, api) / SB0) ), 2.0);
                Ylow  = static_cast<REAL_T> (-1000.0) * mas::pow(mas::log( spr_fraction / (CalculateSBatF(Flow,  fsh_sel, api) / SB0) ), 2.0);

                deltaY       = (Yhigh - Ylow) / (static_cast<REAL_T> (2.0) * df);                 // First derivative (to find the root of this)
                deltaYprime  = (Yhigh + Ylow - (static_cast<REAL_T> (2.0) * Yrate)) / (df * df);  // Newton-Raphson approximation 2nd deriv
                Fval       -= (deltaY / deltaYprime);
            }

            if (Fval < 0.0)
            {
                std::cout << "Calculation Warning: F is less than 0 in HCR::CalculateFatSPRfraction for SPR fraction " << spr_fraction << "; setting F to 0\n";
                mas::mas_log << "Calculation Warning: F is less than 0 in HCR::CalculateFatSPRfractionfor SPR fraction " << spr_fraction << "; setting F to 0\n";

                Fval = 0.0;
            }

            SBval = CalculateSBatF(Fval, fsh_sel, api);

            return std::make_pair(Fval, SBval);
        }

        // calculate the equilibrium female spawning biomass at a specific fishing mortality
        inline variable CalculateSBatF(const variable& trialF, const std::vector<variable>& fsh_sel, const mas::Subpopulation<REAL_T>& api)
        {
            variable SBatF = 0.0;

            auto Nages = api.ages.size();

            std::vector<variable> Ztmp(Nages);
            std::vector<variable> Ntmp(Nages);

            for (int a = 0; a < Nages; ++a)
            {
                Ztmp[a] = (fsh_sel[a] * trialF) + api.M[a];
            }

            // R0 could be changed to the average recruitment over a specific period
            Ntmp[0] = static_cast<REAL_T> (0.5) * api.recruitment_model->R0;
            SBatF += (Ntmp[0] * api.maturity[0] * api.weight_at_spawning[0] * mas::exp(static_cast<REAL_T> (-1.0) * api.spawning_season_offset * Ztmp[0]));

            for (int a = 1; a < (Nages-1); ++a)
            {
                Ntmp[a] = Ntmp[a-1] * mas::exp(static_cast<REAL_T> (-1.0) * Ztmp[a-1]);
                SBatF  += (Ntmp[a] * api.maturity[a] * api.weight_at_spawning[a] * mas::exp(static_cast<REAL_T> (-1.0) * api.spawning_season_offset * Ztmp[a]));
            }
            Ntmp[Nages-1] = Ntmp[Nages-2] * mas::exp(static_cast<REAL_T> (-1.0) * Ztmp[Nages-2]) /
                            (static_cast<REAL_T> (1.0) - mas::exp(static_cast<REAL_T> (-1.0) * Ztmp[Nages-1]));
            SBatF += (Ntmp[Nages-1] * api.maturity[Nages-1] * api.weight_at_spawning[Nages-1] * mas::exp(static_cast<REAL_T> (-1.0) * api.spawning_season_offset * Ztmp[Nages-1]));

            return (SBatF / api.kg_to_tonnes);
        }

        // this function calculates ABC and OFL for year 'year' using the numbers-at-age at the beginning of the year
        const std::tuple<variable, variable, variable, variable> Evaluate(const int& year, const mas::Subpopulation<REAL_T>& api_males, const mas::Subpopulation<REAL_T>& api_females)
        {
            const REAL_T Tier3_alpha = static_cast<REAL_T> (0.05);

            const int Nages = api_females.ages.size();

            // initialize
            F_ABC = 0.0;
            ABC   = 0.0;
            F_OFL = 0.0;
            OFL   = 0.0;

            // NOTE:  season is HARD-CODED to 1
            const int season = 1;
            std::vector< atl::intrusive_ptr<Fleet<REAL_T> > >& fleets_f = api_females.area->seasonal_fleet_operations[season];
            std::vector< atl::intrusive_ptr<Fleet<REAL_T> > >& fleets_m = api_males.area->seasonal_fleet_operations[season];

            if (fleets_f.size() > 0)
            {
                std::vector<variable> fsh_sel_f(Nages);
                std::vector<variable> fsh_sel_m(Nages);

                variable Zf_ABC, expZf_ABC, Zm_ABC, expZm_ABC, Zf_OFL, expZf_OFL, Zm_OFL, expZm_OFL;

                variable sp_biomass;

                // NOTE:  fleet number is HARD-CODED to 0
                const int fleet = 0;
                for (int a = 0; a < Nages; ++a)
                {
                    fsh_sel_f[a] = fleets_f[fleet]->season_area_selectivity[season][api_females.area->id]->Evaluate(api_females.ages[a]);
                    fsh_sel_m[a] = fleets_m[fleet]->season_area_selectivity[season][api_males.area->id]->Evaluate(api_males.ages[a]);
                }

                // F100 (BRP100.first) had better be close to 0 and
                // SB100 (BRP100.second) had better be close to api.SB0
                BRP100 = CalculateFatSPRfraction(1.00, fsh_sel_f, api_females);
                // std::cout << "SB0 " << api_females.SB0 << std::endl;
                // std::cout << "SB100 " << BRP100.second << std::endl;
                // std::cout << "F100 " << BRP100.first << std::endl;

                BRP40  = CalculateFatSPRfraction(0.40, fsh_sel_f, api_females);
                BRP35  = CalculateFatSPRfraction(0.35, fsh_sel_f, api_females);
                BRP20  = CalculateFatSPRfraction(0.20, fsh_sel_f, api_females);
                BRP05  = CalculateFatSPRfraction(0.05, fsh_sel_f, api_females);


                // GOA walleye pollock HCR has a different SBtarget and SBfloor
                // variable SBtarget = BRP40.second * (BRP35.first / BRP40.first);
                // variable SBfloor  = BRP20.second;
                variable SBtarget = BRP40.second;
                variable SBfloor  = BRP05.second;

                variable FtargetABC = BRP40.first;
                variable FtargetOFL = BRP35.first;

                // initial value
                F_ABC = FtargetABC;
                ABC   = 0.0;
                F_OFL = FtargetOFL;
                OFL   = 0.0;

                // 30 iterations to stabilize the F_ABC and F_OFL values
                for (int i = 0; i < 30; ++i)
                {
                    // calculate spawning biomass in year 'year' at F_ABC
                    sp_biomass = 0.0;
                    for (int a = 0; a < Nages; ++a)
                    {
                        Zf_ABC    = api_females.M[a] + (F_ABC * fsh_sel_f[a]);
                        expZf_ABC = mas::exp(static_cast<REAL_T> (-1.0) * api_females.spawning_season_offset * Zf_ABC);
                        sp_biomass    += (api_females.N_proj[(season - 1) * Nages + a] *
                                          api_females.maturity[a] * api_females.weight_at_spawning[a] * expZf_ABC);
                    }
                    sp_biomass /= api_females.kg_to_tonnes;

                    // std::cout << "SB at F_ABC " << sp_biomass << "\tSBtarget " << SBtarget << "\tF_ABC " << F_ABC << "\tF_OFL " << F_OFL  << "\tABC " << ABC << "\tOFL " << OFL << std::endl;


                    // adjust F_ABC and F_OFL
                    if (sp_biomass < SBfloor)
                    {
                        F_ABC = 0.0;
                        F_OFL = 0.0;
                    }
                    else if (sp_biomass < SBtarget)
                    {
                        F_ABC = FtargetABC * ((sp_biomass / SBtarget) - Tier3_alpha) / (static_cast<REAL_T> (1.0) - Tier3_alpha);
                        F_OFL = FtargetOFL * ((sp_biomass / SBtarget) - Tier3_alpha) / (static_cast<REAL_T> (1.0) - Tier3_alpha);
                    }
                    else
                    {
                        F_ABC = FtargetABC;
                        F_OFL = FtargetOFL;
                    }


                    // calculate ABC and OFL
                    ABC = 0.0;
                    OFL = 0.0;
                    for (int a = 0; a < Nages; ++a)
                    {
                        Zf_ABC    = api_females.M[a] + (F_ABC * fsh_sel_f[a]);
                        expZf_ABC = mas::exp(static_cast<REAL_T> (-1.0) * Zf_ABC);
                        ABC += (api_females.N_proj[(season - 1) * Nages + a] * api_females.weight_at_catch_time[a] *
                                (static_cast<REAL_T> (1.0) - expZf_ABC) * (F_ABC * fsh_sel_f[a]) / Zf_ABC);

                        Zm_ABC    = api_males.M[a] + (F_ABC * fsh_sel_m[a]);
                        expZm_ABC = mas::exp(static_cast<REAL_T> (-1.0) * Zm_ABC);
                        ABC += (api_males.N_proj[(season - 1) * Nages + a] * api_males.weight_at_catch_time[a] *
                                (static_cast<REAL_T> (1.0) - expZm_ABC) * (F_ABC * fsh_sel_m[a]) / Zm_ABC);


                        Zf_OFL    = api_females.M[a] + (F_OFL * fsh_sel_f[a]);
                        expZf_OFL = mas::exp(static_cast<REAL_T> (-1.0) * Zf_OFL);
                        OFL += (api_females.N_proj[(season - 1) * Nages + a] * api_females.weight_at_catch_time[a] *
                                (static_cast<REAL_T> (1.0) - expZf_OFL) * (F_OFL * fsh_sel_f[a]) / Zf_OFL);

                        Zm_OFL    = api_males.M[a] + (F_OFL * fsh_sel_m[a]);
                        expZm_OFL = mas::exp(static_cast<REAL_T> (-1.0) * Zm_OFL);
                        OFL += (api_males.N_proj[(season - 1) * Nages + a] * api_males.weight_at_catch_time[a] *
                                (static_cast<REAL_T> (1.0) - expZm_OFL) * (F_OFL * fsh_sel_m[a]) / Zm_OFL);
                    }
                    ABC /= api_females.kg_to_tonnes;
                    OFL /= api_females.kg_to_tonnes;
                }

            }

            return std::make_tuple(F_ABC, ABC, F_OFL, OFL);
        }

        const std::string Name()
        {
            return "NPFMC Tier 3 HCR";
        }

    };


    // NWFSC - west coast HCR
    template<typename REAL_T>
    struct PFMC_HCR : HCRBase<REAL_T>
    {
        typedef typename VariableTrait<REAL_T>::variable variable;

        variable F_ABC, ABC, F_OFL, OFL;

        const std::tuple<variable, variable, variable, variable> Evaluate(const int& year, const mas::Subpopulation<REAL_T>& api_males, const mas::Subpopulation<REAL_T>& api_females)
        {
            // initialize
            F_ABC = 0.0;
            ABC   = 0.0;
            F_OFL = 0.0;
            OFL   = 0.0;

            return std::make_tuple(F_ABC, ABC, F_OFL, OFL);
        }

        const std::string Name()
        {
            return "PFMC HCR";
        }

    };

}


#endif /* MAS_HARVEST_CONTROL_RULE_HPP */




