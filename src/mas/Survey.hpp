/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Survey.hpp
 * Author: matthewsupernaw
 *
 * Created on January 30, 2017, 3:22 PM
 */

#ifndef SURVEY_HPP
#define SURVEY_HPP

#include "Selectivity.hpp"
#include "Area.hpp"
#include "NLLComponents.hpp"
namespace mas {


    template<typename REAL_T>
    struct Area;

    template<typename REAL_T>
    struct Survey : public mas::ModelObject<REAL_T> {
        int id;
        int years;
        int seasons;
        int ages;
        std::vector<bool> active_years;

        typedef typename VariableTrait<REAL_T>::variable variable;

        std::vector<variable> survey_biomass_total;
        std::vector<variable> survey_abundance;
        std::vector<variable> survey_numbers_at_age;
        std::vector<variable> survey_biomass_at_age;
        std::vector<variable> survey_proportion_at_age;
        std::vector<variable> survey_biomass_proportion_at_age;


        std::vector<variable> survey_biomass_total_males;
        std::vector<variable> survey_abundance_males;
        std::vector<variable> survey_numbers_at_age_males;
        std::vector<variable> survey_biomass_at_age_males;
        std::vector<variable> survey_proportion_at_age_males;
        std::vector<variable> survey_biomass_proportion_at_age_males;


        std::vector<variable> survey_biomass_total_females;
        std::vector<variable> survey_abundance_females;
        std::vector<variable> survey_numbers_at_age_females;
        std::vector<variable> survey_biomass_at_age_females;
        std::vector<variable> survey_proportion_at_age_females;
        std::vector<variable> survey_biomass_proportion_at_age_females;

        std::vector<std::vector<variable>* > survey_numbers_at_age_both;
        std::vector<std::vector<variable>* > survey_biomass_at_age_both;




        std::string name;
        int population;
        int selectivity_model_id;
        std::vector<int> area_ids;
        std::shared_ptr<DataObject<REAL_T> > survey_biomass_data;
        // NOTE:  need sex-specific proportions-at-age, proportions-at-length, and mean size-at-age data
        std::shared_ptr<DataObject<REAL_T> > survey_proportion_at_age_data_N;
        std::shared_ptr<DataObject<REAL_T> > survey_proportion_at_age_data;
        std::shared_ptr<DataObject<REAL_T> > survey_proportion_at_length_data_N;
        std::shared_ptr<DataObject<REAL_T> > survey_proportion_at_length_data;
        std::shared_ptr<DataObject<REAL_T> > survey_mean_size_at_age_data;
        std::shared_ptr<DataObject<REAL_T> > survey_mean_weight_at_age_data;


        std::vector<std::shared_ptr<DataObject<REAL_T> > > data_objects;
        std::vector<variable> nll_component_values;

        std::shared_ptr<mas::SelectivityBase<REAL_T> > selectivity;
        std::unordered_map<int, std::unordered_map<int, int> > area_season_selectivity_ids;
        std::unordered_map<int, std::unordered_map<int, std::shared_ptr<mas::SelectivityBase<REAL_T> > > > area_season_selectivity;

        std::unordered_map<int, std::unordered_map<int, int> > season_area_selectivity_ids;
        std::unordered_map<int, std::unordered_map<int, std::shared_ptr<mas::SelectivityBase<REAL_T> > > > season_area_selectivity;

        std::unordered_map<int, std::unordered_map<int, REAL_T> > season_area_survey_fraction;
        std::unordered_map<int, std::unordered_map<int, REAL_T> > area_season_survey_fraction;


        std::unordered_map<int, std::unordered_map<int, std::shared_ptr<mas::Area< REAL_T> > > > seasonal_areas_of_operation;

