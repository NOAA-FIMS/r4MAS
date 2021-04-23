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
#include "MaximumSustainableYield.hpp"
#include "third_party/ATL/lib/Variable.hpp"
namespace mas {

    /** @defgroup pop_dynamics Population Dynamics
     * 
     *  @{
     */


    template<class REAL_T>
    class Population;

    /**
     * Run time calculated information for a population in a specific area, by sex.
     */
    template<class REAL_T>
    struct Subpopulation {
        typedef typename mas::VariableTrait<REAL_T>::variable variable;
        int id;
        int uid = -9999;
        mas::FishSexType sex;
        std::vector<variable> ages;
        int spawning_season = 1;

        /*  NOTE:  these values need to be defined elsewhere*/
        variable spawning_season_offset = 0.0;
        variable catch_season_offset = 1.0;
        variable survey_season_offset = 1.0;

        variable kg_to_tonnes = 1000.0; // convert kg to metric tonnes
        REAL_T sex_fraction_value = 0.5;
        /*==================================*/
        bool males = true;

        Population<REAL_T>* natal_population;
        bool natal_homing = false;
        std::shared_ptr<Area<REAL_T> > area;
        std::shared_ptr<Area<REAL_T> > natal_area;
        std::shared_ptr<GrowthBase<REAL_T> > growth_model;
        std::shared_ptr<mas::NaturalMortality<REAL_T> > natural_mortality_model; // population - area specific
        std::shared_ptr<mas::RecruitmentBase<REAL_T> > recruitment_model; // population - area specific
        std::unordered_map<int, std::shared_ptr<mas::RecruitmentBase<REAL_T> > > seasonal_recruitment_models;


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
        variable unfished_spawning_biomass_per_recruit;
        variable unfished_spawners_per_recruit;
        variable fished_spawning_biomass_per_recruit;
        variable fished_spawners_per_recruit;
        variable fished_yield_per_recruit;
        variable fished_max_yield_per_recruit;

        //biological reference points
        std::vector<variable> F100;
        std::vector<variable> SB100;
        std::vector<variable> F40;
        std::vector<variable> SB40;
        std::vector<variable> F35;
        std::vector<variable> SB35;
        std::vector<variable> F20;
        std::vector<variable> S20;
        std::vector<variable> SB_target;
        variable avgR;


        variable SB0; // derived variable, retrieve from the recruitment functor at finalize

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
        std::vector<variable> spawning_stock_biomass;
        std::vector<variable> redistributed_recruits;
        std::vector<variable> immigrant_recruits;
        std::vector<variable> immigrant_recruits_biomass;
        std::vector<variable> emigrant_recruits;
        std::vector<variable> emigrant_recruits_biomass;
        std::vector<variable> imigrants;
        std::vector<variable> imigrants_biomass;
        std::vector<variable> emigrants;
        std::vector<variable> emigrants_biomass;
        std::vector<variable> growth;
        std::vector<variable> recruitment;
        std::vector<variable> spawning_numbers_at_age;
        std::vector<variable> biomass_total;
        std::vector<variable> fishing_mortality_total;
        std::vector<variable> sum_selectivity;

        std::vector<variable> Z;
        std::vector<variable> P; //selectivity at age.
        variable sum_of_Z;
        std::vector<variable> S;

        std::vector<variable> numbers_at_age;
        std::vector<variable> biomass_at_age;
        std::vector<variable> abundance;


        std::vector<variable> survey_numbers_at_age; //survey numbers at age
        std::vector<variable> survey_index_at_age; //survey index at age
        std::vector<REAL_T> survey_biomass_total; //survey index total

        std::vector<variable> catch_at_age;
        std::vector<variable> catch_biomass_at_age;
        std::vector<REAL_T> catch_biomass_total;



        std::vector<variable> expected_N;
        std::vector<variable> N_proj; // numbers-at-age in endyear + 1

        static uint32_t length_weight_key_carryout;

        MaximumSustainableYield<REAL_T> msy;

        REAL_T MSY; //     maximum sustainable yield
        REAL_T Dmsy; //    dead discards at MSY
        REAL_T Fmsy; //    fishing rate at MSY
        REAL_T SSBmsy; //  spawning stock biomass at msy
        REAL_T Rmsy; //    equilibrium recruitment at msy
        REAL_T Bmsy; //    total biomass (male and female) at msy
        REAL_T Emsy; //    exploitation rate at msy (total catch / number of fish)
        REAL_T spr_msy; // spawners per recruit at msy
        REAL_T SPRmsy; //  spawning potential ratio (spr_msy/spr_virgin)


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
            spawning_numbers_at_age.resize(years * seasons * ages.size());
            spawning_stock_biomass.resize(years * seasons);
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
            this->sum_selectivity.resize(years * seasons * this->ages.size());
            this->fishing_mortality_total.resize(years * seasons);
            //            F_at_age.resize(years * seasons * ages.size());
#warning needs to be more efficient, possibly allocating unused memory
            std::unordered_set<int>::iterator fit;
            for (fit = this->active_fleets.begin(); fit != this->active_fleets.end(); ++fit) {
                F_at_age[(*fit)].resize(years * seasons * ages.size());
                Z_at_age[(*fit)].resize(years * seasons * ages.size());
            }
            emigrants.resize(years * seasons * ages.size());
            emigrants_biomass.resize(years * seasons * ages.size());
            imigrants.resize(years * seasons * ages.size());
            imigrants_biomass.resize(years * seasons * ages.size());
            growth.resize(years * seasons * ages.size());
            Z.resize(years * seasons * ages.size());
            F.resize(years * seasons * ages.size());
            P.resize(years * seasons * ages.size());
            S.resize(years * seasons * ages.size());
            biomass_total.resize(years * seasons);
            survey_numbers_at_age.resize(years * seasons * ages.size());
            survey_index_at_age.resize(years * seasons * ages.size());
            survey_biomass_total.resize(years, seasons);
            numbers_at_age.resize((years + 1) * seasons * ages.size());
            biomass_at_age.resize(years * seasons * ages.size());
            catch_at_age.resize(years * seasons * ages.size());
            catch_biomass_at_age.resize(years * seasons * ages.size());
            catch_biomass_total.resize(years * seasons);
            expected_N.resize(years * seasons * ages.size());
            N_proj.resize(seasons * ages.size());
        }

