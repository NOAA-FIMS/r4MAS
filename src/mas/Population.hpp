/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
/*
 * File:   Population.hpp
 * Author: matthewsupernaw
 *
 * Created on September 14, 2016, 1:44 PM
 */
#ifndef POPULATION_HPP
#define POPULATION_HPP
#include <memory>
#include <vector>
#include <iomanip>
#include <unordered_map>
#include <unordered_set>
#include "Area.hpp"
//#include "../AutoDiff_Standalone/AutoDiff/AutoDiff.hpp"
#include "Movement.hpp"
#include "Recruitment.hpp"
#include "HarvestControlRule.hpp"
#include "third_party/ATL/lib/Variable.hpp"
#include "third_party/GoogleChartsPlusPlus.hpp"
namespace mas {

    /** @defgroup pop_dynamics Population Dynamics
     * 
     *  @{
     */


    template<class REAL_T>
    class Population;

    /**
     *Runtime calculated information for a population in a specific area, by sex.
     */
    template<class REAL_T>
    struct AreaPopulationInfo {
        typedef typename mas::VariableTrait<REAL_T>::variable variable;
        int id;
        int uid = -9999;
        int spawning_season = 1;
        /*  NOTE:  these values need to be defined elsewhere*/
        variable spawning_season_offset = 1.0;
        variable catch_season_offset = 0.5;
        variable survey_season_offset = 0.5;
        variable kg_to_tonnes = 1000.0; // convert kg to metric tonnes
        REAL_T sex_fraction_value = 0.5;
        /*==================================*/
        bool males = true;
        mas::FishSexType sex;
        Population<REAL_T>* natal_population;
        bool natal_homing = false;
        std::shared_ptr<Area<REAL_T> > area;
        std::shared_ptr<Area<REAL_T> > natal_area;
        std::shared_ptr<GrowthBase<REAL_T> > growth_model;
        std::shared_ptr<mas::NaturalMortality<REAL_T> > natural_mortality_model; // population - area specific
        std::shared_ptr<mas::RecruitmentBase<REAL_T> > recruitment_model; // population - area specific
        std::unordered_map<int, std::shared_ptr<mas::RecruitmentBase<REAL_T> > > seasonal_recruitment_models;
        //        std::unordered_map<int, std::shared_ptr<mas::RecruitmentBase<REAL_T> > > season_fishing_mortality;
        std::vector<variable> ages; // should be the same across areas
        std::vector<REAL_T> maturity; // should be the same across areas, same length as ages
        // these PARAMETERS can be fixed or estimated; should be in model config
#warning hard coded value initialF
        variable initialF = static_cast<REAL_T> (0.0);
        std::vector<variable> initialS;
        std::vector<variable> M;
        std::vector<variable> F; // this PARAMETER vector is shared between males and females
        std::unordered_set<int> active_fleets;
        std::unordered_map<int, std::vector<variable> > F_at_age; // access by fleet id these values may differ between males and females
        std::unordered_map<int, std::vector<variable> > Z_at_age; // access by fleet id these values may differ between males and females
        std::unordered_map<int, std::vector<variable> > SN_at_age; // access by fleet id these values may differ between males and females
        // these are shared by males and females for a specific population and area
        variable R0; // PARAMETER:  fixed or estimated
        variable S0;
        REAL_T unfished_spawning_biomass_per_recruit;
        variable fished_spawning_biomass_per_recruit;
        variable fished_yield_per_recruit;
        REAL_T fished_max_yield_per_recruit;

        variable h; // PARAMETER:  fixed or estimated
        variable SB0; // derived variable
        std::vector<variable> initial_numbers;
        std::vector<variable> initial_equilibrium_numbers;
        std::vector<variable> initial_deviations; // PARAMETER:  fixed or estimated
        //
        int years;
        int seasons;
        int forecast_years = 5;

        std::vector<variable> length_at_season_start;
        std::vector<variable> length_at_spawning;
        std::vector<variable> length_at_catch_time;
        std::vector<variable> length_at_survey_time;
        std::vector<variable> weight_at_season_start;
        std::vector<variable> weight_at_spawning;
        std::vector<variable> weight_at_catch_time;
        std::vector<variable> weight_at_survey_time;
        std::vector<variable> equilibrium_to_survival_at_spawning;
        std::vector<variable> fecundity_at_age;
        std::vector<variable> spawning_biomass;
        std::vector<variable> imigrants;
        std::vector<variable> redistributed_recruits;
        std::vector<variable> immigrant_recruits;
        std::vector<variable> emigrant_recruits;
        std::vector<variable> emigrants;
        std::vector<variable> growth;
        std::vector<variable> recruitment;
        std::vector<variable> abundance;
        std::vector<variable> Z;
        std::vector<variable> P; //selectivity at age.
        variable sum_of_Z;
        std::vector<variable> S;
        std::vector<variable> SN; //survey numbers at age
        std::vector<variable> S_Index; //survey index at age
        std::vector<REAL_T> S_Index_Total; //survey index total
        std::vector<variable> N;
        std::vector<variable> C;
        std::vector<variable> C_Biomass;
        std::vector<REAL_T> C_Biomass_Total;
        std::vector<variable> expected_N;
        std::vector<variable> N_proj; // numbers-at-age in endyear + 1
        //        std::vector<variable> S;
        static uint32_t length_weight_key_carryout;



        /**************************
         * Forecasting Containers *
         **************************/
        std::vector<std::vector<REAL_T> > BootNumbers;

        void Initialize() {

            typename Area<REAL_T>::active_fleets_iterator af_it;
            for (af_it = this->area->active_fleets.begin(); af_it != this->area->active_fleets.end(); ++af_it) {
                (*af_it)->f_at_age[this->area->id][this->id].resize(years * seasons * ages.size());
                (*af_it)->z_at_age[this->area->id][this->id].resize(years * seasons * ages.size());
                if (this->sex == mas::MALE) {
                    (*af_it)->f_at_age_males[this->area->id][this->id].resize(years * seasons * ages.size());
                    (*af_it)->z_at_age_males[this->area->id][this->id].resize(years * seasons * ages.size());
                } else {
                    (*af_it)->f_at_age_females[this->area->id][this->id].resize(years * seasons * ages.size());
                    (*af_it)->z_at_age_females[this->area->id][this->id].resize(years * seasons * ages.size());
                }
            }
            length_at_season_start.resize(length_weight_key_carryout + this->years * this->seasons * this->ages.size() + 1);
            length_at_spawning.resize(length_weight_key_carryout + this->years * this->seasons * this->ages.size());
            length_at_catch_time.resize(length_weight_key_carryout + this->years * this->seasons * this->ages.size());
            length_at_survey_time.resize(length_weight_key_carryout + this->years * this->seasons * this->ages.size());
            weight_at_season_start.resize(length_weight_key_carryout + this->years * this->seasons * this->ages.size() + 1);
            weight_at_spawning.resize(length_weight_key_carryout + this->years * this->seasons * this->ages.size());
            weight_at_catch_time.resize(length_weight_key_carryout + this->years * this->seasons * this->ages.size());
            weight_at_survey_time.resize(length_weight_key_carryout + this->years * this->seasons * this->ages.size());
            equilibrium_to_survival_at_spawning.resize(years * seasons * ages.size());
            fecundity_at_age.resize(years * seasons * ages.size());
            spawning_biomass.resize(years * seasons);
            recruitment.resize(years * seasons);
            redistributed_recruits.resize(years * seasons);
            immigrant_recruits.resize(years * seasons);
            emigrant_recruits.resize(years * seasons);
            abundance.resize(years * seasons);
            initial_numbers.resize(this->ages.size()*3);
            initial_equilibrium_numbers.resize(this->ages.size()*3);
            // these are all (fixed or estimated) PARAMETERS
#warning hard coded vector initialS
            initialS.resize(this->ages.size(), 1.0); //fixed
            M.resize(this->ages.size()); //estimated
            initial_deviations.resize(this->ages.size()); //fixed

            F.resize(years * seasons);
            //            F_at_age.resize(years * seasons * ages.size());
#warning needs to be more efficient, possibly allocating unused memory
            std::unordered_set<int>::iterator fit;
            for (fit = this->active_fleets.begin(); fit != this->active_fleets.end(); ++fit) {
                F_at_age[(*fit)].resize(years * seasons * ages.size());
                Z_at_age[(*fit)].resize(years * seasons * ages.size());
            }
            emigrants.resize(years * seasons * ages.size());
            imigrants.resize(years * seasons * ages.size());
            growth.resize(years * seasons * ages.size());
            Z.resize(years * seasons * ages.size());
            F.resize(years * seasons * ages.size());
            P.resize(years * seasons * ages.size());
            S.resize(years * seasons * ages.size());
            SN.resize(years * seasons * ages.size());
            S_Index.resize(years * seasons * ages.size());
            S_Index_Total.resize(years, seasons);
            N.resize(years * seasons * ages.size());
            C.resize(years * seasons * ages.size());
            C_Biomass.resize(years * seasons * ages.size());
            C_Biomass_Total.resize(years * seasons);
            expected_N.resize(years * seasons * ages.size());
            N_proj.resize(seasons * ages.size());
        }

