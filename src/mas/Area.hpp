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
#include "MaximumSustainableYield.hpp"


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
        int references = 0;
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
        std::vector<variable> abundance;
        std::vector<variable> biomass;
        std::vector<variable> proportion_at_age;
        std::vector<variable> catch_proportion_at_age;
        std::vector<variable> catch_biomass_proportion_at_age;
        std::vector<variable> catch_at_age;
        std::vector<variable> catch_biomass_at_age;
        std::vector<variable> survey_numbers_at_age; //survey numbers at age
        std::vector<variable> survey_biomass_at_age; //survey biomass at age
        std::vector<variable> survey_proportion_at_age;
        std::vector<variable> survey_biomass_proportion_at_age;
        std::vector<variable> recruits;
        std::vector<variable> spawning_stock_biomass;


        std::vector<variable> numbers_at_age_males;
        std::vector<variable> abundance_males;
        std::vector<variable> biomass_males;
        std::vector<variable> proportion_at_age_males;
        std::vector<variable> catch_proportion_at_age_males;
        std::vector<variable> catch_biomass_proportion_at_age_males;
        std::vector<variable> catch_at_age_males;
        std::vector<variable> catch_biomass_at_age_males;
        std::vector<variable> survey_numbers_at_age_males; //survey numbers at age
        std::vector<variable> survey_biomass_at_age_males; //survey biomass at age
        std::vector<variable> survey_proportion_at_age_males;
        std::vector<variable> survey_biomass_proportion_at_age_males;
        std::vector<variable> recruits_males;
        std::vector<variable> spawning_stock_biomass_males;

        std::vector<variable> numbers_at_age_females;
        std::vector<variable> abundance_females;
        std::vector<variable> biomass_females;
        std::vector<variable> proportion_at_age_females;
        std::vector<variable> catch_proportion_at_age_females;
        std::vector<variable> catch_biomass_proportion_at_age_females;
        std::vector<variable> catch_at_age_females;
        std::vector<variable> catch_biomass_at_age_females;
        std::vector<variable> survey_numbers_at_age_females; //survey numbers at age
        std::vector<variable> survey_biomass_at_age_females; //survey biomass at age
        std::vector<variable> survey_proportion_at_age_females;
        std::vector<variable> survey_biomass_proportion_at_age_females;
        std::vector<variable> recruits_females;
        std::vector<variable> spawning_stock_biomass_females;


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




        std::map<int, std::vector< atl::intrusive_ptr<Fleet<REAL_T> > > > seasonal_fleet_operations;
        std::map<int, std::vector< atl::intrusive_ptr<Survey<REAL_T> > > > seasonal_survey_operations;
        std::set<atl::intrusive_ptr<Fleet<REAL_T> > > active_fleets; //set of fleets that may be operational in this area
        typedef typename std::set<atl::intrusive_ptr<Fleet<REAL_T> > >::iterator active_fleets_iterator;
        typedef typename std::map<int, std::vector< atl::intrusive_ptr<Fleet<REAL_T> > > >::iterator seasonal_fleet_operations_iterator;

        std::map<int, std::map<int, atl::intrusive_ptr<mas::Survey<REAL_T> > > > survey_models; //population, season keyed
        typedef typename std::map<int, std::map<int, atl::intrusive_ptr<mas::Survey<REAL_T> > > > ::iterator survey_model_iterator;
        typedef typename std::map<int, atl::intrusive_ptr<mas::Survey<REAL_T> > >::iterator survey_season_iterator;

        std::vector<atl::intrusive_ptr<DataObject<REAL_T> > > survey_biomass_data;
        std::vector<atl::intrusive_ptr<DataObject<REAL_T> > > survey_proportion_data_N;
        std::vector<atl::intrusive_ptr<DataObject<REAL_T> > > survey_proportion_data;
        std::vector<atl::intrusive_ptr<DataObject<REAL_T> > > survey_proportion_length_data_N;
        std::vector<atl::intrusive_ptr<DataObject<REAL_T> > > survey_proportion_length_data;
        std::vector<atl::intrusive_ptr<DataObject<REAL_T> > > survey_mean_size_data;

        std::vector<atl::intrusive_ptr<DataObject<REAL_T> > > catch_biomass_data;
        std::vector<atl::intrusive_ptr<DataObject<REAL_T> > > catch_proportion_data_N;
        std::vector<atl::intrusive_ptr<DataObject<REAL_T> > > catch_proportion_data;
        std::vector<atl::intrusive_ptr<DataObject<REAL_T> > > catch_proportion_length_data_N;
        std::vector<atl::intrusive_ptr<DataObject<REAL_T> > > catch_proportion_length_data;
        std::vector<atl::intrusive_ptr<DataObject<REAL_T> > > catch_mean_size_data;


        //runtime
        variable catch_biomass_component;
        variable survey_biomass_component;
        variable fishery_age_comp_component;
        variable survey_age_comp_component;
        variable recruitment_deviations_component;
        
        REAL_T nsubpopulations = 0.0;
        MaximumSustainableYield<REAL_T> msy;
        

        void Initialize(size_t years, size_t seasons, size_t ages) {
            this->years = years;
            this->seasons = seasons;
            this->ages = ages;


            numbers_at_age.resize(years * seasons * ages);
            abundance.resize(years * seasons);
            biomass.resize(years * seasons);
            catch_at_age.resize(years * seasons * ages);
            catch_biomass_at_age.resize(years * seasons * ages);
            proportion_at_age.resize(years * seasons * ages);
            catch_proportion_at_age.resize(years * seasons * ages);
            catch_biomass_proportion_at_age.resize(years * seasons * ages);
            survey_numbers_at_age.resize(years * seasons * ages);
            survey_biomass_at_age.resize(years * seasons * ages);
            survey_proportion_at_age.resize(years * seasons * ages);
            survey_biomass_proportion_at_age.resize(years * seasons * ages);
            recruits.resize(years * seasons);
            spawning_stock_biomass.resize(years * seasons);


            numbers_at_age_males.resize(years * seasons * ages);
            abundance_males.resize(years * seasons);
            biomass_males.resize(years * seasons);
            catch_at_age_males.resize(years * seasons * ages);
            catch_biomass_at_age_males.resize(years * seasons * ages);
            proportion_at_age_males.resize(years * seasons * ages);
            catch_proportion_at_age_males.resize(years * seasons * ages);
            catch_biomass_proportion_at_age_males.resize(years * seasons * ages);
            survey_numbers_at_age_males.resize(years * seasons * ages);
            survey_biomass_at_age_males.resize(years * seasons * ages);
            survey_proportion_at_age_males.resize(years * seasons * ages);
            survey_biomass_proportion_at_age_males.resize(years * seasons * ages);
            recruits_males.resize(years * seasons);
            spawning_stock_biomass_males.resize(years * seasons);

            numbers_at_age_females.resize(years * seasons * ages);
            abundance_females.resize(years * seasons);
            biomass_females.resize(years * seasons);
            catch_at_age_females.resize(years * seasons * ages);
            catch_biomass_at_age_females.resize(years * seasons * ages);
            proportion_at_age_females.resize(years * seasons * ages);
            catch_proportion_at_age_females.resize(years * seasons * ages);
            catch_biomass_proportion_at_age_females.resize(years * seasons * ages);
            survey_numbers_at_age_females.resize(years * seasons * ages);
            survey_biomass_at_age_females.resize(years * seasons * ages);
            survey_proportion_at_age_females.resize(years * seasons * ages);
            survey_biomass_proportion_at_age_females.resize(years * seasons * ages);
            recruits_females.resize(years * seasons);
            spawning_stock_biomass_females.resize(years * seasons);

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
                mas::VariableTrait<REAL_T>::SetValue(abundance[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(biomass[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(abundance_males[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(biomass_males[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(abundance_females[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(biomass_females[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(catch_biomass_total[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(survey_biomass_total[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(catch_biomass_total_males[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(survey_biomass_total_males[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(catch_biomass_total_females[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(survey_biomass_total_females[i], static_cast<REAL_T> (0.0));
            }
        }

        void PushNumbersAndBiomass(const std::vector<variable>& survey_numbers_at_age_,
                const std::vector<variable>& survey_biomass_at_age_,
                const std::vector<variable>& numbers_at_age_,
                const std::vector<variable>& catch_at_age_,
                const std::vector<variable>& catch_biomass_at_age_,
                const std::vector<variable>& recruits_,
                const std::vector<variable>& spawning_stock_biomass_,
                const std::vector<variable>& abundance_,
                const std::vector<variable>& biomass_,
                int population,
                mas::FishSexType sex = mas::UNDIFFERENTIATED) {



            int i;
            switch (sex) {

                case mas::MALE:

                    for (i = 0; i < this->numbers_at_age.size(); i++) {
                        survey_numbers_at_age[i] += survey_numbers_at_age_[i];
                        survey_biomass_at_age[i] += survey_biomass_at_age_[i];
                        numbers_at_age[i] += numbers_at_age_[i];
                        catch_at_age[i] += catch_at_age_[i];
                        catch_biomass_at_age[i] += catch_biomass_at_age_[i];
                        survey_numbers_at_age_males[i] += survey_numbers_at_age[i];
                        survey_biomass_at_age_males[i] += survey_biomass_at_age[i];
                        numbers_at_age_males[i] += numbers_at_age[i];
                        catch_at_age_males[i] += catch_at_age[i];
                        catch_biomass_at_age_males[i] += catch_biomass_at_age[i];
                    }
                    break;

                case mas::FEMALE:
                    for (i = 0; i < this->numbers_at_age.size(); i++) {
                        survey_numbers_at_age[i] += survey_numbers_at_age_[i];
                        survey_biomass_at_age[i] += survey_biomass_at_age_[i];
                        numbers_at_age[i] += numbers_at_age_[i];
                        catch_at_age[i] += catch_at_age_[i];
                        catch_biomass_at_age[i] += catch_biomass_at_age_[i];
                        survey_numbers_at_age_females[i] += survey_numbers_at_age_[i];
                        survey_biomass_at_age_females[i] += survey_biomass_at_age_[i];
                        numbers_at_age_females[i] += numbers_at_age_[i];
                        catch_at_age_females[i] += catch_at_age_[i];
                        catch_biomass_at_age_females[i] += catch_biomass_at_age_[i];
                    }

                    break;

                case mas::UNDIFFERENTIATED:
                    //UNDIFFERENTIATED
                    for (i = 0; i < this->numbers_at_age.size(); i++) {
                        survey_numbers_at_age[i] += survey_numbers_at_age_[i];
                        survey_biomass_at_age[i] += survey_biomass_at_age_[i];
                        numbers_at_age[i] += numbers_at_age_[i];
                        catch_at_age[i] += catch_at_age_[i];
                        catch_biomass_at_age[i] += catch_biomass_at_age_[i];
                    }
                    break;

            }

            switch (sex) {

                case mas::MALE:

                    for (i = 0; i < this->recruits.size(); i++) {
                        this->recruits_males[i] += recruits_[i];
                        this->abundance_males[i] += abundance_[i];
                        this->biomass_males[i] += biomass_[i];
                        this->spawning_stock_biomass_males[i] += spawning_stock_biomass_[i];

                        this->recruits[i] += recruits_[i];
                        this->abundance[i] += abundance_[i];
                        this->biomass[i] += biomass_[i];
                        this->spawning_stock_biomass[i] += spawning_stock_biomass_[i];
                    }
                    break;

                case mas::FEMALE:
                    for (i = 0; i < this->recruits.size(); i++) {
                        this->recruits_females[i] += recruits_[i];
                        this->abundance_females[i] += abundance_[i];
                        this->biomass_females[i] += biomass_[i];
                        this->spawning_stock_biomass_females[i] += spawning_stock_biomass_[i];

                        this->recruits[i] += recruits_[i];
                        this->abundance[i] += abundance_[i];
                        this->biomass[i] += biomass_[i];
                        this->spawning_stock_biomass[i] += spawning_stock_biomass_[i];
                    }

                    break;

                case mas::UNDIFFERENTIATED:
            
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
                    size_t index2 = y * this->seasons + s;

                    abundance[index2] = total_n;
                    abundance_females[index2] = total_n_females;
                    abundance_males[index2] = total_n_males;

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
            const std::vector< atl::intrusive_ptr<Fleet<REAL_T> > >& area_fleets = area.seasonal_fleet_operations.at(s);

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