        inline void Reset() {


            std::fill(this->catch_biomass_total.begin(), this->catch_biomass_total.end(), static_cast<REAL_T> (0.0));
            std::fill(this->survey_biomass_total.begin(), this->survey_biomass_total.end(), static_cast<REAL_T> (0.0));
            for (int i = 0; i < numbers_at_age.size(); i++) {
                mas::VariableTrait<REAL_T>::SetValue(numbers_at_age[i], static_cast<REAL_T> (0.0));
            }

            for (int i = 0; i < survey_numbers_at_age.size(); i++) {
                mas::VariableTrait<REAL_T>::SetValue(emigrants[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(emigrants_biomass[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(imigrants[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(imigrants_biomass[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(growth[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(Z[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(F[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(P[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(S[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(biomass_at_age[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(survey_numbers_at_age[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(survey_index_at_age[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(catch_at_age[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(catch_biomass_at_age[i], static_cast<REAL_T> (0.0));
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
                mas::VariableTrait<REAL_T>::SetValue(spawning_stock_biomass[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(biomass_total[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(fishing_mortality_total[i], static_cast<REAL_T> (0.0));
            }
            for (int i = 0; i < this->initial_numbers.size(); i++) {
                mas::VariableTrait<REAL_T>::SetValue(initial_numbers[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(initial_equilibrium_numbers[i], static_cast<REAL_T> (0.0));

            }

            for (int i = 0; i < this->sum_selectivity.size(); i++) {
                mas::VariableTrait<REAL_T>::SetValue(this->sum_selectivity[i], static_cast<REAL_T> (0.0));
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
                    for (int a = 0; a < ages.size(); ++a) {

                        //dimension folded index
                        size_t index = y * this->seasons * this->ages.size() + s * this->ages.size() + a;

                        // length at age
                        length_at_season_start[index] = length_at_season_start_temp[a];
                        length_at_spawning[index] = length_at_spawning_temp[a];
                        length_at_catch_time[index] = length_at_catch_time_temp[a];
                        length_at_survey_time[index] = length_at_survey_time_temp[a];


                        //weight at season start
                        this->growth_model->GetWeight(y, s,
                                this->length_at_season_start[a],
                                ages[a].GetValue(),
                                this->sex, this->id, mas::MEAN_WEIGHT_AT_AGE_SEASON_START, weight_at_season_start[index]);

                        //weight at spawning
                        this->growth_model->GetWeight(y, s,
                                this->length_at_spawning[a],
                                ages[a].GetValue() + this->spawning_season_offset.GetValue(),
                                this->sex, this->id, mas::MEAN_WEIGHT_AT_AGE_SPAWNING, weight_at_spawning[index]);


                        this->fecundity_at_age[index] = this->weight_at_spawning[index] * this->maturity[a];

                        //weight at catch time
                        this->growth_model->GetWeight(y, s,
                                this->length_at_catch_time[a],
                                ages[a].GetValue() + this->catch_season_offset.GetValue(),
                                this->sex, this->id, mas::CATCH_MEAN_WEIGHT_AT_AGE, weight_at_catch_time[index]);

                        //weight at survey time
                        this->growth_model->GetWeight(y, s,
                                this->length_at_survey_time[a],
                                ages[a].GetValue() + this->survey_season_offset.GetValue(),
                                this->sex, this->id, mas::SURVEY_MEAN_WEIGHT_AT_AGE, weight_at_survey_time[index]);

                    }

                }
            }

        }

        inline REAL_T CalculateUnfishedSpawningBiomassPerRecruit() {


            std::vector<variable > sb_per_recruit(this->years);
            std::vector<variable > s_per_recruit(this->years);
            for (int y = 0; y < this->years; y++) {

                variable ntemp0 = 1.0;
                for (int s = 0; s < this->seasons; s++) {
                    for (int a = 0; a < ages.size() - 1; a++) {
                        size_t index = y * this->seasons * this->ages.size() + (s) * this->ages.size() + a;

                        sb_per_recruit[y] += ntemp0 * (this->weight_at_spawning[index] * this->M[a]) *
                                mas::mfexp(-1.0 * this->spawning_season_offset * M[a]);

                        s_per_recruit[y] += ntemp0 * (this->fecundity_at_age[index] * this->M[a]) *
                                mas::mfexp(-1.0 * this->spawning_season_offset * M[a]);

                        ntemp0 *= mas::mfexp(-1.0 * this->spawning_season_offset * M[a]);

                    }


                    size_t plus_group = this->ages.size() - 1;
                    size_t index = y * this->seasons * this->ages.size() + (s) * this->ages.size() + plus_group;
                    ntemp0 /= (1.0 - mas::mfexp(-1.0 * M[plus_group]));

                    sb_per_recruit[y] += ntemp0 * (this->weight_at_spawning[index] * this->M[this->ages.size() - 1])
                            * mas::mfexp(-1.0 * this->spawning_season_offset * M[plus_group]);

                    s_per_recruit[y] += ntemp0 * (this->fecundity_at_age[index] * this->M[this->ages.size() - 1])
                            * mas::mfexp(-1.0 * this->spawning_season_offset * M[plus_group]);

                }
            }

            this->unfished_spawning_biomass_per_recruit = (sb_per_recruit[this->years - 1]);
            this->unfished_spawners_per_recruit = (s_per_recruit[this->years - 1]);

            variable ret = sb_per_recruit[this->years - 1];

            return ret.GetValue();
        }

        inline REAL_T CalculateFishedSpawningBiomassPerRecruit_(REAL_T F) {


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
                        s_per_recruit[y] += ntemp0 * (this->weight_at_spawning[index] * this->M[a]) *
                                mas::mfexp(-1.0 * this->spawning_season_offset * M[a]
                                - z_sum - P[index] * faa[index]);
                        ntemp0 *= mas::mfexp(-1.0 * this->spawning_season_offset * M[a]);

                    }//end a

                    //plus group
                    size_t plus_index = this->ages.size() - 1;
                    size_t index = y * this->seasons * this->ages.size() + (s) * this->ages.size() + plus_index;

                    ntemp0 /= (1.0 - mas::mfexp(-1.0 * M[plus_index]));

                    s_per_recruit[y] += ntemp0 * (this->weight_at_spawning[index] * this->M[this->ages.size() - 1])
                            * mas::mfexp(-1.0 * M[plus_index] -
                            z_sum - this->spawning_season_offset * P[index] * faa[index]);
                }//end s
            }//end y

            this->fished_spawning_biomass_per_recruit = s_per_recruit[this->years - 1] * this->sex_fraction_value;
            variable ret = s_per_recruit[this->years - 1] * this->sex_fraction_value;
            return ret.GetValue();
        }

        inline REAL_T CalculateFishedSpawningBiomassPerRecruit(int year, int season, REAL_T F) {
            for (int i = 0; i < this->ages.size(); i++) {
                size_t index = year * this->seasons * this->ages.size() + season * this->ages.size() + i;
                REAL_T temp = 0.0;
                for (int j = 0; j < i; j++) {
                    size_t index2 = year * this->seasons * this->ages.size() + season * this->ages.size() + j;
                    temp += F * this->S[index2].GetValue() + this->M[j].GetValue();
                }
            }
        }

        inline REAL_T CalculateYieldPerRecruit(REAL_T F) {

            REAL_T ret;
            for (int i = 0; i < this->ages.size(); i++) {
                size_t index = (this->years - 1) * this->seasons * this->ages.size() + (this->seasons - 1) * this->ages.size() + i;
                ret += ((this->weight_at_catch_time[index].GetValue() * F * this->sum_selectivity[index].GetValue()) /
                        (F * this->sum_selectivity[index].GetValue() + this->M[i].GetValue())) * this->CalculateProbabilityOfCaptureAtAge(i, this->years - 1, this->seasons - 1, F) *
                        this->CalculateProbabilityOfSurvivalToAge(i, this->years - 1, this->seasons - 1, F);


                std::cout << ((this->weight_at_catch_time[index].GetValue() * F * this->sum_selectivity[index].GetValue()) /
                        (F * this->sum_selectivity[index].GetValue() + this->M[i].GetValue())) << " * " << this->CalculateProbabilityOfCaptureAtAge(i, this->years - 1, this->seasons - 1, F) << " * " <<
                        this->CalculateProbabilityOfSurvivalToAge(i, this->years - 1, this->seasons - 1, F) << std::endl;
                //                        +
                //                        this->CalculateProbabilityOfCaptureAtAgeAOrOlder(this->ages.size() - 1, this->years - 1, this->seasons - 1, F) *
                //                        this->CalculateProbabilityOfSurvivalToAge(i, this->years - 1, this->seasons - 1, F);
            }

            return ret + this->CalculateProbabilityOfCaptureAtAgeAOrOlder(this->ages.size() - 1, this->years - 1, this->seasons - 1, F) *
                    this->CalculateProbabilityOfSurvivalToAge(this->ages.size() - 1, this->years - 1, this->seasons - 1, F);

        }

        inline REAL_T CalculateProbabilityOfSurvivalToAge(int a, int year, int season, REAL_T F) {
            REAL_T sum_z = 0;
            for (int i = 0; i < a; i++) {
                size_t index = year * this->seasons * this->ages.size() + (season) * this->ages.size() + i;
                sum_z += F * this->sum_selectivity[index].GetValue() + this->M[i].GetValue();
            }
            return std::exp(-1.0 * sum_z);
        }

        inline REAL_T CalculateProbabilityOfCaptureAtAge(int a, int year, int season, REAL_T F) {
            size_t index = year * this->seasons * this->ages.size() + (season) * this->ages.size() + a;
            return (1.0 - std::exp(-1.0 * F * this->Z[index].GetValue()));
        }

        inline REAL_T CalculateProbabilityOfCaptureAtAgeAOrOlder(int a, int year, int season, REAL_T F) {
            size_t index = year * this->seasons * this->ages.size() + (season) * this->ages.size() + a;
            return (this->weight_at_catch_time[index].GetValue() * F * this->sum_selectivity[index].GetValue()) /
                    (F * this->sum_selectivity[index].GetValue() + this->M[a].GetValue());
        }

        inline REAL_T CalculateSPR(REAL_T F) {
            variable ret = this->CalculateFishedSpawningBiomassPerRecruit(this->years - 1, this->seasons - 1, F) / this->CalculateUnfishedSpawningBiomassPerRecruit();
            return ret.GetValue();
        }

        inline REAL_T CalculateFSPR(REAL_T fraction) {
            REAL_T distance = std::numeric_limits<REAL_T>::max();
            REAL_T fspr = 0.0;
            for (REAL_T f = 0.0; f < 3.0; f += 0.01) {

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
            REAL_T sbpr = this->CalculateFishedSpawningBiomassPerRecruit(this->years - 1, this->seasons - 1, F);
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

            //note initial_numbers and initial_equilibrium_numbers vectors
            //are both size 3*ages.size()


            this->R0 = this->sex_fraction_value * mas::exp(this->recruitment_model->log_R0);
            this->initial_equilibrium_numbers[0] = this->sex_fraction_value * mas::exp(this->recruitment_model->log_R0);


            int a;
            for (a = 1; a < this->ages.size(); a++) {
                this->initial_equilibrium_numbers[a] = this->initial_equilibrium_numbers[a - 1] *
                        mas::exp(static_cast<REAL_T> (-1.0) * (this->M[a - 1]));
            }
            variable m = this->M[a - 1];
            variable sum;
            for (a = this->ages.size(); a < this->initial_equilibrium_numbers.size(); a++) {
                this->initial_equilibrium_numbers[a] = this->initial_equilibrium_numbers[a - 1] * mas::exp(static_cast<REAL_T> (-1.0) * (m));
                sum += this->initial_equilibrium_numbers[a];
            }

            this->initial_equilibrium_numbers[ this->ages.size() - 1] += sum +
                    (this->initial_equilibrium_numbers[this->initial_equilibrium_numbers.size() - 1] * mas::exp(static_cast<REAL_T> (-1.0) * (m))) /
                    (1.0 - mas::exp(static_cast<REAL_T> (-1.0) * (m)));
        }

        void CalculateInitialNumbers() {
            //            variable sigma_r = this->recruitment_model->sigma_r;
            int a;
            for (a = 0; a < this->ages.size(); a++) {
                this->initial_numbers[a] = (mas::exp(static_cast<REAL_T> (-1.0) *
                        this->M[a] - this->initialF) * this->initial_equilibrium_numbers[a] *
                        mas::exp(initial_deviations[a] - static_cast<REAL_T> (0.5)
                        /* mas::pow(sigma_r, static_cast<REAL_T> (2.0))*/));
            }

            //            /*
            //                this->initial_numbers[a] = (mas::exp(static_cast<REAL_T> (-1.0) *
            //                        this->M[a] - this->initialF) * this->initial_equilibrium_numbers[a]/*
            //                        mas::exp(initial_deviations[a] - static_cast<REAL_T> (0.5))*/);
            //             */

        }

        void CalculateUnfishedEquilSpawningBiomass() {
            if (this->sex == FEMALE) {

                this->R0 = this->sex_fraction_value * mas::mfexp(this->recruitment_model->log_R0);
                this->S0 = this->unfished_spawners_per_recruit * this->R0;
                variable temp;

                for (int a = 0; a < this->ages.size(); a++) {
                    temp += this->initial_equilibrium_numbers[a] *
                            this->weight_at_spawning[a] * this->maturity[a] *
                            mas::exp(-1.0 * this->spawning_season_offset * this->M[a]);
                }
                this->recruitment_model->SB0[this->id][this->area->id] = temp;
                this->SB0 = temp;
            }
        }

        variable spr_unfished(int year) {

            avgR = 0.0;
            variable Ntmp = 1.0;
            variable SBtmp;
            size_t a = 0;
            size_t s = 0;
            for (size_t i = 0; i < year; i++) {
                size_t index = year * this->seasons * this->ages.size() + s * this->ages.size() + a;
                avgR += this->numbers_at_age[index];
            }
            avgR /= (REAL_T)this->years;
            for (a = 0; a < this->ages.size(); a++) {
                size_t index = year * this->seasons * this->ages.size() + s * this->ages.size() + a;
                SBtmp += (Ntmp * this->maturity[a] * this->weight_at_spawning[index] * mas::exp(-1.0 * this->spawning_season_offset * this->M[a]));
                Ntmp *= (mas::exp(-1.0 * this->M[a]));
            }
            size_t index = year * this->seasons * this->ages.size() + s * this->ages.size() + a - 1;
            Ntmp /= (mas::exp(-1.0 * this->M[a - 1]));
            SBtmp += (Ntmp * this->maturity[a - 1] * this->weight_at_spawning[index] * mas::exp(-1.0 * this->spawning_season_offset * this->M[a - 1]));

            return SBtmp;
        }

        variable spr_ratio(const variable& trial_F, int curryr) {

            std::vector<variable> Ntmp(this->ages.size());
            std::vector<variable> srvtmp(this->ages.size());
            std::vector<variable> Ftmp(this->ages.size());

            variable SBcurr;
            variable SB0;
            variable phi0;
            size_t jj = 0;

            size_t s = 0;
            for (jj = 0; jj < this->ages.size(); jj++) {
                size_t index = curryr * this->seasons * this->ages.size() + s * this->ages.size() + jj;

                Ftmp[jj] = this->P[index] * trial_F;
                srvtmp[jj] = Ftmp[jj] + this->M[jj];
            }
            SB0 = spr_unfished(curryr);
            phi0 = SB0 / avgR;

            jj = 0;
            size_t index = curryr * this->seasons * this->ages.size() + s * this->ages.size() + jj;
            Ntmp[jj] = avgR;
            SBcurr += (Ntmp[jj] * this->maturity[jj] * this->weight_at_spawning[index] * mas::exp(-1.0 * this->spawning_season_offset * srvtmp[jj]));

            for (jj = 1; jj <this->ages.size(); jj++) {
                size_t index = curryr * this->seasons * this->ages.size() + s * this->ages.size() + jj;
                Ntmp[jj] = Ntmp[jj - 1] * mas::exp(-1.0 * srvtmp[jj - 1]);
                SBcurr += (Ntmp[jj] * this->maturity[jj] * this->weight_at_spawning[index] * mas::exp(-1.0 * this->spawning_season_offset * srvtmp[jj]));
            }
            size_t end_age = this->ages.size() - 1;

            Ntmp[end_age] = (Ntmp[end_age - 1] * mas::exp(-srvtmp[end_age - 1]) / (1.0 - mas::exp(-1.0 * srvtmp[end_age])));
            SBcurr += (Ntmp[end_age] * this->maturity[jj] * this->weight_at_spawning[index] * mas::exp(-1.0 * this->spawning_season_offset * srvtmp[end_age]));
            return (SBcurr / SB0);
        }

        variable get_spr_rates(const variable& spr_percent, int curryr) {
            REAL_T df = 1.e-3;
            variable F1, F2, F3;
            variable yld1, yld2, yld3;
            variable dyld, dyldp;
            F1 = 0.3;

            for (int ii = 1; ii <= 10; ii++) // arbitrary fixed intervals
            {
                F2 = F1 + df;
                F3 = F1 - df;
                yld1 = /*-1000. */ mas::pow(mas::log(spr_percent / spr_ratio(F1, curryr)), 2.0);
                yld2 = /*-1000. */ mas::pow(mas::log(spr_percent / spr_ratio(F2, curryr)), 2.0);
                yld3 = /*-1000. */ mas::pow(mas::log(spr_percent / spr_ratio(F3, curryr)), 2.0);
                dyld = (yld2 - yld3) / (2. * df); // First derivative (to find the root of this)
                dyldp = (yld3 - (2. * yld1) + yld2) / (df * df); // Newton-Raph approximation 2nd deriv
                F1 -= (dyld / dyldp);
            }
            return F1;
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
                        temp_n += this->numbers_at_age[index].GetValue();
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

        //        /**
        //         *  Calculate the F required to harvest the observed catch biomass
        //         *  by 1 fleet in one time period (year) in 1 area with 1 population 
        //         *  using bisection search*/
        //        inline void FitInitialF(size_t max_iterations = 1000, REAL_T tolerance = 1e-5) {
        //            bool recording = variable::tape.recording;
        //            variable::tape.recording = false;
        //            mas::Tape<REAL_T> tape_l; //local tape
        //            tape_l.derivative_trace_level = atl::SECOND_ORDER_REVERSE;
        //            atl::Variable<REAL_T> f_l;
        //            atl::Variable<REAL_T> c_biomass;
        //            atl::Variable<REAL_T> sum;
        //            this->initialF.SetValue(static_cast<REAL_T> (0.0));
        //            tape_l.recording = true;
        //            REAL_T previous = 0;
        //            for (size_t iter = 0; iter < max_iterations; iter++) {
        //                f_l.SetValue(0.0);
        //                c_biomass.SetValue(0.0);
        //                sum.SetValue(0.0);
        //                std::vector< std::shared_ptr<mas::Fleet<REAL_T> > >& fleets = this->area->seasonal_fleet_operations[1];
        //                for (int f = 0; f < fleets.size(); f++) {
        //                    for (int a = 0; a < this->ages.size(); a++) {
        //                        //                        variable length_at_catch = this->growth_model->Evaluate(this->ages[a].GetValue() + this->catch_season_offset, this->sex);
        //                        //                        variable wct =  this->weight_at_catch_time[a];
        //
        //                        c_biomass.Assign(tape_l, c_biomass + this->weight_at_catch_time[a] *
        //                                fleets[f]->season_area_selectivity[1][this->area->id]->Evaluate(this->ages[a]) *
        //                                this->initialF * this->initial_equilibrium_numbers[a] *
        //                                (1.0 - atl::exp(-this->M[a] - this->initialF)) /
        //                                (this->M[a] + this->initialF), tape_l.NextIndex());
        //
        //                    }
        //                    REAL_T C_B = fleets[f]->catch_biomass_data->get(0, 0); //*1000.0;
        //
        //                    f_l.Assign(tape_l, f_l + ((C_B) - (c_biomass))*((C_B) - (c_biomass)));
        //                }
        //                std::cout << tape_l.Value(this->initialF.info->id) << "   " << tape_l.stack_current << "---->\n";
        //                tape_l.AccumulateSecondOrder();
        //                REAL_T deltaX = tape_l.Value(this->initialF.info->id) / tape_l.Value(this->initialF.info->id, this->initialF.info->id);
        //                if (std::fabs(tape_l.Value(this->initialF.info->id)) <= tolerance ||
        //                        deltaX != deltaX) {
        //                    tape_l.Reset();
        //                    break;
        //                }
        //                this->initialF.SetValue(this->initialF.GetValue() - deltaX);
        //                if (iter) {
        //                    if (std::fabs(previous - this->initialF.GetValue()) < 1e-5) {
        //                        tape_l.Reset();
        //                        break;
        //                    }
        //                }
        //                previous = this->initialF.GetValue();
        //                tape_l.Reset();
        //            }
        //            variable::tape.recording = recording;
        //        }

        /**
         *  Calculate the F required to harvest the observed catch biomass
         *  by 1 fleet in one time period (year) in 1 area with 1 population 
         *  using bisection search*/
        inline void FCalc() {
            //            if (this->initialF.GetValue() == 0.0) {

            bool recording = variable::tape.recording;
            variable::tape.recording = false;
            //                this->InitialNumbersEquilibrium();
            this->initialF = static_cast<REAL_T> (0.0);
            std::vector< std::shared_ptr<Fleet<REAL_T> > >& fleets = this->area->seasonal_fleet_operations[1];
            variable wct, wct2;

            for (int f = 0; f < fleets.size(); f++) {

                if (fleets[f]->catch_biomass_data.get() != NULL) {
                    //                        std::shared_ptr<mas::FishingMortality<REAL_T> >& fm =
                    //                                fleets[f]->season_area_fishing_mortality[1][this->area->id];
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
            //            }

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
                F[index] = static_cast<REAL_T> (0.0);
                S[index] = static_cast<REAL_T> (0.0);

                for (int f = 0; f < fleets.size(); f++) {

                    variable ff = fleets[f]->area_season_fishing_mortality[this->area->id][season]->Evaluate(year, (season - 1));
                    variable s = fleets[f]->season_area_selectivity[season][this->area->id]->Evaluate(this->ages, a);
                    this->sum_selectivity[index] += s;
                    variable f_a_fleet = ff * s;

                    fleets[f]->f_at_age[this->area->id][this->id][index]
                            = f_a_fleet;

                    //push to fleet...should we weight to Finalize???
                    if (this->sex == FEMALE) {

                        fleets[f]->f_at_age_females[this->area->id][this->id][index]
                                = f_a_fleet;
                    } else {

                        fleets[f]->f_at_age_males[this->area->id][this->id][index]
                                = f_a_fleet;
                    }

                    Z[index] += f_a_fleet;
                    F[index] += ff; ////static_cast<REAL_T>(fleets.size());//REAL_T;
                    P[index] += s;

                }
                Z[index] += this->M[a];
                S[index] = mas::exp(-1.0 * Z[index]);
                sum_of_Z += Z[index];

            }
            for (int f = 0; f < fleets.size(); f++) {
                variable ff = fleets[f]->area_season_fishing_mortality[this->area->id][season]->Evaluate(year, (season - 1));
                this->fishing_mortality_total[year * this->seasons + (season - 1)] += ff;
            }
            this->fishing_mortality_total[year * this->seasons + (season - 1)] /= static_cast<REAL_T> (fleets.size());

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
                this->recruitment[year * seasons + (season - 1)] = this->initial_numbers[0]+
                        ((this->recruitment_model->recruitment_deviations[0]));
            } else {
                if (season == this->spawning_season) {
                    //previous year spawning biomass
                    variable sb = this->spawning_stock_biomass[(year - 1) * seasons + (season - 1)];

                    this->recruitment[year * seasons + (season - 1)] =
                            static_cast<REAL_T> (this->sex_fraction_value) * this->recruitment_model->Evaluate(this->id, this->area->id, sb) *
                            mas::exp(this->recruitment_model->bias_correction + //either 0.0 or -0.5*sigma_r^2
                            this->recruitment_model->recruitment_deviations[year * seasons + (season - 1)]);

                } else {

                    std::cout << "recruitment model not found!!!\n";
                    exit(0);
                }

                this->numbers_at_age[year * this->seasons * this->ages.size() + (season - 1) * this->ages.size() + 0] =
                        this->recruitment[year * seasons + (season - 1)];
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
                    this->numbers_at_age[a] = this->initial_numbers[a];
                }
            } else {
                int y = year;
                int s = season;

                this->DecrementTime(y, s);

                for (int a = 1; a < ages.size(); a++) {

                    //current year index
                    size_t index1 = year * this->seasons * this->ages.size() + (season - 1) * this->ages.size() + a;

                    //previous year/age group index
                    size_t index2 = y * this->seasons * this->ages.size() + (s - 1) * this->ages.size() + a - 1;

                    this->numbers_at_age[index1] =
                            this->numbers_at_age[index2] *
                            mas::exp(static_cast<REAL_T> (-1.0) *
                            Z[index2]);
                    this->biomass_at_age[index1] = this->numbers_at_age[index1] * this->weight_at_season_start[index1];

                }
                //current year plus group index
                size_t index1 = year * this->seasons * this->ages.size() + (season - 1) * this->ages.size() + this->ages.size() - 1;

                //previous year plus group index
                size_t index2 = y * this->seasons * this->ages.size() + (s - 1) * this->ages.size() + this->ages.size() - 1;

                //plus group
                this->numbers_at_age[index1] +=
                        this->numbers_at_age[index2] *
                        mas::exp(static_cast<REAL_T> (-1.0) *
                        Z[index2]);

                this->biomass_at_age[index1] += this->numbers_at_age[index1] * this->weight_at_season_start[index1];

            }


        }

        REAL_T sum(const std::valarray<REAL_T>& val) {
            REAL_T ret = 0.0;
            for (int i = 0; i < val.size(); i++) {
                ret += val[i];
            }
            return ret;
        }

        REAL_T sum_product(const std::valarray<REAL_T>& x, const std::valarray<REAL_T>& y) {
            REAL_T ret = 0.0;
            for (int i = 0; i < y.size(); i++) {
                ret += x[i] * y[i];
            }
            return ret;
        }

        REAL_T sum_quitient(const std::valarray<REAL_T>& x, const std::valarray<REAL_T>& y) {
            REAL_T ret = 0.0;
            for (int i = 0; i < x.size(); i++) {
                ret += x[i] / y[i];
            }
            return ret;
        }

        REAL_T min(const std::valarray<REAL_T>& val) {
            REAL_T ret = std::numeric_limits<REAL_T>::max();
            for (int i = 0; i < val.size(); i++) {
                if (val[i] < ret) {
                    ret = val[i];
                }
            }
            return ret;
        }

        REAL_T max(const std::valarray<REAL_T>& val) {
            REAL_T ret = std::numeric_limits<REAL_T>::min();
            for (int i = 0; i < val.size(); i++) {
                if (val[i] > ret) {
                    ret = val[i];
                }
            }
            return ret;
        }

        std::valarray<REAL_T> fabs(const std::valarray<REAL_T>& val) {
            std::valarray<REAL_T> ret(val.size());
            for (int i = 0; i < val.size(); i++) {
                ret[i] = std::fabs(val[i]);

            }
            return ret;
        }

        void CalculateMSY(REAL_T maxF = 1.0, REAL_T step = 0.01) {
            /**
             * This code was ported from BAM. Original Author: Kyle Shertzer
             */
            bool recording = mas::VariableTrait<REAL_T>::IsRecording();

            mas::VariableTrait<REAL_T>::SetRecording(false);

            int year = this->years - 1;
            int season = this->seasons - 1;
            int nages = ages.size();

            std::vector<REAL_T> F;
            for (REAL_T f = 0.0; f <= maxF; f += step) {
                F.push_back(f);
            }




            std::valarray<REAL_T> spr(F.size()); //equilibrium spr at F
            std::valarray<REAL_T> spr_ratio(F.size()); //equilibrium spr at F
            std::vector<REAL_T> S_eq(F.size()); //equilibrium SSB at F
            std::vector<REAL_T> R_eq(F.size()); //equilibrium recruitment at F
            std::vector<REAL_T> B_eq(F.size()); //equilibrium biomass at F
            std::vector<REAL_T> L_eq(F.size()); //equilibrium landings at F
            std::vector<REAL_T> D_eq(F.size()); //equilibrium dead discards at F
            std::vector<REAL_T> E_eq(F.size()); //equilibrium exploitation rate at F (landings only)
            std::valarray<REAL_T> L_eq_knum(F.size());
            std::valarray<REAL_T> SSB_eq(F.size());

            REAL_T spr_F0 = 0.0;


            std::vector<REAL_T> N0(this->ages.size(), 1.0);
            for (int iage = 1; iage < nages; iage++) {
                N0[iage] = N0[iage - 1] * std::exp(-1.0 * M[iage - 1].GetValue());
            }
            N0[nages - 1] = N0[nages - 2] * std::exp(-1.0 * M[nages - 2].GetValue()) / (1.0 - std::exp(-1.0 * M[nages - 1].GetValue()));



            std::valarray<REAL_T> reprod(nages);
            std::valarray<REAL_T> selL(nages);
            std::valarray<REAL_T> selZ(nages);
            std::valarray<REAL_T> M_age(nages);
            std::valarray<REAL_T> wgt(nages);

            for (int a = 0; a < ages.size(); a++) {
                //dimension folded index
                size_t index = year * this->seasons * this->ages.size() + (season) * this->ages.size() + a;

                //is this ssb_unfished?
                reprod[a] = this->weight_at_spawning[index].GetValue() * (this->maturity[a] * this->sex_fraction_value);
                spr_F0 += N0[a] * reprod[a];
                selL[a] = this->sum_selectivity[index].GetValue();
                selZ[a] = this->sum_selectivity[index].GetValue();
                M_age[a] = this->M[a].GetValue();
                wgt[a] = this->weight_at_catch_time[index].GetValue();
            }



            std::valarray<REAL_T> L_age(nages); //#landings at age
            std::valarray<REAL_T> D_age(nages); //#dead discards at age
            std::valarray<REAL_T> F_age(nages); //#F at age
            std::valarray<REAL_T> Z_age(nages); //#Z at age

            // BEGIN ALGORITHM
            for (int i = 0; i < F.size(); i++) {

                std::valarray<REAL_T> FL_age = F[i] * selL;
                //std::valarray<REAL_T> FD_age = F[i] * selD;
                std::valarray<REAL_T> Z_age = M_age + F[i] * selZ;

                std::valarray<REAL_T> N_age(nages);
                std::valarray<REAL_T> N_age_spawn(nages);

                N_age[0] = 1.0;

                for (int iage = 1; iage < nages; iage++) {
                    N_age[iage] = N_age[iage - 1] * std::exp(-1.0 * Z_age[iage - 1]);
                }

                //last age is pooled
                N_age[nages - 1] = N_age[nages - 2] * std::exp(-1.0 * Z_age[nages - 2]) /
                        (1.0 - std::exp(-1.0 * Z_age[nages - 1]));


                N_age_spawn = (N_age *
                        std::exp((-1.0 * Z_age * this->spawning_season_offset.GetValue())));

                N_age_spawn[nages - 1] = (N_age_spawn[nages - 2]*(std::exp(-1. * (Z_age[nages - 2]*(1.0 - this->spawning_season_offset.GetValue()) +
                        Z_age[nages - 1] * this->spawning_season_offset.GetValue())))) / (1.0 - std::exp(-1. * Z_age[nages - 1]));


                spr[i] = sum_product(N_age, reprod);
                //                                                R_eq[i] = (R0 / ((5.0 * steep - 1.0) * spr[i]))*
                //                                                        (BC * 4.0 * steep * spr[i] - spr_F0 * (1.0 - steep));
                R_eq[i] = this->recruitment_model->CalculateEquilibriumRecruitment(
                        this->recruitment_model->CalculateEquilibriumSpawningBiomass(spr[i])); //*1000*this->sex_fraction_value;



                if (R_eq[i] < 0.0000001) {
                    R_eq[i] = 0.0000001;
                }

                N_age *= R_eq[i];
                N_age_spawn *= R_eq[i];



                S_eq[i] = sum_product(N_age, reprod);
                B_eq[i] = sum_product(N_age, wgt);


                for (int iage = 0; iage < nages; iage++) {
                    L_age[iage] = N_age[iage]*
                            (FL_age[iage] / Z_age[iage])*(1.0 - std::exp(-1.0 * Z_age[iage]));
                    //                            D_age[iage] = N_age[iage]*
                    //                                              (FD_age[iage] / Z_age[iage])*(1. - exp(-1.0 * Z_age[iage]))
                }


                SSB_eq[i] = sum_product(N_age_spawn, reprod);


                L_eq[i] = sum_product(L_age, wgt);
                E_eq[i] = sum(L_age) / sum(N_age);
                L_eq_knum[i] = (sum(L_age) / 1000.0);

            }
            int max_index = 0;
            REAL_T max = 1e-18; //std::numeric_limits<REAL_T>::min();
            spr_ratio = spr / spr_F0;
            REAL_T F01_dum = min(fabs(spr_ratio - 0.001));
            REAL_T F30_dum = min(fabs(spr_ratio - 0.3));
            REAL_T F35_dum = min(fabs(spr_ratio - 0.35));
            REAL_T F40_dum = min(fabs(spr_ratio - 0.4));
            size_t F01_out;
            size_t F30_out = 0;
            size_t F35_out = 0;
            size_t F40_out = 0;
            //            std::cout << "F30_dum " << F30_dum << "\n";
            //            std::cout << "F35_dum " << F35_dum << "\n";
            //            std::cout << "F40_dum " << F40_dum << "\n";

            for (int i = 0; i < L_eq.size(); i++) {

                if (L_eq[i] >= max) {
                    max = L_eq[i];
                    max_index = i;
                }

                //                if (std::fabs(spr_ratio[i] - 0.001) == F01_dum) {
                //                    F01_out = F[i];
                //                }


                if (std::fabs(spr_ratio[i] - 0.3) == F30_dum) {
                    F30_out = i;
                }
                if (std::fabs(spr_ratio[i] - 0.35) == F35_dum) {
                    F35_out = i;
                }
                if (std::fabs(spr_ratio[i] - 0.4) == F40_dum) {
                    F40_out = i;
                }
            }
            REAL_T msy_mt_out = max; //msy in whole weight 
            REAL_T SSB_msy_out;
            REAL_T B_msy_out;
            REAL_T R_msy_out;
            REAL_T msy_knum_out;
            REAL_T F_msy_out;
            REAL_T spr_msy_out;
            int index_m = 0;
            for (int i = 0; i < F.size(); i++) {
                if (L_eq[i] == msy_mt_out) {

                    SSB_msy_out = SSB_eq[i];
                    B_msy_out = B_eq[i] * this->sex_fraction_value;
                    R_msy_out = R_eq[i]*1000.0 * this->sex_fraction_value;
                    msy_knum_out = L_eq_knum[i];
                    F_msy_out = F[i];
                    spr_msy_out = spr[i];
                    index_m = i;
                }
            }

            this->msy.msy = msy_mt_out * this->sex_fraction_value;
            this->msy.spr_F0 = spr_F0;
            this->msy.F_msy = F_msy_out;
            this->msy.spr_msy = spr[index_m];
            this->msy.SR_msy = spr[index_m] / spr_F0;
            this->msy.R_msy = R_msy_out;
            this->msy.SSB_msy = SSB_msy_out;
            this->msy.B_msy = B_msy_out;
            this->msy.E_msy = E_eq[index_m];

            this->msy.F30 = F[F30_out];
            this->msy.spr_F30_msy = spr[F30_out];
            this->msy.SR_F30_msy = spr[F30_out] / spr_F0;
            this->msy.R_F30_msy = R_eq[F30_out];
            this->msy.SSB_F30_msy = S_eq[F30_out];
            this->msy.B_F30_msy = B_eq[F30_out];
            this->msy.E_F30_msy = E_eq[F30_out];

            this->msy.F35 = F[F35_out];
            this->msy.spr_F35_msy = spr[F35_out];
            this->msy.SR_F35_msy = spr[F35_out] / spr_F0;
            this->msy.R_F35_msy = R_eq[F35_out];
            this->msy.SSB_F35_msy = S_eq[F35_out];
            this->msy.B_F35_msy = B_eq[F35_out];
            this->msy.E_F35_msy = E_eq[F35_out];

            this->msy.F40 = F[F40_out];
            this->msy.spr_F40_msy = spr[F40_out];
            this->msy.SR_F40_msy = spr[F40_out] / spr_F0;
            this->msy.R_F40_msy = R_eq[F40_out];
            this->msy.SSB_F40_msy = S_eq[F40_out];
            this->msy.B_F40_msy = B_eq[F40_out];
            this->msy.E_F40_msy = E_eq[F40_out];

            std::cout << std::scientific;
            //
            std::cout << "\n\nFmax: " << maxF << "\n";
            std::cout << "Step: " << step << "\n";
            std::cout << "\n\nF_msy: " << F[max_index] << "\n";
            std::cout << "F30: " << F[F30_out] << "\n";
            std::cout << "F35: " << F[F35_out] << "\n";
            std::cout << "F40: " << F[F40_out] << "\n";
            spr_msy_out = spr[max_index];
            std::cout << "msy: " << this->msy.msy << "\n";
            std::cout << "spr_msy: " << spr[max_index] << "\n";
            std::cout << "SR_msy: " << spr_msy_out / spr_F0 << "\n";
            //                        std::cout << "D_msy_out" << D_eq[max_index] << "\n";
            std::cout << "R_msy: " << R_eq[max_index] << "\n";
            std::cout << "SSB_msy: " << this->msy.SSB_msy << "\n";
            std::cout << "B_msy: " << this->msy.B_msy << "\n";
            std::cout << "E_msy: " << E_eq[max_index] << "\n";
            std::cout << "R0: " << this->R0 << "\n";
            std::cout << "S0: " << this->S0 << "\n";
            std::cout << "E_msy: " << E_eq[max_index] << "\n";
            std::cout << "Alpha: " << this->recruitment_model->GetAlpha() << "\n";
            std::cout << "Beta: " << this->recruitment_model->GetBeta() << "\n\n";

            mas::VariableTrait<REAL_T>::SetRecording(recording);

        }

        void Finalize() {

            for (int y = 0; y < this->years; y++) {
                for (int s = 0; s < this->seasons; s++) {
                    variable abundance_ = 0.0;
                    variable biomass_ = 0.0;
                    for (int a = 0; a < this->ages.size(); a++) {

                        size_t index = y * this->seasons * this->ages.size() + s * ages.size() + a;
                        abundance_ += this->numbers_at_age[index];
                        biomass_ += this->numbers_at_age[index] * this->weight_at_season_start[index];
                    }
                    size_t index = y * this->seasons + s;
                    this->abundance[index] = abundance_;
                    this->biomass_total[index] = biomass_;
                }
            }
        }

        inline void SettleMovedFish(int year, int season) {
            int a = 0;
            size_t index = year * this->seasons + (season - 1);
            this->recruitment[index] += this->immigrant_recruits[index];
            this->recruitment[index] -= this->emigrant_recruits[index];
#warning need to add imigrant and emigrant recruitment biomass here

            for (a = 0; a < ages.size(); a++) {

                index = year * this->seasons * this->ages.size() + (season - 1) * this->ages.size() + a;
                this->numbers_at_age[index] += this->imigrants[index];
                this->biomass_at_age[index] += this->imigrants_biomass[index];
                this->numbers_at_age[index] -= this->emigrants[index];
                this->biomass_at_age[index] -= this->emigrants_biomass[index];
                if (this->numbers_at_age[index] < 0) {
                    std::cout << this->id << " " << (this->numbers_at_age[index] + this->emigrants[index]) << "-= " << this->emigrants[index];
                    std::cout << " = " << this->numbers_at_age[index];

                }

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
            // beginning of end year (this->years) + 1
            int year = this->years;
            int season = 1;
            int y = year;
            int s = season;
            this->DecrementTime(y, s);
            for (int a = 1; a < ages.size(); a++) {

                size_t index = year * this->seasons * this->ages.size()+ (s - 1) * this->ages.size() + a;
                this->numbers_at_age[index] =
                        this->numbers_at_age[y * this->seasons * this->ages.size() + (s - 1) * this->ages.size() + a - 1] *
                        mas::exp(static_cast<REAL_T> (-1.0) * Z[y * this->seasons * this->ages.size() + (s - 1) * this->ages.size() + a - 1]);
            }
            // plus group
            size_t index = year * this->seasons * this->ages.size()+ (s - 1) * this->ages.size() + this->ages.size() - 1;
            size_t index2 = year * this->seasons * this->ages.size()+ (s - 1) * this->ages.size();
            this->numbers_at_age[index] +=
                    this->numbers_at_age[y * this->seasons * this->ages.size() + (s - 1) * this->ages.size() + (this->ages.size() - 1)] *
                    mas::exp(static_cast<REAL_T> (-1.0) * Z[y * this->seasons * this->ages.size() + (s - 1) * this->ages.size() + (this->ages.size() - 1)]);
            // median age-0 recruits
            this->numbers_at_age[index2] = static_cast<REAL_T> (this->sex_fraction_value) * mas::exp(this->recruitment_model->log_R0);
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
                    //dimension folded index
                    size_t index = year * this->seasons * this->ages.size() + (season - 1) * this->ages.size() + a;

                    //spawning numbers at age
                    this->equilibrium_to_survival_at_spawning[index] =
                            mas::exp(static_cast<REAL_T> (-1.0) * this->spawning_season_offset * Z[index]);

                    this->spawning_numbers_at_age[index] =
                            this->equilibrium_to_survival_at_spawning[index] *
                            this->numbers_at_age[index];


                    sb += this->fecundity_at_age[index] * this->spawning_numbers_at_age[index];

                }

                this->spawning_stock_biomass[year * this->seasons + (season - 1)] = sb;

            } else {

                this->spawning_stock_biomass[year * this->seasons + (season - 1)] = -999.0;
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
                        fca = (numbers_at_age[index] * (fleets[f]->f_at_age_females[this->area->id][this->id][index]
                                / Z[index]) * (1.0 - mas::exp(static_cast<REAL_T> (-1.0) * Z[index])));
                    } else {
                        fca = (numbers_at_age[index] * (fleets[f]->f_at_age_males[this->area->id][this->id][index]
                                / Z[index]) * (1.0 - mas::exp(static_cast<REAL_T> (-1.0) * Z[index])));
                    }


                    //add  to the fleet[i]'s total catch numbers at age
                    fleets[f]->catch_at_age[index] += fca;
                    //contribute to this population segments total catch numbers at age
                    catch_at_age[index] += fca;
                    //contribute to this population segments total catch biomass at age
                    catch_biomass_at_age[index] += fca * this->weight_at_catch_time[index];

                    total_CB += catch_biomass_at_age[index].GetValue();

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
            this->catch_biomass_total[year * this->seasons + season - 1] = total_CB;


        }

        /**
         * Evaluates survey numbers at age for all ages and season
         * @param year
         * @param season
         */
        inline void CalculateSurveyNumbersAtAge(int year, int season) {
            std::vector< std::shared_ptr<Survey<REAL_T> > >& surveys = this->area->seasonal_survey_operations[season];

            REAL_T total_SI = static_cast<REAL_T> (0.0);
            size_t index2 = year * this->seasons + (season - 1);
            for (int a = 0; a < this->ages.size(); a++) {
                size_t index = year * this->seasons * this->ages.size() + (season - 1) * this->ages.size() + a;

                variable weight = this->weight_at_survey_time[index];
                for (int s = 0; s < surveys.size(); s++) {

                    // NOTE:  the survey has a catchability (q) associated with it
                    variable saa = (surveys[s]->q) *
                            this->numbers_at_age[index] * surveys[s]->season_area_selectivity[season][this->area->id]->Evaluate(this->ages, a) *
                            mas::exp(static_cast<REAL_T> (-1.0) * survey_season_offset * this->Z[index]);

                    this->survey_numbers_at_age[index] += saa;
                    surveys[s]->survey_numbers_at_age[index] += saa;

                    this->survey_index_at_age[index] += saa * weight;
                    total_SI += saa.GetValue() * weight.GetValue();
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
            this->survey_biomass_total[index2] = total_SI;
        }

        inline void PushToArea() {

            this->area->PushNumbersAndBiomass(this->survey_numbers_at_age,
                    this->survey_index_at_age,
                    this->numbers_at_age,
                    this->catch_at_age,
                    this->catch_biomass_at_age,
                    this->recruitment,
                    this->spawning_stock_biomass,
                    this->abundance,
                    this->biomass_total,
                    this->id,
                    sex);
        }

        inline void PushNumbersToFleet() {

        }
    };

    template<typename REAL_T>
    uint32_t Subpopulation<REAL_T>::length_weight_key_carryout = 10;

    template<typename REAL_T>
    std::ostream& operator<<(std::ostream& out, mas::Subpopulation<REAL_T>& pi) {
        out << std::fixed;
        out << std::setprecision(7);
        out << "Population " << pi.natal_population->id << "\n";
        out << "Area " << pi.area->id << "\n";
        out << "R0\t" << mas::exp(pi.recruitment_model->log_R0).GetValue() << "\n";
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
                    out << pi.numbers_at_age[y * pi.seasons * pi.ages.size() + (s - 1) * pi.ages.size() + a] << " ";
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
                    out << pi.catch_at_age[y * pi.seasons * pi.ages.size() + (s - 1) * pi.ages.size() + a] << " ";
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
                    out << pi.catch_biomass_at_age[y * pi.seasons * pi.ages.size() + (s - 1) * pi.ages.size() + a] << " ";
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
                    out << pi.survey_numbers_at_age[index] << " ";
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
                    out << pi.survey_index_at_age[y * pi.seasons * pi.ages.size() + (s - 1) * pi.ages.size() + a] << " ";
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
                out << pi.spawning_stock_biomass[y * pi.seasons + (s - 1)] << " ";
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
    std::string ToHtml(mas::Subpopulation<REAL_T>& pi) {
        std::stringstream out;

        return out.str();
    }

    template<typename REAL_T>
    std::string AddLatexCharts(mas::Subpopulation<REAL_T>& pi) {
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
                out << index << " " << pi.catch_biomass_total[index] << " \n";
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
                out << index << " " << pi.survey_biomass_total[index] << " \n";
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

                    out << pi.ages[a] << "   " << pi.catch_at_age[y * pi.seasons * pi.ages.size() + (s) * pi.ages.size() + a] << "\n";
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

                    out << (REAL_T) pi.ages[a] << "   " << pi.numbers_at_age[y * pi.seasons * pi.ages.size() + (s) * pi.ages.size() + a] << "\n";
                }
                out << "};\n";
                out << "\\addlegendentry{Numbers}\n";
                out << "\\addplot [color= blue, mark=*] table {\n";
                for (int a = 0; a < pi.ages.size(); a++) {

                    out << pi.ages[a] << "   " << pi.catch_at_age[y * pi.seasons * pi.ages.size() + (s) * pi.ages.size() + a] << "\n";
                }
                out << "};\n";
                out << "\\addlegendentry{Catch}\n";
                out << "\\addplot [color= yellow, mark=*] table {\n";
                for (int a = 0; a < pi.ages.size(); a++) {

                    out << pi.ages[a] << "  " << pi.survey_numbers_at_age[y * pi.seasons * pi.ages.size() + (s) * pi.ages.size() + a] << "\n";
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

        //totals for this population
        std::vector<REAL_T> numbers_at_age;
        std::vector<REAL_T> survey_numbers_at_age;
        std::vector<REAL_T> survey_biomass_at_age;
        std::vector<REAL_T> survey_biomass_total;
        std::vector<REAL_T> catch_numbers_at_age;
        std::vector<REAL_T> catch_biomass_at_age;
        std::vector<REAL_T> catch_biomass_total;
        std::vector<REAL_T> recruits;
        std::vector<REAL_T> spawning_stock_biomass;
        std::vector<REAL_T> biomass_total;
        std::vector<REAL_T> abundance;
        std::vector<REAL_T> fishing_mortality;

        //females
        std::vector<REAL_T> numbers_at_age_females;
        std::vector<REAL_T> survey_numbers_at_age_females;
        std::vector<REAL_T> survey_biomass_at_age_females;
        std::vector<REAL_T> survey_biomass_total_females;
        std::vector<REAL_T> catch_numbers_at_age_females;
        std::vector<REAL_T> catch_biomass_at_age_females;
        std::vector<REAL_T> catch_biomass_total_females;
        std::vector<REAL_T> recruits_females;
        std::vector<REAL_T> spawning_stock_biomass_females;
        std::vector<REAL_T> biomass_total_females;
        std::vector<REAL_T> abundance_females;
        std::vector<REAL_T> fishing_mortality_females;

        //males
        std::vector<REAL_T> numbers_at_age_males;
        std::vector<REAL_T> survey_numbers_at_age_males;
        std::vector<REAL_T> survey_biomass_at_age_males;
        std::vector<REAL_T> survey_biomass_total_males;
        std::vector<REAL_T> catch_numbers_at_age_males;
        std::vector<REAL_T> catch_biomass_at_age_males;
        std::vector<REAL_T> catch_biomass_total_males;
        std::vector<REAL_T> recruits_males;
        std::vector<REAL_T> spawning_stock_biomass_males;
        std::vector<REAL_T> biomass_total_males;
        std::vector<REAL_T> abundance_males;
        std::vector<REAL_T> fishing_mortality_males;


        std::shared_ptr<Area<REAL_T> > natal_area; //birth area
        std::vector<std::shared_ptr<Area<REAL_T> > > areas_list; //all areas
        //Movement Tracking
        typedef typename std::unordered_map<int, Subpopulation<REAL_T> >::iterator cohort_iterator;
        std::unordered_map<int, Subpopulation<REAL_T> > males;
        std::unordered_map<int, Subpopulation<REAL_T> > females;
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

        //averages across all subpopulations
        MaximumSustainableYield<REAL_T> msy;
        MaximumSustainableYield<REAL_T> msy_females;
        MaximumSustainableYield<REAL_T> msy_males;


        REAL_T msy_average;
        REAL_T f_msy_average;
        REAL_T s_msy_average;
        REAL_T fmax_average;
        //number of forecast years
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
            //            survey_numbers_at_age.resize(0);
            //            survey_numbers_at_age.resize(years * seasons * ages, static_cast<REAL_T> (0.0));
            //            survey_index_at_age.resize(0);
            //            survey_index_at_age.resize(years * seasons * ages, static_cast<REAL_T> (0.0));
            //            catch_numbers_at_age.resize(0);
            //            catch_numbers_at_age.resize(years * seasons * ages, static_cast<REAL_T> (0.0));
            //            catch_index_at_age.resize(0);
            //            catch_index_at_age.resize(years * seasons * ages, static_cast<REAL_T> (0.0));
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

                    Subpopulation<REAL_T>& male_info_from = this->males[(i + 1)];
                    Subpopulation<REAL_T>& female_info_from = this->females[(i + 1)];

                    for (int j = 0; j < male_fractions.size(); j++) {

                        Subpopulation<REAL_T>& male_info_to = this->males[(j + 1)];
                        Subpopulation<REAL_T>& female_info_to = this->females[(j + 1)];
                        if (i != j) {
                            variable tempm = rercruit_fractions[i][j] * male_info_from.recruitment[year * this->seasons + (season - 1)];
                            variable tempf = rercruit_fractions[i][j] * female_info_from.recruitment[year * this->seasons + (season - 1)];
                            male_info_to.redistributed_recruits[year * this->seasons + (season - 1)] += tempm;
                            male_info_to.immigrant_recruits[year * this->seasons + (season - 1)] += tempm;
                            male_info_from.emigrant_recruits[year * this->seasons + (season - 1)] += tempm;
                            female_info_to.redistributed_recruits[year * this->seasons + (season - 1)] += tempf;
                            female_info_to.immigrant_recruits[year * this->seasons + (season - 1)] += tempf;
                            female_info_from.emigrant_recruits[year * this->seasons + (season - 1)] += tempf;

                            for (int a = 0; a < this->ages; a++) {

                                // will need sex-specific Z at some point
                                size_t index = year * this->seasons * this->ages + (season - 1) * this->ages + a;
                                // move survivors only
                                variable emigrantsm = male_fractions[i][j] * male_info_from.numbers_at_age[index]; // *
                                // mas::exp(static_cast<REAL_T> (-1.0) * male_info_from.Z[index]);

                                male_info_from.emigrants[index] += emigrantsm;
                                male_info_from.emigrants_biomass[index] += emigrantsm * male_info_from.weight_at_season_start[index];

                                variable imigrantsm = male_fractions[i][j] * male_info_from.numbers_at_age[index]; // *
                                //    mas::exp(static_cast<REAL_T> (-1.0) * male_info_from.Z[index]);

                                male_info_to.imigrants[index] += imigrantsm;
                                male_info_to.imigrants_biomass[index] += imigrantsm * male_info_from.weight_at_season_start[index];

                                variable emigrantsf = female_fractions[i][j] * female_info_from.numbers_at_age[index]; // *
                                //   mas::exp(static_cast<REAL_T> (-1.0) * female_info_from.Z[index]);

                                female_info_from.emigrants[index] += emigrantsf;

                                variable imigrantsf = female_fractions[i][j] * female_info_from.numbers_at_age[index]; // *
                                //  mas::exp(static_cast<REAL_T> (-1.0) * female_info_from.Z[index]);

                                female_info_to.imigrants[index] += imigrantsf;
                                female_info_to.imigrants_biomass[index] += imigrantsf * female_info_from.weight_at_season_start[index];

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


                //                males[areas_list[a]->id].unfished_spawning_biomass_per_recruit =
                //                       females[areas_list[a]->id].unfished_spawning_biomass_per_recruit;// CalculateUnfishedSpawningBiomassPerRecruit();


                females[areas_list[a]->id].CalculateInitialNumbersEquilibrium();
                males[areas_list[a]->id].CalculateInitialNumbersEquilibrium();

                females[areas_list[a]->id].CalculateUnfishedSpawningBiomassPerRecruit();
                females[areas_list[a]->id].CalculateUnfishedEquilSpawningBiomass();

                males[areas_list[a]->id].SB0 = females[areas_list[a]->id].SB0;
                males[areas_list[a]->id].S0 = females[areas_list[a]->id].S0;


                females[areas_list[a]->id].FCalc();
                males[areas_list[a]->id].FCalc(); // = females[areas_list[a]->id].initialF;///.GetValue();

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


            numbers_at_age.resize(this->years * this->seasons * this->ages);
            survey_numbers_at_age.resize(this->years * this->seasons * this->ages);
            survey_biomass_at_age.resize(this->years * this->seasons * this->ages);
            survey_biomass_total.resize(this->years * this->seasons);
            catch_numbers_at_age.resize(this->years * this->seasons * this->ages);
            catch_biomass_at_age.resize(this->years * this->seasons * this->ages);
            catch_biomass_total.resize(this->years * this->seasons);
            recruits.resize(this->years * this->seasons);
            spawning_stock_biomass.resize(this->years * this->seasons);
            biomass_total.resize(this->years * this->seasons);
            abundance.resize(this->years * this->seasons);
            fishing_mortality.resize(this->years * this->seasons);

            numbers_at_age_females.resize(this->years * this->seasons * this->ages);
            survey_numbers_at_age_females.resize(this->years * this->seasons * this->ages);
            survey_biomass_at_age_females.resize(this->years * this->seasons * this->ages);
            survey_biomass_total_females.resize(this->years * this->seasons);
            catch_numbers_at_age_females.resize(this->years * this->seasons * this->ages);
            catch_biomass_at_age_females.resize(this->years * this->seasons * this->ages);
            catch_biomass_total_females.resize(this->years * this->seasons);
            recruits_females.resize(this->years * this->seasons);
            spawning_stock_biomass_females.resize(this->years * this->seasons);
            biomass_total_females.resize(this->years * this->seasons);
            abundance_females.resize(this->years * this->seasons);
            fishing_mortality_females.resize(this->years * this->seasons);

            numbers_at_age_males.resize(this->years * this->seasons * this->ages);
            survey_numbers_at_age_males.resize(this->years * this->seasons * this->ages);
            survey_biomass_at_age_males.resize(this->years * this->seasons * this->ages);
            survey_biomass_total_males.resize(this->years * this->seasons);
            catch_numbers_at_age_males.resize(this->years * this->seasons * this->ages);
            catch_biomass_at_age_males.resize(this->years * this->seasons * this->ages);
            catch_biomass_total_males.resize(this->years * this->seasons);
            recruits_males.resize(this->years * this->seasons);
            spawning_stock_biomass_males.resize(this->years * this->seasons);
            biomass_total_males.resize(this->years * this->seasons);
            abundance_males.resize(this->years * this->seasons);
            fishing_mortality_males.resize(this->years * this->seasons);
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
                        std::cout << this->catch_numbers_at_age[y * this->seasons * this->ages + (s - 1) * this->ages + a] << " ";
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
                        std::cout << this->catch_biomass_at_age[y * this->seasons * this->ages + (s - 1) * this->ages + a] << " ";
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
                        std::cout << this->survey_numbers_at_age[y * this->seasons * this->ages + (s - 1) * this->ages + a] << " ";
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

                        std::cout << this->survey_biomass_at_age[y * this->seasons * this->ages + (s - 1) * this->ages + a] << " ";
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
            int y;
            for (y = 0; y < this->years; y++) {
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
                        males[areas_list[area]->id].CalculateSpawningBiomass(y, s); //not needed until Finalize
                        females[areas_list[area]->id].CalculateSpawningBiomass(y, s); //not needed until Finalize


                        /******************************************
                         * Recruitment
                         *****************************************/
                        males[areas_list[area]->id].CalculateRecruitment(y, s);
                        females[areas_list[area]->id].CalculateRecruitment(y, s);


                        /******************************************
                         * Apply Movement
                         *****************************************/
                        this->MoveFish(y, s);

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
                     * Settle moved fish
                     *****************************************/
                    for (int d = 0; d < areas_list.size(); d++) {
                        males[areas_list[d]->id].SettleMovedFish(y, s);
                        females[areas_list[d]->id].SettleMovedFish(y, s);
                    }//end season
                }//end year

            }

            //plus year

            for (int area = 0; area < areas_list.size(); area++) {

                /******************************************
                 * Numbers at Age at year end plus one
                 *****************************************/
                males[areas_list[area]->id].CalculateNumbersAtAgeEndYearPlusOne();
                females[areas_list[area]->id].CalculateNumbersAtAgeEndYearPlusOne();
            }


        }

        /**
         * Totals derived quantities from all of the area/population/sex segments.
         */
        void Finalize() {
            this->ComputeBiologicalReferencePoints();


            //by age
            for (int al = 0; al < areas_list.size(); al++) {
                for (int y = 0; y < this->years; y++) {
                    for (int s = 1; s <= this->seasons; s++) {
                        for (int a = 0; a < this->ages; a++) {
                            size_t index = y * this->seasons * this->ages + (s - 1) * this->ages + a;
                            females[areas_list[al]->id].Finalize();
                            males[areas_list[al]->id].Finalize();
                            //total numbers from subpopulations
                            this->numbers_at_age[index] +=
                                    males[areas_list[al]->id].numbers_at_age[index].GetValue() +
                                    females[areas_list[al]->id].numbers_at_age[index].GetValue();

                            this->numbers_at_age_females[index] +=
                                    females[areas_list[al]->id].numbers_at_age[index].GetValue();

                            this->numbers_at_age_males[index] +=
                                    males[areas_list[al]->id].numbers_at_age[index].GetValue();

                            //total catch numbers from subpopulations
                            this->catch_numbers_at_age[index] +=
                                    males[areas_list[al]->id].catch_at_age[index].GetValue() +
                                    females[areas_list[al]->id].catch_at_age[index].GetValue();

                            this->catch_numbers_at_age_females[index] +=
                                    females[areas_list[al]->id].catch_at_age[index].GetValue();

                            this->catch_numbers_at_age_males[index] +=
                                    males[areas_list[al]->id].catch_at_age[index].GetValue();

                            //total catch biomass from subpopulations
                            this->catch_biomass_at_age[index] +=
                                    males[areas_list[al]->id].catch_biomass_at_age[index].GetValue() +
                                    females[areas_list[al]->id].catch_biomass_at_age[index].GetValue();

                            this->catch_biomass_at_age_females[index] +=
                                    females[areas_list[al]->id].catch_biomass_at_age[index].GetValue();

                            this->catch_biomass_at_age_males[index] +=
                                    males[areas_list[al]->id].catch_biomass_at_age[index].GetValue();


                            //total survey numbers from subpopulations
                            this->survey_numbers_at_age[index] +=
                                    males[areas_list[al]->id].survey_numbers_at_age[index].GetValue() +
                                    females[areas_list[al]->id].survey_numbers_at_age[index].GetValue();

                            this->survey_numbers_at_age_females[index] +=
                                    females[areas_list[al]->id].survey_numbers_at_age[index].GetValue();

                            this->survey_numbers_at_age_males[index] +=
                                    males[areas_list[al]->id].survey_numbers_at_age[index].GetValue();

                            //total survey biomass from subpopulations
                            this->survey_biomass_at_age[index] +=
                                    males[areas_list[al]->id].survey_index_at_age[index].GetValue() +
                                    females[areas_list[al]->id].survey_index_at_age[index].GetValue();

                            this->survey_biomass_at_age_females[index] +=
                                    females[areas_list[al]->id].survey_index_at_age[index].GetValue();

                            this->survey_biomass_at_age_males[index] +=
                                    males[areas_list[al]->id].survey_index_at_age[index].GetValue();


                        }
                    }
                }
            }
            //by time
            for (int al = 0; al < areas_list.size(); al++) {
                for (int y = 0; y < this->years; y++) {
                    for (int s = 1; s <= this->seasons; s++) {

                        size_t index = y * this->seasons + (s - 1);

                        this->abundance[index] +=
                                males[areas_list[al]->id].abundance[index].GetValue() +
                                females[areas_list[al]->id].abundance[index].GetValue();

                        this->abundance_females[index] +=
                                females[areas_list[al]->id].abundance[index].GetValue();

                        this->abundance_males[index] +=
                                males[areas_list[al]->id].abundance[index].GetValue();

                        this->spawning_stock_biomass[index] +=
                                males[areas_list[al]->id].spawning_stock_biomass[index].GetValue() +
                                females[areas_list[al]->id].spawning_stock_biomass[index].GetValue();

                        this->spawning_stock_biomass_females[index] +=
                                females[areas_list[al]->id].spawning_stock_biomass[index].GetValue();

                        this->spawning_stock_biomass_males[index] +=
                                males[areas_list[al]->id].spawning_stock_biomass[index].GetValue();

                        this->biomass_total[index] +=
                                males[areas_list[al]->id].biomass_total[index].GetValue() +
                                females[areas_list[al]->id].biomass_total[index].GetValue();

                        this->biomass_total_females[index] +=
                                females[areas_list[al]->id].biomass_total[index].GetValue();

                        this->biomass_total_males[index] +=
                                males[areas_list[al]->id].biomass_total[index].GetValue();

                        this->catch_biomass_total[index] +=
                                males[areas_list[al]->id].catch_biomass_total[index] +
                                females[areas_list[al]->id].catch_biomass_total[index];

                        this->catch_biomass_total_females[index] +=
                                females[areas_list[al]->id].catch_biomass_total[index];

                        this->catch_biomass_total_males[index] +=
                                males[areas_list[al]->id].catch_biomass_total[index];

                        this->survey_biomass_total[index] +=
                                males[areas_list[al]->id].survey_biomass_total[index] +
                                females[areas_list[al]->id].survey_biomass_total[index];

                        this->survey_biomass_total_females[index] +=
                                females[areas_list[al]->id].survey_biomass_total[index];

                        this->survey_biomass_total_males[index] +=
                                males[areas_list[al]->id].survey_biomass_total[index];

                        this->fishing_mortality[index] +=
                                (males[areas_list[al]->id].fishing_mortality_total[index].GetValue() +
                                females[areas_list[al]->id].fishing_mortality_total[index].GetValue()) / 2.0 / this->areas_list.size();

                        this->fishing_mortality_females[index] +=
                                females[areas_list[al]->id].fishing_mortality_total[index].GetValue() / this->areas_list.size();

                        this->fishing_mortality_males[index] +=
                                males[areas_list[al]->id].fishing_mortality_total[index].GetValue() / this->areas_list.size();

                        this->recruits[index] += males[areas_list[al]->id].recruitment[index].GetValue() +
                                females[areas_list[al]->id].recruitment[index].GetValue();

                        this->recruits_females[index] +=
                                females[areas_list[al]->id].recruitment[index].GetValue();

                        this->recruits_males[index] +=
                                males[areas_list[al]->id].recruitment[index].GetValue();
                    }
                }
            }
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
            variable::tape.recording = false;
            REAL_T msy;
            REAL_T f_msy;
            REAL_T s_msy;
            REAL_T fmax;
            for (int a = 0; a < areas_list.size(); a++) {

                males[areas_list[a]->id].CalculateMSY();
                females[areas_list[a]->id].CalculateMSY();

                this->msy.spr_F0 += males[areas_list[a]->id].msy.spr_F0 / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.F_msy += males[areas_list[a]->id].msy.F_msy / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.spr_msy += males[areas_list[a]->id].msy.spr_msy / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.SR_msy += males[areas_list[a]->id].msy.SR_msy / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.R_msy += males[areas_list[a]->id].msy.R_msy / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.SSB_msy += males[areas_list[a]->id].msy.SSB_msy;
                this->msy.B_msy += males[areas_list[a]->id].msy.B_msy / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.E_msy += males[areas_list[a]->id].msy.E_msy / static_cast<REAL_T> ((areas_list.size()*2.0));

                this->msy.F30 += males[areas_list[a]->id].msy.F30 / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.spr_F30_msy += males[areas_list[a]->id].msy.spr_F30_msy / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.SR_F30_msy += males[areas_list[a]->id].msy.SR_F30_msy / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.R_F30_msy += males[areas_list[a]->id].msy.R_F30_msy / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.SSB_F30_msy += males[areas_list[a]->id].msy.SSB_F30_msy / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.B_F30_msy += males[areas_list[a]->id].msy.B_F30_msy / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.E_F30_msy += males[areas_list[a]->id].msy.E_F30_msy / static_cast<REAL_T> ((areas_list.size()*2.0));

                this->msy.F35 += males[areas_list[a]->id].msy.F35 / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.spr_F35_msy += males[areas_list[a]->id].msy.spr_F35_msy / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.SR_F35_msy += males[areas_list[a]->id].msy.SR_F35_msy / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.R_F35_msy += males[areas_list[a]->id].msy.R_F35_msy / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.SSB_F35_msy += males[areas_list[a]->id].msy.SSB_F35_msy / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.B_F35_msy += males[areas_list[a]->id].msy.B_F35_msy / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.E_F35_msy += males[areas_list[a]->id].msy.E_F35_msy / static_cast<REAL_T> ((areas_list.size()*2.0));

                this->msy.F40 += males[areas_list[a]->id].msy.F40 / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.spr_F40_msy += males[areas_list[a]->id].msy.spr_F40_msy / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.SR_F40_msy += males[areas_list[a]->id].msy.SR_F40_msy / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.R_F40_msy += males[areas_list[a]->id].msy.R_F40_msy / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.SSB_F40_msy += males[areas_list[a]->id].msy.SSB_F40_msy / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.B_F40_msy += males[areas_list[a]->id].msy.B_F40_msy / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.E_F40_msy += males[areas_list[a]->id].msy.E_F40_msy / static_cast<REAL_T> ((areas_list.size()*2.0));

                this->msy.spr_F0 += females[areas_list[a]->id].msy.spr_F0 / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.F_msy += females[areas_list[a]->id].msy.F_msy / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.spr_msy += females[areas_list[a]->id].msy.spr_msy / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.SR_msy += females[areas_list[a]->id].msy.SR_msy / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.R_msy += females[areas_list[a]->id].msy.R_msy / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.SSB_msy += females[areas_list[a]->id].msy.SSB_msy;
                this->msy.B_msy += females[areas_list[a]->id].msy.B_msy / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.E_msy += females[areas_list[a]->id].msy.E_msy / static_cast<REAL_T> ((areas_list.size()*2.0));

                this->msy.F30 += females[areas_list[a]->id].msy.F30 / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.spr_F30_msy += females[areas_list[a]->id].msy.spr_F30_msy / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.SR_F30_msy += females[areas_list[a]->id].msy.SR_F30_msy / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.R_F30_msy += females[areas_list[a]->id].msy.R_F30_msy / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.SSB_F30_msy += females[areas_list[a]->id].msy.SSB_F30_msy / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.B_F30_msy += females[areas_list[a]->id].msy.B_F30_msy / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.E_F30_msy += females[areas_list[a]->id].msy.E_F30_msy / static_cast<REAL_T> ((areas_list.size()*2.0));

                this->msy.F35 += females[areas_list[a]->id].msy.F35 / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.spr_F35_msy += females[areas_list[a]->id].msy.spr_F35_msy / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.SR_F35_msy += females[areas_list[a]->id].msy.SR_F35_msy / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.R_F35_msy += females[areas_list[a]->id].msy.R_F35_msy / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.SSB_F35_msy += females[areas_list[a]->id].msy.SSB_F35_msy / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.B_F35_msy += females[areas_list[a]->id].msy.B_F35_msy / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.E_F35_msy += females[areas_list[a]->id].msy.E_F35_msy / static_cast<REAL_T> ((areas_list.size()*2.0));

                this->msy.F40 += females[areas_list[a]->id].msy.F40 / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.spr_F40_msy += females[areas_list[a]->id].msy.spr_F40_msy / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.SR_F40_msy += females[areas_list[a]->id].msy.SR_F40_msy / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.R_F40_msy += females[areas_list[a]->id].msy.R_F40_msy / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.SSB_F40_msy += females[areas_list[a]->id].msy.SSB_F40_msy / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.B_F40_msy += females[areas_list[a]->id].msy.B_F40_msy / static_cast<REAL_T> ((areas_list.size()*2.0));
                this->msy.E_F40_msy += females[areas_list[a]->id].msy.E_F40_msy / static_cast<REAL_T> ((areas_list.size()*2.0));


                this->msy_males.spr_F0 += males[areas_list[a]->id].msy.spr_F0 / static_cast<REAL_T> ((areas_list.size()));
                this->msy_males.F_msy += males[areas_list[a]->id].msy.F_msy / static_cast<REAL_T> ((areas_list.size()));
                this->msy_males.spr_msy += males[areas_list[a]->id].msy.spr_msy / static_cast<REAL_T> ((areas_list.size()));
                this->msy_males.SR_msy += males[areas_list[a]->id].msy.SR_msy / static_cast<REAL_T> ((areas_list.size()));
                this->msy_males.R_msy += males[areas_list[a]->id].msy.R_msy / static_cast<REAL_T> ((areas_list.size()));
                this->msy_males.SSB_msy += males[areas_list[a]->id].msy.SSB_msy;
                this->msy_males.B_msy += males[areas_list[a]->id].msy.B_msy / static_cast<REAL_T> ((areas_list.size()));
                this->msy_males.E_msy += males[areas_list[a]->id].msy.E_msy / static_cast<REAL_T> ((areas_list.size()));

                this->msy_males.F30 += males[areas_list[a]->id].msy.F30 / static_cast<REAL_T> ((areas_list.size()));
                this->msy_males.spr_F30_msy += males[areas_list[a]->id].msy.spr_F30_msy / static_cast<REAL_T> ((areas_list.size()));
                this->msy_males.SR_F30_msy += males[areas_list[a]->id].msy.SR_F30_msy / static_cast<REAL_T> ((areas_list.size()));
                this->msy_males.R_F30_msy += males[areas_list[a]->id].msy.R_F30_msy / static_cast<REAL_T> ((areas_list.size()));
                this->msy_males.SSB_F30_msy += males[areas_list[a]->id].msy.SSB_F30_msy / static_cast<REAL_T> ((areas_list.size()));
                this->msy_males.B_F30_msy += males[areas_list[a]->id].msy.B_F30_msy / static_cast<REAL_T> ((areas_list.size()));
                this->msy_males.E_F30_msy += males[areas_list[a]->id].msy.E_F30_msy / static_cast<REAL_T> ((areas_list.size()));

                this->msy_males.F35 += males[areas_list[a]->id].msy.F35 / static_cast<REAL_T> ((areas_list.size()));
                this->msy_males.spr_F35_msy += males[areas_list[a]->id].msy.spr_F35_msy / static_cast<REAL_T> ((areas_list.size()));
                this->msy_males.SR_F35_msy += males[areas_list[a]->id].msy.SR_F35_msy / static_cast<REAL_T> ((areas_list.size()));
                this->msy_males.R_F35_msy += males[areas_list[a]->id].msy.R_F35_msy / static_cast<REAL_T> ((areas_list.size()));
                this->msy_males.SSB_F35_msy += males[areas_list[a]->id].msy.SSB_F35_msy / static_cast<REAL_T> ((areas_list.size()));
                this->msy_males.B_F35_msy += males[areas_list[a]->id].msy.B_F35_msy / static_cast<REAL_T> ((areas_list.size()));
                this->msy_males.E_F35_msy += males[areas_list[a]->id].msy.E_F35_msy / static_cast<REAL_T> ((areas_list.size()));

                this->msy_males.F40 += males[areas_list[a]->id].msy.F40 / static_cast<REAL_T> ((areas_list.size()));
                this->msy_males.spr_F40_msy += males[areas_list[a]->id].msy.spr_F40_msy / static_cast<REAL_T> ((areas_list.size()));
                this->msy_males.SR_F40_msy += males[areas_list[a]->id].msy.SR_F40_msy / static_cast<REAL_T> ((areas_list.size()));
                this->msy_males.R_F40_msy += males[areas_list[a]->id].msy.R_F40_msy / static_cast<REAL_T> ((areas_list.size()));
                this->msy_males.SSB_F40_msy += males[areas_list[a]->id].msy.SSB_F40_msy / static_cast<REAL_T> ((areas_list.size()));
                this->msy_males.B_F40_msy += males[areas_list[a]->id].msy.B_F40_msy / static_cast<REAL_T> ((areas_list.size()));
                this->msy_males.E_F40_msy += males[areas_list[a]->id].msy.E_F40_msy / static_cast<REAL_T> ((areas_list.size()));

                this->msy_females.spr_F0 += females[areas_list[a]->id].msy.spr_F0 / static_cast<REAL_T> ((areas_list.size()));
                this->msy_females.F_msy += females[areas_list[a]->id].msy.F_msy / static_cast<REAL_T> ((areas_list.size()));
                this->msy_females.spr_msy += females[areas_list[a]->id].msy.spr_msy / static_cast<REAL_T> ((areas_list.size()));
                this->msy_females.SR_msy += females[areas_list[a]->id].msy.SR_msy / static_cast<REAL_T> ((areas_list.size()));
                this->msy_females.R_msy += females[areas_list[a]->id].msy.R_msy / static_cast<REAL_T> ((areas_list.size()));
                this->msy_females.SSB_msy += females[areas_list[a]->id].msy.SSB_msy;
                this->msy_females.B_msy += females[areas_list[a]->id].msy.B_msy / static_cast<REAL_T> ((areas_list.size()));
                this->msy_females.E_msy += females[areas_list[a]->id].msy.E_msy / static_cast<REAL_T> ((areas_list.size()));

                this->msy_females.F30 += females[areas_list[a]->id].msy.F30 / static_cast<REAL_T> ((areas_list.size()));
                this->msy_females.spr_F30_msy += females[areas_list[a]->id].msy.spr_F30_msy / static_cast<REAL_T> ((areas_list.size()));
                this->msy_females.SR_F30_msy += females[areas_list[a]->id].msy.SR_F30_msy / static_cast<REAL_T> ((areas_list.size()));
                this->msy_females.R_F30_msy += females[areas_list[a]->id].msy.R_F30_msy / static_cast<REAL_T> ((areas_list.size()));
                this->msy_females.SSB_F30_msy += females[areas_list[a]->id].msy.SSB_F30_msy / static_cast<REAL_T> ((areas_list.size()));
                this->msy_females.B_F30_msy += females[areas_list[a]->id].msy.B_F30_msy / static_cast<REAL_T> ((areas_list.size()));
                this->msy_females.E_F30_msy += females[areas_list[a]->id].msy.E_F30_msy / static_cast<REAL_T> ((areas_list.size()));

                this->msy_females.F35 += females[areas_list[a]->id].msy.F35 / static_cast<REAL_T> ((areas_list.size()));
                this->msy_females.spr_F35_msy += females[areas_list[a]->id].msy.spr_F35_msy / static_cast<REAL_T> ((areas_list.size()));
                this->msy_females.SR_F35_msy += females[areas_list[a]->id].msy.SR_F35_msy / static_cast<REAL_T> ((areas_list.size()));
                this->msy_females.R_F35_msy += females[areas_list[a]->id].msy.R_F35_msy / static_cast<REAL_T> ((areas_list.size()));
                this->msy_females.SSB_F35_msy += females[areas_list[a]->id].msy.SSB_F35_msy / static_cast<REAL_T> ((areas_list.size()));
                this->msy_females.B_F35_msy += females[areas_list[a]->id].msy.B_F35_msy / static_cast<REAL_T> ((areas_list.size()));
                this->msy_females.E_F35_msy += females[areas_list[a]->id].msy.E_F35_msy / static_cast<REAL_T> ((areas_list.size()));

                this->msy_females.F40 += females[areas_list[a]->id].msy.F40 / static_cast<REAL_T> ((areas_list.size()));
                this->msy_females.spr_F40_msy += females[areas_list[a]->id].msy.spr_F40_msy / static_cast<REAL_T> ((areas_list.size()));
                this->msy_females.SR_F40_msy += females[areas_list[a]->id].msy.SR_F40_msy / static_cast<REAL_T> ((areas_list.size()));
                this->msy_females.R_F40_msy += females[areas_list[a]->id].msy.R_F40_msy / static_cast<REAL_T> ((areas_list.size()));
                this->msy_females.SSB_F40_msy += females[areas_list[a]->id].msy.SSB_F40_msy / static_cast<REAL_T> ((areas_list.size()));
                this->msy_females.B_F40_msy += females[areas_list[a]->id].msy.B_F40_msy / static_cast<REAL_T> ((areas_list.size()));
                this->msy_females.E_F40_msy += females[areas_list[a]->id].msy.E_F40_msy / static_cast<REAL_T> ((areas_list.size()));

                //                msy = 0.0;
                //                f_msy = 0.0;
                //                s_msy = 0.0;
                //
                //                //                males[areas_list[a]->id].CalculateBiologicalReferencePoints(this->years - 1, 0);
                //                //                females[areas_list[a]->id].CalculateBiologicalReferencePoints(this->years - 1, 0);
                //                /******************************************
                //                 * Push info to areas
                //                 *****************************************/
                //                males[areas_list[a]->id].CalculateMaximumSustainableYield(msy, f_msy, s_msy);
                //
                //                this->msy_average += msy;
                //                this->f_msy_average += f_msy;
                //                this->s_msy_average += s_msy;
                //
                //                std::cout << "Male MSY:\n";
                //                std::cout << "alpha = " << males[areas_list[a]->id].recruitment_model->GetAlpha() << "\n";
                //                std::cout << "beta = " << males[areas_list[a]->id].recruitment_model->GetBeta() << "\n";
                //                std::cout << "F30 = " << males[areas_list[a]->id].CalculateFSPR(0.30) << "\n";
                //                std::cout << "F40 = " << males[areas_list[a]->id].CalculateFSPR(0.40) << "\n";
                //                std::cout << "Fmax = " << males[areas_list[a]->id].CalculateFMax(fs) << "\n";
                //                std::cout << "msy = " << msy << "\n";
                //                std::cout << "f_msy = " << f_msy << "\n";
                //                std::cout << "s_msy = " << s_msy << "\n";
                //                std::cout << "R0 = " << males[areas_list[a]->id].R0 << "\n";
                //                std::cout << "SB0 = " << males[areas_list[a]->id].SB0 << "\n";
                //                std::cout << "S0 = " << males[areas_list[a]->id].S0 << "\n\n";
                //
                //                msy = 0.0;
                //                f_msy = 0.0;
                //                s_msy = 0.0;
                //
                //                females[areas_list[a]->id].CalculateMaximumSustainableYield(msy, f_msy, s_msy);
                //                this->msy_average += msy;
                //                this->f_msy_average += f_msy;
                //                this->s_msy_average += s_msy;
                //                std::cout << "Female MSY:\n";
                //                std::cout << "unfished_spr = " << females[areas_list[a]->id].unfished_spawners_per_recruit << "\n";
                //                std::cout << "alpha = " << females[areas_list[a]->id].recruitment_model->GetAlpha() << "\n";
                //                std::cout << "beta = " << females[areas_list[a]->id].recruitment_model->GetBeta() << "\n";
                //                std::cout << "F30 = " << females[areas_list[a]->id].CalculateFSPR(0.30) << "\n";
                //                std::cout << "F40 = " << females[areas_list[a]->id].CalculateFSPR(0.40) << "\n";
                //                std::cout << "Fmax = " << females[areas_list[a]->id].CalculateFMax(fs) << "\n";
                //                std::cout << "msy = " << msy << "\n";
                //                std::cout << "f_msy = " << f_msy << "\n";
                //                std::cout << "s_msy = " << s_msy << "\n";
                //                std::cout << "R0 = " << females[areas_list[a]->id].R0 << "\n";
                //                std::cout << "SB0 = " << females[areas_list[a]->id].SB0 << "\n";
                //                std::cout << "S0 = " << females[areas_list[a]->id].S0 << "\n\n";

                //                males[areas_list[a]->id].BuildBootStrapNumbers();
                //                females[areas_list[a]->id].BuildBootStrapNumbers();

            }
            //
            //            this->msy_average /= (this->areas_list.size() * 2.0);
            //            this->f_msy_average /= (this->areas_list.size() * 2.0);
            //            this->s_msy_average /= (this->areas_list.size() * 2.0);
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