        typedef typename std::unordered_map<int, std::unordered_map<int, int> >::iterator season_area_selectivity_ids_iterator;
        typedef typename std::unordered_map<int, std::unordered_map<int, int> >::const_iterator season_area_selectivity_ids_const_iterator;
        typedef typename std::unordered_map<int, std::shared_ptr<mas::SelectivityBase<REAL_T> > >::iterator area_sectivity_iterator;
        typedef typename std::unordered_map<int, std::unordered_map<int, int> >::iterator season_area_id_iterator;
        typedef typename std::unordered_map<int, int>::iterator area_id_iteraor;
        typedef typename std::unordered_map<int, int>::iterator season_id_iteraor;
        typedef typename std::unordered_map<int, std::unordered_map<int, std::shared_ptr<mas::SelectivityBase<REAL_T> > > >::iterator season_area_selectivity_iterator;

        REAL_T survey_fraction_of_year = 0.75;
        REAL_T CV = .2;

        variable q;
        std::vector<variable> q_deviations;

        //runtime
        variable survey_biomass_component;
        variable survey_age_comp_component;

        int survey_age_comp_likelihood_component_id = -999;
        std::shared_ptr<mas::NLLFunctor<REAL_T> > survey_age_comp_likelihood_component;

        int survey_biomass_likelihood_component_id = -999;
        std::shared_ptr<mas::NLLFunctor<REAL_T> > survey_biomass_likelihood_component;

        REAL_T chi_squared;
        REAL_T g_test;
        REAL_T rmse;
        REAL_T rmsle;
        REAL_T AIC;
        REAL_T BIC;
        

        std::vector<NLLComponent<REAL_T> > nll_components;
        variable nll;

        void Initialize(size_t years, size_t seasons, size_t ages) {
            this->years = years;
            this->seasons = seasons;
            this->ages = ages;


            survey_biomass_total.resize(years * seasons);
            survey_abundance.resize(years * seasons);
            survey_numbers_at_age.resize(years * seasons * ages);
            survey_biomass_at_age.resize(years * seasons * ages);
            survey_proportion_at_age.resize(years * seasons * ages);
            survey_biomass_proportion_at_age.resize(years * seasons * ages);

            survey_biomass_total_males.resize(years * seasons);
            survey_abundance_males.resize(years * seasons);
            survey_numbers_at_age_males.resize(years * seasons * ages);
            survey_biomass_at_age_males.resize(years * seasons * ages);
            survey_proportion_at_age_males.resize(years * seasons * ages);
            survey_biomass_proportion_at_age_males.resize(years * seasons * ages);

            survey_biomass_total_females.resize(years * seasons);
            survey_abundance_females.resize(years * seasons);
            survey_numbers_at_age_females.resize(years * seasons * ages);
            survey_biomass_at_age_females.resize(years * seasons * ages);
            survey_proportion_at_age_females.resize(years * seasons * ages);
            survey_biomass_proportion_at_age_females.resize(years * seasons * ages);


            this->survey_biomass_at_age_both.resize(2);
            this->survey_numbers_at_age_both.resize(2);

            this->survey_numbers_at_age_both[mas::FEMALE] = &(this->survey_numbers_at_age_females);
            this->survey_numbers_at_age_both[mas::MALE] = &(this->survey_numbers_at_age_males);

            this->survey_biomass_at_age_both[mas::FEMALE] = &(this->survey_biomass_at_age_females);
            this->survey_biomass_at_age_both[mas::MALE] = &(this->survey_biomass_at_age_males);
        }