        inline void Reset() {
            SB0 = static_cast<REAL_T> (0.0);

            std::fill(this->C_Biomass_Total.begin(), this->C_Biomass_Total.end(), static_cast<REAL_T> (0.0));
            std::fill(this->S_Index_Total.begin(), this->S_Index_Total.end(), static_cast<REAL_T> (0.0));
            for (int i = 0; i < N.size(); i++) {
                mas::VariableTrait<REAL_T>::SetValue(emigrants[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(imigrants[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(growth[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(Z[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(F[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(P[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(S[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(SN[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(S_Index[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(N[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(C[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(C_Biomass[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(expected_N[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(fecundity_at_age[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(equilibrium_to_survival_at_spawning[i], static_cast<REAL_T> (0.0));
            }
            for (int i = 0; i < recruitment.size(); i++) {
                mas::VariableTrait<REAL_T>::SetValue(recruitment[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(redistributed_recruits[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(immigrant_recruits[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(emigrant_recruits[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(abundance[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(spawning_biomass[i], static_cast<REAL_T> (0.0));
            }
            for (int i = 0; i < this->ages.size(); i++) {
                mas::VariableTrait<REAL_T>::SetValue(initial_numbers[i], static_cast<REAL_T> (0.0));
            }
            for (int i = 0; i < this->length_at_spawning.size(); i++) {
                mas::VariableTrait<REAL_T>::SetValue(weight_at_season_start[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(length_at_season_start[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(length_at_spawning[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(length_at_catch_time[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(length_at_survey_time[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(weight_at_spawning[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(weight_at_catch_time[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(weight_at_survey_time[i], static_cast<REAL_T> (0.0));
            }
            mas::VariableTrait<REAL_T>::SetValue(weight_at_season_start[this->length_at_spawning.size()], static_cast<REAL_T> (0.0));
            mas::VariableTrait<REAL_T>::SetValue(length_at_season_start[this->length_at_spawning.size()], static_cast<REAL_T> (0.0));
            std::unordered_set<int>::iterator fit;
            for (fit = this->active_fleets.begin(); fit != this->active_fleets.end(); ++fit) {
                std::fill(F_at_age[(*fit)].begin(), F_at_age[(*fit)].end(), static_cast<REAL_T> (0.0));
                std::fill(Z_at_age[(*fit)].begin(), Z_at_age[(*fit)].end(), static_cast<REAL_T> (0.0));
            }
            typename Area<REAL_T>::active_fleets_iterator af_it;
            for (af_it = this->area->active_fleets.begin(); af_it != this->area->active_fleets.end(); ++af_it) {
                std::vector<variable>& f_l = (*af_it)->f_at_age[this->area->id][this->id];
                std::vector<variable>& z_l = (*af_it)->z_at_age[this->area->id][this->id];
                std::fill(f_l.begin(), f_l.end(), static_cast<REAL_T> (0.0));
                std::fill(z_l.begin(), z_l.end(), static_cast<REAL_T> (0.0));
                if (this->sex == mas::MALE) {
                    std::vector<variable>& f_m_l = (*af_it)->f_at_age_males[this->area->id][this->id];
                    std::vector<variable>& z_m_l = (*af_it)->z_at_age_males[this->area->id][this->id];
                    std::fill(f_m_l.begin(), f_m_l.end(), static_cast<REAL_T> (0.0));
                    std::fill(z_m_l.begin(), z_m_l.end(), static_cast<REAL_T> (0.0));
                } else {
                    std::vector<variable>& f_f_l = (*af_it)->f_at_age_females[this->area->id][this->id];
                    std::vector<variable>& z_f_l = (*af_it)->z_at_age_females[this->area->id][this->id];
                    std::fill(f_f_l.begin(), f_f_l.end(), static_cast<REAL_T> (0.0));
                    std::fill(z_f_l.begin(), z_f_l.end(), static_cast<REAL_T> (0.0));
                }
            }
        }

        /**
         * Evaluates length at age and weight at age
         *
         * time of spawning, peak fishing, and survey could differ across areas for a population
         */
        inline void CalculateWeightAtAge() {

            std::vector<variable> length_at_season_start_temp(this->ages.size());
            std::vector<variable> length_at_spawning_temp(this->ages.size());
            std::vector<variable> length_at_catch_time_temp(this->ages.size());
            std::vector<variable> length_at_survey_time_temp(this->ages.size());

            for (int i = 0; i < ages.size(); ++i) {
                variable a = ages[i];
                length_at_season_start_temp[i] = this->growth_model->Evaluate(a, this->sex);
                length_at_spawning_temp[i] = this->growth_model->Evaluate(a + this->spawning_season_offset, this->sex);
                length_at_catch_time_temp[i] = this->growth_model->Evaluate(a + this->catch_season_offset, this->sex);
                length_at_survey_time_temp[i] = this->growth_model->Evaluate(a + this->survey_season_offset, this->sex);
            }

            for (int y = 0; y < this->years; y++) {
                for (int s = 0; s < this->seasons; s++) {
                    for (int i = 0; i < ages.size(); ++i) {
                        //                        variable a = ages[i];
                        size_t index = y * this->seasons * this->ages.size() + (s) * this->ages.size() + i;
                        // length at age
                        length_at_season_start[index] = length_at_season_start_temp[i]; //this->growth_model->Evaluate(a, this->sex);
                        length_at_spawning[index] = length_at_spawning_temp[i]; //this->growth_model->Evaluate(a + this->spawning_season_offset, this->sex);
                        length_at_catch_time[index] = length_at_catch_time_temp[i]; //this->growth_model->Evaluate(a + this->catch_fraction_of_year, this->sex);
                        length_at_survey_time[index] = length_at_survey_time_temp[i]; //this->growth_model->Evaluate(a + this->survey_fraction_of_year, this->sex);
                        /* variable wst =*/ this->growth_model->GetWeight(y, s,
                                this->length_at_season_start[i],
                                ages[i].GetValue(),
                                this->sex, this->id, mas::MEAN_WEIGHT_AT_AGE_SEASON_START, weight_at_season_start[index]);
                        // weight at age
                        //                        weight_at_season_start[index] = wst; //this->growth_model->Weight(length_at_season_start[i], a, this->sex);
                        /*variable wspt =*/ this->growth_model->GetWeight(y, s,
                                this->length_at_spawning[i],
                                ages[i].GetValue() + this->spawning_season_offset.GetValue(),
                                this->sex, this->id, mas::MEAN_WEIGHT_AT_AGE_SPAWNING, weight_at_spawning[index]);
                        //                        weight_at_spawning[index] = wspt; //this->growth_model->Weight(length_at_spawning[i], a + this->spawning_season_offset, this->sex);
                        //                       
                        /*variable wct =*/ this->growth_model->GetWeight(y, s,
                                this->length_at_catch_time[i],
                                ages[i].GetValue() + this->catch_season_offset.GetValue(),
                                this->sex, this->id, mas::CATCH_MEAN_WEIGHT_AT_AGE, weight_at_catch_time[index]);
                        //                        weight_at_catch_time[index] = wct; //this->growth_model->Weight(length_at_catch_time[i], a + this->catch_fraction_of_year, this->sex);
                        /*variable wsurvt = */this->growth_model->GetWeight(y, s,
                                this->length_at_survey_time[i],
                                ages[i].GetValue() + this->survey_season_offset.GetValue(),
                                this->sex, this->id, mas::SURVEY_MEAN_WEIGHT_AT_AGE, weight_at_survey_time[index]);

                    }
                }
            }

        }

        inline REAL_T CalculateUnfishedSpawningBiomassPerRecruit() {


            std::vector<variable > s_per_recruit(this->years);
            for (int y = 0; y < this->years; y++) {
                variable ntemp0 = 1.0;
                for (int s = 0; s < this->seasons; s++) {
                    for (int a = 0; a < ages.size() - 1; a++) {
                        size_t index = y * this->seasons * this->ages.size() + (s) * this->ages.size() + a;
                        //                        std::cout<<"s_per_recruit[y] +="<< ntemp0 <<"*"<< (this->spawning_season_offset*this->weight_at_spawning[index] * this->maturity[a]).GetValue() <<"*"
                        //                                <<atl::mfexp(-1.0 * this->spawning_season_offset * M[a]).GetValue()<<"\n";
                        s_per_recruit[y] += ntemp0 * (this->spawning_season_offset * this->weight_at_spawning[index] * this->maturity[a]) *
                                atl::mfexp(-1.0 * this->spawning_season_offset * M[a]);
                        ntemp0 *= atl::mfexp(-1.0 * this->spawning_season_offset * M[a]);
                    }
                    size_t plus_group = this->ages.size() - 1;
                    size_t index = y * this->seasons * this->ages.size() + (s) * this->ages.size() + plus_group;
                    ntemp0 /= (1.0 - atl::mfexp(-1.0 * M[plus_group]));
                    s_per_recruit[y] += ntemp0 * (this->spawning_season_offset * this->spawning_season_offset * this->weight_at_spawning[index] * this->maturity[plus_group])
                            * atl::mfexp(-1.0 * this->spawning_season_offset * M[plus_group]);
                    //                    std::cout<<"s_per_recruit["<<y<<"] "<<s_per_recruit[y] <<"\n";
                    //                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                }
            }
            //            exit(0);
            this->unfished_spawning_biomass_per_recruit = s_per_recruit[this->years - 1]; // * this->sex_fraction_value;
            variable ret = s_per_recruit[this->years - 1]; // * this->sex_fraction_value;
            return ret.GetValue();
        }

        inline REAL_T CalculateFishedSpawningBiomassPerRecruit(REAL_T F) {


            std::vector<variable> faa(this->F.size());
            variable z_sum;

            for (int y = 0; y < this->years; y++) {
                for (int s = 0; s < this->seasons; s++) {
                    for (int a = 0; a < ages.size() - 1; a++) {
                        size_t index = y * this->seasons * this->ages.size() + (s) * this->ages.size() + a;
                        faa[index] = P[index] * F;
                        z_sum = faa[index] + M[a];
                    }
                }
            }

            std::vector<variable > s_per_recruit(this->years);
            for (int y = 0; y < this->years; y++) {
                variable ntemp0 = 1.0;
                for (int s = 0; s < this->seasons; s++) {

                    for (int a = 0; a < ages.size() - 1; a++) {
                        size_t index = y * this->seasons * this->ages.size() + (s) * this->ages.size() + a;
                        s_per_recruit[y] += ntemp0 * (this->spawning_season_offset * this->weight_at_spawning[index] * this->maturity[a]) *
                                atl::mfexp(-1.0 * this->spawning_season_offset * M[a]
                                - z_sum - this->catch_season_offset * P[index] * faa[index]);
                        ntemp0 *= atl::mfexp(-1.0 * this->spawning_season_offset * M[a]);

                    }//end a

                    //plus group
                    size_t plus_index = this->ages.size() - 1;
                    size_t index = y * this->seasons * this->ages.size() + (s) * this->ages.size() + plus_index;

                    ntemp0 /= (1.0 - atl::mfexp(-1.0 * M[plus_index]));

                    s_per_recruit[y] += ntemp0 * (this->spawning_season_offset * this->weight_at_spawning[index] * this->maturity[plus_index])
                            * atl::mfexp(-1.0 * this->spawning_season_offset * M[plus_index] -
                            z_sum - this->catch_season_offset * P[index] * faa[index]);
                }//end s
            }//end y

            this->fished_spawning_biomass_per_recruit = s_per_recruit[this->years - 1] * this->sex_fraction_value;
            variable ret = s_per_recruit[this->years - 1] * this->sex_fraction_value;
            return ret.GetValue();
        }

        inline REAL_T CalculateYieldPerRecruit(REAL_T F) {

            std::vector<variable> faa(this->F.size());
            variable z_sum;

            for (int y = 0; y < this->years; y++) {
                for (int s = 0; s < this->seasons; s++) {
                    for (int a = 0; a < ages.size() - 1; a++) {
                        size_t index = y * this->seasons * this->ages.size() + (s) * this->ages.size() + a;
                        faa[index] = P[index] * F;
                        z_sum = faa[index] + M[a];
                    }
                }
            }

            std::vector<variable > s_per_recruit(this->years);
            for (int y = 0; y < this->years; y++) {
                variable ntemp0 = 1.0;
                for (int s = 0; s < this->seasons; s++) {
                    for (int a = 0; a < ages.size() - 1; a++) {
                        size_t index = y * this->seasons * this->ages.size() + (s) * this->ages.size() + a;

                        s_per_recruit[y] += ntemp0 * ((this->catch_season_offset * this->weight_at_catch_time[index] * faa[index]) / (faa[index]
                                + M[a]))*(1.0 - atl::mfexp(-1.0 * (faa[index] + M[a]))) * atl::mfexp(-1.0 * z_sum);
                        ntemp0 *= atl::mfexp(-1.0 * this->catch_season_offset * M[a]);

                    }
                    size_t plus_index = this->ages.size() - 1;
                    size_t index = y * this->seasons * this->ages.size() + (s) * this->ages.size() + plus_index;
                    ntemp0 /= (1.0 - atl::mfexp(-1.0 * catch_season_offset * M[plus_index]));
                    s_per_recruit[y] += ntemp0 * (catch_season_offset * this->weight_at_catch_time[index] * faa[index]) * atl::mfexp(-1.0 * z_sum);
                }
            }

            this->fished_yield_per_recruit = s_per_recruit[this->years - 1] * this->sex_fraction_value;
            variable ret = s_per_recruit[this->years - 1] * this->sex_fraction_value;
            return ret.GetValue();
        }

        inline REAL_T CalculateSPR(REAL_T F) {

            variable ret = this->CalculateFishedSpawningBiomassPerRecruit(F) / this->unfished_spawning_biomass_per_recruit; //CalculateUnfishedSpawningBiomassPerRecruit();
            return ret.GetValue();
        }

        inline REAL_T CalculateFSPR(REAL_T fraction) {
            REAL_T distance = std::numeric_limits<REAL_T>::max();
            REAL_T fspr = 0.0;
            for (int f = 0.0; f < 3.0; f += 0.01) {

                REAL_T spr = this->CalculateSPR(f);
                REAL_T dist = std::fabs(spr - fraction);

                if (dist < distance) {
                    distance = dist;
                    fspr = f;
                }

            }

            return fspr;
        }

        inline REAL_T CalculateFMax(const std::vector<REAL_T>& f) {
            REAL_T max_yield_per_recruit = std::numeric_limits<REAL_T>::min();
            REAL_T Fmax = 0.0;
            for (int i = 0; i < f.size(); i++) {
                REAL_T mypr = this->CalculateYieldPerRecruit(f[i]);

                if (mypr > max_yield_per_recruit) {
                    max_yield_per_recruit = mypr;
                    Fmax = f[i];
                }
            }
            return Fmax;
        }

        inline REAL_T CalculateEquilibriumSpawningBiomass(REAL_T F) {
            REAL_T sbpr = this->CalculateFishedSpawningBiomassPerRecruit(F);
            return this->recruitment_model->CalculateEquilibriumSpawningBiomass(sbpr);
        }

        inline REAL_T CalculateEquilibriumRecruitment(REAL_T F) {
            REAL_T esb = CalculateEquilibriumSpawningBiomass(F);
            return this->recruitment_model->CalculateEquilibriumRecruitment(esb);
        }

        inline REAL_T CalculateEquilibriumYield(REAL_T F) {
            return this->CalculateEquilibriumRecruitment(F) *
                    this->CalculateYieldPerRecruit(F);
        }

        void CalculateMaximumSustainableYield(
                REAL_T& msy,
                REAL_T& f_msy,
                REAL_T& s_msy) {

            REAL_T max_yield = std::numeric_limits<REAL_T>::min();
            REAL_T f_max = 0.0;
            REAL_T s_max = 0.0;

            for (REAL_T f = 0.0; f <= 3.0; f += 0.01) {
                REAL_T equil_yield = this->CalculateEquilibriumYield(f);

                if (equil_yield > max_yield) {
                    max_yield = equil_yield;

                    REAL_T esb = this->CalculateEquilibriumSpawningBiomass(f);


                    f_max = f;
                    s_max = esb;
                }

            }

            msy = max_yield;
            f_msy = f_max;
            s_msy = s_max;
        }

        void InitializeM() {
            for (int a = 0; a < this->ages.size(); a++) {
                this->M[a] = this->natural_mortality_model->Evaluate(a);
            }
        }

        void CalculateInitialNumbersEquilibrium() {
            // need R0, initial M-at-age by sex, initial F, and initial fsh sel-at-age by sex
            if (this->sex == mas::MALE)
                this->R0 = this->sex_fraction_value * atl::mfexp(this->recruitment_model->log_R0);

            // fraction female is assumed to be 0.5
            this->initial_equilibrium_numbers[0] = this->R0;
            int a;
            for (a = 1; a < this->ages.size(); a++) {
                this->initial_equilibrium_numbers[a] = this->initial_equilibrium_numbers[a - 1] *
                        atl::exp(static_cast<REAL_T> (-1.0) * (this->M[a - 1]));
            }
            variable m = this->M[a - 1];
            variable sum;
            for (; a < this->initial_equilibrium_numbers.size(); a++) {
                this->initial_equilibrium_numbers[a] = this->initial_equilibrium_numbers[a - 1] * atl::exp(static_cast<REAL_T> (-1.0) * (m));
                sum += this->initial_equilibrium_numbers[a];
            }
            this->initial_equilibrium_numbers[ this->ages.size() - 1] += sum +
                    (this->initial_numbers[initial_numbers.size() - 1] * atl::exp(static_cast<REAL_T> (-1.0) * (m))) /
                    (1.0 - atl::exp(static_cast<REAL_T> (-1.0) * (m)));
        }

        void CalculateInitialNumbers() {

            variable sigma_r = this->recruitment_model->sigma_r;
            int a;
            for (a = 0; a < this->ages.size(); a++) {
                this->initial_numbers[a] = (this->initial_equilibrium_numbers[a] *
                        atl::exp(initial_deviations[a] - static_cast<REAL_T> (0.5)
                        * atl::pow(sigma_r, static_cast<REAL_T> (2.0))));
            }
        }

        void CalculateUnfishedEquilSpawningBiomass() {
            if (this->sex == FEMALE) {
                this->R0 = this->sex_fraction_value * atl::mfexp(this->recruitment_model->log_R0);
                this->recruitment_model->SB0[this->id][this->area->id] =
                        this->unfished_spawning_biomass_per_recruit * this->R0;
            }
        }

        void BuildBootStrapNumbers() {
            this->BootNumbers.resize(this->years,
                    std::vector<REAL_T>(this->ages.size()));

            std::vector<std::vector<REAL_T> >
                    compressed_numbers(this->years,
                    std::vector<REAL_T>(this->ages.size()));

            std::vector<std::vector<REAL_T> >
                    compressed_survivability(this->years,
                    std::vector<REAL_T>(this->ages.size()));

            std::vector<REAL_T> NAAbsn(this->ages.size() + 1);

            for (int y = 0; y < this->years; y++) {
                for (int a = 0; a < this->ages.size(); a++) {
                    REAL_T temp_n = 0.0;
                    REAL_T temp_s = 0.0;
                    for (int s = 0; s < this->seasons; s++) {
                        size_t index = (y * this->seasons * this->ages.size())+
                                (s * this->ages.size()) + a;
                        temp_n += this->N[index].GetValue();
                        temp_s += this->S[index].GetValue();
                    }
                    compressed_numbers[y][a] = temp_n;
                    std::cout << compressed_numbers[y][a] << "  ";
                    compressed_survivability[y][a] = temp_s;
                }
                std::cout << "\n";
            }

            REAL_T tempR = 0.0;
            for (int y = 0; y < this->years; y++) {

                tempR += std::log(compressed_numbers[y][0]);

            }
            NAAbsn[0] = std::exp(tempR / (this->years + 1.0));

            //NAAbsn(iage) = NAA(nyears, iage - 1) * S(nyears, iage - 1);
            std::cout << "BSN Recruitment Vector:\nSex " << this->sex << "\n";
            for (int a = 1; a < this->ages.size(); a++) {
                NAAbsn[a] = compressed_numbers[this->years - 1][a - 1] *
                        compressed_survivability[this->years - 1][a - 1];
                std::cout << NAAbsn[a] << " ";
            }
            NAAbsn[this->ages.size()] =
                    compressed_numbers[this->years - 1][this->ages.size() - 1] *
                    compressed_survivability[this->years - 1][this->ages.size() - 1];
            std::cout << NAAbsn[this->ages.size()] << " ";
            std::cout << "\n\n\n";

        }

        inline void IncrementTime(int& y, int& s) {
            if (s == this->seasons) {
                y += 1;
                s = 1;
            } else {
                s++;
            }
        }

        inline void DecrementTime(int& y, int& s) {
            if (s == 1) {
                y -= 1;
                s = seasons;
            } else {
                s--;
            }
        }

        /**
         *  Calculate the F required to harvest the observed catch biomass
         *  by 1 fleet in one time period (year) in 1 area with 1 population 
         *  using bisection search*/
        inline void FitInitialF(size_t max_iterations = 1000, REAL_T tolerance = 1e-5) {
            atl::Tape<REAL_T> tape_l; //local tape
            tape_l.derivative_trace_level = atl::SECOND_ORDER_REVERSE;
            atl::Variable<REAL_T> f_l;
            atl::Variable<REAL_T> c_biomass;
            atl::Variable<REAL_T> sum;
            this->initialF.SetValue(static_cast<REAL_T> (0.0));
            tape_l.recording = true;
            REAL_T previous = 0;
            for (size_t iter = 0; iter < max_iterations; iter++) {
                f_l.SetValue(0.0);
                c_biomass.SetValue(0.0);
                sum.SetValue(0.0);
                std::vector< std::shared_ptr<mas::Fleet<REAL_T> > >& fleets = this->area->seasonal_fleet_operations[1];
                for (int f = 0; f < fleets.size(); f++) {
                    for (int a = 0; a < this->ages.size(); a++) {
                        variable length_at_catch = this->growth_model->Evaluate(this->ages[a].GetValue() + this->catch_season_offset, this->sex);
                        variable wct = this->growth_model->GetWeight(0, 0,
                                length_at_catch,
                                this->ages[a].GetValue() + this->catch_season_offset.GetValue(),
                                this->sex, this->id, mas::CATCH_MEAN_WEIGHT_AT_AGE);

                        c_biomass.Assign(tape_l, c_biomass + this->weight_at_catch_time[a] *
                                fleets[f]->season_area_selectivity[1][this->area->id]->Evaluate(this->ages[a]) *
                                this->initialF * this->initial_numbers[a] *
                                (1.0 - atl::exp(-this->natural_mortality_model->Evaluate(a) - this->initialF)) /
                                (this->natural_mortality_model->Evaluate(a) + this->initialF));

                    }
                    REAL_T C_B = fleets[f]->catch_biomass_data->get(0, 0)*1000.0;

                    f_l.Assign(tape_l, f_l + ((C_B) - (c_biomass))*((C_B) - (c_biomass)));
                }
                tape_l.AccumulateSecondOrder();
                REAL_T deltaX = tape_l.Value(this->initialF.info->id) / tape_l.Value(this->initialF.info->id, this->initialF.info->id);
                if (std::fabs(tape_l.Value(this->initialF.info->id)) <= tolerance ||
                        deltaX != deltaX) {
                    break;
                }
                this->initialF.SetValue(this->initialF.GetValue() - deltaX);
                if (iter) {
                    if (std::fabs(previous - this->initialF.GetValue()) < 1e-5) {
                        break;
                    }
                }
                previous = this->initialF.GetValue();
                tape_l.Reset();
            }
        }

        /**
         *  Calculate the F required to harvest the observed catch biomass
         *  by 1 fleet in one time period (year) in 1 area with 1 population 
         *  using bisection search*/
        inline void FCalc() {
            if (this->initialF.GetValue() == 0.0) {



                bool recording = variable::tape.recording;
                variable::tape.recording = false;
                //                this->InitialNumbersEquilibrium();
                this->initialF = static_cast<REAL_T> (0.0);
                std::vector< std::shared_ptr<Fleet<REAL_T> > >& fleets = this->area->seasonal_fleet_operations[1];
                variable wct, wct2;

                for (int f = 0; f < fleets.size(); f++) {

                    if (fleets[f]->catch_biomass_data.get() != NULL) {
                        std::shared_ptr<mas::FishingMortality<REAL_T> >& fm =
                                fleets[f]->season_area_fishing_mortality[1][this->area->id];
                        REAL_T Epsilon = 0.0001;
                        REAL_T LowerBound = 0.0;
                        REAL_T UpperBound = 10.0;
                        REAL_T HasConverged = 0;
                        int Iteration = 0;
                        int amax = this->ages.size();

                        while (HasConverged == 0) {
                            REAL_T MidPoint = (LowerBound + UpperBound) / 2.0;
                            Iteration = Iteration + 1;
                            if ((UpperBound - MidPoint) < Epsilon) {
                                HasConverged = 1;
                                this->initialF += MidPoint;
                                break;
                            }
                            REAL_T tmp1 = 0.0;
                            for (int a = 0; a < amax; a++) {
                                //                                variable length_at_catch = this->growth_model->Evaluate(this->ages[a].GetValue() + this->catch_fraction_of_year, this->sex);

                                REAL_T N_ = this->initial_equilibrium_numbers[a].GetValue();


                                tmp1 = tmp1 + this->weight_at_catch_time[a].GetValue()/*this->weight_at_catch_time[a].GetValue()*/ *
                                        fleets[f]->season_area_selectivity[1][this->area->id]->Evaluate(this->ages[a]).GetValue() *
                                        MidPoint * N_ *
                                        (1.0 - std::exp(-this->natural_mortality_model->Evaluate(a).GetValue() - MidPoint)) /
                                        (this->natural_mortality_model->Evaluate(a).GetValue() + MidPoint);
                            }
#warning switch to observed catch biomass
                            //data is in MT
                            REAL_T C_B = fleets[f]->catch_biomass_data->get(0, 0); //this->C_Biomass[y * this->seasons + s].GetValue();//observed catch biomass
                            tmp1 = tmp1 - C_B;
                            REAL_T tmp2 = 0.0;
                            for (int a = 0; a < amax; a++) {


                                REAL_T N_ = this->initial_equilibrium_numbers[a].GetValue();

                                tmp2 = tmp2 + this->weight_at_catch_time[a].GetValue()/*this->weight_at_catch_time[a].GetValue()*/ *
                                        fleets[f]->season_area_selectivity[1][this->area->id]->Evaluate(this->ages[a]).GetValue() *
                                        UpperBound * N_ *
                                        (1.0 - std::exp(-this->natural_mortality_model->Evaluate(a).GetValue() - UpperBound)) /
                                        (this->natural_mortality_model->Evaluate(a).GetValue() + UpperBound);
                            }
                            tmp2 = tmp2 - C_B;
                            if (tmp1 * tmp2 <= 0.0) {
                                LowerBound = MidPoint;
                            } else {
                                UpperBound = MidPoint;
                            }
                        }
                    }
                }
                variable::tape.recording = recording;
            }

        }

        /**
         * Evaluates mortality for all ages in a year and season.
         * @param year
         * @param season
         */
        inline void CalculateMortality(int year, int season) {
            std::vector< std::shared_ptr<Fleet<REAL_T> > >& fleets = this->area->seasonal_fleet_operations[season];
            this->sum_of_Z = 0.0;
            for (int a = 0; a< this->ages.size(); a++) {
                size_t index = year * this->seasons * this->ages.size() + (season - 1) * this->ages.size() + a;
                Z[index] = static_cast<REAL_T> (0.0);

                for (int f = 0; f < fleets.size(); f++) {


                    variable ff = fleets[f]->area_season_fishing_mortality[this->area->id][season]->Evaluate(year, (season - 1));
                    variable s = fleets[f]->season_area_selectivity[season][this->area->id]->Evaluate(this->ages, a);
                    variable f_a_fleet = ff * s;

                    fleets[f]->f_at_age[this->area->id][this->id][index]
                            = f_a_fleet;
                    if (this->sex == FEMALE) {
                        fleets[f]->f_at_age_females[this->area->id][this->id][index]
                                = f_a_fleet;
                    } else {
                        fleets[f]->f_at_age_males[this->area->id][this->id][index]
                                = f_a_fleet;
                    }

                    Z[index] += f_a_fleet;
                    F[index] += ff;
                    P[index] += s;

                }
                Z[index] += this->M[a]; // + f_a_total;
                S[index] = atl::exp(-1.0 * Z[index]);
                sum_of_Z += Z[index];

            }

        }

        /**
         * Evaluates spawn and recruitment for all ages in a year and season.
         *
         * @param year
         * @param season
         */
        inline void CalculateRecruitment(int year, int season) {
            //#warning add compiler hint here
            if (year == 0 && season == 1) {
                this->recruitment[year * seasons + (season - 1)] = this->initial_equilibrium_numbers[0]+
                        (this->recruitment_model->recruitment_deviations[0]);
            } else {
                if (season == this->spawning_season) {
                    //previous year spawning biomass
                    variable sb = this->spawning_biomass[(year - 1) * seasons + (season - 1)] * this->kg_to_tonnes;

                    this->recruitment[year * seasons + (season - 1)] =
                            this->recruitment_model->Evaluate(this->id, this->area->id, sb) *
                            atl::exp(static_cast<REAL_T> (-0.5) *
                            atl::pow(this->recruitment_model->sigma_r, static_cast<REAL_T> (2.0)) +
                            this->recruitment_model->recruitment_deviations[year * seasons + (season - 1)]);
                } else {
                    std::cout << "recruitment model not found!!!\n";
                    exit(0);
                }
                
                this->N[year * this->seasons * this->ages.size() + (season - 1) * this->ages.size() + 0] =
                        static_cast<REAL_T> (this->sex_fraction_value) * this->recruitment[year * seasons + (season - 1)];
            }
        }

        /**
         * Evaluates numbers at age for all ages in a year and season.
         * NO MOVEMENT IN HERE
         *
         * @param year
         * @param season
         */
        inline void CalculateNumbersAtAge(int year, int season) {
            if (year == 0 && season == 1) {
                for (int a = 0; a < ages.size(); a++) {
                    this->N[a] = this->initial_numbers[a];
                }
            } else {
                int y = year;
                int s = season;
                this->DecrementTime(y, s);

                for (int a = 1; a < ages.size(); a++) {

                    this->N[year * this->seasons * this->ages.size() + (season - 1) * this->ages.size() + a] =
                            this->N[y * this->seasons * this->ages.size() + (s - 1) * this->ages.size() + a - 1] *
                            atl::exp(static_cast<REAL_T> (-1.0) *
                            Z[y * this->seasons * this->ages.size() + (s - 1) * this->ages.size() + a - 1]);
                 
                }
                // plus group
                this->N[year * this->seasons * this->ages.size() + (season - 1) * this->ages.size() + this->ages.size() - 1] +=
                        this->N[y * this->seasons * this->ages.size() + (s - 1) * this->ages.size() + this->ages.size() - 1] *
                        atl::exp(static_cast<REAL_T> (-1.0) *
                        Z[y * this->seasons * this->ages.size() + (s - 1) * this->ages.size() + this->ages.size() - 1]);
//                std::cout << this->N[year * this->seasons * this->ages.size() + (season - 1) * this->ages.size() + this->ages.size() - 1]<<"\n";
            }

        }

        inline void SettleMovedFish(int year, int season) {
            int a = 0;
            size_t index = year * this->seasons + (season - 1);
            this->recruitment[index] += this->immigrant_recruits[index];
            this->recruitment[index] -= this->emigrant_recruits[index];


            for (a = 1; a < ages.size(); a++) {
                index = year * this->seasons * this->ages.size() + (season - 1) * this->ages.size() + a;
                this->N[index] += this->imigrants[index];
                this->N[index] -= this->emigrants[index];

            }
        }

        /**
         * Evaluates numbers at age for all ages at the beginning of year 'year' plus one
         * for projections
         * NO MOVEMENT IN HERE
         *
         * @param year
         */
        inline void CalculateNumbersAtAgeEndYearPlusOne() {
            // beginning of endyear (this->years) + 1
            int year = this->years;
            int season = 1;
            int y = year;
            int s = season;
            this->DecrementTime(y, s);
            for (int a = 1; a < ages.size(); a++) {
                this->N_proj[(season - 1) * this->ages.size() + a] =
                        this->N[y * this->seasons * this->ages.size() + (s - 1) * this->ages.size() + a - 1] *
                        atl::exp(static_cast<REAL_T> (-1.0) * Z[y * this->seasons * this->ages.size() + (s - 1) * this->ages.size() + a - 1]);
            }
            // plus group
            this->N_proj[(season - 1) * this->ages.size() + (this->ages.size() - 1)] +=
                    this->N[y * this->seasons * this->ages.size() + (s - 1) * this->ages.size() + (this->ages.size() - 1)] *
                    atl::exp(static_cast<REAL_T> (-1.0) * Z[y * this->seasons * this->ages.size() + (s - 1) * this->ages.size() + (this->ages.size() - 1)]);
            // median age-0 recruits
            this->N_proj[(season - 1) * this->ages.size() + 0] = static_cast<REAL_T> (this->sex_fraction_value) * atl::exp(this->recruitment_model->log_R0);
        }

        /**
         * Evaluates female spawning biomass for all ages in a year and season.
         * NOTE:  this needs to be rewritten to be calculated for females only
         *        and the values shared with males
         *
         * @param year
         * @param season
         */
        inline void CalculateSpawningBiomass(int year, int season) {
            if (season == this->spawning_season) {

                variable sb = static_cast<REAL_T> (0.0);
                for (int a = 0; a < ages.size(); a++) {
                    size_t index = year * this->seasons * this->ages.size() + (season - 1) * this->ages.size() + a;
                    this->equilibrium_to_survival_at_spawning[index] =
                            atl::exp(static_cast<REAL_T> (-1.0) * this->spawning_season_offset * Z[index]);

                    this->fecundity_at_age[index] = this->weight_at_spawning[index] * this->maturity[a];
                    sb += this->fecundity_at_age[index] * this->equilibrium_to_survival_at_spawning[index] * this->N[year * this->seasons * this->ages.size() + (season - 1) * this->ages.size() + a];
                }
                this->spawning_biomass[year * this->seasons + (season - 1)] = sb / this->kg_to_tonnes;
            } else {
                this->spawning_biomass[year * this->seasons + (season - 1)] = -999.0;
            }
        }

        /**
         * Evaluates catch at age for all ages and season
         *
         * @param year
         * @param season
         */
        inline void CalculateCatchAtAge(int year, int season) {

            //get a vector of fleets operating this area at this season
            std::vector< std::shared_ptr<Fleet<REAL_T> > >& fleets = this->area->seasonal_fleet_operations[season];

            //loop through the ages and calculate catch numbers and catch biomass
            //for both this local segment of the population and fleet[i]. 
            REAL_T total_CB = static_cast<REAL_T> (0.0);



            for (int a = 0; a < this->ages.size(); a++) {

                //dimension folded index(year,season,age)
                size_t index = year * this->seasons * this->ages.size() + (season - 1) * this->ages.size() + a;

                //loop through fleets operating in this area at this season
                for (int f = 0; f < fleets.size(); f++) {
                    //fleet f at age for this area,year,season, age

                    //local fleet f at age 
                    variable f_a; // =
                    variable fca;
                    if (this->sex == FEMALE) {
                        fca = (N[index] * (fleets[f]->f_at_age_females[this->area->id][this->id][index]
                                / Z[index]) * (1.0 - atl::exp(static_cast<REAL_T> (-1.0) * Z[index])));
                    } else {
                        fca = (N[index] * (fleets[f]->f_at_age_males[this->area->id][this->id][index]
                                / Z[index]) * (1.0 - atl::exp(static_cast<REAL_T> (-1.0) * Z[index])));
                    }


                    //add  to the fleet[i]'s total catch numbers at age
                    fleets[f]->catch_at_age[index] += fca;
                    //contribute to this population segments total catch numbers at age
                    C[index] += fca;
                    //contribute to this population segments total catch biomass at age
                    C_Biomass[index] += fca * this->weight_at_catch_time[index];

                    total_CB += C_Biomass[index].GetValue();

                    //add  to the fleet[i]'s total catch numbers at age
                    fleets[f]->catch_biomass_at_age[index] +=
                            fca * this->weight_at_catch_time[index];

                    if (this->males) {
                        fleets[f]->catch_at_age_males[index] += fca;
                        fleets[f]->catch_biomass_at_age_males[index] +=
                                fca * this->weight_at_catch_time[index];
                    } else {

                        fleets[f]->catch_at_age_females[index] += fca;
                        fleets[f]->catch_biomass_at_age_females[index] +=
                                fca * this->weight_at_catch_time[index];
                    }
                }

            }
            this->C_Biomass_Total[year * this->seasons + season - 1] = total_CB;
        

        }

        /**
         * Evaluates survey numbers at age for all ages and season
         * @param year
         * @param season
         */
        inline void CalculateSurveyNumbersAtAge(int year, int season) {
            std::vector< std::shared_ptr<Survey<REAL_T> > >& surveys = this->area->seasonal_survey_operations[season];

            REAL_T total_SI = static_cast<REAL_T> (0.0);

            for (int a = 0; a < this->ages.size(); a++) {
                size_t index = year * this->seasons * this->ages.size() + (season - 1) * this->ages.size() + a;
                variable weight = this->weight_at_survey_time[index];
                for (int s = 0; s < surveys.size(); s++) {

                    // NOTE:  the survey has a catchability (q) associated with it
                    variable saa = surveys[s]->q *
                            this->N[index] * surveys[s]->season_area_selectivity[season][this->area->id]->Evaluate(this->ages, a) *
                            atl::exp(static_cast<REAL_T> (-1.0) * survey_season_offset * this->Z[index]);

                    this->SN[index] += saa;
                    surveys[s]->survey_numbers_at_age[index] += saa;

                    this->S_Index[index] += saa * weight;
                    surveys[s]->survey_biomass_at_age[index] += saa * weight;

                    if (this->males) {
                        surveys[s]->survey_numbers_at_age_males[index] += saa;
                        surveys[s]->survey_biomass_at_age_males[index] += saa * weight;
                    } else {
                        surveys[s]->survey_numbers_at_age_females[index] += saa;
                        surveys[s]->survey_biomass_at_age_females[index] += saa * weight;
                    }
                }
            }

        }

        inline void PushToArea() {

            this->area->PushNumbersAndBiomass(this->SN,
                    this->S_Index,
                    this->N,
                    this->C,
                    this->C_Biomass,
                    this->id,
                    sex);
        }

        inline void PushNumbersToFleet() {
            bool r = variable::tape.recording;
            variable::tape.recording = false;
            if (this->sex == mas::FEMALE) {
                for (int s = 1; s <= this->seasons; s++) {
                    std::vector< std::shared_ptr<Fleet<REAL_T> > >& fleets =
                            this->area->seasonal_fleet_operations[s];
                    for (int i = 0; i < fleets.size(); i++) {
                        for (int j = 0; j < N.size(); j++) {
                            fleets[i]->numbers_at_age[j] += this->N[j];
                            fleets[i]->numbers_at_age_females[j] += this->N[j];
                        }
                    }
                }
            } else {
                for (int s = 1; s <= this->seasons; s++) {
                    std::vector< std::shared_ptr<Fleet<REAL_T> > >& fleets =
                            this->area->seasonal_fleet_operations[s];
                    for (int i = 0; i < fleets.size(); i++) {
                        for (int j = 0; j < N.size(); j++) {

                            fleets[i]->numbers_at_age[j] += this->N[j];
                            fleets[i]->numbers_at_age_males[j] += this->N[j];
                        }
                    }
                }
            }
            variable::tape.recording = r;
        }
    };
    template<typename REAL_T>
    uint32_t AreaPopulationInfo<REAL_T>::length_weight_key_carryout = 10;

    template<typename REAL_T>
    std::ostream& operator<<(std::ostream& out, mas::AreaPopulationInfo<REAL_T>& pi) {
        out << std::fixed;
        out << std::setprecision(7);
        out << "Population " << pi.natal_population->id << "\n";
        out << "Area " << pi.area->id << "\n";
        out << "R0\t" << atl::exp(pi.recruitment_model->log_R0).GetValue() << "\n";
        out << "h\t" << pi.recruitment_model->h << "\n";
        out << "SB0\t" << pi.recruitment_model->SB0[pi.id][pi.area->id] << "\n";
        out << "\n\n";
        out << "Natural Mortality at Age (M)\n";
        if (pi.males) {
            out << "Males\n";
        } else {
            out << "Females\n";
        }
        for (int a = 0; a < pi.ages.size(); a++) {
            out << pi.M[a] << " ";
        }
        out << "\n\n";
        out << std::fixed;
        out << std::setprecision(8);
        out << "Initial F = " << pi.initialF << "\n\n";
        out << std::fixed;
        out << std::setprecision(2);
        out << "Fully-selected Fishing Mortality\n";
        if (pi.males) {
            out << "Males\n";
        } else {
            out << "Females\n";
        }
        for (int s = 1; s <= pi.seasons; s++) {
            std::vector< std::shared_ptr<Fleet<REAL_T> > >& fleets = pi.area->seasonal_fleet_operations[s];
            if (fleets.size() > 0) {
                for (int f = 0; f < fleets.size(); f++) {
                    for (int y = 0; y < pi.years; y++) {
                        out << fleets[f]->area_season_fishing_mortality[pi.area->id][s]->Evaluate(y, (s - 1)) << " ";
                    }
                    out << "\n";
                }
                out << "\n";
            } else {
                out << "no fleet-specific fishing mortality available\n";
            }
        }
        out << "\n\n";
        out << "Fishery Selectivity\n";
        if (pi.males) {
            out << "Males\n";
        } else {
            out << "Females\n";
        }
        for (int s = 1; s <= pi.seasons; s++) {
            std::vector< std::shared_ptr<Fleet<REAL_T> > >& fleets = pi.area->seasonal_fleet_operations[s];
            if (fleets.size() > 0) {
                for (int f = 0; f < fleets.size(); f++) {
                    for (int a = 0; a < pi.ages.size(); a++) {
                        out << fleets[f]->season_area_selectivity[s][pi.area->id]->Evaluate(pi.ages[a]) << " ";
                    }
                    out << "\n";
                }
                out << "\n";
            } else {
                out << "no fleet-specific fishery selectivity available\n";
            }
        }
        out << "\n\n";
        out << "Total Mortality at Age (Z)\n";
        if (pi.males) {
            out << "Males\n";
        } else {
            out << "Females\n";
        }
        for (int a = 0; a < pi.ages.size(); a++) {
            for (int s = 1; s <= pi.seasons; s++) {
                for (int y = 0; y < pi.years; y++) {
                    out << pi.Z[y * pi.seasons * pi.ages.size() + (s - 1) * pi.ages.size() + a] << " ";
                }
            }
            out << "\n";
        }

        out << "\n\n";
        out << "Population " << pi.natal_population->id << "\n";
        out << "Area " << pi.area->id << "\n";
        out << "initial equilibrium numbers\n";
        if (pi.males) {
            out << "Males\n";
        } else {
            out << "Females\n";
        }
        for (int a = 0; a < pi.ages.size(); a++) {
            out << pi.initial_equilibrium_numbers[a] << " ";
        }

        out << "\n\n";
        out << "Population " << pi.natal_population->id << "\n";
        out << "Area " << pi.area->id << "\n";
        out << "initial numbers\n";
        if (pi.males) {
            out << "Males\n";
        } else {
            out << "Females\n";
        }
        for (int a = 0; a < pi.ages.size(); a++) {
            out << pi.initial_numbers[a] << " ";
        }
        out << "\n\n";
        out << "Population " << pi.natal_population->id << "\n";
        out << "Area " << pi.area->id << "\n";
        out << "Total age-0 recruits\n";
        for (int y = 0; y < pi.recruitment.size(); y++) {
            out << pi.recruitment[y] << " ";
        }
        out << "\n\n";
        out << "Population " << pi.natal_population->id << "\n";
        out << "Area " << pi.area->id << "\n";
        out << "Numbers at Age\n";
        if (pi.males) {
            out << "Males\n";
        } else {
            out << "Females\n";
        }
        for (int a = 0; a < pi.ages.size(); a++) {
            for (int s = 1; s <= pi.seasons; s++) {
                for (int y = 0; y < pi.years; y++) {
                    out << pi.N[y * pi.seasons * pi.ages.size() + (s - 1) * pi.ages.size() + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";
        out << "Population " << pi.natal_population->id << "\n";
        out << "Area " << pi.area->id << "\n";
        out << "Catch Numbers at Age\n";
        if (pi.males) {
            out << "Males\n";
        } else {
            out << "Females\n";
        }
        for (int a = 0; a < pi.ages.size(); a++) {
            for (int s = 1; s <= pi.seasons; s++) {
                for (int y = 0; y < pi.years; y++) {
                    out << pi.C[y * pi.seasons * pi.ages.size() + (s - 1) * pi.ages.size() + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";
        out << "Population " << pi.natal_population->id << "\n";
        out << "Area " << pi.area->id << "\n";
        out << "Catch Biomass at Age\n";
        if (pi.males) {
            out << "Males\n";
        } else {
            out << "Females\n";
        }
        for (int a = 0; a < pi.ages.size(); a++) {
            for (int s = 1; s <= pi.seasons; s++) {
                for (int y = 0; y < pi.years; y++) {
                    out << pi.C_Biomass[y * pi.seasons * pi.ages.size() + (s - 1) * pi.ages.size() + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";
        out << "Population " << pi.natal_population->id << "\n";
        out << "Area " << pi.area->id << "\n";
        out << "Survey Numbers at Age\n";
        if (pi.males) {
            out << "Males\n";
        } else {
            out << "Females\n";
        }
        for (int a = 0; a < pi.ages.size(); a++) {
            for (int s = 1; s <= pi.seasons; s++) {
                for (int y = 0; y < pi.years; y++) {
                    size_t index = y * pi.seasons * pi.ages.size() + (s - 1) * pi.ages.size() + a;
                    out << pi.SN[index] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";
        out << "Population " << pi.natal_population->id << "\n";
        out << "Area " << pi.area->id << "\n";
        out << "Survey Index at Age\n";
        if (pi.males) {
            out << "Males\n";
        } else {
            out << "Females\n";
        }
        for (int a = 0; a < pi.ages.size(); a++) {
            for (int s = 1; s <= pi.seasons; s++) {
                for (int y = 0; y < pi.years; y++) {
                    out << pi.S_Index[y * pi.seasons * pi.ages.size() + (s - 1) * pi.ages.size() + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";
        out << "Population " << pi.natal_population->id << "\n";
        out << "Area " << pi.area->id << "\n";
        out << "Total Immigrants\n";
        if (pi.males) {
            out << "Males\n";
        } else {
            out << "Females\n";
        }
        for (int a = 0; a < pi.ages.size(); a++) {
            for (int s = 1; s <= pi.seasons; s++) {
                for (int y = 0; y < pi.years; y++) {
                    out << pi.imigrants[y * pi.seasons * pi.ages.size() + (s - 1) * pi.ages.size() + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";
        out << "Population " << pi.natal_population->id << "\n";
        out << "Area " << pi.area->id << "\n";
        out << "Total Emigrants\n";
        if (pi.males) {
            out << "Males\n";
        } else {
            out << "Females\n";
        }
        for (int a = 0; a < pi.ages.size(); a++) {
            for (int s = 1; s <= pi.seasons; s++) {
                for (int y = 0; y < pi.years; y++) {
                    out << pi.emigrants[y * pi.seasons * pi.ages.size() + (s - 1) * pi.ages.size() + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";
        out << "Population " << pi.natal_population->id << "\n";
        out << "Area " << pi.area->id << "\n";
        out << "Spawning Biomass\n";
        if (pi.males) {
            out << "Males\n";
        } else {
            out << "Females\n";
        }
        for (int s = 1; s <= pi.seasons; s++) {
            for (int y = 0; y < pi.years; y++) {
                out << pi.spawning_biomass[y * pi.seasons + (s - 1)] << " ";
            }
            out << "\n";
        }
        out << "\n\n";
        out << std::fixed;
        out << "Population " << pi.natal_population->id << "\n";
        out << "Area " << pi.area->id << "\n";
        out << "fecundity_at_age\n";
        if (pi.males) {
            out << "Males\n";
        } else {
            out << "Females\n";
        }
        for (int a = 0; a < pi.ages.size(); a++) {
            out << pi.fecundity_at_age[a] << " ";
        }
        out << "\n\n";
        out << "Population " << pi.natal_population->id << "\n";
        out << "Area " << pi.area->id << "\n";
        out << "length_at_spawning\n";
        if (pi.males) {
            out << "Males\n";
        } else {
            out << "Females\n";
        }
        for (int a = 0; a < pi.ages.size(); a++) {
            out << pi.length_at_spawning[a] << " ";
        }
        out << "\n\n";
        out << "Population " << pi.natal_population->id << "\n";
        out << "Area " << pi.area->id << "\n";
        out << "weight_at_spawning\n";
        if (pi.males) {
            out << "Males\n";
        } else {
            out << "Females\n";
        }
        for (int y = 0; y < pi.years; y++) {
            for (int s = 1; s <= pi.seasons; s++) {
                for (int a = 0; a < pi.ages.size(); a++) {
                    out << pi.weight_at_spawning[y * pi.seasons * pi.ages.size() + (s - 1) * pi.ages.size() + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";
        out << std::fixed;
        out << "Population " << pi.natal_population->id << "\n";
        out << "Area " << pi.area->id << "\n";
        out << "length_at_season_start\n";
        if (pi.males) {
            out << "Males\n";
        } else {
            out << "Females\n";
        }
        for (int a = 0; a < pi.ages.size(); a++) {
            out << pi.length_at_season_start[a] << " ";
        }
        out << "\n";
        out << std::fixed;
        out << "Population " << pi.natal_population->id << "\n";
        out << "Area " << pi.area->id << "\n";
        out << "weight_at_season_start\n";
        if (pi.males) {
            out << "Males\n";
        } else {
            out << "Females\n";
        }
        for (int y = 0; y < pi.years; y++) {
            for (int s = 1; s <= pi.seasons; s++) {
                for (int a = 0; a < pi.ages.size(); a++) {
                    out << pi.weight_at_season_start[y * pi.seasons * pi.ages.size() + (s - 1) * pi.ages.size() + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";
        out << std::fixed;
        out << "Population " << pi.natal_population->id << "\n";
        out << "Area " << pi.area->id << "\n";
        out << "length_at_catch_time\n";
        if (pi.males) {
            out << "Males\n";
        } else {
            out << "Females\n";
        }
        for (int a = 0; a < pi.ages.size(); a++) {
            out << pi.length_at_catch_time[a] << " ";
        }
        out << "\n";
        out << std::fixed;
        out << "Population " << pi.natal_population->id << "\n";
        out << "Area " << pi.area->id << "\n";
        out << "weight_at_catch_time\n";
        if (pi.males) {
            out << "Males\n";
        } else {
            out << "Females\n";
        }
        for (int a = 0; a < pi.ages.size(); a++) {
            for (int s = 1; s <= pi.seasons; s++) {
                for (int y = 0; y < pi.years; y++) {
                    out << pi.weight_at_catch_time[y * pi.seasons * pi.ages.size() + (s - 1) * pi.ages.size() + a] << " ";
                }
            }
            out << "\n";
        }
        //        for (int a = 0; a < pi.ages.size(); a++) {
        //            out << pi.weight_at_catch_time[a] << " ";
        //        }
        out << "\n\n";
        out << std::fixed;
        out << "Population " << pi.natal_population->id << "\n";
        out << "Area " << pi.area->id << "\n";
        out << "length_at_survey_time\n";
        if (pi.males) {
            out << "Males\n";
        } else {
            out << "Females\n";
        }
        for (int a = 0; a < pi.ages.size(); a++) {
            out << pi.length_at_survey_time[a] << " ";
        }
        out << "\n";
        out << std::fixed;
        out << "Population " << pi.natal_population->id << "\n";
        out << "Area " << pi.area->id << "\n";
        out << "weight_at_survey_time\n";
        if (pi.males) {
            out << "Males\n";
        } else {
            out << "Females\n";
        }
        for (int a = 0; a < pi.ages.size(); a++) {
            for (int s = 1; s <= pi.seasons; s++) {
                for (int y = 0; y < pi.years; y++) {
                    out << pi.weight_at_survey_time[y * pi.seasons * pi.ages.size() + (s - 1) * pi.ages.size() + a] << " ";
                }
            }
            out << "\n";
        }
        //        for (int a = 0; a < pi.ages.size(); a++) {
        //            out << pi.weight_at_survey_time[a] << " ";
        //        }
        out << "\n\n";
        out << "\n\n";
        out << std::fixed;
        out << "Population " << pi.natal_population->id << "\n";
        out << "Area " << pi.area->id << "\n";
        out << "equilibrium_to_survival_at_spawning\n";
        if (pi.males) {
            out << "Males\n";
        } else {
            out << "Females\n";
        }
        for (int a = 0; a < pi.ages.size(); a++) {
            out << pi.equilibrium_to_survival_at_spawning[a] << " ";
        }
        out << "\n";
        out << "\n\n";
        out << std::fixed;
        out << "Population " << pi.natal_population->id << "\n";
        out << "Area " << pi.area->id << "\n";
        out << "length_at_catch_time\n";
        if (pi.males) {
            out << "Males\n";
        } else {
            out << "Females\n";
        }
        for (int a = 0; a < pi.ages.size(); a++) {
            out << pi.length_at_catch_time[a] << " ";
        }
        out << "\n\n";
        out << "Population " << pi.natal_population->id << "\n";
        out << "Area " << pi.area->id << "\n";
        out << "weight_at_catch_time\n";
        if (pi.males) {
            out << "Males\n";
        } else {
            out << "Females\n";
        }
        for (int a = 0; a < pi.ages.size(); a++) {
            out << pi.weight_at_catch_time[a] << " ";
        }
        out << "\n\n";
        out << std::fixed;
        out << "Population " << pi.natal_population->id << "\n";
        out << "Area " << pi.area->id << "\n";
        out << "Maturity-at-age\n";
        if (pi.males) {
            out << "Males\n";
        } else {
            out << "Females\n";
        }
        for (int a = 0; a < pi.maturity.size(); a++) {
            out << pi.maturity[a] << " ";
        }
        out << "\n";
        out << "\n\n";
        out << std::fixed;
        out << "Redistributed Recruits " << pi.natal_population->id << "\n";
        out << "Area " << pi.area->id << "\n";
        out << "Recruitment\n";
        if (pi.males) {
            out << "Males\n";
        } else {
            out << "Females\n";
        }
        for (int y = 0; y < pi.redistributed_recruits.size(); y++) {
            out << pi.redistributed_recruits[y] << " ";
        }
        out << "\n\n";
        out << "\n*********************************************************************************************************************\n";
        out << "\n\n";

        return out;
    }

    template<typename REAL_T>
    std::string ToHtml(mas::AreaPopulationInfo<REAL_T>& pi) {
        std::stringstream out;
        out << std::fixed;
        out << std::setprecision(15);
        out << "<br>Population: " << pi.natal_population->id;
        out << "<br>Area: " << pi.area->id;
        if (pi.males) {
            out << "<br>Sex: Males\n";
        } else {
            out << "<br>Sex: Females";
        }
        out << "<br><br>";
        gcpp::LineChart<REAL_T> inum_chart;
        inum_chart.title = "Inital Numbers At Age";
        inum_chart.data_table.AddColumn("number", "Age");
        inum_chart.data_table.AddColumn("number", "Numbers");
        for (int i = 0; i < pi.ages.size(); i++) {
            std::vector<REAL_T> recv(2);
            recv[0] = pi.ages[i];
            recv[1] = pi.initial_numbers[i].GetValue();
            inum_chart.data_table.AddRow(recv);
        }
        out << inum_chart.Create();
        out << "<br><br>Recruitment model: " << pi.recruitment_model->Name();
        out << "<br>R0\t" << atl::exp(pi.recruitment_model->log_R0).GetValue();
        out << "<br>h\t" << pi.recruitment_model->h << "\n";
        out << "<br>SB0\t" << pi.recruitment_model->SB0[pi.id][pi.area->id];
        out << "<br>";
        gcpp::LineChart<REAL_T> rec_chart;
        rec_chart.data_table.AddColumn("number", "Year");
        rec_chart.data_table.AddColumn("number", "Recruits");
        rec_chart.title = "Yearly Recruits";
        for (int i = 0; i < pi.recruitment.size(); i++) {
            std::vector<REAL_T> recv(2);
            recv[0] = 0 + i;
            recv[1] = pi.recruitment[i].GetValue();
            rec_chart.data_table.AddRow(recv);
        }
        out << rec_chart.Create();
        gcpp::LineChart<REAL_T> sbiomass_chart;
        sbiomass_chart.data_table.AddColumn("number", "Year");
        sbiomass_chart.data_table.AddColumn("number", "Biomass");
        sbiomass_chart.title = "Yearly Spawning Biomass";
        for (int i = 0; i < pi.spawning_biomass.size(); i++) {
            std::vector<REAL_T> recv(2);
            recv[0] = 0 + i;
            recv[1] = pi.spawning_biomass[i].GetValue();
            sbiomass_chart.data_table.AddRow(recv);
        }
        out << sbiomass_chart.Create();

        return out.str();
    }

    template<typename REAL_T>
    std::string AddLatexCharts(mas::AreaPopulationInfo<REAL_T>& pi) {
        std::stringstream out;
        out.precision(5);
        out << std::fixed;
        std::string sex = "Females";
        if (pi.sex == mas::MALE) {
            sex = "Males";
        }


        out << "\\begin{tikzpicture}\n";
        out << "\\begin{axis}[\n";
        out << "title= " "Pop" << pi.id << "-" << sex << "-Area" << pi.area->id << "Catch Biomass Index,\n";
        out << "xlabel=Time,\n";
        out << "ylabel=Index]\n";
        out << "]\n";
        out << "\\addplot [color= blue, mark=+] table {\n";
        for (int y = 0; y < pi.years; y++) {
            for (int s = 0; s < pi.seasons; s++) {
                size_t index = y * pi.seasons + s;
                out << index << " " << pi.C_Biomass_Total[index] << " \n";
            }
        }
        out << "};\n";
        out << "\\end{axis} \n";
        out << "\\end{tikzpicture}\n";
        out << "\\newline\n";

        out << "\\begin{tikzpicture}\n";
        out << "\\begin{axis}[\n";
        out << "title= " "Pop" << pi.id << "-" << sex << "-Area" << pi.area->id << "Survey Index,\n";
        out << "xlabel=Time,\n";
        out << "ylabel=Index]\n";
        out << "]\n";
        out << "\\addplot [color= red, mark=+] table {\n";
        for (int y = 0; y < pi.years; y++) {
            for (int s = 0; s < pi.seasons; s++) {
                size_t index = y * pi.seasons + s;
                out << index << " " << pi.S_Index_Total[index] << " \n";
            }
        }
        out << "};\n";
        out << "\\end{axis} \n";
        out << "\\end{tikzpicture}\n";
        out << "\\newline\n";

        out << "\\begin{tikzpicture}\n";
        out << "\\begin{axis}[\n";
        out << "title= " "Pop" << pi.id << "-" << sex << "-Area" << pi.area->id << "Catch At Age,\n";
        out << "xlabel=Age,\n";
        out << "ylabel=Numbers]\n";
        out << "]\n";

        for (int y = 0; y < pi.years; y++) {
            for (int s = 0; s < pi.seasons; s++) {
                out << "\\addplot [] table {\n";
                for (int a = 0; a < pi.ages.size(); a++) {

                    out << pi.ages[a] << "   " << pi.C[y * pi.seasons * pi.ages.size() + (s) * pi.ages.size() + a] << "\n";
                }
                out << "};\n";

            }
        }

        out << "\\end{axis} \n";
        out << "\\end{tikzpicture}\n";
        out << "\\newline\n";


        for (int y = 0; y < pi.years; y++) {
            for (int s = 0; s < pi.seasons; s++) {
                out << "\\begin{tikzpicture}\n";
                out << "\\begin{axis}[\n";
                out << "title= " "Pop" << pi.id << "-" << sex << "-Area" << pi.area->id << "-Yr" << (y + 1) << "-S" << (s + 1) << ",\n";
                out << "xlabel=Age,\n";
                out << "ylabel=Numbers]\n";
                out << "]\n";
                out << "\\addplot [color= red, mark=*] table {\n";
                for (int a = 0; a < pi.ages.size(); a++) {

                    out << (REAL_T) pi.ages[a] << "   " << pi.N[y * pi.seasons * pi.ages.size() + (s) * pi.ages.size() + a] << "\n";
                }
                out << "};\n";
                out << "\\addlegendentry{Numbers}\n";
                out << "\\addplot [color= blue, mark=*] table {\n";
                for (int a = 0; a < pi.ages.size(); a++) {

                    out << pi.ages[a] << "   " << pi.C[y * pi.seasons * pi.ages.size() + (s) * pi.ages.size() + a] << "\n";
                }
                out << "};\n";
                out << "\\addlegendentry{Catch}\n";
                out << "\\addplot [color= yellow, mark=*] table {\n";
                for (int a = 0; a < pi.ages.size(); a++) {

                    out << pi.ages[a] << "  " << pi.SN[y * pi.seasons * pi.ages.size() + (s) * pi.ages.size() + a] << "\n";
                }
                out << "};\n";
                out << "\\addlegendentry{Survey}\n";
                out << "\\end{axis} \n";
                out << "\\end{tikzpicture}\n";
                out << "\\newline\n";

            }
        }
        return out.str();
    }

    template<typename REAL_T>
    class Population : public mas::ModelObject<REAL_T> {
    public:

        int phase = 1;
        int previous_phase = 1;
        /*********************************************
         * Area specific natural mortality           *
         *********************************************/
        std::map<int, int> male_natural_mortality_ids; //area, natural mortality model
        std::map<int, int> female_natural_mortality_ids; //area, natural mortality model
        typedef typename std::map<int, int>::iterator male_natural_mortality_ids_iterator;
        typedef typename std::map<int, int>::iterator female_natural_mortality_ids_iterator;
        /*********************************************
         * Area specific recruitment                 *
         *********************************************/
        std::unordered_map<int, std::unordered_map<int, int> > area_season_recruitment_ids;
        typedef typename std::unordered_map<int, std::unordered_map<int, int> >::iterator season_area_id_iterator;
        std::unordered_map<int, std::unordered_map<int, std::shared_ptr<mas::RecruitmentBase<REAL_T> > > > area_season_recruitment;
        std::unordered_map<int, std::unordered_map<int, int> > season_area_recruitment_ids;
        std::unordered_map<int, std::unordered_map<int, std::shared_ptr<mas::RecruitmentBase<REAL_T> > > > season_area_recruitment;
        std::map<int, int> recruitment_ids; //area, recruitment model
        typedef typename std::map<int, int>::iterator recruitment_ids_iterator;
        // typedef typename std::map<int, int>::iterator recruitment_season_ids_iterator;
        typedef typename mas::VariableTrait<REAL_T>::variable variable;
        std::string name;
        int natal_area_id;
        int movement_model_id;
        bool natal_homing = false;
        bool natal_recruitment = false;
        bool move_fish_before_lh = false;
        int years;
        int seasons;
        int areas;
        int ages;
        int growth_id;
        REAL_T female_fraction_value = 0.5;
        std::vector<variable> SN; //survey numbers at age
        std::vector<variable> SN_Biomass; //survey numbers at age
        std::vector<variable> C;
        std::vector<variable> C_Biomass;
        std::shared_ptr<Area<REAL_T> > natal_area; //birth area
        std::vector<std::shared_ptr<Area<REAL_T> > > areas_list; //all areas
        //Movement Tracking
        typedef typename std::unordered_map<int, AreaPopulationInfo<REAL_T> >::iterator cohort_iterator;
        std::unordered_map<int, AreaPopulationInfo<REAL_T> > males;
        std::unordered_map<int, AreaPopulationInfo<REAL_T> > females;
        std::unordered_map<int, int > movement_models_ids; //season keyed
        typedef typename std::unordered_map<int, int >::iterator movement_model_id_iterator;
        std::shared_ptr<mas::Movement<REAL_T> > movement_model;
        std::unordered_map<int, std::shared_ptr<mas::Movement<REAL_T> > > movement_models; //year keyed
        typedef typename std::unordered_map<int, std::shared_ptr<mas::Movement<REAL_T> > >::iterator movement_model_iterator;
        //Estimable
        std::unordered_map<int, std::pair<bool, std::vector<variable> > > initial_deviations_males; // area indexed
        std::unordered_map<int, std::pair<bool, std::vector<variable> > > initial_deviations_females; // area indexed
        typedef typename std::unordered_map<int, std::pair<bool, std::vector<variable> > >::iterator initial_deviations_iterator;
        std::vector<std::vector<variable> > movement_coefficients;
        std::unordered_map<int, std::unordered_map<int, std::vector<REAL_T> > > maturity_models; //area / sex
        typedef typename std::unordered_map<int, std::unordered_map<int, std::vector<REAL_T> > >::iterator maturity_models_iterator;
        typedef typename std::unordered_map<int, std::vector<REAL_T> >::iterator area_maturity_model_iterator;
        //    typedef typename std::unordered_map<std::vector<std::vector<variable> > >::iterator movement_coefficient_iterator;
        // std::vector<mas::InitialNumbers<typename mas::VariableTrait<REAL_T>::variable > > initial_numbers;
        std::shared_ptr<mas::HCRBase<REAL_T> > harvest_control_rule;
        std::unordered_set<int> active_fleets;
        int forecast_years = 5;

        Population() {
        }

        Population(int years,
                int seasons,
                int areas,
                const std::shared_ptr<Area<REAL_T> >& natal_area,
                const std::vector<std::shared_ptr<Area<REAL_T> > >& areas_list) :
        years(years),
        seasons(seasons),
        areas(areas),
        natal_area(natal_area),
        areas_list(areas) {

            for (int d = 0; d < areas_list.size(); d++) {
                males[areas_list[d]->id].forecast_years = this->forecast_years;
                females[areas_list[d]->id].forecast_years = this->forecast_years;

                males[areas_list[d]->id].natal_homing = this->natal_homing;
                males[areas_list[d]->id].area = areas_list[d];
                males[areas_list[d]->id].natal_area = this->natal_area;
                males[areas_list[d]->id].Initialize();
                females[areas_list[d]->id].natal_homing = this->natal_homing;
                females[areas_list[d]->id].area = areas_list[d];
                females[areas_list[d]->id].natal_area = this->natal_area;
                females[areas_list[d]->id].males = false;
                females[areas_list[d]->id].Initialize();
            }
        }

        void Prepare() {
            SN.resize(0);
            SN.resize(years * seasons * ages, static_cast<REAL_T> (0.0));
            SN_Biomass.resize(0);
            SN_Biomass.resize(years * seasons * ages, static_cast<REAL_T> (0.0));
            C.resize(0);
            C.resize(years * seasons * ages, static_cast<REAL_T> (0.0));
            C_Biomass.resize(0);
            C_Biomass.resize(years * seasons * ages, static_cast<REAL_T> (0.0));
            for (int d = 0; d < males.size(); d++) {
                males[areas_list[d]->id].Reset();
                females[areas_list[d]->id].Reset();
            }
        }

        inline void IncrementTime(int& y, int& s) {
            if (s == this->seasons) {
                y += 1;
                s = 1;
            } else {

                s++;
            }
        }

        inline void DecrementTime(int& y, int& s) {
            if (s == 1) {
                y -= 1;
                s = seasons;
            } else {

                s--;
            }
        }

        inline void MoveFish(int year, int season) {
            int y = year;
            int s = season;
            //            IncrementTime(y, s);
            movement_model_iterator it = this->movement_models.find(year + 1);
            if (it != this->movement_models.end()) {
                int ss = season - 1;
                std::vector<std::vector<variable> >& male_fractions = (*it).second->male_connectivity[ss];
                std::vector<std::vector<variable> >& female_fractions = (*it).second->female_connectivity[ss];
                std::vector<std::vector<variable> >& rercruit_fractions = (*it).second->recruit_connectivity[ss];


                //should be square
                for (int i = 0; i < male_fractions.size(); i++) {
                    AreaPopulationInfo<REAL_T>& male_info_from = this->males[(i + 1)];
                    AreaPopulationInfo<REAL_T>& female_info_from = this->females[(i + 1)];
                    for (int j = 0; j < male_fractions.size(); j++) {
                        AreaPopulationInfo<REAL_T>& male_info_to = this->males[(j + 1)];
                        AreaPopulationInfo<REAL_T>& female_info_to = this->females[(j + 1)];
                        if (i != j) {
                            variable tempm = rercruit_fractions[i][j] * male_info_from.recruitment[year * this->seasons + (season - 1)];
                            variable tempf = rercruit_fractions[i][j] * female_info_from.recruitment[year * this->seasons + (season - 1)];
                            male_info_to.redistributed_recruits[year * this->seasons + (season - 1)] += tempm;
                            male_info_to.immigrant_recruits[year * this->seasons + (season - 1)] += tempm;
                            male_info_to.emigrant_recruits[year * this->seasons + (season - 1)] += tempm;
                            female_info_to.redistributed_recruits[year * this->seasons + (season - 1)] += tempf;
                            female_info_to.immigrant_recruits[year * this->seasons + (season - 1)] += tempf;
                            female_info_to.emigrant_recruits[year * this->seasons + (season - 1)] += tempf;
                            for (int a = 0; a < this->ages; a++) {

                                // will need sex-specific Z at some point
                                size_t index = year * this->seasons * this->ages + (season - 1) * this->ages + a;
                                // move survivors only
                                male_info_from.emigrants[index] += male_fractions[i][j] * male_info_from.N[index] *
                                        atl::exp(static_cast<REAL_T> (-1.0) * male_info_from.Z[index]);
                                male_info_to.imigrants[index] += male_fractions[i][j] * male_info_from.N[index] *
                                        atl::exp(static_cast<REAL_T> (-1.0) * male_info_from.Z[index]);
                                female_info_from.emigrants[index] += female_fractions[i][j] * female_info_from.N[index] *
                                        atl::exp(static_cast<REAL_T> (-1.0) * female_info_from.Z[index]);
                                female_info_to.imigrants[index] += female_fractions[i][j] * female_info_from.N[index] *
                                        atl::exp(static_cast<REAL_T> (-1.0) * female_info_from.Z[index]);
#warning need to move biomass also
                            }
                        }
                    }
                }
            } else {

                std::cout << "Configuration Error: Population " << this->id << " has no movement model defined for year " << (year + 1) << "\n";
                mas_log << "Configuration Error: Population " << this->id << " has no movement model defined for year " << (year + 1) << "\n";
            }
        }

        void PrepareDeviations(std::vector<variable>& deviations) {
            variable sum = 0.0;
            for (int i = 0; i < deviations.size(); i++) {
                sum += deviations[i];
            }
            for (int i = 0; i < deviations.size(); i++) {

                deviations[i] -= sum / static_cast<REAL_T> (deviations.size());
            }
        }

        void InitializePopulationinAreas() {
            for (int a = 0; a < areas_list.size(); a++) {

                initial_deviations_iterator id_it = this->initial_deviations_females.find(areas_list[a]->id);
                if (id_it != this->initial_deviations_females.end()) {
                    std::vector<variable>& devs = (*id_it).second.second;
                    if ((*id_it).second.first == true) {
                        this->PrepareDeviations(devs);
                    }
                    std::vector<variable>& ldevs = females[areas_list[a]->id].initial_deviations;
                    for (int i = 0; i < ldevs.size(); i++) {
                        ldevs[i] = devs[i];
                    }
                }

                id_it = this->initial_deviations_males.find(areas_list[a]->id);
                if (id_it != this->initial_deviations_males.end()) {
                    std::vector<variable>& devs = (*id_it).second.second;
                    if ((*id_it).second.first == true) {
                        this->PrepareDeviations(devs);
                    }
                    std::vector<variable>& ldevs = males[areas_list[a]->id].initial_deviations;
                    for (int i = 0; i < ldevs.size(); i++) {

                        ldevs[i] = devs[i];
                    }
                }

                females[areas_list[a]->id].sex_fraction_value = this->female_fraction_value;
                males[areas_list[a]->id].sex_fraction_value = (1.0 - this->female_fraction_value);
                females[areas_list[a]->id].CalculateWeightAtAge();
                males[areas_list[a]->id].CalculateWeightAtAge();
                females[areas_list[a]->id].InitializeM();
                males[areas_list[a]->id].InitializeM();
                females[areas_list[a]->id].CalculateUnfishedSpawningBiomassPerRecruit();
                //                males[areas_list[a]->id].unfished_spawning_biomass_per_recruit =
                //                       females[areas_list[a]->id].unfished_spawning_biomass_per_recruit;// CalculateUnfishedSpawningBiomassPerRecruit();
                females[areas_list[a]->id].CalculateUnfishedEquilSpawningBiomass();
                //                males[areas_list[a]->id].SB0 = females[areas_list[a]->id].SB0;
                females[areas_list[a]->id].CalculateInitialNumbersEquilibrium();
                males[areas_list[a]->id].CalculateInitialNumbersEquilibrium();
                females[areas_list[a]->id].FCalc();
                males[areas_list[a]->id].initialF = females[areas_list[a]->id].initialF.GetValue();
                females[areas_list[a]->id].CalculateInitialNumbers();
                males[areas_list[a]->id].CalculateInitialNumbers();
            }
        }

        void Initialize() {
            for (int a = 0; a < areas_list.size(); a++) {
                std::unordered_set<int>::iterator fit;
                for (fit = this->active_fleets.begin(); fit != this->active_fleets.end(); ++fit) {
                    males[areas_list[a]->id].active_fleets.insert((*fit));
                    females[areas_list[a]->id].active_fleets.insert((*fit));
                }
            }
            for (int a = 0; a < areas_list.size(); a++) {
                males[areas_list[a]->id].Initialize();
                females[areas_list[a]->id].Initialize();
                std::vector<REAL_T>& mv = this->maturity_models[areas_list[a]->id][mas::MALE];
                if (mv.size() != this->ages) {
                    mv.resize(this->ages, REAL_T(.5));
                }
                males[areas_list[a]->id].maturity.resize(mv.size());
                for (int i = 0; i < mv.size(); i++) {
                    males[areas_list[a]->id].maturity[i] = mv[i];
                }
                std::vector<REAL_T>& fv = this->maturity_models[areas_list[a]->id][mas::MALE];
                if (fv.size() != this->ages) {
                    fv.resize(this->ages, REAL_T(.5));
                }
                females[areas_list[a]->id].maturity.resize(fv.size());
                for (int i = 0; i < mv.size(); i++) {
                    females[areas_list[a]->id].maturity[i] = fv[i];
                }
                int area = males[areas_list[a]->id].area->id;
                typename std::unordered_map<int, std::shared_ptr<mas::RecruitmentBase<REAL_T> > >::iterator it;
                for (it = this->area_season_recruitment[area].begin(); it != this->area_season_recruitment[area].end(); ++it) {
                    males[areas_list[a]->id].seasonal_recruitment_models[(*it).first] = (*it).second;
                }
                area = females[areas_list[a]->id].area->id;
                for (it = this->area_season_recruitment[area].begin(); it != this->area_season_recruitment[area].end(); ++it) {

                    females[areas_list[a]->id].seasonal_recruitment_models[(*it).first] = (*it).second;
                }
            }
            maturity_models_iterator mit;
        }

        void Show() {
            for (int a = 0; a < areas_list.size(); a++) {
                std::cout << males[areas_list[a]->id];
                std::cout << females[areas_list[a]->id];
            }
            std::cout << "Population: " << this->id << "\n";
            std::cout << "Expected Total Catch Numbers At Age\n";
            for (int a = 0; a < this->ages; a++) {
                for (int y = 0; y < this->years; y++) {
                    for (int s = 1; s <= this->seasons; s++) {
                        std::cout << this->C[y * this->seasons * this->ages + (s - 1) * this->ages + a] << " ";
                    }
                }
                std::cout << "\n";
            }
            std::cout << "\n\n";
            std::cout << "Population: " << this->id << "\n";
            std::cout << "Expected Total Catch Biomass At Age\n";
            for (int a = 0; a < this->ages; a++) {
                for (int y = 0; y < this->years; y++) {
                    for (int s = 1; s <= this->seasons; s++) {
                        std::cout << this->C_Biomass[y * this->seasons * this->ages + (s - 1) * this->ages + a] << " ";
                    }
                }
                std::cout << "\n";
            }
            std::cout << "\n\n";
            std::cout << "Population: " << this->id << "\n";
            std::cout << "Expected Total Survey Numbers At Age\n";
            for (int a = 0; a < this->ages; a++) {
                for (int y = 0; y < this->years; y++) {
                    for (int s = 1; s <= this->seasons; s++) {
                        std::cout << this->SN[y * this->seasons * this->ages + (s - 1) * this->ages + a] << " ";
                    }
                }
                std::cout << "\n";
            }
            std::cout << "\n\n";
            std::cout << "Population: " << this->id << "\n";
            std::cout << "Expected Total Survey Index At Age\n";
            for (int a = 0; a < this->ages; a++) {
                for (int y = 0; y < this->years; y++) {
                    for (int s = 1; s <= this->seasons; s++) {

                        std::cout << this->SN_Biomass[y * this->seasons * this->ages + (s - 1) * this->ages + a] << " ";
                    }
                }
                std::cout << "\n";
            }
            std::cout << "\n\n";
        }

        virtual const std::string Name() {

            return this->name;
        }

        virtual const std::string ToJSONString() {
            std::stringstream ss;
            ss.setf(std::ios::fixed, std::ios::floatfield);
            ss << "\"population\":{\n";
            ss << "\"id\":" << this->id << ",\n";
            ss << "\"name\":\"" << this->Name() << "\",\n";
            ss << "\"hcr\":\"" << this->harvest_control_rule->Name() << "\",\n";
            ss << "\"movement\": [\n";
            movement_model_id_iterator mit;
            for (mit = this->movement_models_ids.begin(); mit != this->movement_models_ids.end(); ++mit) {
                if (std::next(mit) != this->movement_models_ids.end()) {
                    ss << "{\n";
                    ss << "  \"year\": " << (*mit).first << ",\n";
                    ss << "  \"id\": " << (*mit).second << "\n";
                    ss << "},\n";
                } else {
                    ss << "{\n";
                    ss << "  \"year\": " << (*mit).first << ",\n";
                    ss << "  \"id\": " << (*mit).second << "\n";
                    ss << "}\n";
                }
            }
            ss << "],\n";
            ss << "\"maturity\": [\n";
            maturity_models_iterator mmit;
            for (mmit = this->maturity_models.begin(); mmit != this->maturity_models.end(); ++mmit) {
                area_maturity_model_iterator ammit;
                for (ammit = (*mmit).second.begin(); ammit != (*mmit).second.end(); ++ammit) {
                    ss << "{\n";
                    ss << "    \"sex\": " << ((0 == (*ammit).first) ? "\"female\",\n" : "\"male\",\n");
                    ss << "    \"area\": " << (*mmit).first << ",\n";
                    ss << "    \"values\":[\n";
                    for (int i = 0; i < (*ammit).second.size() - 1; i++) {
                        ss << "                 " << (*ammit).second.at(i) << ",\n";
                    }
                    ss << "                 " << (*ammit).second.at((*ammit).second.size() - 1) << "\n";
                    ss << "               ] \n";
                    if ((std::next(mmit) == this->maturity_models.end()) && std::next(ammit) == (*mmit).second.end()) {
                        ss << "}\n";
                    } else {
                        ss << "},\n";
                    }
                }
            }
            ss << "],\n";
            ss << "\"parameters\": {\n";
            ss << "\"growth\": " << this->growth_id << ",\n";
            ss << "\"natural_mortality\": [ \n";
            male_natural_mortality_ids_iterator nmmit;
            for (nmmit = this->male_natural_mortality_ids.begin();
                    nmmit != this->male_natural_mortality_ids.end(); ++nmmit) {
                ss << "{\n";
                ss << "   \"id\": " << (*nmmit).second << ",\n";
                ss << "   \"area\": " << (*nmmit).first << ",\n";
                ss << "   \"sex\": \"male\"\n},";
            }
            female_natural_mortality_ids_iterator nfmit;
            for (nfmit = this->female_natural_mortality_ids.begin();
                    nfmit != this->female_natural_mortality_ids.end(); ++nfmit) {
                ss << "{\n";
                ss << "   \"id\": " << (*nfmit).second << ",\n";
                ss << "   \"area\": " << (*nfmit).first << ",\n";
                if (std::next(nfmit) == this->female_natural_mortality_ids.end()) {
                    ss << "   \"sex\": \"female\"\n}\n";
                } else {
                    ss << "   \"sex\": \"female\"\n},\n";
                }
            }
            ss << "],\n";
            ss << "\"recruitment\" : [\n";
            typename Population<REAL_T>::season_area_id_iterator sait;
            typename std::unordered_map<int, int>::iterator it;
            for (sait = this->season_area_recruitment_ids.begin(); sait != this->season_area_recruitment_ids.end(); ++sait) {
                int season = (*sait).first;
                for (it = (*sait).second.begin(); it != (*sait).second.end(); ++it) {
                    int area = (*it).first;
                    int id = (*it).second;
                    ss << "{\n";
                    ss << "   \"season\" : " << season << ",\n";
                    ss << "   \"area\" : " << area << ",\n";
                    ss << "   \"id\" : " << id << "\n";
                    if (std::next(it) != (*sait).second.end()) {
                        ss << "},\n";
                    } else {
                        ss << "}\n";
                    }
                }
                if (std::next(sait) != this->season_area_recruitment_ids.end()) {
                    ss << ",\n";
                } else {
                    ss << "\n";
                }
            }
            ss << "]";
            ss << "}\n";
            ss << "}";

            return ss.str();
        }

        void Evaluate() {
            InitializePopulationinAreas();
            for (int y = 0; y < this->years; y++) {
                for (int s = 1; s <= this->seasons; s++) {
                    for (int area = 0; area < areas_list.size(); area++) {

                        /******************************************
                         * Mortality
                         *****************************************/
                        males[areas_list[area]->id].CalculateMortality(y, s);
                        females[areas_list[area]->id].CalculateMortality(y, s);
                        /******************************************
                         * Numbers at Age
                         *****************************************/
                        males[areas_list[area]->id].CalculateNumbersAtAge(y, s);
                        females[areas_list[area]->id].CalculateNumbersAtAge(y, s);
                        /******************************************
                         * Spawning Biomass
                         *****************************************/
                        males[areas_list[area]->id].CalculateSpawningBiomass(y, s);
                        females[areas_list[area]->id].CalculateSpawningBiomass(y, s);
                        /******************************************
                         * Recruitment
                         *****************************************/
                        males[areas_list[area]->id].CalculateRecruitment(y, s);
                        females[areas_list[area]->id].CalculateRecruitment(y, s);
                        /******************************************
                         * Catch Numbers at Age
                         *****************************************/
                        males[areas_list[area]->id].CalculateCatchAtAge(y, s);
                        females[areas_list[area]->id].CalculateCatchAtAge(y, s);
                        /******************************************
                         * Survey Numbers at Age
                         *****************************************/
                        males[areas_list[area]->id].CalculateSurveyNumbersAtAge(y, s);
                        females[areas_list[area]->id].CalculateSurveyNumbersAtAge(y, s);
                    }// end area

                    /******************************************
                     * Apply Movement
                     *****************************************/
                    this->MoveFish(y, s);


                    /******************************************
                     * Settle moved fish
                     *****************************************/
                    //                    for (int d = 0; d < areas_list.size(); d++) {
                    //                        males[areas_list[d]->id].SettleMovedFish(y, s);
                    //                        females[areas_list[d]->id].SettleMovedFish(y, s);
                    //                    }//end season
                }//end year

            }
            //            for (int d = 0; d < areas_list.size(); d++) {
            //                males[areas_list[d]->id].CalculateSPR();
            //                std::cout << "\n\n";
            //                females[areas_list[d]->id].CalculateSPR();
            //                std::cout << "\n\n";
            //            }

            //                        exit(0);

            /**
             * Compute totals for this population
             */
            //            for (int y = 0; y < this->years; y++) {
            //                for (int s = 1; s <= this->seasons; s++) {
            //
            //                    for (int al = 0; al < areas_list.size(); al++) {
            //                        for (int a = 0; a < this->ages; a++) {
            //                            this->C[y * this->seasons * this->ages + (s - 1) * this->ages + a] +=
            //                                    males[areas_list[al]->id].C[y * this->seasons * this->ages + (s - 1) * this->ages + a] +
            //                                    females[areas_list[al]->id].C[y * this->seasons * this->ages + (s - 1) * this->ages + a];
            //
            //                            this->C_Biomass[y * this->seasons * this->ages + (s - 1) * this->ages + a] +=
            //                                    males[areas_list[al]->id].C_Biomass[y * this->seasons * this->ages + (s - 1) * this->ages + a] +
            //                                    females[areas_list[al]->id].C_Biomass[y * this->seasons * this->ages + (s - 1) * this->ages + a];
            //
            //                            this->SN[y * this->seasons * this->ages + (s - 1) * this->ages + a] +=
            //                                    males[areas_list[al]->id].SN[y * this->seasons * this->ages + (s - 1) * this->ages + a] +
            //                                    females[areas_list[al]->id].SN[y * this->seasons * this->ages + (s - 1) * this->ages + a];
            //
            //                            this->SN_Biomass[y * this->seasons * this->ages + (s - 1) * this->ages + a] +=
            //                                    males[areas_list[al]->id].SN_Biomass[y * this->seasons * this->ages + (s - 1) * this->ages + a] +
            //                                    females[areas_list[al]->id].SN_Biomass[y * this->seasons * this->ages + (s - 1) * this->ages + a];
            //                        }
            //                    }
            //                }
            //            }
      
        }

        void Forecast() {
            for (int y = 0; y < this->forecast_years; y++) {
                for (int s = 1; s <= this->seasons; s++) {
                    for (int d = 0; d < areas_list.size(); d++) {





                    }
                }
            }
        }

        void ComputeBiologicalReferencePoints() {
            std::cout << std::fixed;
            std::vector<double> fs;
            for (double f = 0.01; f < 3.0; f += 0.01) {
                fs.push_back(f);
            }
            for (int a = 0; a < areas_list.size(); a++) {

                REAL_T msy;
                REAL_T f_msy;
                REAL_T s_msy;
                /******************************************
                 * Push info to areas
                 *****************************************/
                males[areas_list[a]->id].CalculateMaximumSustainableYield(msy, f_msy, s_msy);
                males[areas_list[a]->id].CalculateFishedSpawningBiomassPerRecruit(0.1);
                std::cout << "Male MSY:\n";
                std::cout << "alpha = " << males[areas_list[a]->id].recruitment_model->GetAlpha() << "\n";
                std::cout << "beta = " << males[areas_list[a]->id].recruitment_model->GetBeta() << "\n";
                std::cout << "Fmax = " << males[areas_list[a]->id].CalculateFMax(fs) << "\n";
                std::cout << "msy = " << msy << "\n";
                std::cout << "f_msy = " << f_msy << "\n";
                std::cout << "s_msy = " << s_msy << "\n\n";

                females[areas_list[a]->id].CalculateMaximumSustainableYield(msy, f_msy, s_msy);
                std::cout << "Female MSY:\n";
                std::cout << "unfished_spr = " << females[areas_list[a]->id].unfished_spawning_biomass_per_recruit << "\n";
                std::cout << "alpha = " << females[areas_list[a]->id].recruitment_model->GetAlpha() << "\n";
                std::cout << "beta = " << females[areas_list[a]->id].recruitment_model->GetBeta() << "\n";
                std::cout << "Fmax = " << females[areas_list[a]->id].CalculateFMax(fs) << "\n";
                std::cout << "msy = " << msy << "\n";
                std::cout << "f_msy = " << f_msy << "\n";
                std::cout << "s_msy = " << s_msy << "\n\n";

                males[areas_list[a]->id].BuildBootStrapNumbers();
                females[areas_list[a]->id].BuildBootStrapNumbers();

            }


        }

        void PushToAreasAndFleets() {
            for (int a = 0; a < areas_list.size(); a++) {

                /******************************************
                 * Push info to areas
                 *****************************************/
                males[areas_list[a]->id].PushToArea();
                males[areas_list[a]->id].PushNumbersToFleet();
                females[areas_list[a]->id].PushToArea();
                females[areas_list[a]->id].PushNumbersToFleet();
            }
        }

        void PushToFleets() {
            for (int a = 0; a < areas_list.size(); a++) {

                /******************************************
                 * Push info to areas
                 *****************************************/
                males[areas_list[a]->id].PushNumbersToFleet();
                females[areas_list[a]->id].PushNumbersToFleet();
            }
        }
    };

    template<typename REAL_T >
    std::string ToHTML(mas::Population<REAL_T>& pop) {
        std::stringstream out;
        out << "<br>Population Name: " << pop.name;
        out << "<br>Population Id: " << pop.id;
        out << "<br>Population Area Info: ";
        for (int d = 0; d < pop.areas_list.size(); d++) {

            out << mas::ToHtml(pop.males[pop.areas_list[d]->id]);
            out << mas::ToHtml(pop.females[pop.areas_list[d]->id]);
        }
        return out.str();
    }

    template<typename REAL_T >
    std::string ToLatexCharts(mas::Population<REAL_T>& pop) {
        std::stringstream out;


        for (int d = 0; d < pop.areas_list.size(); d++) {

            out << mas::AddLatexCharts(pop.females[pop.areas_list[d]->id]) << "\n\n";
            out << mas::AddLatexCharts(pop.males[pop.areas_list[d]->id]) << "\n\n";
        }
        return out.str();
    }

    template<typename REAL_T >
    std::ostream& operator<<(std::ostream& out, mas::Population<REAL_T>& pop) {
        typedef typename mas::VariableTrait<REAL_T>::variable variable;
        out << "Population Name: " << pop.name << "\n";
        out << "Population Id: " << pop.id << "\n";
        //        out << "Population Natal Area: " << pop.natal_area->id << "\n";
        out << "Population Area Info: ";
        for (int d = 0; d < pop.areas_list.size(); d++) {
            out << pop.males[pop.areas_list[d]->id] << " \n\n";
            out << pop.females[pop.areas_list[d]->id] << " \n\n";
        }
        out << "\n\n";
        out << "Population HCR values\n";
        out << "Population Name: " << pop.name << "\n";
        out << "Population Id: " << pop.id << "\n";
        //        out << "Population Harvest Control Rule Name: " << pop.harvest_control_rule->Name() << "\n";
        //        for (int d = 0; d < pop.areas_list.size(); d++) {
        //            auto m = pop.males[pop.areas_list[d]->id];
        //            auto f = pop.females[pop.areas_list[d]->id];
        //            m.NumbersAtAgeEndYearPlusOne();
        //            f.NumbersAtAgeEndYearPlusOne();
        //            /*
        //            std::cout << "Male projection numbers-at-age";
        //            for (int a = 0; a < pop.ages; ++a)
        //            {
        //                std::cout << "\t" << m.N_proj[a];
        //            }
        //            std::cout << std::endl;
        //            std::cout << "Female projection numbers-at-age";
        //            for (int a = 0; a < pop.ages; ++a)
        //            {
        //                std::cout << "\t" << f.N_proj[a];
        //            }
        //            std::cout << std::endl;
        //             */
        //            out << "\nArea Id: " << pop.areas_list[d]->id << "\n";
        //            auto HCR = pop.harvest_control_rule->Evaluate(f.years, m, f);
        //            out << "HCR F_ABC: " << std::get<0>(HCR) << "\n";
        //            out << "HCR ABC: " << std::get<1>(HCR) << "\n";
        //            out << "HCR F_OFL: " << std::get<2>(HCR) << "\n";
        //            out << "HCR OFL: " << std::get<3>(HCR) << "\n";
        //        }
        //        out << "\n\n";
        //        out << "\n*********************************************************************************************************************\n";
        //        out << "\n\n";
        return out;
    }
}

/** @} */ // end of group pop_dynamics
#endif /* POPULATION_HPP */