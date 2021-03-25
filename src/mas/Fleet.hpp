/*
 * File:   Fleet.hpp
 *
 * Author: Matthew Supernaw
 * National Oceanic and Atmospheric Administration
 * National Marine Fisheries Service
 * Sustainable Fisheries Division
 * St. Petersburg, FL, 33701
 *
 * Created on September 16, 2016, 12:34 PM
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

#ifndef MAS_FLEET_HPP
#define MAS_FLEET_HPP

#include <unordered_set>
#include <random>
#include "Common.hpp"
#include "Selectivity.hpp"
#include "Area.hpp"
#include "NLLComponents.hpp"


namespace mas {

    template<typename REAL_T>
    struct Fleet : mas::ModelObject<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;
        variable f;
        std::string name;


        int years;
        int seasons;
        int ages;
        bool has_catch_data_annual_biomass;
        bool has_catch_data_biomass_time_by_age;
        bool has_catch_data_numbers_at_age;
        bool has_catch_data_proportion_at_age;
        std::vector<bool> active_years;
        std::shared_ptr<DataObject<REAL_T> > catch_biomass_data;

        // NOTE:  need sex-specific proportions-at-age, proportions-at-length, and mean size-at-age data
        std::shared_ptr<DataObject<REAL_T> > catch_proportion_at_age_data_N;
        std::shared_ptr<DataObject<REAL_T> > catch_proportion_at_age_data;
        std::shared_ptr<DataObject<REAL_T> > catch_proportion_at_length_data_N;
        std::shared_ptr<DataObject<REAL_T> > catch_proportion_at_length_data;
        std::shared_ptr<DataObject<REAL_T> > catch_mean_size_at_age_data;
        std::shared_ptr<DataObject<REAL_T> > catch_mean_weight_at_age_data;
        std::shared_ptr<DataObject<REAL_T> > catch_mean_weight_at_age_data_female;
        std::shared_ptr<DataObject<REAL_T> > catch_mean_weight_at_age_data_male;


        std::vector<std::shared_ptr<DataObject<REAL_T> > > data_objects;
        std::vector<variable> nll_component_values;

        int fishery_age_comp_likelihood_component_id = -999;
        std::shared_ptr<mas::NLLFunctor<REAL_T> > fishery_age_comp_likelihood_component;

        int fishery_biomass_likelihood_component_id = -999;
        std::shared_ptr<mas::NLLFunctor<REAL_T> > fishery_biomass_likelihood_component;

        int fishery_biomass_index_likelihood_component_id = -999;
        std::shared_ptr<mas::NLLFunctor<REAL_T> > fishery_biomass_index_likelihood_component;

        std::unordered_set<int> operational_areas;
        std::unordered_map<int, std::unordered_map<int, int> > season_area_selectivity_ids;
        std::unordered_map<int, std::unordered_map<int, std::vector<int> > > season_area_selectivity_ensemble_ids;
        std::unordered_map<int, std::unordered_map<int, std::shared_ptr<mas::SelectivityBase<REAL_T> > > > season_area_selectivity;

        std::unordered_map<int, std::unordered_map<int, int> > area_season_selectivity_ids;
        std::unordered_map<int, std::unordered_map<int, std::shared_ptr<mas::SelectivityBase<REAL_T> > > > area_season_selectivity;

        std::unordered_map<int, std::unordered_map<int, REAL_T> > season_area_catch_fraction;
        std::unordered_map<int, std::unordered_map<int, REAL_T> > area_season_catch_fraction;


        std::unordered_map<int, std::unordered_map<int, int> > area_season_fishing_mortality_ids;
        std::unordered_map<int, std::unordered_map<int, std::shared_ptr<mas::FishingMortality<REAL_T> > > > area_season_fishing_mortality;

        std::unordered_map<int, std::unordered_map<int, int> > season_area_fishing_mortality_ids;
        std::unordered_map<int, std::unordered_map<int, std::vector<int> > > season_area_fishing_mortality_ensemble_ids;
        std::unordered_map<int, std::unordered_map<int, std::shared_ptr<mas::FishingMortality<REAL_T> > > > season_area_fishing_mortality;


        typedef typename std::unordered_map<int, std::unordered_map<int, int> >::iterator season_area_selectivity_ids_iterator;
        typedef typename std::unordered_map<int, std::shared_ptr<mas::SelectivityBase<REAL_T> > >::iterator area_sectivity_iterator;
        typedef typename std::unordered_map<int, std::unordered_map<int, int> >::iterator season_area_id_iterator;
        typedef typename std::unordered_map<int, int>::iterator area_id_iterator;
        typedef typename std::unordered_map<int, int>::iterator season_id_iterator;
        typedef typename std::unordered_map<int, std::unordered_map<int, std::shared_ptr<mas::SelectivityBase<REAL_T> > > >::iterator season_area_selectivity_iterator;
        typedef typename std::unordered_map<int, std::unordered_map<int, std::shared_ptr<mas::FishingMortality<REAL_T> > > >::iterator season_area_fishing_mortality_iterator;
        typedef typename std::unordered_map<int, std::shared_ptr<mas::FishingMortality<REAL_T> > >::iterator area_fishing_mortality_iterator;
        typedef typename std::unordered_map<int, std::shared_ptr<mas::FishingMortality<REAL_T> > >::iterator delta_method_fishing_mortality_iterator;







        std::vector<variable> catch_biomass_total;
        std::vector<variable> catch_abundance;
        std::vector<variable> catch_proportion_at_age;
        std::vector<variable> catch_biomass_proportion_at_age;
        std::vector<variable> catch_at_age;
        std::unordered_map<int, std::unordered_map<int, std::vector<variable> > > f_at_age; //area, population indexed
        std::unordered_map<int, std::unordered_map<int, std::vector<variable> > > z_at_age; //area, population indexed
        std::vector<variable> catch_biomass_at_age;
        std::vector<variable> catch_spawning_biomass_at_age;

        std::vector<std::vector<variable>* > catch_at_age_both;
        std::vector<std::vector<variable>* > catch_biomass_at_age_both;


        std::vector<variable> catch_biomass_total_males;
        std::vector<variable> catch_abundance_males;
        std::vector<variable> catch_proportion_at_age_males;
        std::vector<variable> catch_biomass_proportion_at_age_males;
        std::vector<variable> catch_at_age_males;
        std::unordered_map<int, std::unordered_map<int, std::vector<variable> > > f_at_age_males; //area, population indexed
        std::unordered_map<int, std::unordered_map<int, std::vector<variable> > > z_at_age_males; //area, population indexed
        std::vector<variable> catch_biomass_at_age_males;
        std::vector<variable> catch_spawning_biomass_at_age_males;

        std::vector<variable> catch_biomass_total_females;
        std::vector<variable> catch_abundance_females;
        std::vector<variable> catch_proportion_at_age_females;
        std::vector<variable> catch_biomass_proportion_at_age_females;
        std::vector<variable> catch_at_age_females;
        std::vector<variable> catch_spawning_biomass_at_age_females;

        std::unordered_map<int, std::unordered_map<int, std::vector<variable> > > f_at_age_females; //area, population indexed
        std::unordered_map<int, std::unordered_map<int, std::vector<variable> > > z_at_age_females; //area, population indexed
        std::vector<variable> catch_biomass_at_age_females;


        variable catch_biomass_component;
        variable fishery_age_comp_component;
        variable nll;
        REAL_T CV = .05;
        REAL_T sample_size = 200; //for operating model runs
        REAL_T catch_biomass_chi_squared;
        REAL_T fishery_age_comp_chi_squared;
        REAL_T catch_fraction_of_year = 0.5;


        //goodness of fit
        REAL_T chi_squared;
        REAL_T g_test;
        REAL_T rmse;
        REAL_T rmsle;
        REAL_T r_squared;
        REAL_T AIC;
        REAL_T BIC;

        std::vector<NLLComponent<REAL_T> > nll_components;

        void Initialize(size_t years, size_t seasons, size_t ages) {

            this->years = years;
            this->seasons = seasons;
            this->ages = ages;

            catch_abundance.resize(years * seasons);
            catch_at_age.resize(years * seasons * ages);
            catch_biomass_at_age.resize(years * seasons * ages);
            catch_proportion_at_age.resize(years * seasons * ages);
            catch_biomass_proportion_at_age.resize(years * seasons * ages);
            catch_spawning_biomass_at_age.resize(years * ages);

            catch_abundance_males.resize(years * seasons);
            catch_at_age_males.resize(years * seasons * ages);
            catch_biomass_at_age_males.resize(years * seasons * ages);
            catch_proportion_at_age_males.resize(years * seasons * ages);
            catch_biomass_proportion_at_age_males.resize(years * seasons * ages);
            catch_spawning_biomass_at_age_males.resize(years * ages);

            catch_abundance_females.resize(years * seasons);
            catch_at_age_females.resize(years * seasons * ages);
            catch_biomass_at_age_females.resize(years * seasons * ages);
            catch_proportion_at_age_females.resize(years * seasons * ages);
            catch_biomass_proportion_at_age_females.resize(years * seasons * ages);
            catch_spawning_biomass_at_age_females.resize(years * ages);

            catch_biomass_total.resize(years * seasons);
            catch_biomass_total_males.resize(years * seasons);
            catch_biomass_total_females.resize(years * seasons);

            this->catch_biomass_at_age_both.resize(2);
            this->catch_at_age_both.resize(2);

            this->catch_at_age_both[mas::FEMALE] = &(this->catch_at_age_females);
            this->catch_at_age_both[mas::MALE] = &(this->catch_at_age_males);

            this->catch_biomass_at_age_both[mas::FEMALE] = &(this->catch_biomass_at_age_females);
            this->catch_biomass_at_age_both[mas::MALE] = &(this->catch_biomass_at_age_males);
        }

        void InitializeNLLComponents() {

            this->active_years.resize(this->years, true);
            this->nll_component_values.resize(this->data_objects.size());
            std::stringstream ss;
            ss << "_fleet_" << this->id << "_likelihood_component";
            //            ss << this->id;
            std::string tag = ss.str();


            for (size_t i = 0; i < this->data_objects.size(); i++) {


                ss.str("");
                switch (data_objects[i]->type) {
                    case mas::CATCH_BIOMASS:
                        //                        for (int ay = 0; ay < this->active_years.size(); ay++) {
                        //                            this->active_years[ay] = this->data_objects[i]->has_row_data[ay];
                        //                        }
                        switch (data_objects[i]->sex_type) {

                            case mas::FEMALE:
                                ss.str("");
                                ss << "catch_biomass_female_" << tag;
                                nll_component_values[i] = variable();
                                nll_component_values[i].SetName(ss.str());
                                this->nll_components.push_back(mas::NLLComponent<REAL_T>(&catch_biomass_total_females,
                                        this->data_objects[i],
                                        this->fishery_biomass_likelihood_component));
                                break;

                            case mas::MALE:
                                ss.str("");
                                ss << "catch_biomass_male" << tag;
                                nll_component_values[i] = variable();
                                nll_component_values[i].SetName(ss.str());
                                this->nll_components.push_back(mas::NLLComponent<REAL_T>(&catch_biomass_total_males,
                                        this->data_objects[i],
                                        this->fishery_biomass_likelihood_component));
                                break;

                            case mas::UNDIFFERENTIATED:
                                ss.str("");
                                ss << "catch_biomass_undifferentiated" << tag;
                                nll_component_values[i] = variable();
                                nll_component_values[i].SetName(ss.str());
                                this->nll_components.push_back(mas::NLLComponent<REAL_T>(&catch_biomass_total,
                                        this->data_objects[i],
                                        this->fishery_biomass_likelihood_component));
                                break;
                        }

                        break;
                    case mas::CATCH_PROPORTION_AT_AGE_N:
                        break;
                    case mas::CATCH_PROPORTION_AT_AGE:
                        //                        for (int ay = 0; ay < this->active_years.size(); ay++) {
                        //                            this->active_years[ay] = this->data_objects[i]->has_row_data[ay];
                        //                        }
                        switch (data_objects[i]->sex_type) {

                            case mas::FEMALE:
                                ss.str("");
                                ss << "catch_proportion_at_age_female" << tag;
                                nll_component_values[i] = variable();
                                nll_component_values[i].SetName(ss.str());
                                this->nll_components.push_back(mas::NLLComponent<REAL_T>(&catch_proportion_at_age_females,
                                        this->data_objects[i],
                                        this->fishery_age_comp_likelihood_component));
                                break;

                            case mas::MALE:
                                ss.str("");
                                ss << "catch_proportion_at_age_male" << tag;
                                nll_component_values[i] = variable();
                                nll_component_values[i].SetName(ss.str());
                                this->nll_components.push_back(mas::NLLComponent<REAL_T>(&catch_proportion_at_age_males,
                                        this->data_objects[i],
                                        this->fishery_age_comp_likelihood_component));
                                break;

                            case mas::UNDIFFERENTIATED:
                                ss.str("");
                                ss << "catch_proportion_undifferentiated" << tag;
                                nll_component_values[i] = variable();
                                nll_component_values[i].SetName(ss.str());
                                this->nll_components.push_back(mas::NLLComponent<REAL_T>(&catch_proportion_at_age,
                                        this->data_objects[i],
                                        this->fishery_age_comp_likelihood_component));
                                break;
                        }
                        break;
                    case mas::CATCH_PROPORTION_AT_LENGTH_N:
                        break;
                    case mas::CATCH_PROPORTION_AT_LENGTH:
                        break;
                    case mas::CATCH_MEAN_SIZE_AT_AGE:
                        break;
                    case mas::CATCH_MEAN_WEIGHT_AT_AGE:
                        break;
                }
            }

        }

        void Prepare() {
            this->fishery_age_comp_component = 0.0;
            this->catch_biomass_component = 0.0;
            this->fishery_age_comp_chi_squared = 0.0;
            this->catch_biomass_chi_squared = 0.0;

            for (int i = 0; i < this->catch_at_age.size(); i++) {
                mas::VariableTrait<REAL_T>::SetValue(catch_at_age[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(catch_biomass_at_age[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(catch_proportion_at_age[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(catch_biomass_proportion_at_age[i], static_cast<REAL_T> (0.0));

                mas::VariableTrait<REAL_T>::SetValue(catch_at_age_males[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(catch_biomass_at_age_males[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(catch_proportion_at_age_males[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(catch_biomass_proportion_at_age_males[i], static_cast<REAL_T> (0.0));

                mas::VariableTrait<REAL_T>::SetValue(catch_at_age_females[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(catch_biomass_at_age_females[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(catch_proportion_at_age_females[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(catch_biomass_proportion_at_age_females[i], static_cast<REAL_T> (0.0));
            }


            for (int i = 0; i < this->catch_biomass_total.size(); i++) {
                mas::VariableTrait<REAL_T>::SetValue(catch_abundance[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(catch_abundance_males[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(catch_abundance_females[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(catch_biomass_total[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(catch_biomass_total_males[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(catch_biomass_total_females[i], static_cast<REAL_T> (0.0));
            }

        }

        bool OperatesInArea(int area) {
            std::unordered_set<int>::iterator it;
            it = this->operational_areas.find(area);
            return (it != this->operational_areas.end());
        }

        inline void ComputeProportions() {
            for (int y = 0; y < this->years; y++) {
                if (this->active_years[y]) {
                    for (int s = 0; s < this->seasons; s++) {

                        size_t sindex = y * this->seasons + s;
                        variable total_c;
                        variable total_c_males;
                        variable total_c_females;
                        variable& total_c_b = this->catch_biomass_total[y * this->seasons + s];
                        variable& total_c_b_males = this->catch_biomass_total_males[y * this->seasons + s];
                        variable& total_c_b_females = this->catch_biomass_total_females[y * this->seasons + s];
                        total_c_b = static_cast<REAL_T> (1e-5);
                        total_c_b_males = static_cast<REAL_T> (1e-5);
                        total_c_b_females = static_cast<REAL_T> (1e-5);
                        size_t index = 0;
                        for (int a = 0; a <this->ages; a++) {
                            index = y * this->seasons * this->ages + (s * this->ages) + a;

                            total_c += catch_at_age[index];
                            total_c_males += catch_at_age_males[index];
                            total_c_females += catch_at_age_females[index];

                            total_c_b += catch_biomass_at_age[index];
                            total_c_b_males += catch_biomass_at_age_males[index];
                            total_c_b_females += catch_biomass_at_age_females[index];
                        }
                        catch_abundance[sindex] = total_c;
                        catch_abundance_males[sindex] = total_c_males;
                        catch_abundance_females[sindex] = total_c_females;

                        for (int a = 0; a <this->ages; a++) {
                            index = y * this->seasons * this->ages + (s) * this->ages + a;

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
        }

        void ComputeNLLComponents() {
            this->nll = 0.0;
            for (int nllc = 0; nllc < this->nll_components.size(); nllc++) {
                this->nll_component_values[nllc] = static_cast<REAL_T> (0.0);
                this->nll_components[nllc].Evaluate(this->nll_component_values[nllc]);
                this->nll += this->nll_component_values[nllc];
            }
        }

        void ApplyOperatingModelError() {
            this->catch_biomass_data =
                    std::make_shared<mas::DataObject<REAL_T> >();
            this->catch_biomass_data->sex_type = mas::UNDIFFERENTIATED;
            this->catch_biomass_data->id = this->id;
            this->catch_biomass_data->dimensions = 2;
            this->catch_biomass_data->imax = this->years;
            this->catch_biomass_data->jmax - this->seasons;
            this->catch_biomass_data->data.resize(this->years * this->seasons);
            this->catch_biomass_data->observation_error.resize(this->years * this->seasons);

            this->catch_proportion_at_age_data =
                    std::make_shared<mas::DataObject<REAL_T> >();
            this->catch_proportion_at_age_data->sex_type = mas::UNDIFFERENTIATED;
            this->catch_proportion_at_age_data->id = this->id;
            this->catch_proportion_at_age_data->dimensions = 3;
            this->catch_proportion_at_age_data->imax = this->years;
            this->catch_proportion_at_age_data->jmax = this->seasons;
            this->catch_proportion_at_age_data->kmax = this->ages;
            this->catch_proportion_at_age_data->data.resize(this->years * this->seasons * this->ages);
            this->catch_proportion_at_age_data->sample_size.resize(this->years * this->seasons);



            REAL_T sd = std::sqrt(std::log(1 + std::pow(this->CV, 2.0)));

            std::default_random_engine generator;
            std::normal_distribution<double> distribution(0.0, sd);
            //fill in observed data 
            for (int y = 0; y < this->years; y++) {
                for (int s = 0; s < this->seasons; s++) {

                    this->catch_biomass_data->get(y, s) =
                            this->catch_biomass_total[y * this->seasons + s].GetValue()
                            *std::exp(distribution(generator));
                    
                    std::cout<<this->catch_biomass_total[y * this->seasons + s].GetValue()<<"\n";
                    this->catch_biomass_data->get_error(y, s) = this->CV;
                    
                    REAL_T total_c = 0.0;
                    std::vector<REAL_T> probs(this->ages);

                    for (int a = 0; a < this->ages; a++) {
                        size_t index = y * this->seasons * this->ages + (s * this->ages) + a;
                        total_c += catch_at_age[index].GetValue();
                        //                        this->catch_proportion_at_age_data->get(y, s, a) =
                        //                                this->catch_proportion_at_age[y * this->seasons * this->ages +
                        //                                s * this->ages + a];
                    }

                    for (int a = 0; a < this->ages; a++) {
                        size_t index = y * this->seasons * this->ages + (s * this->ages) + a;
                        probs[a] = this->catch_at_age[index].GetValue() / total_c;
                    }

                    std::default_random_engine generator;
                    std::uniform_int_distribution<int> distribution(140, 300);

                    this->catch_proportion_at_age_data->sample_size[y * this->seasons + s] =
                            distribution(generator);
                    std::vector<int> ret = mas::rmultinom(this->catch_proportion_at_age_data->sample_size[y * s + s], probs);
                    for (int a = 0; a < this->ages; a++) {

                        this->catch_proportion_at_age_data->get(y, s, a) =
                                (REAL_T) ret[a] /
                                this->catch_proportion_at_age_data->sample_size[y * this->seasons + s];
                    }



                }
            }


        }

        std::string NLLComponentsToString() {
            std::stringstream ss;
            ss << "Fleet: " << this->id << std::endl;
            for (int nllc = 0; nllc < this->nll_components.size(); nllc++) {
                ss << std::setw(15) << this->nll_component_values[nllc].GetName() << ":" << this->nll_component_values[nllc] << std::endl;
                ss << this->nll_components[nllc];
            }
            return ss.str();
        }

        /**
         * Pearson's chi-squared and g-test for nll components.
         */
        void ComputeGoodnessOfFit() {

            for (int i = 0; i < this->nll_components.size(); i++) {
                this->nll_components[i].Finalize();
                this->chi_squared += this->nll_components[i].chi_square;
                this->g_test += this->nll_components[i].g_test;
                this->rmse += this->nll_components[i].rmse;
                this->rmsle += this->nll_components[i].rmsle;
                this->AIC += this->nll_components[i].AIC;
                this->BIC += this->nll_components[i].BIC;
            }
        }

        inline void EvaluateBiomassComponent(int year, int season) {
            REAL_T o = catch_biomass_data->get_error(year, season);
            REAL_T temp = catch_biomass_data->get(year, season);


            if (temp != catch_biomass_data->missing_value) {
                this->catch_biomass_component += .5 * mas::pow(std::log(temp + o) -
                        mas::log(this->catch_biomass_total[year * seasons + season] + o), 2.0) / .05;
            }
        }

        inline void EvaluateAgeCompComponent(int year, int season) {
            REAL_T o = .001;
#warning hardcoded fishery age comp sample size
            REAL_T temp_sample_size = static_cast<REAL_T> (25.0);
            if (temp_sample_size != this->catch_proportion_at_age_data->missing_value) {
                for (int a = 0; a <this->ages; a++) {
                    REAL_T temp_data = this->catch_proportion_at_age_data->get(year, season, a);
                    o = catch_proportion_at_age_data->get_error(year, season, a);
                    size_t index = year * this->seasons * this->ages + (season) * this->ages + a;
                    this->fishery_age_comp_component -= temp_sample_size * temp_data *
                            (mas::log(this->catch_proportion_at_age[index] + o) - std::log(temp_data + o));
                }
            }
        }

        virtual const std::string ToJSONString() {
            std::stringstream ss;
            ss.setf(std::ios::fixed, std::ios::floatfield);
            ss << "\"fleet\": {\n";
            ss << "\"id\" : " << this->id << ",\n";
            ss << "\"name\" : \"" << this->name << "\",\n";
            season_area_selectivity_ids_iterator it; //same definition as mortality

            ss << "\"fishing_mortality\" : [\n";
            for (it = this->area_season_fishing_mortality_ids.begin(); it != this->area_season_fishing_mortality_ids.end(); ++it) {
                ss << "{\n";
                typename std::unordered_map<int, int>::iterator jt;
                int area = (*it).first;
                for (jt = (*it).second.begin(); jt != (*it).second.end(); ++jt) {
                    ss << "\"id\" : " << (*jt).second << ",\n";
                    ss << "\"season\" : " << (*jt).first << ",\n";
                    ss << "\"area\" : " << area << "\n";
                    if (std::next(jt) != (*it).second.end()) {
                        ss << "},\n";
                    } else {
                        ss << "}\n";
                    }
                }
            }
            ss << "],\n";


            ss << "\"selectivity\" : [\n";
            for (it = this->area_season_selectivity_ids.begin(); it != this->area_season_selectivity_ids.end(); ++it) {
                ss << "{\n";
                typename std::unordered_map<int, int>::iterator jt;
                int area = (*it).first;
                for (jt = (*it).second.begin(); jt != (*it).second.end(); ++jt) {
                    ss << "\"id\" : " << (*jt).second << ",\n";
                    ss << "\"season\" : " << (*jt).first << ",\n";
                    ss << "\"area\" : " << area << "\n";
                    if (std::next(jt) != (*it).second.end()) {
                        ss << "},\n";
                    } else {
                        ss << "}\n";
                    }
                }
            }
            ss << "]\n";

            ss << "}";

            return ss.str();

        }

    };

    template<typename REAL_T >
    std::ostream& operator<<(std::ostream& out, const mas::Fleet<REAL_T>& fleet) {

        out << "Fleet:\n";
        out << "Name: " << fleet.name << "\n";
        out << "Id: " << fleet.id << "\n";
        out << "\n";

        out << "Expected Catch at Age:\nTotal\n";

        for (int a = 0; a < fleet.ages; a++) {
            for (int y = 0; y < fleet.years; y++) {
                for (int s = 0; s < fleet.seasons; s++) {
                    out << fleet.catch_at_age[y * fleet.seasons * fleet.ages + (s) * fleet.ages + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";

        out << "Fleet " << fleet.id << "\n";
        out << "Expected Catch at Age:\nMales\n";

        for (int a = 0; a < fleet.ages; a++) {
            for (int y = 0; y < fleet.years; y++) {
                for (int s = 0; s < fleet.seasons; s++) {
                    out << fleet.catch_at_age_males[y * fleet.seasons * fleet.ages + (s) * fleet.ages + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";

        out << "Fleet " << fleet.id << "\n";
        out << "Expected Catch at Age:\nFemales\n";

        for (int a = 0; a < fleet.ages; a++) {
            for (int y = 0; y < fleet.years; y++) {
                for (int s = 0; s < fleet.seasons; s++) {
                    out << fleet.catch_at_age_males[y * fleet.seasons * fleet.ages + (s) * fleet.ages + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";

        out << "\n\n";
        out << "Fleet " << fleet.id << "\n";
        out << "Expected Catch Proportions at Age:\nTotal\n";

        for (int a = 0; a < fleet.ages; a++) {
            for (int y = 0; y < fleet.years; y++) {
                for (int s = 0; s < fleet.seasons; s++) {
                    out << fleet.catch_proportion_at_age[y * fleet.seasons * fleet.ages + (s) * fleet.ages + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";

        out << "\n\n";
        out << "Fleet " << fleet.id << "\n";
        out << "Expected Catch Proportions at Age:\nMales\n";

        for (int a = 0; a < fleet.ages; a++) {
            for (int y = 0; y < fleet.years; y++) {
                for (int s = 0; s < fleet.seasons; s++) {
                    out << fleet.catch_proportion_at_age_males[y * fleet.seasons * fleet.ages + (s) * fleet.ages + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";

        out << "Observed Catch Proportions at Age:\nMales\n";

        for (int a = 0; a < fleet.ages; a++) {
            for (int y = 0; y < fleet.years; y++) {
                for (int s = 0; s < fleet.seasons; s++) {
                    out << fleet.catch_proportion_at_age_data->get(y, s, a) << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";

        out << "\n\n";
        out << "Fleet " << fleet.id << "\n";
        out << "Expected Catch Proportions at Age:\nFemales\n";

        for (int a = 0; a < fleet.ages; a++) {
            for (int y = 0; y < fleet.years; y++) {
                for (int s = 0; s < fleet.seasons; s++) {
                    out << fleet.catch_proportion_at_age_females[y * fleet.seasons * fleet.ages + (s) * fleet.ages + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";

        out << "Observed Catch Proportions at Age:\nFemales\n";

        for (int a = 0; a < fleet.ages; a++) {
            for (int y = 0; y < fleet.years; y++) {
                for (int s = 0; s < fleet.seasons; s++) {
                    out << fleet.catch_proportion_at_age_data->get(y, s, a) << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";

        out << "\n\n";
        out << "Fleet " << fleet.id << "\n";
        out << "Expected Catch Biomass:\nTotal\n";

        for (int s = 0; s < fleet.seasons; s++) {
            for (int y = 0; y < fleet.years; y++) {
                out << fleet.catch_biomass_total[y * fleet.seasons + s] << " ";
            }
            out << "\n";
        }
        out << "\n\n";

        out << "Observed Catch Biomass:\nTotal\n";
        if (fleet.catch_biomass_data != NULL) {
            for (int s = 0; s < fleet.seasons; s++) {
                for (int y = 0; y < fleet.years; y++) {
                    out << fleet.catch_biomass_data->get(y, s) << " ";
                }
                out << "\n";
            }
            out << "\n\n";
        }
        out << "\n\n";
        out << "Fleet " << fleet.id << "\n";
        out << "Expected Catch Biomass:\nMales\n";

        for (int a = 0; a < fleet.ages; a++) {
            for (int y = 0; y < fleet.years; y++) {
                for (int s = 0; s < fleet.seasons; s++) {
                    out << fleet.catch_biomass_at_age_males[y * fleet.seasons * fleet.ages + (s) * fleet.ages + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";

        out << "\n\n";
        out << "Fleet " << fleet.id << "\n";
        out << "Expected Catch Biomass:\nFemales\n";

        for (int a = 0; a < fleet.ages; a++) {
            for (int y = 0; y < fleet.years; y++) {
                for (int s = 0; s < fleet.seasons; s++) {
                    out << fleet.catch_biomass_at_age_females[y * fleet.seasons * fleet.ages + (s) * fleet.ages + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";

        out << "\n*********************************************************************************************************************\n";

        out << "\n\n";

        return out;
    }


}


#endif /* MAS_FLEET_HPP */