        void InitializeNLLComponents() {
            this->active_years.resize(this->years, true);

            this->nll_component_values.resize(this->data_objects.size());
            std::stringstream ss;
            ss << "_survey_" << this->id << "_likelihood_component";
            //            ss << this->id;
            std::string tag = ss.str();


            for (size_t i = 0; i < this->data_objects.size(); i++) {


                switch (data_objects[i]->type) {
                    case mas::SURVEY_BIOMASS:
                        //                        for (int ay = 0; ay < this->active_years.size(); ay++) {
                        //                            this->active_years[ay] = this->data_objects[i]->has_row_data[ay];
                        //                        }
                        switch (data_objects[i]->sex_type) {

                            case mas::FEMALE:
                                ss.str("");
                                ss << "survey_index_female" << tag;
                                nll_component_values[i] = variable();
                                nll_component_values[i].SetName(ss.str());
                                this->nll_components.push_back(mas::NLLComponent<REAL_T>(&survey_biomass_total_females,
                                        this->data_objects[i],
                                        this->survey_biomass_likelihood_component));

                                break;

                            case mas::MALE:
                                ss.str("");
                                ss << "survey_index_male" << tag;
                                nll_component_values[i] = variable();
                                nll_component_values[i].SetName(ss.str());
                                this->nll_components.push_back(mas::NLLComponent<REAL_T>(&survey_biomass_total_males,
                                        this->data_objects[i],
                                        this->survey_biomass_likelihood_component));

                                break;

                            case mas::UNDIFFERENTIATED:
                                ss.str("");
                                ss << "survey_index_undifferentiated" << tag;
                                nll_component_values[i] = variable();
                                nll_component_values[i].SetName(ss.str());
                                this->nll_components.push_back(mas::NLLComponent<REAL_T>(&survey_biomass_total,
                                        this->data_objects[i],
                                        this->survey_biomass_likelihood_component));
                                break;


                        }


                        break;
                    case mas::SURVEY_PROPORTION_AT_AGE_N:
                        break;
                    case mas::SURVEY_PROPORTION_AT_AGE:
                        //                        for (int ay = 0; ay < this->active_years.size(); ay++) {
                        //                            this->active_years[ay] = this->data_objects[i]->has_row_data[ay];
                        //                        }
                        switch (data_objects[i]->sex_type) {

                            case mas::FEMALE:
                                ss.str("");
                                ss << "survey_proportion_at_age_female" << tag;
                                nll_component_values[i] = variable();
                                nll_component_values[i].SetName(ss.str());
                                this->nll_components.push_back(mas::NLLComponent<REAL_T>(&survey_proportion_at_age_females,
                                        this->data_objects[i],
                                        this->survey_age_comp_likelihood_component));
                                break;
                            case mas::MALE:
                                ss.str("");
                                ss << "survey_proportion_at_age_male" << tag;
                                nll_component_values[i] = variable();
                                nll_component_values[i].SetName(ss.str());
                                this->nll_components.push_back(mas::NLLComponent<REAL_T>(&survey_proportion_at_age_males,
                                        this->data_objects[i],
                                        this->survey_age_comp_likelihood_component));
                                break;

                            case mas::UNDIFFERENTIATED:
                                ss.str("");
                                ss << "survey_proportion_undifferentiated" << tag;
                                nll_component_values[i] = variable();
                                nll_component_values[i].SetName(ss.str());
                                this->nll_components.push_back(mas::NLLComponent<REAL_T>(&survey_proportion_at_age,
                                        this->data_objects[i],
                                        this->survey_age_comp_likelihood_component));
                                break;
                        }
                        break;
                    case mas::SURVEY_PROPORTION_AT_LENGTH_N:
                        break;
                    case mas::SURVEY_PROPORTION_AT_LENGTH:
                        break;
                    case mas::SURVEY_MEAN_SIZE_AT_AGE:
                        break;
                    case mas::SURVEY_MEAN_WEIGHT_AT_AGE:
                        break;
                }
            }

        }

