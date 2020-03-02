/*
 * File:   Area.hpp
 *
 * Author: Matthew Supernaw
 * National Oceanic and Atmospheric Administration
 * National Marine Fisheries Service
 * Sustainable Fisheries Division
 * St. Petersburg, FL, 33701
 *
 * Created on September 16, 2016, 12:36 PM
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

#ifndef MAS_AREA_HPP
#define MAS_AREA_HPP
#include "Recruitment.hpp"
#include "Mortality.hpp"
#include "Growth.hpp"
#include "Fecundity.hpp"
#include "Selectivity.hpp"
#include "Fleet.hpp"
#include "Survey.hpp"

namespace mas {

    template<typename REAL_T>
    struct Area {
        typedef typename mas::VariableTrait<REAL_T>::variable variable;
        std::string name;
        REAL_T logscale_standard_deviation_catch_biomass = static_cast<REAL_T> (.1);
        REAL_T logscale_standard_deviation_survey_biomass = static_cast<REAL_T> (.3);
        int id;

        int years;
        int seasons;
        int ages;

        // int growth_model_id;
        // int recruitment_model_id;
        // int mortality_model_id;
        // int fecundity_model_id;

        std::vector<variable> catch_biomass_total;
        std::vector<variable> survey_biomass_total; //survey numbers at age

        std::vector<variable> catch_biomass_total_males;
        std::vector<variable> survey_biomass_total_males; //survey numbers at age

        std::vector<variable> catch_biomass_total_females;
        std::vector<variable> survey_biomass_total_females; //survey numbers at age

        std::vector<variable> numbers_at_age;
        std::vector<variable> numbers_total;
        std::vector<variable> proportion_at_age;
        std::vector<variable> catch_proportion_at_age;
        std::vector<variable> catch_biomass_proportion_at_age;
        std::vector<variable> catch_at_age;
        std::vector<variable> catch_biomass_at_age;
        std::vector<variable> survey_numbers_at_age; //survey numbers at age
        std::vector<variable> survey_biomass_at_age; //survey biomass at age
        std::vector<variable> survey_proportion_at_age;
        std::vector<variable> survey_biomass_proportion_at_age;


        std::vector<variable> numbers_at_age_males;
        std::vector<variable> numbers_total_males;
        std::vector<variable> proportion_at_age_males;
        std::vector<variable> catch_proportion_at_age_males;
        std::vector<variable> catch_biomass_proportion_at_age_males;
        std::vector<variable> catch_at_age_males;
        std::vector<variable> catch_biomass_at_age_males;
        std::vector<variable> survey_numbers_at_age_males; //survey numbers at age
        std::vector<variable> survey_biomass_at_age_males; //survey biomass at age
        std::vector<variable> survey_proportion_at_age_males;
        std::vector<variable> survey_biomass_proportion_at_age_males;

        std::vector<variable> numbers_at_age_females;
        std::vector<variable> numbers_total_females;
        std::vector<variable> proportion_at_age_females;
        std::vector<variable> catch_proportion_at_age_females;
        std::vector<variable> catch_biomass_proportion_at_age_females;
        std::vector<variable> catch_at_age_females;
        std::vector<variable> catch_biomass_at_age_females;
        std::vector<variable> survey_numbers_at_age_females; //survey numbers at age
        std::vector<variable> survey_biomass_at_age_females; //survey biomass at age
        std::vector<variable> survey_proportion_at_age_females;
        std::vector<variable> survey_biomass_proportion_at_age_females;


        std::vector<variable> N_diff2;
        std::vector<variable> N_Proportion_diff2;
        std::vector<variable> C_Proportion_diff2;
        std::vector<variable> C_Biomass_Proportion_diff2;
        std::vector<variable> C_diff2;
        std::vector<variable> C_Biomass_diff2;
        std::vector<variable> SN_diff2; //survey numbers at age
        std::vector<variable> SN_Biomass_diff2; //survey biomass at age
        std::vector<variable> SN_Proportion_diff2;
        std::vector<variable> SN_Biomass_Proportion_diff2;


        // NOTE:  these are population-area-specific, not area-specific
        // std::shared_ptr<GrowthBase<REAL_T> > growth_model;
        // std::shared_ptr<RecruitmentBase<REAL_T> > recruitment_model;
        // std::shared_ptr<NaturalMortality<REAL_T> > mortality_model;
        // std::shared_ptr<FecundityBase<REAL_T> > fecundity_model;

        std::map<int, std::vector< std::shared_ptr<Fleet<REAL_T> > > > seasonal_fleet_operations;
        std::map<int, std::vector< std::shared_ptr<Survey<REAL_T> > > > seasonal_survey_operations;
        std::set<std::shared_ptr<Fleet<REAL_T> > > active_fleets;//set of fleets that may be operational in this area
        typedef typename  std::set<std::shared_ptr<Fleet<REAL_T> > >::iterator active_fleets_iterator;
        typedef typename std::map<int, std::vector< std::shared_ptr<Fleet<REAL_T> > > >::iterator seasonal_fleet_operations_iterator;

        std::map<int, std::map<int, std::shared_ptr<mas::Survey<REAL_T> > > > survey_models; //population, season keyed
        typedef typename std::map<int, std::map<int, std::shared_ptr<mas::Survey<REAL_T> > > > ::iterator survey_model_iterator;
        typedef typename std::map<int, std::shared_ptr<mas::Survey<REAL_T> > >::iterator survey_season_iterator;

        std::vector<std::shared_ptr<DataObject<REAL_T> > > survey_biomass_data;
        std::vector<std::shared_ptr<DataObject<REAL_T> > > survey_proportion_data_N;
        std::vector<std::shared_ptr<DataObject<REAL_T> > > survey_proportion_data;
        std::vector<std::shared_ptr<DataObject<REAL_T> > > survey_proportion_length_data_N;
        std::vector<std::shared_ptr<DataObject<REAL_T> > > survey_proportion_length_data;
        std::vector<std::shared_ptr<DataObject<REAL_T> > > survey_mean_size_data;

        std::vector<std::shared_ptr<DataObject<REAL_T> > > catch_biomass_data;
        std::vector<std::shared_ptr<DataObject<REAL_T> > > catch_proportion_data_N;
        std::vector<std::shared_ptr<DataObject<REAL_T> > > catch_proportion_data;
        std::vector<std::shared_ptr<DataObject<REAL_T> > > catch_proportion_length_data_N;
        std::vector<std::shared_ptr<DataObject<REAL_T> > > catch_proportion_length_data;
        std::vector<std::shared_ptr<DataObject<REAL_T> > > catch_mean_size_data;


        //runtime
        variable catch_biomass_component;
        variable survey_biomass_component;
        variable fishery_age_comp_component;
        variable survey_age_comp_component;
        variable recruitment_deviations_component;

        void Initialize(size_t years, size_t seasons, size_t ages) {
            this->years = years;
            this->seasons = seasons;
            this->ages = ages;


            numbers_at_age.resize(years * seasons * ages);
            catch_at_age.resize(years * seasons * ages);
            catch_biomass_at_age.resize(years * seasons * ages);
            proportion_at_age.resize(years * seasons * ages);
            catch_proportion_at_age.resize(years * seasons * ages);
            catch_biomass_proportion_at_age.resize(years * seasons * ages);
            survey_numbers_at_age.resize(years * seasons * ages);
            survey_biomass_at_age.resize(years * seasons * ages);
            survey_proportion_at_age.resize(years * seasons * ages);
            survey_biomass_proportion_at_age.resize(years * seasons * ages);


            numbers_at_age_males.resize(years * seasons * ages);
            catch_at_age_males.resize(years * seasons * ages);
            catch_biomass_at_age_males.resize(years * seasons * ages);
            proportion_at_age_males.resize(years * seasons * ages);
            catch_proportion_at_age_males.resize(years * seasons * ages);
            catch_biomass_proportion_at_age_males.resize(years * seasons * ages);
            survey_numbers_at_age_males.resize(years * seasons * ages);
            survey_biomass_at_age_males.resize(years * seasons * ages);
            survey_proportion_at_age_males.resize(years * seasons * ages);
            survey_biomass_proportion_at_age_males.resize(years * seasons * ages);


            numbers_at_age_females.resize(years * seasons * ages);
            catch_at_age_females.resize(years * seasons * ages);
            catch_biomass_at_age_females.resize(years * seasons * ages);
            proportion_at_age_females.resize(years * seasons * ages);
            catch_proportion_at_age_females.resize(years * seasons * ages);
            catch_biomass_proportion_at_age_females.resize(years * seasons * ages);
            survey_numbers_at_age_females.resize(years * seasons * ages);
            survey_biomass_at_age_females.resize(years * seasons * ages);
            survey_proportion_at_age_females.resize(years * seasons * ages);
            survey_biomass_proportion_at_age_females.resize(years * seasons * ages);


            N_diff2.resize(years * seasons * ages);
            C_diff2.resize(years * seasons * ages);
            C_Biomass_diff2.resize(years * seasons * ages);
            N_Proportion_diff2.resize(years * seasons * ages);
            C_Proportion_diff2.resize(years * seasons * ages);
            C_Biomass_Proportion_diff2.resize(years * seasons * ages);
            SN_diff2.resize(years * seasons * ages);
            SN_Biomass_diff2.resize(years * seasons * ages);
            SN_Proportion_diff2.resize(years * seasons * ages);
            SN_Biomass_Proportion_diff2.resize(years * seasons * ages);
            catch_biomass_total.resize(years * seasons);
            survey_biomass_total.resize(years * seasons);
            catch_biomass_total_males.resize(years * seasons);
            survey_biomass_total_males.resize(years * seasons);
            catch_biomass_total_females.resize(years * seasons);
            survey_biomass_total_females.resize(years * seasons);
        }

        void Prepare() {

            for (int i = 0; i < this->numbers_at_age.size(); i++) {
                //                mas::VariableTrait<REAL_T>::SetValue(SN[i], static_cast<REAL_T> (0.0));
                //                mas::VariableTrait<REAL_T>::SetValue(SN_Biomass[i], static_cast<REAL_T> (0.0));
                //                mas::VariableTrait<REAL_T>::SetValue(N[i], static_cast<REAL_T> (0.0));
                //                mas::VariableTrait<REAL_T>::SetValue(C[i], static_cast<REAL_T> (0.0));
                //                mas::VariableTrait<REAL_T>::SetValue(C_Biomass[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(numbers_at_age[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(catch_at_age[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(catch_biomass_at_age[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(proportion_at_age[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(catch_proportion_at_age[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(catch_biomass_proportion_at_age[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(survey_numbers_at_age[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(survey_biomass_at_age[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(survey_proportion_at_age[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(survey_biomass_proportion_at_age[i], static_cast<REAL_T> (0.0));

                mas::VariableTrait<REAL_T>::SetValue(numbers_at_age_males[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(catch_at_age_males[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(catch_biomass_at_age_males[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(proportion_at_age_males[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(catch_proportion_at_age_males[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(catch_biomass_proportion_at_age_males[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(survey_numbers_at_age_males[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(survey_biomass_at_age_males[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(survey_proportion_at_age_males[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(survey_biomass_at_age_males[i], static_cast<REAL_T> (0.0));

                mas::VariableTrait<REAL_T>::SetValue(numbers_at_age_females[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(catch_at_age_females[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(catch_biomass_at_age_females[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(proportion_at_age_females[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(catch_proportion_at_age_females[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(catch_biomass_proportion_at_age_females[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(survey_numbers_at_age_females[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(survey_biomass_at_age_females[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(survey_proportion_at_age_females[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(survey_biomass_proportion_at_age_females[i], static_cast<REAL_T> (0.0));
            }


            for (int i = 0; i < this->catch_biomass_total.size(); i++) {
                mas::VariableTrait<REAL_T>::SetValue(catch_biomass_total[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(survey_biomass_total[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(catch_biomass_total_males[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(survey_biomass_total_males[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(catch_biomass_total_females[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(survey_biomass_total_females[i], static_cast<REAL_T> (0.0));
            }
        }

        void PushNumbersAndBiomass(const std::vector<variable>& SN_, //survey numbers at age
                const std::vector<variable>& SN_Biomass_, //survey numbers at age
                const std::vector<variable>& N_,
                const std::vector<variable>& C_,
                const std::vector<variable>& C_Biomass_, int population,
                mas::FishSexType sex = mas::UNDIFFERENTIATED) {

            int i;
            switch (sex) {

                case mas::MALE:

                    for (i = 0; i < this->numbers_at_age.size(); i++) {
                        survey_numbers_at_age[i] += SN_[i];
                        survey_biomass_at_age[i] += SN_Biomass_[i];
                        numbers_at_age[i] += N_[i];
                        catch_at_age[i] += C_[i];
                        catch_biomass_at_age[i] += C_Biomass_[i];
                        survey_numbers_at_age_males[i] += SN_[i];
                        survey_biomass_at_age_males[i] += SN_Biomass_[i];
                        numbers_at_age_males[i] += N_[i];
                        catch_at_age_males[i] += C_[i];
                        catch_biomass_at_age_males[i] += C_Biomass_[i];
                    }
                    break;

                case mas::FEMALE:
                    for (i = 0; i < this->numbers_at_age.size(); i++) {
                        survey_numbers_at_age[i] += SN_[i];
                        survey_biomass_at_age[i] += SN_Biomass_[i];
                        numbers_at_age[i] += N_[i];
                        catch_at_age[i] += C_[i];
                        catch_biomass_at_age[i] += C_Biomass_[i];
                        survey_numbers_at_age_females[i] += SN_[i];
                        survey_biomass_at_age_females[i] += SN_Biomass_[i];
                        numbers_at_age_females[i] += N_[i];
                        catch_at_age_females[i] += C_[i];
                        catch_biomass_at_age_females[i] += C_Biomass_[i];
                    }

                    break;

                case mas::UNDIFFERENTIATED:
                    //UNDIFFERENTIATED
                    for (i = 0; i < this->numbers_at_age.size(); i++) {
                        survey_numbers_at_age[i] += SN_[i];
                        survey_biomass_at_age[i] += SN_Biomass_[i];
                        numbers_at_age[i] += N_[i];
                        catch_at_age[i] += C_[i];
                        catch_biomass_at_age[i] += C_Biomass_[i];
                    }
                    break;

            }
        }

        inline void ComputeProportions() {
            for (int y = 0; y < this->years; y++) {
                for (int s = 0; s < this->seasons; s++) {

                    variable total_sn;
                    variable total_sn_males;
                    variable total_sn_females;
                    variable& total_sn_b = this->survey_biomass_total[y * this->seasons + s];
                    variable& total_sn_b_males = this->survey_biomass_total_males[y * this->seasons + s];
                    variable& total_sn_b_females = this->survey_biomass_total_males[y * this->seasons + s];
                    total_sn_b = static_cast<REAL_T> (0.0);
                    variable total_n;
                    variable total_n_males;
                    variable total_n_females;
                    variable total_c;
                    variable total_c_males;
                    variable total_c_females;
                    variable& total_c_b = this->catch_biomass_total[y * this->seasons + s];
                    variable& total_c_b_males = this->catch_biomass_total_males[y * this->seasons + s];
                    variable& total_c_b_females = this->catch_biomass_total_females[y * this->seasons + s];
                    total_c_b = static_cast<REAL_T> (0.0);
                    size_t index = 0;
                    for (int a = 0; a <this->ages; a++) {
                        index = y * this->seasons * this->ages + (s) * this->ages + a;
                        total_sn += survey_numbers_at_age[index];
                        total_sn_males += survey_numbers_at_age_males[index];
                        total_sn_females += survey_numbers_at_age_females[index];

                        total_sn_b += survey_biomass_at_age[index];
                        total_sn_b_males += survey_biomass_at_age_males[index];
                        total_sn_b_females += survey_biomass_at_age_females[index];

                        total_n += numbers_at_age[index];
                        total_n_males += numbers_at_age_males[index];
                        total_n_females += numbers_at_age_females[index];

                        total_c += catch_at_age[index];
                        total_c_males += catch_at_age_males[index];
                        total_c_females += catch_at_age_females[index];

                        total_c_b += catch_biomass_at_age[index];
                        total_c_b_males += catch_biomass_at_age_males[index];
                        total_c_b_females += catch_biomass_at_age_females[index];
                    }


                    for (int a = 0; a <this->ages; a++) {
                        index = y * this->seasons * this->ages + (s) * this->ages + a;
                        survey_proportion_at_age[index] = survey_numbers_at_age[index] / total_sn;
                        survey_proportion_at_age_males[index] = survey_numbers_at_age_males[index] / total_sn_males;
                        survey_proportion_at_age_females[index] = survey_numbers_at_age_females[index] / total_sn_females;

                        survey_biomass_proportion_at_age[index] = survey_biomass_at_age[index] / total_sn_b;
                        survey_biomass_proportion_at_age_males[index] = survey_biomass_at_age_males[index] / total_sn_b_males;
                        survey_biomass_proportion_at_age_females[index] = survey_biomass_at_age_females[index] / total_sn_b_females;

                        proportion_at_age[index] = numbers_at_age[index] / total_n;
                        proportion_at_age_males[index] = numbers_at_age_males[index] / total_n_males;
                        proportion_at_age_females[index] = numbers_at_age_females[index] / total_n_females;

                        catch_proportion_at_age[index] = catch_at_age[index] / total_c;
                        catch_proportion_at_age_males[index] = catch_at_age_males[index] / total_c_males;
                        catch_proportion_at_age_females[index] = catch_at_age_females[index] / total_c_females;

                        catch_biomass_proportion_at_age[index] = catch_biomass_at_age[index] / total_c_b;
                        catch_biomass_proportion_at_age_males[index] = catch_biomass_at_age_males[index] / total_c_b_males;
                        catch_biomass_proportion_at_age_females[index] = catch_biomass_at_age_females[index] / total_c_b_females;

                    }

                }
            }
        }

        const variable Compute() {

            this->fishery_age_comp_component = 0.0;
            this->catch_biomass_component = 0.0;
            this->survey_biomass_component = 0.0;

            this->survey_age_comp_component = 0.0;


            //this is all nonsense.
            variable f;
            variable c_f;
            variable c_b_nll;
            variable c_p_f;
            variable s_f;
            variable s_p_f;
            variable s_b_nll;
            size_t index = 0;
            REAL_T cl = 0;
            REAL_T sl = 0;


            for (int y = 0; y < this->years; y++) {
                for (int s = 0; s < this->seasons; s++) {
                    REAL_T temp = static_cast<REAL_T> (0.0);
                    REAL_T temp_m = static_cast<REAL_T> (0.0);
                    REAL_T temp_f = static_cast<REAL_T> (0.0);
                    for (int i = 0; i < this->catch_biomass_data.size(); i++) {
                        //                        temp += catch_biomass_data[i]->get(y, s);
                        switch (catch_biomass_data[i]->sex_type) {

                            case mas::UNDIFFERENTIATED:
                                temp += catch_biomass_data[i]->get(y, s);
                                break;

                            case mas::MALE:
                                temp += catch_biomass_data[i]->get(y, s);
                                temp_m += catch_biomass_data[i]->get(y, s);
                                break;

                            case mas::FEMALE:
                                temp += catch_biomass_data[i]->get(y, s);
                                temp_f += catch_biomass_data[i]->get(y, s);
                                break;

                        }
                    }
                    //                    std::cout << "temp = " << temp << "\n";
                    this->catch_biomass_component += .5 * atl::pow(std::log(temp + .00001) - atl::log(this->catch_biomass_total[y * seasons + s] + .00001), 2.0) / .05;
                    //                    c_f += (((atl::log(temp / this->catch_biomass_total[y * seasons + s])) / this->logscale_standard_deviation_catch_biomass
                    //                            + .5 * this->logscale_standard_deviation_catch_biomass))*
                    //                            (((atl::log(temp / this->catch_biomass_total[y * seasons + s])) / this->logscale_standard_deviation_catch_biomass
                    //                            + .5 * this->logscale_standard_deviation_catch_biomass));
                    //                    std::cout << "c_f = " << c_f << "\n";
                    cl += std::log(this->logscale_standard_deviation_catch_biomass);

                    temp = static_cast<REAL_T> (0.0);
                    for (int i = 0; i < this->survey_biomass_data.size(); i++) {
                        temp += survey_biomass_data[i]->get(y, s);
                    }
                    //                    s_f += ((this->survey_biomass_total[y * seasons + s] - temp)*
                    //                            (this->survey_biomass_total[y * seasons + s] - temp));

                    this->survey_biomass_component += .5 * atl::pow(std::log(temp + .0001) - atl::log(this->survey_biomass_total[y * seasons + s] + .0001), 2.0) / .05;
                    //                    s_f += (((atl::log(temp - this->survey_biomass_total[y * seasons + s])) / this->logscale_standard_deviation_survey_biomass
                    //                            + .5 * this->logscale_standard_deviation_survey_biomass))*
                    //                            (((atl::log(temp - this->survey_biomass_total[y * seasons + s])) / this->logscale_standard_deviation_survey_biomass +
                    //                            .5 * this->logscale_standard_deviation_survey_biomass));
                    //                    sl += std::log(this->logscale_standard_deviation_catch_biomass);

                    temp = static_cast<REAL_T> (0.0);
                    for (int a = 0; a <this->ages; a++) {
                        for (int i = 0; i < this->catch_proportion_data.size(); i++) {
                            index = y * this->seasons * this->ages + (s) * this->ages + a;
                            //                            c_p_f += (this->catch_proportion_at_age[index] - this->catch_proportion_data[i]->get(y, s, a))*
                            //                                    (this->catch_proportion_at_age[index] - this->catch_proportion_data[i]->get(y, s, a));
                            this->fishery_age_comp_component -= this->catch_proportion_data_N[i]->get(y, s) * this->catch_proportion_data[i]->get(y, s, a) *
                                    (atl::log(this->catch_proportion_at_age[index] + .0001) - std::log(this->catch_proportion_data[i]->get(y, s, a) + .0001));
                        }
                    }

                    //
                    for (int a = 0; a <this->ages; a++) {
                        for (int i = 0; i < this->survey_proportion_data.size(); i++) {
                            index = y * this->seasons * this->ages + (s) * this->ages + a;
                            //                            s_p_f += (this->survey_proportion_at_age[index] - this->survey_proportion_data[i]->get(y, s, a))*
                            //                                    (this->survey_proportion_at_age[index] - this->survey_proportion_data[i]->get(y, s, a));
                            this->survey_age_comp_component -= this->survey_proportion_data_N[i]->get(y, s) * this->survey_proportion_data[i]->get(y, s, a) *
                                    (atl::log(this->survey_proportion_at_age[index] + .0001) - std::log(this->survey_proportion_data[i]->get(y, s, a) + .0001));
                        }
                    }
                }
            }
            c_b_nll = .1 + .5 * c_f;
            s_b_nll = .3 + .5 * s_f;

            //            std::cout << c_b_nll << "  " << s_b_nll << "\n";

            f = c_b_nll + s_b_nll; //+static_cast<REAL_T> (.5) * atl::log(s_p_f) + static_cast<REAL_T> (.5) * atl::log(c_p_f);

            //            f = static_cast<REAL_T> (.5) * atl::log(c_f) + static_cast<REAL_T> (.5) * atl::log(s_f) +
            //                    static_cast<REAL_T> (.5) * atl::log(s_p_f) + static_cast<REAL_T> (.5) * atl::log(c_p_f);

            return f;
        }

        /**
         * Returns a reference to the catch biomass component for the
         * likelihood function.
         *
         * @return
         */
        const variable& GetCatchBiomassComponent() const {
            return catch_biomass_component;
        }

        /**
         * Returns a reference to the survey age composition component for the
         * likelihood function.
         *
         * @return
         */
        const variable& GetSurveyAgeCompComponent() const {
            return survey_age_comp_component;
        }

        /**
         * Returns a reference to the survey biomass component for the
         * likelihood function.
         *
         * @return
         */
        const variable& GetSurveyBiomassComponent() const {
            return survey_biomass_component;
        }

        /**
         * Returns a reference to the fishery age composition component for the
         * likelihood function.
         *
         * @return
         */
        const variable& GetFisheryAgeCompComponent() const {
            return fishery_age_comp_component;
        }

        /**
         * Returns a reference to the recruitment deviations component for the
         * likelihood function.
         *
         * @return
         */
        const variable& GetRecruitmentDeviationsComponent() const {
            return recruitment_deviations_component;
        }

        virtual const std::string ToJSONString() {
            std::stringstream ss;
            ss << "\"area\" : {\n";
            ss << "\"id\" : " << this->id << ",\n";
            ss << "\"name\" : \"" << this->name << "\"\n";
            ss << "}";
            return ss.str();
        }


    };

    template<typename REAL_T>
    std::ostream& operator<<(std::ostream& out, const mas::Area<REAL_T>& area) {

        out << "Area:\n";
        out << "Name: " << area.name << "\n";
        out << "Id: " << area.id << "\n";
        out << "\n";

        // out << "Growth Model: " << area.growth_model->id << "\n";
        // out << "Recruitment Model: " << area.recruitment_model->id << "\n";
        // out << "Mortality Model: " << area.mortality_model->id << "\n\n";
        out << "Catch Biomass Objects: " << area.catch_biomass_data.size() << "\n";
        out << "Catch Proportion Objects: " << area.catch_proportion_data.size() << "\n";
        out << "Catch Proportion Length Objects: " << area.catch_proportion_length_data.size() << "\n";
        out << "Catch Mean Size Objects: " << area.catch_mean_size_data.size() << "\n";
        out << "Survey Index Objects: " << area.survey_biomass_data.size() << "\n";
        out << "Survey Proportion Objects: " << area.survey_proportion_data.size() << "\n";
        out << "Survey Proportion Length Objects: " << area.survey_proportion_length_data.size() << "\n";
        out << "Survey Mean Size Objects: " << area.survey_mean_size_data.size() << "\n";
        out << "\n\n";


        // output fully-selected fishing mortality and selectivity-at-age for all seasons and fleets in each season
        for (int s = 1; s <= area.seasons; ++s) {
            const std::vector< std::shared_ptr<Fleet<REAL_T> > >& area_fleets = area.seasonal_fleet_operations.at(s);

            if (!area_fleets.empty()) {
                for (int f = 0; f < area_fleets.size(); ++f) {
                    /*
                     * WHY DOESN'T THIS WORK???
                    out << "Fleet fishing mortality by year:\n";
                    out << "Area " << area.id << " Fleet " << area_fleets[f]->id << " Season " << s << "\n";
                    for (int y = 0; y < area_fleets[f]->years; y++)
                    {
                        out << area_fleets[f]->area_season_fishing_mortality[area.id][s]->Evaluate(y, s) << " ";
                    }
                    out << "\n\n";
                     */

                    out << "Fleet selectivity-at-age:\n";
                    out << "Area " << area.id << " Fleet " << area_fleets[f]->id << " Season " << s << "\n";
                    for (int a = 0; a < area_fleets[f]->ages; a++) {
                        out << area_fleets[f]->season_area_selectivity[s][area.id]->Evaluate(a) << " ";
                    }
                    out << "\n\n";
                }
            }
            out << "\n";
        }


        out << "Area " << area.id << "\n";
        out << "Expected Numbers of Fish at Age:\n";

        for (int a = 0; a < area.ages; a++) {
            for (int y = 0; y < area.years; y++) {
                for (int s = 0; s < area.seasons; s++) {
                    out << area.numbers_at_age[y * area.seasons * area.ages + (s) * area.ages + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";
        out << "Area " << area.id << "\n";
        out << "Expected Proportions of Fish at Age:\nTotal\n";

        for (int a = 0; a < area.ages; a++) {
            for (int y = 0; y < area.years; y++) {
                for (int s = 0; s < area.seasons; s++) {
                    out << area.proportion_at_age[y * area.seasons * area.ages + (s) * area.ages + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";

        out << "\n\n";
        out << "Area " << area.id << "\n";
        out << "Expected Proportions of Fish at Age:\nMales\n";

        for (int a = 0; a < area.ages; a++) {
            for (int y = 0; y < area.years; y++) {
                for (int s = 0; s < area.seasons; s++) {
                    out << area.proportion_at_age_males[y * area.seasons * area.ages + (s) * area.ages + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";

        out << "\n\n";
        out << "Area " << area.id << "\n";
        out << "Expected Proportions of Fish at Age:\nFemales\n";

        for (int a = 0; a < area.ages; a++) {
            for (int y = 0; y < area.years; y++) {
                for (int s = 0; s < area.seasons; s++) {
                    out << area.proportion_at_age_females[y * area.seasons * area.ages + (s) * area.ages + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";

        out << "\n\n";
        out << "Area " << area.id << "\n";
        out << "Expected Catch at Age:\nTotal\n";

        for (int a = 0; a < area.ages; a++) {
            for (int y = 0; y < area.years; y++) {
                for (int s = 0; s < area.seasons; s++) {
                    out << area.catch_at_age[y * area.seasons * area.ages + (s) * area.ages + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";
        out << "Area " << area.id << "\n";
        out << "Expected Catch at Age:\nMales\n";

        for (int a = 0; a < area.ages; a++) {
            for (int y = 0; y < area.years; y++) {
                for (int s = 0; s < area.seasons; s++) {
                    out << area.catch_at_age_males[y * area.seasons * area.ages + (s) * area.ages + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";

        out << "\n\n";
        out << "Area " << area.id << "\n";
        out << "Expected Catch at Age:\nFemales\n";

        for (int a = 0; a < area.ages; a++) {
            for (int y = 0; y < area.years; y++) {
                for (int s = 0; s < area.seasons; s++) {
                    out << area.catch_at_age_males[y * area.seasons * area.ages + (s) * area.ages + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";

        out << "\n\n";
        out << "Area " << area.id << "\n";
        out << "Expected Catch Proportions at Age:\nTotal\n";

        for (int a = 0; a < area.ages; a++) {
            for (int y = 0; y < area.years; y++) {
                for (int s = 0; s < area.seasons; s++) {
                    out << area.catch_proportion_at_age[y * area.seasons * area.ages + (s) * area.ages + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";

        out << "\n\n";
        out << "Area " << area.id << "\n";
        out << "Expected Catch Proportions at Age:\nMales\n";

        for (int a = 0; a < area.ages; a++) {
            for (int y = 0; y < area.years; y++) {
                for (int s = 0; s < area.seasons; s++) {
                    out << area.catch_proportion_at_age_males[y * area.seasons * area.ages + (s) * area.ages + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";

        out << "\n\n";
        out << "Area " << area.id << "\n";
        out << "Expected Catch Proportions at Age:\nFemales\n";

        for (int a = 0; a < area.ages; a++) {
            for (int y = 0; y < area.years; y++) {
                for (int s = 0; s < area.seasons; s++) {
                    out << area.catch_proportion_at_age_females[y * area.seasons * area.ages + (s) * area.ages + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";

        out << "Area " << area.id << "\n";
        out << "Expected Catch Biomass:\nTotal\n";

        for (int a = 0; a < area.ages; a++) {
            for (int y = 0; y < area.years; y++) {
                for (int s = 0; s < area.seasons; s++) {
                    out << area.catch_biomass_at_age[y * area.seasons * area.ages + (s) * area.ages + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";

        out << "\n\n";
        out << "Area " << area.id << "\n";
        out << "Expected Catch Biomass:\nMales\n";

        for (int a = 0; a < area.ages; a++) {
            for (int y = 0; y < area.years; y++) {
                for (int s = 0; s < area.seasons; s++) {
                    out << area.catch_biomass_at_age_males[y * area.seasons * area.ages + (s) * area.ages + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";

        out << "\n\n";
        out << "Area " << area.id << "\n";
        out << "Expected Catch Biomass:\nFemales\n";

        for (int a = 0; a < area.ages; a++) {
            for (int y = 0; y < area.years; y++) {
                for (int s = 0; s < area.seasons; s++) {
                    out << area.catch_biomass_at_age_females[y * area.seasons * area.ages + (s) * area.ages + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";


        out << "\n\n";
        out << "Area " << area.id << "\n";
        out << "Expected Catch Biomass:\nTotal\n";
        for (int s = 0; s < area.seasons; s++) {
            for (int y = 0; y < area.years; y++) {
                out << area.catch_biomass_total[y * area.seasons + s] << " ";
            }
            out << "\n";
        }
        out << "\n\n";

        out << "Area " << area.id << "\n";
        out << "Expected Survey Numbers at Age:\nTotal\n";

        for (int a = 0; a < area.ages; a++) {
            for (int y = 0; y < area.years; y++) {
                for (int s = 0; s < area.seasons; s++) {
                    out << area.survey_numbers_at_age[y * area.seasons * area.ages + (s) * area.ages + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";

        out << "\n\n";
        out << "Area " << area.id << "\n";
        out << "Expected Survey Numbers at Age:\nMales\n";

        for (int a = 0; a < area.ages; a++) {
            for (int y = 0; y < area.years; y++) {
                for (int s = 0; s < area.seasons; s++) {
                    out << area.survey_numbers_at_age_males[y * area.seasons * area.ages + (s) * area.ages + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";

        out << "\n\n";
        out << "Area " << area.id << "\n";
        out << "Expected Survey Numbers at Age:\nFemales\n";

        for (int a = 0; a < area.ages; a++) {
            for (int y = 0; y < area.years; y++) {
                for (int s = 0; s < area.seasons; s++) {
                    out << area.survey_numbers_at_age_females[y * area.seasons * area.ages + (s) * area.ages + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";

        out << "Area " << area.id << "\n";
        out << "Expected Survey Proportions at Age:\nTotal\n";

        for (int a = 0; a < area.ages; a++) {
            for (int y = 0; y < area.years; y++) {
                for (int s = 0; s < area.seasons; s++) {
                    out << area.survey_proportion_at_age[y * area.seasons * area.ages + (s) * area.ages + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";

        out << "\n\n";
        out << "Area " << area.id << "\n";
        out << "Expected Survey Proportions at Age:\nMales\n";

        for (int a = 0; a < area.ages; a++) {
            for (int y = 0; y < area.years; y++) {
                for (int s = 0; s < area.seasons; s++) {
                    out << area.survey_proportion_at_age_males[y * area.seasons * area.ages + (s) * area.ages + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";

        out << "\n\n";
        out << "Area " << area.id << "\n";
        out << "Expected Survey Proportions at Age:\nFemales\n";

        for (int a = 0; a < area.ages; a++) {
            for (int y = 0; y < area.years; y++) {
                for (int s = 0; s < area.seasons; s++) {
                    out << area.survey_proportion_at_age_females[y * area.seasons * area.ages + (s) * area.ages + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";

        out << "Area " << area.id << "\n";
        out << "Expected Survey Index Total:\nTotal\n";
        for (int a = 0; a < area.ages; a++) {
            for (int y = 0; y < area.years; y++) {
                for (int s = 0; s < area.seasons; s++) {
                    out << area.survey_biomass_at_age[y * area.seasons * area.ages + (s) * area.ages + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";

        out << "Area " << area.id << "\n";
        out << "Expected Survey Index Total:\n Males\n";
        for (int a = 0; a < area.ages; a++) {
            for (int y = 0; y < area.years; y++) {
                for (int s = 0; s < area.seasons; s++) {
                    out << area.survey_biomass_at_age_males[y * area.seasons * area.ages + (s) * area.ages + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";

        out << "Area " << area.id << "\n";
        out << "Expected Survey Index Total:\n Females\n";
        for (int a = 0; a < area.ages; a++) {
            for (int y = 0; y < area.years; y++) {
                for (int s = 0; s < area.seasons; s++) {
                    out << area.survey_biomass_at_age_females[y * area.seasons * area.ages + (s) * area.ages + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";

        out << "Area " << area.id << "\n";
        out << "Expected Survey Index:\n";
        for (int s = 0; s < area.seasons; s++) {
            for (int y = 0; y < area.years; y++) {
                out << area.survey_biomass_total[y * area.seasons + s] << " ";
            }
            out << "\n";
        }
        out << "\n\n";

        out << "\n\n";

        out << "\n ********************************************************************************************************************* \n";

        out << "\n\n";

        return out;
    }


}


#endif /* MAS_AREA_HPP */

