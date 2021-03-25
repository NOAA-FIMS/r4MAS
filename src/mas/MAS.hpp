/*
 * File:   MAS.hpp
 *
 * Author: Matthew Supernaw
 * National Oceanic and Atmospheric Administration
 * National Marine Fisheries Service
 * Sustainable Fisheries Division
 * St. Petersburg, FL, 33701
 *
 * Created on September 16, 2016, 12:35 PM
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

#ifndef MAS_MAS_HPP
#define MAS_MAS_HPP


#include "Information.hpp"


#define MAS_VERSION_MAJOR 1
#define MAS_VERSION_MINOR 01
#define MAS_VERSION_PATCH 0


namespace mas {

    template<typename REAL_T>
    class MAS {
        typedef typename VariableTrait<REAL_T>::variable variable;
        std::unordered_map<int, mas::Population<REAL_T> > populations;



        std::string data_file;
        std::string config_file;

        //totals
        std::vector<variable> N;
        std::vector<variable> N_Proportions;
        std::vector<variable> C;
        std::vector<variable> C_Biomass;
        std::vector<variable> C_Proportions;
        std::vector<variable> SurveyNumbers;
        std::vector<variable> Survey_Biomass;
        std::vector<variable> Survey_Proportions;
        int calls = 0;
        variable catch_biomass_component;
        variable survey_biomass_component;
        variable fishery_age_comp_component;
        variable survey_age_comp_component;
        variable recruitment_deviations_component;
        variable nll_fleets;
        variable nll_surveys;
    public:
        int phase;
        mas::Information< REAL_T> info;
        atl::RealMatrix<REAL_T > variance_covaiance;
        atl::RealVector<REAL_T> std_dev;
        variable recruitment_likelihood;
        variable selectivity_likelihood;

        //goodness of fit
        REAL_T chi_squared;
        REAL_T g_test;
        REAL_T rmse;
        REAL_T rmsle;
        REAL_T AIC;
        REAL_T BIC;

        MAS() {
        }

        MAS(const MAS<REAL_T>& orig) {
        }

        virtual ~MAS() {
        }

        void Initialize(const std::string& config_file, const std::string& data_file) {
            info.ParseConfig(config_file);
            info.ParseData(data_file);
            info.CreateModel();
        }

        inline void Run(variable& f) {

            f = static_cast<REAL_T> (0.0);
            this->nll_fleets = 0.0;
            this->nll_surveys = 0.0;
            this->catch_biomass_component = 0.0;
            this->survey_biomass_component = 0.0;
            this->fishery_age_comp_component = 0.0;
            this->survey_age_comp_component = 0.0;
            this->recruitment_likelihood = 0.0;
            this->selectivity_likelihood = 0.0;

            typename mas::Information<REAL_T>::fleet_iterator fit;
            typename mas::Information<REAL_T>::survey_model_iterator sit;
            typename std::unordered_map<int, std::shared_ptr<mas::Population<REAL_T> > >::iterator it;
            std::unordered_map<int, std::shared_ptr<mas::Population<REAL_T> > >& pops =
                    info.GetPopulations();
            typename mas::Information<REAL_T>::area_iterator ait;
            typename mas::Information<REAL_T>::recruitment_model_iterator rit;
            typename mas::Information<REAL_T>::selectivity_model_iterator selex_it;
            typename mas::Information<REAL_T>::movement_model_iterator mit;

            //            for (ait = info.areas.begin(); ait != info.areas.end(); ait++) {
            //                (*ait).second->Prepare();
            //            }

            for (mit = info.movement_models.begin(); mit != info.movement_models.end(); ++mit) {
                (*mit).second->Prepare();
            }

            for (rit = info.recruitment_models.begin(); rit != info.recruitment_models.end(); rit++) {
                (*rit).second->Prepare();
            }
            //
            for (selex_it = info.selectivity_models.begin(); selex_it != info.selectivity_models.end(); selex_it++) {
                (*selex_it).second->Update(this->info.ages);
            }


#warning not needed until report time
            //            /**
            //             * Prepare areas for evaluation. Resets runtime information.
            //             */
            //            for (ait = info.areas.begin(); ait != info.areas.end(); ait++) {
            //                (*ait).second->Prepare();
            //            }

            /**
             * Prepare Populations for evaluation. Resets runtime
             * information.
             */
            for (it = pops.begin(); it != pops.end(); ++it) {
                //                (*it).second->phase = *phase;
                (*it).second->Prepare();
            }

            for (sit = info.survey_models.begin(); sit != info.survey_models.end(); ++sit) {
                (*sit).second->Prepare();
            }

            for (fit = info.fleets.begin(); fit != info.fleets.end(); ++fit) {
                (*fit).second->Prepare();
            }

            /**
             * Evaluate each population and push final numbers to
             * Area, fleet, and survey objects.
             */
            for (it = pops.begin(); it != pops.end(); ++it) {
                (*it).second->Evaluate();
            }



            //          
            /**
             * Loop through each area and compute proportions for catch, surveys,
             * and numbers.
             */

            for (fit = info.fleets.begin(); fit != info.fleets.end(); ++fit) {
                (*fit).second->ComputeProportions();
            }



            for (sit = info.survey_models.begin(); sit != info.survey_models.end(); ++sit) {
                (*sit).second->ComputeProportions();
            }


            // NOTE:  all of the NLL components have the correct sign in ComputeNLLComponents() already; do not adjust them here

            for (fit = info.fleets.begin(); fit != info.fleets.end(); ++fit) {
                (*fit).second->ComputeNLLComponents();
                this->nll_fleets += (*fit).second->nll;
            }

            for (sit = info.survey_models.begin(); sit != info.survey_models.end(); ++sit) {
                (*sit).second->ComputeNLLComponents();
                this->nll_surveys += (*sit).second->nll; //+= (*sit).second->survey_biomass_component;
                //                this->nll_surveys += (*sit).second->survey_age_comp_component;
            }

            for (rit = info.recruitment_models.begin(); rit != info.recruitment_models.end(); rit++) {
                this->recruitment_likelihood += (*rit).second->LikelihoodComponent();
            }

            for (selex_it = info.selectivity_models.begin(); selex_it != info.selectivity_models.end(); selex_it++) {
                if ((*selex_it).second->used) {
                    this->selectivity_likelihood += (*selex_it).second->LikelihoodComponent(phase);
                }
            }

            //            std::cout<<"recruitment_likelihood = "<<this->recruitment_likelihood<<"\n";
            f = this->nll_fleets + this->nll_surveys + this->recruitment_likelihood + this->selectivity_likelihood;
        }

        void RunOperationalModel() {
            this->catch_biomass_component = 0.0;
            this->survey_biomass_component = 0.0;
            this->fishery_age_comp_component = 0.0;
            this->survey_age_comp_component = 0.0;
            this->recruitment_likelihood = 0.0;
            this->selectivity_likelihood = 0.0;

            typename mas::Information<REAL_T>::fleet_iterator fit;
            typename mas::Information<REAL_T>::survey_model_iterator sit;
            typename std::unordered_map<int, std::shared_ptr<mas::Population<REAL_T> > >::iterator it;
            std::unordered_map<int, std::shared_ptr<mas::Population<REAL_T> > >& pops =
                    info.GetPopulations();
            typename mas::Information<REAL_T>::area_iterator ait;
            typename mas::Information<REAL_T>::recruitment_model_iterator rit;
            typename mas::Information<REAL_T>::selectivity_model_iterator selex_it;
            typename mas::Information<REAL_T>::movement_model_iterator mit;

            //            for (ait = info.areas.begin(); ait != info.areas.end(); ait++) {
            //                (*ait).second->Prepare();
            //            }

            for (mit = info.movement_models.begin(); mit != info.movement_models.end(); ++mit) {
                (*mit).second->Prepare();
            }

            for (rit = info.recruitment_models.begin(); rit != info.recruitment_models.end(); rit++) {
                (*rit).second->Prepare();
            }
            //
            for (selex_it = info.selectivity_models.begin(); selex_it != info.selectivity_models.end(); selex_it++) {
                (*selex_it).second->Update(this->info.ages);
            }


#warning not needed until report time
            //            /**
            //             * Prepare areas for evaluation. Resets runtime information.
            //             */
            //            for (ait = info.areas.begin(); ait != info.areas.end(); ait++) {
            //                (*ait).second->Prepare();
            //            }

            /**
             * Prepare Populations for evaluation. Resets runtime
             * information.
             */
            for (it = pops.begin(); it != pops.end(); ++it) {
                //                (*it).second->phase = *phase;
                (*it).second->Prepare();
            }

            for (sit = info.survey_models.begin(); sit != info.survey_models.end(); ++sit) {
                (*sit).second->Prepare();
            }

            for (fit = info.fleets.begin(); fit != info.fleets.end(); ++fit) {
                (*fit).second->Prepare();
            }

            /**
             * Evaluate each population and push final numbers to
             * Area, fleet, and survey objects.
             */
            for (it = pops.begin(); it != pops.end(); ++it) {
                (*it).second->Evaluate();
            }


            
            

            //          
            /**
             * Loop through each area and compute proportions for catch, surveys,
             * and numbers.
             */

            for (fit = info.fleets.begin(); fit != info.fleets.end(); ++fit) {
                (*fit).second->ComputeProportions();
                (*fit).second->ApplyOperatingModelError();
                
                info.data.push_back((*fit).second->catch_biomass_data);
                info.data.push_back((*fit).second->catch_proportion_at_age_data);
                info.data_dictionary[(*fit).second->id].push_back((*fit).second->catch_biomass_data);
                info.data_dictionary[(*fit).second->id].push_back((*fit).second->catch_proportion_at_age_data);
            }



            for (sit = info.survey_models.begin(); sit != info.survey_models.end(); ++sit) {
                (*sit).second->ComputeProportions();
                (*sit).second->ApplyOperatingModelError();
                info.data.push_back((*sit).second->survey_biomass_data);
                info.data.push_back((*sit).second->survey_proportion_at_age_data);
                info.data_dictionary[(*sit).second->id].push_back((*sit).second->survey_biomass_data);
                info.data_dictionary[(*sit).second->id].push_back((*sit).second->survey_proportion_at_age_data);

            }

        }

        /**
         * Pearson's chi-squared test on biomass and age comp.
         */
        void ComputeGoodnessOfFit() {

            variable f;
            atl::Variable<REAL_T>::tape.Reset();
            atl::Variable<REAL_T>::tape.recording = true;
            atl::Variable<REAL_T>::tape.derivative_trace_level = atl::SECOND_ORDER_REVERSE;
            this->Run(f);
            atl::Variable<REAL_T>::tape.AccumulateSecondOrder();
            typename mas::Information<REAL_T>::fleet_iterator fit;
            typename mas::Information<REAL_T>::survey_model_iterator sit;


            for (sit = info.survey_models.begin(); sit != info.survey_models.end(); ++sit) {
                (*sit).second->ComputeGoodnessOfFit();
                this->chi_squared += (*sit).second->chi_squared;
                this->g_test += (*sit).second->g_test;
                this->rmse += (*sit).second->rmse;
                this->rmsle += (*sit).second->rmsle;
            }

            for (fit = info.fleets.begin(); fit != info.fleets.end(); ++fit) {
                (*fit).second->ComputeGoodnessOfFit();
                this->chi_squared += (*fit).second->chi_squared;
                this->g_test += (*fit).second->g_test;
                this->rmse += (*fit).second->rmse;
                this->rmsle += (*fit).second->rmsle;
                this->AIC += (*fit).second->AIC;
                this->BIC += (*fit).second->BIC;
            }

        }

        void Forecast() {

        }

        void Finalize() {

            std::unordered_map<int, std::shared_ptr<mas::Population<REAL_T> > >& pops =
                    info.GetPopulations();
            typename std::unordered_map<int, std::shared_ptr<mas::Population<REAL_T> > >::iterator it;
            for (it = pops.begin(); it != pops.end(); ++it) {
                (*it).second->PushToAreasAndFleets();
                (*it).second->Finalize();

                //                (*it).second->ComputeBiologicalReferencePoints();
            }
        }

        void Report() {

            //            std::unordered_map<int, std::shared_ptr<mas::Population<REAL_T> > >& pops =
            //                    info.GetPopulations();
            //            typename std::unordered_map<int, std::shared_ptr<mas::Population<REAL_T> > >::iterator it;
            //            this->info.DumpSelectivity("selectivity.txt");
            //            this->info.DumpFishingMortality("fishing_mortality.txt");
            //            atl::Variable<REAL_T>::tape.Reset();
            //            atl::Variable<REAL_T>::tape.recording = true;
            //            atl::Variable<REAL_T> f;
            //            this->Run(f);
            //
            //            /**
            //             * Push final numbers to
            //             * Area, fleet, and survey objects.
            //             */
            //
            //
            //            atl::Variable<REAL_T>::tape.AccumulateFirstOrder();
            //
            //            atl::Variable<REAL_T>::tape.recording = false;
            //
            //            //            for (it = pops.begin(); it != pops.end(); ++it) {
            //            //                (*it).second->PushToAreasAndFleets();
            //            //                (*it).second->ComputeBiologicalReferencePoints();
            //            //            }
            //
            //
            //
            //            std::ofstream latex("populations.tex");
            //            latex << "\\documentclass{article}\n";
            //            latex << "\\usepackage{tikz}\n";
            //            latex << "\\usetikzlibrary{positioning,fit,shapes.geometric,backgrounds}\n";
            //            latex << "\\usepackage{pgfplots}\n";
            //            latex << "\\begin{document}\n\n\n";
            //            std::ofstream out2("populations.txt");
            //
            //
            //
            //            out2 << std::fixed;
            //            for (it = pops.begin(); it != pops.end(); ++it) {
            //                out2 << *(*it).second << std::endl;
            //                latex << mas::ToLatexCharts<REAL_T>(*(*it).second) << "\n";
            //            }
            //            latex << "\\end{document}\n";
            //
            //
            //            latex.close();
            //            out2.close();
            //            /**
            //             * Loop through each area and compute proportions for catch, surveys,
            //             * and numbers.
            //             */
            //
            //            std::ofstream out("mas_report.txt");
            //            std::ofstream nll_out("likelihood.txt");
            //            out << std::fixed;
            //            REAL_T chi_age_comp_fleet = 0.0;
            //            REAL_T chi_biomass_fleet = 0.0;
            //            REAL_T chi_age_comp_survey = 0.0;
            //            REAL_T chi_biomass_survey = 0.0;
            //
            //            out << "Objective Function Value: " << f << "\n\n";
            //
            //            out << std::setw(55) << std::left << "Fleet Likelihood Components " << std::setw(15) << "value" << "RMSE" << std::endl;
            //            typename mas::Information<REAL_T>::fleet_iterator fit;
            //            for (fit = info.fleets.begin(); fit != info.fleets.end(); ++fit) {
            //
            //                out << "Fleet_" << (*fit).second->id << "\n";
            //                for (int i = 0; i < (*fit).second->nll_components.size(); i++) {
            //                    (*fit).second->nll_components[i].Finalize();
            //                    out << std::setw(55) << std::left <<
            //                            (*fit).second->nll_component_values[i].GetName() << std::setw(15)
            //                            << (*fit).second->nll_component_values[i].GetValue() << " "
            //                            << (*fit).second->nll_components[i].chi_square << "\n";
            //                }
            //            }
            //
            //            out << std::endl;
            //            out << std::setw(55) << std::left << "Survey Likelihood Components " << std::setw(15) << "value" << "RMSE" << std::endl;
            //            typename mas::Information<REAL_T>::survey_model_iterator sit;
            //            for (sit = info.survey_models.begin(); sit != info.survey_models.end(); ++sit) {
            //
            //                out << "Survey_" << (*sit).second->id << "\n";
            //                for (int i = 0; i < (*sit).second->nll_components.size(); i++) {
            //                    (*sit).second->nll_components[i].Finalize();
            //                    out << std::setw(55) << std::left <<
            //                            (*sit).second->nll_component_values[i].GetName() <<
            //                            std::setw(15) << (*sit).second->nll_component_values[i].GetValue() <<
            //                            " " << (*sit).second->nll_components[i].chi_square << "\n";
            //                }
            //            }
            //            out << std::endl;
            //            out << "Estimated Parameters:\n";
            //            out << std::setw(45) << std::left << "Name" << std::setw(25) << "Value" << " " << std::setw(25) << "Gradient" << "\n";
            //
            //            for (int i = 0; i < this->info.estimated_parameters.size(); i++) {
            //                out << std::setw(30) << std::left << this->info.estimated_parameters[i]->GetName() << std::setw(25) << std::fixed << this->info.estimated_parameters[i]->info->value << " " << std::setw(25) << std::scientific << atl::Variable<double>::tape.Value(this->info.estimated_parameters[i]->info->id) << "\n";
            //            }
            //            out << "\n\n";
            //            out << std::fixed;
            //
            //            typename mas::Information<REAL_T>::recruitment_model_iterator rit;
            //
            //            mas::Information<double>::area_iterator ait;
            //
            //
            //            //            for (ait = info.areas.begin(); ait != info.areas.end(); ait++) {
            //            //                (*ait).second->ComputeProportions();
            //            //                out << *(*ait).second;
            //            //            }
            //
            //            for (fit = info.fleets.begin(); fit != info.fleets.end(); ++fit) {
            //                out << *(*fit).second;
            //            }
            //
            //
            //            for (sit = info.survey_models.begin(); sit != info.survey_models.end(); ++sit) {
            //                out << *(*sit).second;
            //            }
            //
            //            REAL_T GOF = this->ComputeGoodnessOfFit();
            //            for (fit = info.fleets.begin(); fit != info.fleets.end(); ++fit) {
            //                nll_out << (*fit).second->NLLComponentsToString() << std::endl;
            //            }
            //            for (sit = info.survey_models.begin(); sit != info.survey_models.end(); ++sit) {
            //                nll_out << (*sit).second->NLLComponentsToString() << std::endl;
            //            }
            //            std::cout << "ComputeGoodnessOfFit = " << GOF << "\n\n";
        }

        void HTMLReport() {


            std::unordered_map<int, std::shared_ptr<mas::Population<double> > >& pops =
                    info.GetPopulations();

            typename std::unordered_map<int, std::shared_ptr<mas::Population<double> > >::iterator it;
            std::ofstream out("mas_report.html");


            out << "<html>\n";
            out << "<head><b><p style=\"font-size:xx-large;\">Meta-population Assessment System Report</b></p>";
            out << "</head>\n";
            out << "<body>\n";
            out << std::fixed;
            out << "<b>Study Name:</b> " << this->info.study_name << "\n";
            out << "<br><b>Analyst:</b> " << this->info.analyst << "\n";
            out << "<br><b>Number of Populations:</b> " << this->info.populations.size() << "\n";
            out << "<br><b>Number of Areas:</b> " << this->info.areas.size() << "\n";



            for (it = pops.begin(); it != pops.end(); ++it) {

                out << mas::ToHTML(*(*it).second);

            }

            out << "</body>\n";
            out << "</html>";

            out.close();

        }

        void show(std::ostream& out) {
            std::unordered_map<int, std::shared_ptr<mas::Population<double> > >& pops =
                    info.GetPopulations();
            typename std::unordered_map<int, std::shared_ptr<mas::Population<double> > >::iterator it;

            for (it = pops.begin(); it != pops.end(); ++it) {

                out << *(*it).second;

            }
        }

    private:

    };



}


#endif /* MAS_MAS_HPP */