        void Prepare() {

            for (int i = 0; i < this->survey_numbers_at_age.size(); i++) {

                mas::VariableTrait<REAL_T>::SetValue(survey_numbers_at_age[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(survey_biomass_at_age[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(survey_proportion_at_age[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(survey_biomass_proportion_at_age[i], static_cast<REAL_T> (0.0));


                mas::VariableTrait<REAL_T>::SetValue(survey_numbers_at_age_males[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(survey_biomass_at_age_males[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(survey_proportion_at_age_males[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(survey_biomass_at_age_males[i], static_cast<REAL_T> (0.0));

                mas::VariableTrait<REAL_T>::SetValue(survey_numbers_at_age_females[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(survey_biomass_at_age_females[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(survey_proportion_at_age_females[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(survey_biomass_proportion_at_age_females[i], static_cast<REAL_T> (0.0));
            }


            for (int i = 0; i < this->survey_biomass_total.size(); i++) {
                mas::VariableTrait<REAL_T>::SetValue(survey_biomass_total[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(survey_biomass_total_males[i], static_cast<REAL_T> (0.0));
                mas::VariableTrait<REAL_T>::SetValue(survey_biomass_total_females[i], static_cast<REAL_T> (0.0));
            }
        }

        inline void ComputeProportions() {
            
            for (int y = 0; y < this->years; y++) {
                if (this->active_years[y]) {
                    for (int s = 0; s < this->seasons; s++) {
                        size_t sindex = y * this->seasons + s;
                        variable total_sn;
                        variable total_sn_males;
                        variable total_sn_females;
                        variable& total_sn_b = this->survey_biomass_total[y * this->seasons + s];
                        variable& total_sn_b_males = this->survey_biomass_total_males[y * this->seasons + s];
                        variable& total_sn_b_females = this->survey_biomass_total_females[y * this->seasons + s];
                        total_sn_b = static_cast<REAL_T> (0.0);
                        total_sn_b_males = static_cast<REAL_T> (0.0);
                        total_sn_b_females = static_cast<REAL_T> (0.0);

                        size_t index = 0;
                        for (int a = 0; a <this->ages; a++) {
                            index = y * this->seasons * this->ages + (s) * this->ages + a;
                            total_sn += survey_numbers_at_age[index];
                            total_sn_males += survey_numbers_at_age_males[index];
                            total_sn_females += survey_numbers_at_age_females[index];

                            total_sn_b += survey_biomass_at_age[index];
                            total_sn_b_males += survey_biomass_at_age_males[index];
                            total_sn_b_females += survey_biomass_at_age_females[index];

                        }
                        survey_abundance[sindex] = total_sn;
                        survey_abundance_males[sindex] = total_sn_males;
                        survey_abundance_females[sindex] = total_sn_females;


                        for (int a = 0; a <this->ages; a++) {
                            index = y * this->seasons * this->ages + (s) * this->ages + a;


                            survey_proportion_at_age[index] = survey_numbers_at_age[index] / total_sn;
                            survey_proportion_at_age_males[index] = survey_numbers_at_age_males[index] / total_sn_males;
                            survey_proportion_at_age_females[index] = survey_numbers_at_age_females[index] / total_sn_females;

                            survey_biomass_proportion_at_age[index] = survey_biomass_at_age[index] / total_sn_b;
                            survey_biomass_proportion_at_age_males[index] = survey_biomass_at_age_males[index] / total_sn_b_males;
                            survey_biomass_proportion_at_age_females[index] = survey_biomass_at_age_females[index] / total_sn_b_females;

                        }

                    }
                }
            }
        }

        void ComputeNLLComponents() {
            nll = 0.0;
            for (int nllc = 0; nllc < this->nll_components.size(); nllc++) {
                this->nll_components[nllc].nll_functor->CV = this->CV;
                this->nll_component_values[nllc] = static_cast<REAL_T> (0.0);
                this->nll_components[nllc].Evaluate(this->nll_component_values[nllc]);
                //                if (this->nll_component_values[nllc].GetValue() != this->nll_component_values[nllc].GetValue()) {
                //                    mas::mas_log << this->nll_component_values[nllc].GetName() << " signaling NaN" << std::endl;
                //                    std::cout << this->nll_component_values[nllc].GetName() << " signaling NaN" << std::endl;
                //                }
                //                if (std::fabs(this->nll_component_values[nllc].GetValue()) == std::numeric_limits<double>::infinity()) {
                //                    mas::mas_log << this->nll_component_values[nllc].GetName() << " signaling inf" << std::endl;
                //                    std::cout << this->nll_component_values[nllc].GetName() << " signaling inf" << std::endl;
                //                }

                this->nll += this->nll_component_values[nllc];
            }
        }

               void ApplyOperatingModelError() {
            this->survey_biomass_data =
                    std::make_shared<mas::DataObject<REAL_T> >();
            this->survey_biomass_data->sex_type = mas::UNDIFFERENTIATED;
            this->survey_biomass_data->id = this->id;
            this->survey_biomass_data->dimensions = 2;
            this->survey_biomass_data->imax = this->years;
            this->survey_biomass_data->jmax - this->seasons;
            this->survey_biomass_data->data.resize(this->years*this->seasons);
            this->survey_biomass_data->observation_error.resize(this->years*this->seasons);

            this->survey_proportion_at_age_data =
                    std::make_shared<mas::DataObject<REAL_T> >();
            this->survey_proportion_at_age_data->sex_type = mas::UNDIFFERENTIATED;
            this->survey_proportion_at_age_data->id = this->id;
            this->survey_proportion_at_age_data->dimensions = 3;
            this->survey_proportion_at_age_data->imax = this->years;
            this->survey_proportion_at_age_data->jmax = this->seasons;
            this->survey_proportion_at_age_data->kmax = this->ages;
            this->survey_proportion_at_age_data->data.resize(this->years*this->seasons*this->ages);
            this->survey_proportion_at_age_data->sample_size.resize(this->years*this->seasons);
            
            
            REAL_T sd = std::sqrt(1 + std::pow(this->CV, 2.0));

            std::default_random_engine generator;
            std::normal_distribution<double> distribution(0.0, sd);
            //fill in observed data 
            for (int y = 0; y < this->years; y++) {
                for (int s = 0; s < this->seasons; s++) {
                    
                    this->survey_biomass_data->get(y, s) =
                            this->survey_biomass_total[y * this->seasons + s].GetValue();

                    this->survey_biomass_data->get_error(y, s) =
                            this->survey_biomass_data->get(y, s) *
                            std::exp(distribution(generator));

                    REAL_T total_c = 0.0;
                    std::vector<REAL_T> probs(this->ages);

                    for (int a = 0; a < this->ages; a++) {
                       size_t index = y * this->seasons * this->ages + (s * this->ages) + a;
                        total_c += survey_numbers_at_age[index].GetValue();
                        //                        this->survey_proportion_at_age_data->get(y, s, a) =
                        //                                this->survey_proportion_at_age[y * this->seasons * this->ages +
                        //                                s * this->ages + a];
                    }

                    for (int a = 0; a < this->ages; a++) {
                        size_t index = y * this->seasons * this->ages + (s * this->ages) + a;
                        probs[a] = this->survey_numbers_at_age[index].GetValue() / total_c;
                    }

                    std::default_random_engine generator;
                    std::uniform_int_distribution<int> distribution(140, 300);

                    this->survey_proportion_at_age_data->sample_size[y*this->seasons + s] =
                           distribution(generator);
                    std::vector<int> ret =  mas::rmultinom(this->survey_proportion_at_age_data->sample_size[y*s + s], probs);
                    for (int a = 0; a < this->ages; a++) {

                        this->survey_proportion_at_age_data->get(y, s, a) =
                                (REAL_T)ret[a]/ 
                                this->survey_proportion_at_age_data->sample_size[y*this->seasons + s];
                    }



                }
            }


        }

        
        
        
        std::string NLLComponentsToString() {
            std::stringstream ss;
            ss << "Survey: " << this->id << std::endl;
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
                this->AIC+= this->nll_components[i].AIC;
                this->BIC+= this->nll_components[i].BIC;     
            }
        }

        inline void EvaluateBiomassComponent(int year, int season) {


            REAL_T o = this->survey_biomass_data->get_error(year, season);
            REAL_T temp = this->survey_biomass_data->get(year, season);
            if (temp != this->survey_biomass_data->missing_value) {
                this->survey_biomass_component +=
                        (.5) * mas::pow((std::log(temp + o) -
                        mas::log(this->survey_biomass_total[year * seasons + season] + o)
                        + (std::pow(this->CV, 2.0) / 2.0)) / this->CV, 2.0);
            }
        }

        inline void EvaluateAgeCompComponent(int year, int season) {
            REAL_T o = .00001;
            variable sum = 0.0;
#warning hardcoded survey age comp sample size
            // REAL_T temp_sample_size = this->survey_proportion_at_age_data_N->get(year, season);
            REAL_T temp_sample_size = static_cast<REAL_T> (10.0);
            if (temp_sample_size != this->survey_proportion_at_age_data->missing_value) {
                for (int a = 0; a <this->ages; a++) {
                    size_t index = year * this->seasons * this->ages + (season) * this->ages + a;
                    REAL_T temp_data = this->survey_proportion_at_age_data->get(year, season, a);
                    o = this->survey_proportion_at_age_data->get_error(year, season, a);
                    sum -= temp_sample_size * temp_data *
                            (mas::log(this->survey_proportion_at_age[index] + o) - std::log(temp_data + o));
                }
            }

            this->survey_age_comp_component -= sum;
        }

    };

    template<typename REAL_T>
    std::ostream& operator<<(std::ostream& out, const mas::Survey<REAL_T>& survey) {

        typename mas::Survey<REAL_T>::season_area_selectivity_ids_const_iterator sait;
        typename std::unordered_map<int, int>::const_iterator iit;

        out << std::fixed;
        out << "Survey\n";
        out << "Name: " << survey.name << "\n";
        out << "Areas: { \n";

        for (sait = survey.season_area_selectivity_ids.begin(); sait != survey.season_area_selectivity_ids.end(); ++sait) {
            out << "{season: " << (*sait).first << ", areas[ ";
            for (iit = (*sait).second.begin(); iit != (*sait).second.end(); ++iit) {
                out << (*iit).first << " ";
            }
            out << "]}\n";
        }

        out << "}\n";

        out << "Selectivity size: " << survey.area_season_selectivity_ids.size() << "\n";
        /*
         * WHY DOESN'T THIS WORK?????
         * 
        out << "Selectivity id: " << survey.selectivity->id << "\n";
        out << "Selectivity-at-age:";
        for (int a = 0; a < survey.ages; ++a)
        {
            out << " " << survey.selectivity->Evaluate(a);
        }
         */
        out << "\n\n";

        out << "Name: " << survey.name << "\n";
        out << "Id: " << survey.id << "\n";
        out << "Catchability (q): " << survey.q << "\n";
        out << "\n";


        out << "Survey " << survey.id << "\n";
        out << "Expected Survey Numbers at Age:\nMales and Females\n";

        for (int a = 0; a < survey.ages; a++) {
            for (int y = 0; y < survey.years; y++) {
                for (int s = 0; s < survey.seasons; s++) {
                    out << survey.survey_numbers_at_age[y * survey.seasons * survey.ages + (s) * survey.ages + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";

        out << "\n\n";
        out << "Survey " << survey.id << "\n";
        out << "Expected Survey Numbers at Age:\nMales\n";

        for (int a = 0; a < survey.ages; a++) {
            for (int y = 0; y < survey.years; y++) {
                for (int s = 0; s < survey.seasons; s++) {
                    out << survey.survey_numbers_at_age_males[y * survey.seasons * survey.ages + (s) * survey.ages + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";

        out << "\n\n";
        out << "Survey " << survey.id << "\n";
        out << "Expected Survey Numbers at Age:\nFemales\n";

        for (int a = 0; a < survey.ages; a++) {
            for (int y = 0; y < survey.years; y++) {
                for (int s = 0; s < survey.seasons; s++) {
                    out << survey.survey_numbers_at_age_females[y * survey.seasons * survey.ages + (s) * survey.ages + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";

        out << "Survey " << survey.id << "\n";
        out << "Expected Survey Proportions at Age:\nMales and Females\n";

        for (int a = 0; a < survey.ages; a++) {
            for (int y = 0; y < survey.years; y++) {
                for (int s = 0; s < survey.seasons; s++) {
                    out << survey.survey_proportion_at_age[y * survey.seasons * survey.ages + (s) * survey.ages + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";

        out << "\n\n";
        out << "Survey " << survey.id << "\n";
        out << "Expected Survey Proportions at Age:\nMales\n";

        for (int a = 0; a < survey.ages; a++) {
            for (int y = 0; y < survey.years; y++) {
                for (int s = 0; s < survey.seasons; s++) {
                    out << survey.survey_proportion_at_age_males[y * survey.seasons * survey.ages + (s) * survey.ages + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";


        if (survey.survey_proportion_at_age_data.get() != NULL) {
            out << "Observed Survey Proportions at Age:\nMales\n";

            for (int a = 0; a < survey.ages; a++) {
                for (int y = 0; y < survey.years; y++) {
                    for (int s = 0; s < survey.seasons; s++) {
                        out << survey.survey_proportion_at_age_data->get(y, s, a) << " ";
                    }
                }
                out << "\n";
            }
            out << "\n\n";
        }
        out << "\n\n";
        out << "Survey " << survey.id << "\n";
        out << "Expected Survey Proportions at Age:\nFemales\n";

        for (int a = 0; a < survey.ages; a++) {
            for (int y = 0; y < survey.years; y++) {
                for (int s = 0; s < survey.seasons; s++) {
                    out << survey.survey_proportion_at_age_females[y * survey.seasons * survey.ages + (s) * survey.ages + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";
        if (survey.survey_proportion_at_age_data.get() != NULL) {
            out << "Observed Survey Proportions at Age:\nFemales\n";

            for (int a = 0; a < survey.ages; a++) {
                for (int y = 0; y < survey.years; y++) {
                    for (int s = 0; s < survey.seasons; s++) {
                        out << survey.survey_proportion_at_age_data->get(y, s, a) << " ";
                    }
                }
                out << "\n";
            }
            out << "\n\n";
        }
        out << "Expected Survey Index:\nTotal\n";
        out << "Survey " << survey.id << "\n";
        for (int s = 0; s < survey.seasons; s++) {
            for (int y = 0; y < survey.years; y++) {
                out << survey.survey_biomass_total[y * survey.seasons + s] << " ";
            }
            out << "\n";
        }
        out << "\n\n";

        if (survey.survey_biomass_data.get() != NULL) {
            out << "Observed Survey Index:\nTotal\n";
            out << "Survey " << survey.id << "\n";
            for (int s = 0; s < survey.seasons; s++) {
                for (int y = 0; y < survey.years; y++) {
                    out << survey.survey_biomass_data->get(y, s) << " ";
                }
                out << "\n";
            }
            out << "\n\n";
        }

        out << "Expected Survey Index Total:\n Males\n";
        out << "Survey " << survey.id << "\n";
        for (int a = 0; a < survey.ages; a++) {
            for (int y = 0; y < survey.years; y++) {
                for (int s = 0; s < survey.seasons; s++) {
                    out << survey.survey_biomass_at_age_males[y * survey.seasons * survey.ages + (s) * survey.ages + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";

        out << "Expected Survey Index Total:\n Females\n";
        out << "Survey " << survey.id << "\n";
        for (int a = 0; a < survey.ages; a++) {
            for (int y = 0; y < survey.years; y++) {
                for (int s = 0; s < survey.seasons; s++) {
                    out << survey.survey_biomass_at_age_females[y * survey.seasons * survey.ages + (s) * survey.ages + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";

        return out;
    }

}


#endif /* SURVEY_HPP */

