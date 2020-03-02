/*
 * File:   Information.hpp
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

#ifndef MAS_INFORMATION_HPP
#define MAS_INFORMATION_HPP
#include "Common.hpp"
#include "Area.hpp"
#include "Population.hpp"
#include "Season.hpp"
#include "Movement.hpp"
#include "Selectivity.hpp"
#include "Survey.hpp"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "Fleet.hpp"
#include "HarvestControlRule.hpp"
#include <ctime>


//#define MAS_DEBUG

#ifdef MAS_DEBUG
#define INFO_DEBUG std::cout<<"Information.hpp: "<<__func__<<": "<<__LINE__<<std::endl;
#else
#define INFO_DEBUG 
#endif


namespace mas {

    struct EnsembleUnit {
        mas::SubmodelType type;
        std::vector<int> units;
        rapidjson::Document::MemberIterator val;
    };

    template<typename REAL_T>
    class Information : public mas::ModelObject<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;




        typedef typename std::map<int, std::vector<std::shared_ptr<DataObject<REAL_T> > > >::iterator data_iterator;



        std::string data_path;
    public:
        std::map<int, std::vector<std::shared_ptr<DataObject<REAL_T> > > > data_dictionary;
        std::vector<std::shared_ptr<DataObject<REAL_T> > > data;
        std::string analyst = "NA";
        std::string study_name = "NA";
        int nyears;
        int nseasons = 1;
        int first_year;
        int last_year;
        REAL_T spawning_season_offset = .25;
        REAL_T catch_fraction_of_year = .5;
        REAL_T survey_fraction_of_year = .5;

        std::vector<variable> ages;
        std::vector<REAL_T> ages_real;
        bool natal_movement = false;
        bool valid_configuration = true;




        std::unordered_map<int, std::shared_ptr<mas::Area<REAL_T> > > areas;
        std::unordered_map<int, std::shared_ptr<mas::Season<REAL_T> > > seasons;

        //System sub model objects
        std::unordered_map<int, std::shared_ptr<mas::Population<REAL_T> > > populations;
        std::unordered_map<int, std::shared_ptr<mas::FishingMortality<REAL_T> > > fishing_mortality_models;
        std::unordered_map<int, std::shared_ptr<mas::SelectivityBase<REAL_T> > > selectivity_models;
        std::unordered_map<int, std::shared_ptr<mas::Fleet<REAL_T> > > fleets;

        // NOTE:  why isn't survey_models similar to fleets?
        std::unordered_map<int, std::shared_ptr<mas::Survey<REAL_T> > > survey_models;

        // per population
        std::unordered_map<int, std::shared_ptr<mas::GrowthBase<REAL_T> > > growth_models;
        std::unordered_map<int, std::shared_ptr<mas::FecundityBase<REAL_T> > > fecundity_models;
        std::unordered_map<int, std::shared_ptr<mas::Movement<REAL_T> > > movement_models;

        // per population - area
        std::unordered_map<int, std::shared_ptr<mas::NaturalMortality<REAL_T> > > natural_mortality_models;
        std::unordered_map<int, std::shared_ptr<mas::RecruitmentBase<REAL_T> > > recruitment_models;

        std::unordered_map<int, std::shared_ptr<mas::NLLFunctor<REAL_T> > > likelihood_components;

        //System sub model iterators
        typedef typename std::unordered_map<int, std::shared_ptr<mas::Season<REAL_T> > >::iterator seasons_iterator;
        typedef typename std::unordered_map<int, std::shared_ptr<mas::Area<REAL_T> > >::iterator area_iterator;
        typedef typename std::unordered_map<int, std::shared_ptr<mas::Population<REAL_T> > >::iterator population_iterator;
        typedef typename std::unordered_map<int, std::shared_ptr<mas::GrowthBase<REAL_T> > >::iterator growth_model_iterator;
        typedef typename std::unordered_map<int, std::shared_ptr<mas::RecruitmentBase<REAL_T> > >::iterator recruitment_model_iterator;
        typedef typename std::unordered_map<int, std::shared_ptr<mas::NaturalMortality<REAL_T> > >::iterator natural_mortality_model_iterator;
        typedef typename std::unordered_map<int, std::shared_ptr<mas::FishingMortality<REAL_T> > >::iterator fishing_mortality_model_iterator;
        typedef typename std::unordered_map<int, std::shared_ptr<mas::FecundityBase<REAL_T> > >::iterator fecundity_model_iterator;
        typedef typename std::unordered_map<int, std::shared_ptr<mas::Movement<REAL_T> > >::iterator movement_model_iterator;
        typedef typename std::unordered_map<int, std::shared_ptr<mas::SelectivityBase<REAL_T> > >::iterator selectivity_model_iterator;
        typedef typename std::unordered_map<int, std::shared_ptr<mas::Fleet<REAL_T> > >::iterator fleet_iterator;
        typedef typename std::unordered_map<int, std::shared_ptr<mas::Survey<REAL_T> > >::iterator survey_model_iterator;
        typedef typename std::unordered_map<int, std::shared_ptr<mas::NLLFunctor<REAL_T> > >::iterator likelihood_components_iterator;


        std::vector<EnsembleUnit> ensemble_units;

        Information() {
        }

        Information(const Information<REAL_T>& orig) {
        }

        virtual ~Information() {
        }

        void ParseConfig(const std::string& path) {
            std::time_t result = std::time(nullptr);
            mas_log << "MAS Run: " << std::asctime(std::localtime(&result)) << "\n";

            INFO_DEBUG;
            std::stringstream ss;
            std::ifstream config;
            config.open(path.c_str());
            if (!config.good()) {
                std::cerr << "MAS Configuration file \"" << path << "\" not found.\n";
                mas::mas_log << "MAS Configuration file \"" << path << "\" not found.\n";
                this->valid_configuration = false;
            }

            INFO_DEBUG;
            while (config.good()) {
                std::string line;
                std::getline(config, line);
                ss << line << "\n";
            }
            this->ParseConfig(ss);
        }

        void ParseConfig(const std::stringstream& ss) {
            rapidjson::Document document;
            INFO_DEBUG;
            document.Parse(ss.str().c_str());
            INFO_DEBUG;
            this->ParseConfig(document);
        }

        void ParseConfig(rapidjson::Document& document) {

            rapidjson::Document::MemberIterator mit;

            for (mit = document.MemberBegin(); mit != document.MemberEnd(); ++mit) {
                if (std::string((*mit).name.GetString()) == "analyst") {
                    INFO_DEBUG;
                    this->analyst = std::string((*mit).value.GetString());
                }

                if (std::string((*mit).name.GetString()) == "extended_plus_group") {
                    INFO_DEBUG;
                    mas::AreaPopulationInfo<REAL_T>::length_weight_key_carryout = static_cast<uint32_t> ((*mit).value.GetInt());
                }

#warning should these be here or in the population definition
                if (std::string((*mit).name.GetString()) == "catch_fraction_of_year") {
                    INFO_DEBUG;
                    this->catch_fraction_of_year = static_cast<REAL_T> ((*mit).value.GetDouble());
                }

                if (std::string((*mit).name.GetString()) == "survey_fraction_of_year") {
                    INFO_DEBUG;
                    this->survey_fraction_of_year = static_cast<REAL_T> ((*mit).value.GetDouble());
                }

                if (std::string((*mit).name.GetString()) == "spawning_season_offset") {
                    INFO_DEBUG;
                    this->spawning_season_offset = static_cast<REAL_T> ((*mit).value.GetDouble());
                }

                if (std::string((*mit).name.GetString()) == "study_name") {
                    INFO_DEBUG;
                    this->study_name = std::string((*mit).value.GetString());
                }

                if (std::string((*mit).name.GetString()) == "movement_type") {
                    INFO_DEBUG;
                    if (std::string((*mit).value.GetString()) == std::string("natal_homing")) {
                        this->natal_movement = true;
                    }
                }

                if (std::string((*mit).name.GetString()) == "data") {
                    INFO_DEBUG;
                    this->data_path = std::string((*mit).value.GetString());
                }

                if (std::string((*mit).name.GetString()) == "years") {
                    INFO_DEBUG;
                    this->nyears = (*mit).value.GetInt();
                }

                if (std::string((*mit).name.GetString()) == "first_year") {
                    INFO_DEBUG;
                    this->first_year = (*mit).value.GetInt();
                }

                if (std::string((*mit).name.GetString()) == "last_year") {
                    INFO_DEBUG;
                    this->last_year = (*mit).value.GetInt();
                }

                if (std::string((*mit).name.GetString()) == "ages") {
                    INFO_DEBUG;
                    this->ages.clear();
                    this->ages_real.clear();
                    rapidjson::Value& ages = (*mit).value;
                    if (ages.IsArray()) {
                        for (int i = 0; i < ages.Size(); i++) {
                            this->ages.push_back(variable(static_cast<REAL_T> (ages[i].GetDouble())));
                            this->ages_real.push_back(static_cast<REAL_T> (ages[i].GetDouble()));
                        }

                    } else {
                        std::cout << "Configuration Error: \"ages\" should be a vector.\n";
                        mas::mas_log << "Configuration Error: \"ages\" should be a vector.\n";
                        this->valid_configuration = false;
                    }

                    mas::GrowthBase<REAL_T>::ages.resize(0);
                    mas::GrowthBase<REAL_T>::ages_to_intrpolate.clear();
                    for (int i = 0; i < this->ages.size(); i++) {
                        mas::GrowthBase<REAL_T>::ages.push_back(this->ages[i]);
                        mas::GrowthBase<REAL_T>::ages_to_intrpolate.insert(this->ages[i].GetValue());
                        mas::GrowthBase<REAL_T>::ages_to_intrpolate.insert(this->ages[i].GetValue() + this->catch_fraction_of_year);
                        mas::GrowthBase<REAL_T>::ages_to_intrpolate.insert(this->ages[i].GetValue() + this->survey_fraction_of_year);
                        mas::GrowthBase<REAL_T>::ages_to_intrpolate.insert(this->ages[i].GetValue() + this->spawning_season_offset);
                    }

                }




                if (std::string((*mit).name.GetString()) == "seasons") {
                    //                    HandleSeason(mit);
                    INFO_DEBUG;
                    this->nseasons = (*mit).value.GetInt();
                }

                if (std::string((*mit).name.GetString()) == "recruitment") {
                    INFO_DEBUG;
                    HandleRecruitmentModel(mit);
                }
                if (std::string((*mit).name.GetString()) == "growth") {
                    INFO_DEBUG;
                    HandleGrowthModel(mit);
                }

                if (std::string((*mit).name.GetString()) == "selectivity") {
                    INFO_DEBUG;
                    HandleSelectivityModel(mit);
                }

                if (std::string((*mit).name.GetString()) == "area") {
                    INFO_DEBUG;
                    HandleAreaModel(mit);
                }

                if (std::string((*mit).name.GetString()) == "population") {
                    INFO_DEBUG;
                    HandlePopulationModel(mit);
                }

                if (std::string((*mit).name.GetString()) == "fleet") {
                    INFO_DEBUG;
                    HandleFleetModel(mit);
                }

                if (std::string((*mit).name.GetString()) == "movement") {
                    INFO_DEBUG;
                    HandleMovementModel(mit);
                }

                if (std::string((*mit).name.GetString()) == "natural_mortality") {
                    INFO_DEBUG;
                    HandleNaturalMortalityModel(mit);
                }
                if (std::string((*mit).name.GetString()) == "fishing_mortality") {
                    INFO_DEBUG;
                    HandleFishingMortalityModel(mit);
                }
                if (std::string((*mit).name.GetString()) == "survey") {
                    INFO_DEBUG;
                    HandleSurveyModel(mit);
                }
                if (std::string((*mit).name.GetString()) == "likelihood_component") {
                    INFO_DEBUG;
                    HandleLikelihoodComponent(mit);
                }

                INFO_DEBUG;
            }



        }

        void HandleLikelihoodComponent(rapidjson::Document::MemberIterator& likelihood_component) {
            INFO_DEBUG
            std::shared_ptr < mas::DataObject<REAL_T > > lambda_data(new mas::DataObject<REAL_T>());
            rapidjson::Document::MemberIterator rit;
            rit = (*likelihood_component).value.FindMember("model");

            if (rit == (*likelihood_component).value.MemberEnd()) {
                std::cout << "Configuration Error: Likelihood is required to have a model specified.\n";
                mas::mas_log << "Configuration Error: Likelihood is required to have a model specified.\n";
                this->valid_configuration = false;

            }

            std::shared_ptr < mas::NLLFunctor<REAL_T > > model(NULL);
            std::string smodel = std::string((*rit).value.GetString());


            rit = (*likelihood_component).value.FindMember("id");
            int model_id = 0;
            if (rit == (*likelihood_component).value.MemberEnd()) {
                std::cout << "Configuration Error: Likelihood is required to have a unique identifier\n";
                mas::mas_log << "Configuration Error: Likelihood is required to have a unique identifier\n";
                this->valid_configuration = false;

            } else {
                model_id = (*rit).value.GetInt();
            }

            rit = (*likelihood_component).value.FindMember("lambda");
            if (rit != (*likelihood_component).value.MemberEnd()) {

                //                lambda_data = std::make_shared<mas::DataObject<REAL_T> >();
                rapidjson::Document::MemberIterator lit;
                rapidjson::Document::MemberIterator lvit;

                lit = (*rit).value.FindMember("years");
                if (lit != (*rit).value.MemberEnd()) {
                    lambda_data->imax = (*lit).value.GetInt();
                }

                lit = (*rit).value.FindMember("seasons");
                if (lit != (*rit).value.MemberEnd()) {
                    lambda_data->jmax = (*lit).value.GetInt();
                }

                lit = (*rit).value.FindMember("ages");
                if (lit != (*rit).value.MemberEnd()) {
                    lambda_data->kmax = (*lit).value.GetInt();
                }

                lit = (*rit).value.FindMember("values");
                if (lit != (*rit).value.MemberEnd()) {
                    if ((*lit).value.IsArray()) {
                        rapidjson::Value& v = (*lit).value;
                        for (int i = 0; i < v.Size(); i++) {
                            if (v[i].IsArray()) {
                                for (int j = 0; j < v[i].Size(); j++) {
                                    if (v[i][j].IsArray()) {
                                        for (int k = 0; k < v[i][j].Size(); k++) {
                                            lambda_data->data.push_back(static_cast<REAL_T> (v[i][j][k].GetDouble()));
                                        }
                                    } else {
                                        lambda_data->data.push_back(static_cast<REAL_T> (v[i][j].GetDouble()));
                                    }
                                }
                            } else {
                                lambda_data->data.push_back(static_cast<REAL_T> (v[i].GetDouble()));
                            }
                        }
                    }
                }
            }

            rapidjson::Document::MemberIterator pit = (*likelihood_component).value.FindMember("parameters");



            if (smodel == std::string("dirichlet_multinomial")) {
                model = std::make_shared<mas::DirichletMultinomial<REAL_T> >();
                mas::DirichletMultinomial<REAL_T>* dirichlet = (mas::DirichletMultinomial<REAL_T>*)model.get();

                std::stringstream ss;
                ss << "dirichlet_multinomial_beta_" << model_id;
                VariableTrait<REAL_T>::SetName(dirichlet->beta, ss.str());
                ss.str("");

                if (pit == (*likelihood_component).value.MemberEnd()) {
                    std::cout << "Configuration Waring: Likelihood Component \"Dirichlet-Multinomial \" has no parameter definitions. Defaults will be used\n";
                    mas::mas_log << "Configuration Waring: Likelihood Component \"Dirichlet-Multinomial \" has no parameter definitions. Defaults will be used\n";

                } else {

                    rapidjson::Document::MemberIterator ppit = (*pit).value.FindMember("beta");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Likelihood Component \"Dirichlet-Multinomial \" " <<
                                "does not define \"beta\". Model will use the default value of 0.5 and \"beta\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Likelihood Component \"Dirichlet-Multinomial \" " <<
                                "does not define \"beta\". Model will use the default value of 0.5 and \"beta\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Selectivity model \"Dirichlet-Multinomial\" does not provide a initial value for \"beta\".\n";
                            mas::mas_log << "Configuration Warning: Selectivity model \"Dirichlet-Multinomial\" does not provide a initial value for \"beta\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(dirichlet->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(dirichlet->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(dirichlet->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }
                        INFO_DEBUG
                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            dirichlet->Register(dirichlet->beta, phase);
                        }

                    }

                }


            } else if (smodel == std::string("dirichlet_multinomial_robust")) {
                model = std::make_shared<mas::DirichletMultinomialRobust<REAL_T> >();
                mas::DirichletMultinomialRobust<REAL_T>* dirichlet = (mas::DirichletMultinomialRobust<REAL_T>*)model.get();

                rapidjson::Document::MemberIterator eit = (*likelihood_component).value.FindMember("epsilon");
                if (eit != (*likelihood_component).value.MemberEnd()) {
                    dirichlet->epsilon = static_cast<REAL_T> ((*eit).value.GetDouble());
                }

                std::stringstream ss;
                ss << "dirichlet_multinomial_robust_beta_" << model_id;
                VariableTrait<REAL_T>::SetName(dirichlet->beta, ss.str());
                ss.str("");

                if (pit == (*likelihood_component).value.MemberEnd()) {
                    std::cout << "Configuration Waring: Likelihood Component \"Dirichlet-Multinomial Robust\" has no parameter definitions. Defaults will be used\n";
                    mas::mas_log << "Configuration Waring: Likelihood Component \"Dirichlet-Multinomial Robust\" has no parameter definitions. Defaults will be used\n";

                } else {

                    rapidjson::Document::MemberIterator ppit = (*pit).value.FindMember("beta");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Likelihood Component \"Dirichlet-Multinomial Robust\" " <<
                                "does not define \"beta\". Model will use the default value of 0.5 and \"beta\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Likelihood Component \"Dirichlet-Multinomial Robust\" " <<
                                "does not define \"beta\". Model will use the default value of 0.5 and \"beta\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Selectivity model \"Dirichlet-Multinomial Robust\" does not provide a initial value for \"beta\".\n";
                            mas::mas_log << "Configuration Warning: Selectivity model \"Dirichlet-Multinomial Robust\" does not provide a initial value for \"beta\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(dirichlet->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(dirichlet->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(dirichlet->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }
                        INFO_DEBUG
                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            dirichlet->Register(dirichlet->beta, phase);
                        }

                    }

                }

                //no estimable parameters for these component types
            } else if (smodel == std::string("multinomial")) {
                model = std::make_shared<mas::Multinomial<REAL_T> >();

            } else if (smodel == std::string("multinomial_robust")) {
                model = std::make_shared<mas::MultinomialRobust<REAL_T> >();
            } else if (smodel == std::string("lognormal")) {
                model = std::make_shared<mas::Lognormal<REAL_T> >();
                rapidjson::Document::MemberIterator eit = (*likelihood_component).value.FindMember("standard_error");
                if (eit != (*likelihood_component).value.MemberEnd()) {
                    mas::Lognormal<REAL_T>* models = (mas::Lognormal<REAL_T>*)model.get();
                    models->sigma = static_cast<REAL_T> ((*eit).value.GetDouble());
                }

            }

            model->id = model_id;
            model->lambda = lambda_data;


            likelihood_components_iterator it = this->likelihood_components.find(model->id);
            if (it != this->likelihood_components.end()) {
                std::cout << "Configuration Error: More than one likelihood components with the same identifier defined. Likelihood components require a unique id.\n";
                std::cout << "Configuration Error: More than one likelihood components with the same identifier defined. Likelihood components require a unique id.\n";
                this->valid_configuration = false;
            } else {
                this->likelihood_components[model->id] = model;
            }

        }

        void HandleSurveyModel(rapidjson::Document::MemberIterator & survey_model) {
            std::shared_ptr<mas::Survey<REAL_T> > model(new mas::Survey<REAL_T>());
            std::stringstream ss;
            INFO_DEBUG
            rapidjson::Document::MemberIterator rit = (*survey_model).value.FindMember("id");
            int model_id = 0;
            if (rit == (*survey_model).value.MemberEnd()) {
                std::cout << "Configuration Error: Survey is required to have a unique identifier\n";
                mas::mas_log << "Configuration Error: Survey is required to have a unique identifier\n";
                this->valid_configuration = false;
            } else {
                model_id = (*rit).value.GetInt();
            }

            model->id = model_id;

            rit = (*survey_model).value.FindMember("name");
            if (rit != (*survey_model).value.MemberEnd()) {
                model->name = std::string((*rit).value.GetString());
            }

            rit = (*survey_model).value.FindMember("survey_fraction_of_year");
            if (rit != (*survey_model).value.MemberEnd()) {
                model->survey_fraction_of_year = (*rit).value.GetDouble();
            }

            rit = (*survey_model).value.FindMember("CV");
            if (rit != (*survey_model).value.MemberEnd()) {
                model->CV = (*rit).value.GetDouble();
            }

            //            rit = (*survey_model).value.FindMember("population");
            //            if (rit == (*survey_model).value.MemberEnd()) {
            //                std::cout << "Configuration Error: Survey is required to have a population identifier\n";
            //                mas::mas_log << "Configuration Error: Survey is required to have a population identifier\n";
            //                this->valid_configuration = false;
            //            } else {
            //                model->population = (*rit).value.GetInt();
            //            }

            rit = (*survey_model).value.FindMember("selectivity");

            if (rit != (*survey_model).value.MemberEnd()) {
                if (!(*rit).value.IsArray()) {
                    std::cout << "Configuration Error: Surveys are required to have selectivity definitions in a vector\n";
                    std::cout << "Configuration Error: Surveys are required to have selectivity definitions in a vector\n";
                    this->valid_configuration = false;

                } else {
                    for (int i = 0; i < (*rit).value.Size(); i++) {
                        rapidjson::Value& m = (*rit).value[i];

                        int sid = 0;
                        int sarea = 0;
                        int sseason = 0;
                        REAL_T fraction = 0.5;
                        rapidjson::Document::MemberIterator vit = m.FindMember("id");
                        if (vit == m.MemberEnd()) {
                            std::cout << "Configuration Error: Survey selectivity has no model identifier\n";
                            std::cout << "Configuration Error: Survey selectivity has no model identifier\n";
                            this->valid_configuration = false;

                        } else {
                            sid = (*vit).value.GetInt();
                        }

                        rapidjson::Document::MemberIterator mit = m.FindMember("ensemble");
                        if (mit != m.MemberEnd()) {
                            if (!(*mit).value.IsArray()) {
                                std::cout << "Expected array for survey selectivity ensemble unit, ignored.\n";
                                mas_log << "Expected array for survey selectivity ensemble unit, ignored.\n";
                            } else {
                                EnsembleUnit eunit;
                                eunit.type = mas::SELECTIVITY;
                                rapidjson::Value& e = (*mit).value;
                                eunit.val = vit;
                                for (int i = 0; i < e.Size(); i++) {
                                    eunit.units.push_back(e[i].GetInt());
                                }
                                this->ensemble_units.push_back(eunit);

                            }
                        }


                        mit = m.FindMember("season");
                        if (mit == m.MemberEnd()) {
                            std::cout << "Configuration Error: Survey selectivity season has no identifier\n";
                            std::cout << "Configuration Error: Survey selectivity season has no identifier\n";
                            this->valid_configuration = false;

                        } else {
                            sseason = (*mit).value.GetInt();
                        }

                        mit = m.FindMember("fraction");
                        if (mit == m.MemberEnd()) {
                            std::cout << "Configuration Warning: Survey selectivity season has no catch fraction. Default is 0.5\n";
                            std::cout << "Configuration Error: Survey selectivity season has no catch fraction. Default is 0.5\n";

                        } else {
                            fraction = (*mit).value.GetDouble();
                        }


                        mit = m.FindMember("area");
                        if (mit == m.MemberEnd()) {
                            std::cout << "Configuration Error: Survey selectivity area has no identifier\n";
                            std::cout << "Configuration Error: Survey selectivity area has no identifier\n";
                            this->valid_configuration = false;

                        } else {
                            sarea = (*mit).value.GetInt();
                        }
                        model->season_area_selectivity_ids[sseason][sarea] = sid;
                        model->area_season_selectivity_ids[sarea][sseason] = sid;
                        model->area_season_survey_fraction[sarea][sseason] = fraction;
                        model->season_area_survey_fraction[sseason][sarea] = fraction;
                    }
                }
            }

            rit = (*survey_model).value.FindMember("parameters");

            if (rit == (*survey_model).value.MemberEnd()) {
                std::cout << "Configuration Error: Survey model \"" << model->id << "\" has no parameter definitions.\n";
                mas::mas_log << "Configuration Error: Survey model \"" << model->id << "\" has no parameter definitions.\n";
                this->valid_configuration = false;
            } else {

                bool estimated = false;
                int phase = 1;

                rapidjson::Document::MemberIterator qit = (*rit).value.FindMember("q");

                //1. Get initial value if there is one.
                rapidjson::Document::MemberIterator pm = (*qit).value.FindMember("value");

                if (pm == (*qit).value.MemberEnd()) {
                    std::cout << "Configuration Error: Survey model \"" << model->id << "\" does not provide an initial value for \"q\".\n";
                    mas::mas_log << "Configuration Error: Survey model \"" << model->id << "\" does not provide an initial value for \"q\".\n";
                } else {
                    ss << "survey_q_" << model_id;
                    VariableTrait<REAL_T>::SetName(model->q, ss.str());
                    VariableTrait<REAL_T>::SetValue(model->q, static_cast<REAL_T> ((*pm).value.GetDouble()));
                }

                //2. Get min boundary if there is one.
                pm = (*qit).value.FindMember("min");
                if (pm != (*qit).value.MemberEnd()) {
                    VariableTrait<REAL_T>::SetMinBoundary(model->q, static_cast<REAL_T> ((*pm).value.GetDouble()));
                }

                //3. Get max boundary if there is one.
                pm = (*qit).value.FindMember("max");
                if (pm != (*qit).value.MemberEnd()) {
                    VariableTrait<REAL_T>::SetMaxBoundary(model->q, static_cast<REAL_T> ((*pm).value.GetDouble()));
                }

                pm = (*qit).value.FindMember("estimated");
                if (pm != (*qit).value.MemberEnd()) {
                    std::string e = std::string((*pm).value.GetString());
                    if (e == "true") {
                        estimated = true;
                    }
                }

                if (estimated) {
                    phase = 1;
                    pm = (*qit).value.FindMember("phase");
                    if (pm != (*qit).value.MemberEnd()) {
                        phase = (*pm).value.GetInt();
                    }
                    //register alpha
                    model->Register(model->q, phase);

                }

            }

            rit = (*survey_model).value.FindMember("likelihood_components");

            if (rit != (*survey_model).value.MemberEnd()) {
                if (!(*rit).value.IsArray()) {
                    std::cout << "Configuration Warning: No Likelihood components defined for survey \"" << model->id << "\". Default will be used\n";
                    std::cout << "Configuration Warning: No Likelihood components defined for survey \"" << model->id << "\". Default will be used\n";
                } else {
                    for (int i = 0; i < (*rit).value.Size(); i++) {
                        rapidjson::Value& m = (*rit).value[i];

                        rapidjson::Document::MemberIterator mit = m.FindMember("id");
                        if (mit == m.MemberEnd()) {
                            std::cout << "Configuration Error: Surveys Likelihood component has no model identifier\n";
                            std::cout << "Configuration Error: Surveys Likelihood component has no model identifier\n";
                            this->valid_configuration = false;

                        } else {

                            rapidjson::Document::MemberIterator mit2 = m.FindMember("component");
                            if (mit2 == m.MemberEnd()) {
                                std::cout << "Configuration Error: Surveys Likelihood component has no \"component\" specified.\n";
                                std::cout << "Configuration Error: Surveys Likelihood component has no \"component\" specified.\n";
                                this->valid_configuration = false;

                            } else {
                                std::string c = std::string((*mit2).value.GetString());
                                if (c == "age_comp") {
                                    model->survey_age_comp_likelihood_component_id = (*mit).value.GetInt();
                                } else if (c == "biomass_comp") {
                                    model->survey_biomass_likelihood_component_id = (*mit).value.GetInt();
                                } else {
                                    std::cout << "Configuration Error: Unknown  \"component\" of \" " << c << " \" specified for Survey \"" << model->id << "\" likelihood component definition.\n";
                                    std::cout << "Configuration Error: Unknown  \"component\" of \" " << c << " \" specified for Survey \"" << model->id << "\"  likelihood component definition.\n";
                                    this->valid_configuration = false;
                                }
                            }
                        }

                        //                            
                    }


                }
            }

            survey_model_iterator fit;
            fit = this->survey_models.find(model->id);
            if (fit == this->survey_models.end()) {
                this->survey_models[model->id] = model;
            } else {
                std::cout << "Configuration Error: More than one Survey with the same identifier defined. Surveys require a unique id.\n";
                mas::mas_log << "Configuration Error: More than one Survey with the same identifier defined. Surveys require a unique id.\n";
            }
        }

        void HandleFishingMortalityModel(rapidjson::Document::MemberIterator & mortality_model) {
            INFO_DEBUG
            std::shared_ptr<mas::FishingMortality<REAL_T> > model(new mas::FishingMortality<REAL_T>());
            bool estimated = false;
            bool delta_method = false;
            int phase = 1;
            bool bounded = false;
            REAL_T min = std::numeric_limits<REAL_T>::min();
            REAL_T max = std::numeric_limits<REAL_T>::max();

            rapidjson::Document::MemberIterator rit = (*mortality_model).value.FindMember("id");
            int model_id = 0;
            if (rit == (*mortality_model).value.MemberEnd()) {
                std::cout << "Configuration Error: Fishing Mortality is required to have a unique identifier\n";
                mas::mas_log << "Configuration Error: Fishing Mortality is required to have a unique identifier\n";
                this->valid_configuration = false;

            } else {
                model_id = (*rit).value.GetInt();
            }

            model->id = model_id;

            rapidjson::Document::MemberIterator pit = (*mortality_model).value.FindMember("parameters");

            bool is_random = false;

            if (pit == (*mortality_model).value.MemberEnd()) {
                std::cout << "Configuration Error: Fishing Mortality model has no parameter definitions.\n";
                mas::mas_log << "Configuration Error: Fishing Mortality model has no parameter definitions.\n";
                this->valid_configuration = false;

            } else {

                rapidjson::Document::MemberIterator vit = (*pit).value.FindMember("estimated");
                if (vit != (*pit).value.MemberEnd()) {
                    std::string estring = std::string((*vit).value.GetString());

                    if (estring == "true") {
                        estimated = true;
                    }
                }

                rapidjson::Document::MemberIterator re_it = (*pit).value.FindMember("random_effect");
                if (re_it != (*pit).value.MemberEnd()) {
                    if (std::string((*re_it).value.GetString()) == "true") {
                        is_random = true;
                    }
                }

                vit = (*pit).value.FindMember("delta_method");
                if (vit != (*pit).value.MemberEnd()) {
                    std::string estring = std::string((*vit).value.GetString());

                    if (estring == "true") {
                        delta_method = true;
                    }
                }

                vit = (*pit).value.FindMember("min");
                if (vit != (*pit).value.MemberEnd()) {
                    min = static_cast<REAL_T> ((*vit).value.GetDouble());
                    bounded = true;
                }

                vit = (*pit).value.FindMember("max");
                if (vit != (*pit).value.MemberEnd()) {
                    max = static_cast<REAL_T> ((*vit).value.GetDouble());
                    bounded = true;
                }

                vit = (*pit).value.FindMember("phase");
                if (vit != (*pit).value.MemberEnd()) {
                    phase = (*vit).value.GetInt();
                }

                vit = (*pit).value.FindMember("values");

                if (vit == (*pit).value.MemberEnd()) {
                    std::cout << "Configuration Error: Fishing Mortality model supplied no values\n";
                    mas::mas_log << "Configuration Error: Fishing Mortality model supplied no values\n";
                    this->valid_configuration = false;
                } else {

                    if ((*vit).value.IsArray()) {
                        rapidjson::Value & v = (*vit).value;

                        if (v.IsArray()) {

                            model->fishing_mortality.resize(v.Size());

                            for (int i = 0; i < v.Size(); i++) {
                                rapidjson::Value & vv = v[i];
                                model->fishing_mortality[i].resize(vv.Size());
                                for (int j = 0; j < vv.Size(); j++) {
                                    mas::VariableTrait<REAL_T>::SetValue(model->fishing_mortality[i][j], static_cast<REAL_T> (vv[j].GetDouble()));
                                    if (estimated && !delta_method) {

                                        if (bounded) {
                                            mas::VariableTrait<REAL_T>::SetMinBoundary(model->fishing_mortality[i][j], min);
                                            mas::VariableTrait<REAL_T>::SetMaxBoundary(model->fishing_mortality[i][j], max);
                                        }
                                        std::stringstream ss;
                                        ss << "fishing_mortality[" << i << "][" << j << "]_" << model_id;
                                        mas::VariableTrait<REAL_T>::SetName(model->fishing_mortality[i][j], ss.str());
                                        if (is_random) {
                                            model->RegisterRandom(model->fishing_mortality[i][j]);
                                        } else {
                                            model->Register(model->fishing_mortality[i][j], phase);
                                        }
                                    } else if (delta_method) {
                                        model->fishing_mortality_type = mas::FMETHOD;
                                    }
                                }
                            }


                        } else {
                            std::cout << "Configuration Error: Fishing Mortality is requires values in a matrix[year,season] not a vector\n";
                            std::cout << "Configuration Error: Fishing Mortality is requires values in a matrix[year,season] not a vector\n";
                            this->valid_configuration = false;
                        }


                    } else {
                        std::cout << "Configuration Error: Fishing Mortality is requires values in a matrix[year,season]\n";
                        std::cout << "Configuration Error: Fishing Mortality is requires values in a matrix[year,season]\n";
                        this->valid_configuration = false;
                    }


                }

            }

            fishing_mortality_model_iterator fit = this->fishing_mortality_models.find(model->id);


            if (fit == this->fishing_mortality_models.end()) {
                if (delta_method) {
                    model->fishing_mortality_type = mas::FMETHOD;
                }
                this->fishing_mortality_models[model->id] = model;


            } else {
                std::cout << "Configuration Error: More than one fishing mortality model with the same identifier defined. Fishing mortality models require a unique id.\n";
                mas_log << "Configuration Error: More than one fishing mortality model with the same identifier defined. Fishing mortality models require a unique id.\n";
                this->valid_configuration = false;

            }


        }

        void HandleNaturalMortalityModel(rapidjson::Document::MemberIterator & mortality_model) {
            INFO_DEBUG
            std::shared_ptr<mas::NaturalMortality<REAL_T> > model(new mas::NaturalMortality<REAL_T>());
            bool estimated = false;
            int phase = 1;

            rapidjson::Document::MemberIterator rit = (*mortality_model).value.FindMember("id");
            int model_id = 0;
            if (rit == (*mortality_model).value.MemberEnd()) {
                std::cout << "Configuration Error: Natural Mortality is required to have a unique identifier\n";
                mas::mas_log << "Configuration Error: Natural Mortality is required to have a unique identifier\n";
                this->valid_configuration = false;

            } else {
                model_id = (*rit).value.GetInt();
            }

            model->id = model_id;

            rapidjson::Document::MemberIterator pit = (*mortality_model).value.FindMember("parameters");
            rapidjson::Document::MemberIterator mvit = (*pit).value.FindMember("values");
            if ((*mvit).value.IsArray()) {
                //                        model->male_mortality.resize((*mvit).value.Size());
                rapidjson::Value& v = (*mvit).value;
                model->mortality_vector.resize((*mvit).value.Size());
                for (int i = 0; i < (*mvit).value.Size(); i++) {
                    std::stringstream ss;
                    ss << "natural_mortality[" << i << "]_" << model_id;
                    mas::VariableTrait<REAL_T>::SetName(model->mortality_vector[i], ss.str());
                    mas::VariableTrait<REAL_T>::SetValue(model->mortality_vector[i], static_cast<REAL_T> (v[i].GetDouble()));
                }

            } else {
                std::cout << "Configuration Error: Mortality model values for males must be a vector.\n";
                mas::mas_log << "Configuration Error: Mortality model values for males must be a vector.\n";
                this->valid_configuration = false;
                return;
            }

            mvit = (*pit).value.FindMember("min");
            if (mvit != (*pit).value.MemberEnd()) {
                if ((*mvit).value.IsArray()) {

                    rapidjson::Value& v = (*mvit).value;
                    for (int i = 0; i < (*mvit).value.Size(); i++) {

                        mas::VariableTrait<REAL_T>::SetMinBoundary(model->mortality_vector[i], static_cast<REAL_T> (v[i].GetDouble()));
                    }

                } else {
                    std::cout << "Configuration Error: Mortality model min values for males must be a vector.\n";
                    mas::mas_log << "Configuration Error: Mortality model min values for males must be a vector.\n";
                    this->valid_configuration = false;

                }
            }

            mvit = (*pit).value.FindMember("max");
            if (mvit != (*pit).value.MemberEnd()) {
                if ((*mvit).value.IsArray()) {

                    rapidjson::Value& v = (*mvit).value;
                    for (int i = 0; i < (*mvit).value.Size(); i++) {

                        mas::VariableTrait<REAL_T>::SetMaxBoundary(model->mortality_vector[i], static_cast<REAL_T> (v[i].GetDouble()));
                    }

                } else {
                    std::cout << "Configuration Error: Mortality model min values for males must be a vector.\n";
                    mas::mas_log << "Configuration Error: Mortality model min values for males must be a vector.\n";
                    this->valid_configuration = false;

                }
            }


            rit = (*pit).value.FindMember("estimated");
            if (rit != (*pit).value.MemberEnd()) {
                std::string e = std::string((*rit).value.GetString());
                if (e == "true") {
                    estimated = true;
                }
            }

            if (estimated) {
                phase = 1;
                rit = (*pit).value.FindMember("phase");
                if (rit != (*pit).value.MemberEnd()) {
                    phase = (*rit).value.GetInt();
                }

                //register male mortality parameters
                for (int i = 0; i < model->mortality_vector.size(); i++) {
                    model->Register(model->mortality_vector[i], phase);
                }

            }

            natural_mortality_model_iterator it = this->natural_mortality_models.find(model->id);
            if (it != this->natural_mortality_models.end()) {
                std::cout << "Configuration Error: More than one mortality model with the same identifier defined. Mortality models require a unique id.\n";
                mas::mas_log << "Configuration Error: More than one mortality model with the same identifier defined. Mortality models require a unique id.\n";

                this->valid_configuration = false;

            } else {
                this->natural_mortality_models[model->id] = model;
            }

        }

        void HandleMovementModel(rapidjson::Document::MemberIterator & movement_model) {
            INFO_DEBUG
            std::shared_ptr<mas::Movement<REAL_T> > model(new mas::Movement<REAL_T>());
            bool estimated = false;
            int phase = 1;

            rapidjson::Document::MemberIterator rit = (*movement_model).value.FindMember("id");
            int model_id = 0;
            if (rit == (*movement_model).value.MemberEnd()) {
                std::cout << "Configuration Error: Movement is required to have a unique identifier\n";
                mas::mas_log << "Configuration Error: Movement is required to have a unique identifier\n";
                this->valid_configuration = false;

            } else {
                model_id = (*rit).value.GetInt();
            }
            model->id = model_id;

            //            rit = (*movement_model).value.FindMember("season");
            //            if (rit == (*movement_model).value.MemberEnd()) {
            //                std::cout << "Configuration Error: Movement is required to have a season defined\n";
            //                mas::mas_log << "Configuration Error: Movement is required to have a season defined\n";
            //                this->valid_configuration = false;
            //                ;
            //            } else {
            //                model->season = (*rit).value.GetInt();
            //            }


            rit = (*movement_model).value.FindMember("recruits");
            if ((*rit).value.IsArray()) {
                model->recruit_connectivity.resize((*rit).value.Size());
                for (int i = 0; i < (*rit).value.Size(); i++) {//season
                    rapidjson::Value& m = (*rit).value[i];
                    if (m.IsArray()) {

                        model->recruit_connectivity[i].resize(m.Size());
                        for (int j = 0; j < m.Size(); j++) {//area
                            rapidjson::Value& n = m[j];
                            if (n.IsArray()) {
                                model->recruit_connectivity[i][j].resize(n.Size());

                                for (int k = 0; k < n.Size(); k++) {//area
                                    mas::VariableTrait<REAL_T>::SetValue(model->recruit_connectivity[i][j][k], static_cast<REAL_T> (n[k].GetDouble()));
                                    mas::VariableTrait<REAL_T>::SetMaxBoundary(model->recruit_connectivity[i][j][k], 1.0);
                                    mas::VariableTrait<REAL_T>::SetMinBoundary(model->recruit_connectivity[i][j][k], 0.0);
                                }
                            }
                        }

                    } else {
                        std::cout << "Configuration Error: Movement connectivity rows must be a vector\n";
                        mas::mas_log << "Configuration Error: Movement connectivity rows must be a vector\n";
                        this->valid_configuration = false;

                    }
                }


            } else {
                std::cout << "Configuration Error: Movement connectivity must be a matrix\n";
                mas::mas_log << "Configuration Error: Movement connectivity must be a matrix\n";
                this->valid_configuration = false;

            }


            rit = (*movement_model).value.FindMember("male");
            if ((*rit).value.IsArray()) {
                model->male_connectivity.resize((*rit).value.Size());
                for (int i = 0; i < (*rit).value.Size(); i++) {//season
                    rapidjson::Value& m = (*rit).value[i];
                    if (m.IsArray()) {

                        model->male_connectivity[i].resize(m.Size());
                        for (int j = 0; j < m.Size(); j++) {//area
                            rapidjson::Value& n = m[j];
                            if (n.IsArray()) {
                                model->male_connectivity[i][j].resize(n.Size());

                                for (int k = 0; k < n.Size(); k++) {//area
                                    mas::VariableTrait<REAL_T>::SetValue(model->male_connectivity[i][j][k], static_cast<REAL_T> (n[k].GetDouble()));
                                    mas::VariableTrait<REAL_T>::SetMaxBoundary(model->male_connectivity[i][j][k], 1.0);
                                    mas::VariableTrait<REAL_T>::SetMinBoundary(model->male_connectivity[i][j][k], 0.0);
                                }
                            }
                        }

                    } else {
                        std::cout << "Configuration Error: Movement connectivity rows must be a vector\n";
                        mas::mas_log << "Configuration Error: Movement connectivity rows must be a vector\n";
                        this->valid_configuration = false;

                    }
                }


            } else {
                std::cout << "Configuration Error: Movement connectivity must be a matrix\n";
                mas::mas_log << "Configuration Error: Movement connectivity must be a matrix\n";
                this->valid_configuration = false;

            }

            rit = (*movement_model).value.FindMember("female");
            if ((*rit).value.IsArray()) {
                model->female_connectivity.resize((*rit).value.Size());
                for (int i = 0; i < (*rit).value.Size(); i++) {
                    rapidjson::Value& m = (*rit).value[i];
                    if (m.IsArray()) {

                        model->female_connectivity[i].resize(m.Size());
                        for (int j = 0; j < m.Size(); j++) {
                            rapidjson::Value& n = m[j];
                            if (n.IsArray()) {
                                model->female_connectivity[i][j].resize(n.Size());

                                for (int k = 0; k < n.Size(); k++) {
                                    mas::VariableTrait<REAL_T>::SetValue(model->female_connectivity[i][j][k], static_cast<REAL_T> (n[k].GetDouble()));
                                    mas::VariableTrait<REAL_T>::SetMaxBoundary(model->female_connectivity[i][j][k], 1.0);
                                    mas::VariableTrait<REAL_T>::SetMinBoundary(model->female_connectivity[i][j][k], 0.0);
                                }
                            }
                        }

                    } else {
                        std::cout << "Configuration Error: Movement connectivity rows must be a vector\n";
                        mas::mas_log << "Configuration Error: Movement connectivity rows must be a vector\n";
                        this->valid_configuration = false;

                    }
                }


            } else {
                std::cout << "Configuration Error: Movement connectivity must be a matrix\n";
                mas::mas_log << "Configuration Error: Movement connectivity must be a matrix\n";
                this->valid_configuration = false;
                ;
            }


            rit = (*movement_model).value.FindMember("estimated");
            if (rit != (*movement_model).value.MemberEnd()) {
                std::string e = std::string((*rit).value.GetString());
                if (e == "true") {
                    estimated = true;
                }
            }

            if (estimated) {
                phase = 1;
                rit = (*movement_model).value.FindMember("phase");
                if (rit != (*movement_model).value.MemberEnd()) {
                    phase = (*rit).value.GetInt();

                }
                //register movement coefficients
                for (int i = 0; i < model->male_connectivity.size(); i++) {
                    for (int j = 0; j < model->male_connectivity[i].size(); j++) {
                        for (int k = 0; k < model->male_connectivity[i][0].size(); k++) {
                            std::stringstream ss;
                            ss << "male_connectivity[" << i << "][" << j << "][" << k << "]_" << model->id;
                            model->Register(model->male_connectivity[i][j][k], phase);
                            mas::VariableTrait<REAL_T>::SetName(model->male_connectivity[i][j][k], ss.str());
                        }
                    }
                }

                //register movement coefficients
                for (int i = 0; i < model->female_connectivity.size(); i++) {
                    for (int j = 0; j < model->female_connectivity[i].size(); j++) {
                        for (int k = 0; k < model->female_connectivity[i][0].size(); k++) {
                            std::stringstream ss;
                            ss << "female_connectivity[" << i << "][" << j << "][" << k << "]_" << model->id;
                            model->Register(model->female_connectivity[i][j][k], phase);
                            mas::VariableTrait<REAL_T>::SetName(model->female_connectivity[i][j][k], ss.str());
                        }
                    }
                }

                for (int i = 0; i < model->recruit_connectivity.size(); i++) {
                    for (int j = 0; j < model->recruit_connectivity[i].size(); j++) {
                        for (int k = 0; k < model->recruit_connectivity[i][0].size(); k++) {
                            std::stringstream ss;
                            ss << "recruit_connectivity[" << i << "][" << j << "][" << k << "]_" << model->id;
                            model->Register(model->recruit_connectivity[i][j][k], phase);
                            mas::VariableTrait<REAL_T>::SetName(model->recruit_connectivity[i][j][k], ss.str());
                        }
                    }
                }
            }

            movement_model_iterator it = this->movement_models.find(model->id);
            if (it != this->movement_models.end()) {
                std::cout << "Configuration Error: More than one movement model with the same identifier defined. Movement models require a unique id.\n";
                mas::mas_log << "Configuration Error: More than one movement model with the same identifier defined. Movement models require a unique id.\n";
                this->valid_configuration = false;
            } else {
                this->movement_models[model->id] = model;
            }

        }

        void HandleFleetModel(rapidjson::Document::MemberIterator & fleet_model) {
            INFO_DEBUG
            std::shared_ptr<mas::Fleet<REAL_T> > model(new mas::Fleet<REAL_T>());


            rapidjson::Document::MemberIterator rit = (*fleet_model).value.FindMember("id");
            int model_id = 0;
            if (rit == (*fleet_model).value.MemberEnd()) {
                std::cout << "Configuration Error: Fleets are required to have a unique identifier\n";
                mas::mas_log << "Configuration Error: Fleets are required to have a unique identifier\n";
                this->valid_configuration = false;
            } else {
                model_id = (*rit).value.GetInt();
            }
            model->id = model_id;

            rit = (*fleet_model).value.FindMember("name");
            if (rit != (*fleet_model).value.MemberEnd()) {
                model->name = std::string((*rit).value.GetString());
            } else {
                model->name = "NA";
            }

            rit = (*fleet_model).value.FindMember("catch_fraction_of_year");
            if (rit != (*fleet_model).value.MemberEnd()) {
                model->catch_fraction_of_year = (*rit).value.GetDouble();
            }


            rit = (*fleet_model).value.FindMember("selectivity");

            if (rit != (*fleet_model).value.MemberEnd()) {
                if (!(*rit).value.IsArray()) {
                    std::cout << "Configuration Error: Fleets are required to have selectivity definitions in a vector\n";
                    std::cout << "Configuration Error: Fleets are required to have selectivity definitions in a vector\n";
                    this->valid_configuration = false;

                } else {
                    for (int i = 0; i < (*rit).value.Size(); i++) {
                        rapidjson::Value& m = (*rit).value[i];

                        int sid = 0;
                        int sarea = 0;
                        int sseason = 0;
                        REAL_T fraction = 0.5;
                        rapidjson::Document::MemberIterator vit = m.FindMember("id");
                        if (vit == m.MemberEnd()) {
                            std::cout << "Configuration Error: Fleets selectivity has no model identifier\n";
                            std::cout << "Configuration Error: Fleets selectivity has no model identifier\n";
                            this->valid_configuration = false;

                        } else {
                            sid = (*vit).value.GetInt();
                        }

                        rapidjson::Document::MemberIterator mit = m.FindMember("season");
                        if (mit == m.MemberEnd()) {
                            std::cout << "Configuration Error: Fleets selectivity season has no identifier\n";
                            std::cout << "Configuration Error: Fleets selectivity season has no identifier\n";
                            this->valid_configuration = false;

                        } else {
                            sseason = (*mit).value.GetInt();
                        }

                        mit = m.FindMember("fraction");
                        if (mit == m.MemberEnd()) {
                            std::cout << "Configuration Warning: Fleets selectivity season has no catch fraction. Default is 0.5\n";
                            std::cout << "Configuration Error: Fleets selectivity season has no catch fraction. Default is 0.5\n";

                        } else {
                            fraction = (*mit).value.GetDouble();
                        }

                        mit = m.FindMember("ensemble");
                        if (mit != m.MemberEnd()) {
                            if (!(*mit).value.IsArray()) {
                                std::cout << "Expected array for fleet selectivity ensemble unit, ignored.\n";
                                mas_log << "Expected array for fleet selectivity ensemble unit, ignored.\n";
                            } else {
                                EnsembleUnit eunit;
                                eunit.type = mas::SELECTIVITY;
                                rapidjson::Value& e = (*mit).value;
                                eunit.val = vit;
                                for (int i = 0; i < e.Size(); i++) {
                                    eunit.units.push_back(e[i].GetInt());
                                }
                                this->ensemble_units.push_back(eunit);

                            }
                        }

                        mit = m.FindMember("area");
                        if (mit == m.MemberEnd()) {
                            std::cout << "Configuration Error: Fleets selectivity area has no identifier\n";
                            std::cout << "Configuration Error: Fleets selectivity area has no identifier\n";
                            this->valid_configuration = false;

                        } else {
                            sarea = (*mit).value.GetInt();
                        }
                        model->operational_areas.insert(sarea);
                        model->season_area_selectivity_ids[sseason][sarea] = sid;
                        model->area_season_selectivity_ids[sarea][sseason] = sid;
                        model->area_season_catch_fraction[sarea][sseason] = fraction;
                        model->season_area_catch_fraction[sseason][sarea] = fraction;
                    }
                }
            }


            rit = (*fleet_model).value.FindMember("fishing_mortality");
            if (rit != (*fleet_model).value.MemberEnd()) {
                if (!(*rit).value.IsArray()) {
                    std::cout << "Configuration Error: Fleets are required to have fishing mortality definitions in a vector\n";
                    std::cout << "Configuration Error: Fleets are required to have fishing mortality definitions in a vector\n";
                    this->valid_configuration = false;

                } else {
                    for (int i = 0; i < (*rit).value.Size(); i++) {
                        rapidjson::Value& m = (*rit).value[i];

                        int sid = 0;
                        int sarea = 0;
                        int sseason = 0;
                        rapidjson::Document::MemberIterator vit = m.FindMember("id");
                        if (vit == m.MemberEnd()) {
                            std::cout << "Configuration Error: Fleets fishing mortality has no model identifier\n";
                            std::cout << "Configuration Error: Fleets fishing mortality has no model identifier\n";
                            this->valid_configuration = false;

                        } else {
                            sid = (*vit).value.GetInt();
                        }

                        rapidjson::Document::MemberIterator mit = m.FindMember("ensemble");
                        if (mit != m.MemberEnd()) {
                            if (!(*mit).value.IsArray()) {
                                std::cout << "Expected array for fleet fishing mortality ensemble unit, ignored.\n";
                                mas_log << "Expected array for fleet fishing mortality ensemble unit, ignored.\n";
                            } else {
                                EnsembleUnit eunit;
                                eunit.type = mas::FISHING_MORTALITY;
                                rapidjson::Value& e = (*mit).value;
                                eunit.val = vit;
                                for (int i = 0; i < e.Size(); i++) {
                                    eunit.units.push_back(e[i].GetInt());
                                }
                                this->ensemble_units.push_back(eunit);

                            }
                        }

                        mit = m.FindMember("season");
                        if (mit == m.MemberEnd()) {
                            std::cout << "Configuration Error: Fleets fishing mortality season has no identifier\n";
                            std::cout << "Configuration Error: Fleets fishing mortality season has no identifier\n";
                            this->valid_configuration = false;

                        } else {
                            sseason = (*mit).value.GetInt();
                        }

                        mit = m.FindMember("area");
                        if (mit == m.MemberEnd()) {
                            std::cout << "Configuration Error: Fleets fishing mortality area has no identifier\n";
                            std::cout << "Configuration Error: Fleets fishing mortality area has no identifier\n";
                            this->valid_configuration = false;

                        } else {
                            sarea = (*mit).value.GetInt();
                        }
                        model->season_area_fishing_mortality_ids[sseason][sarea] = sid;
                        model->area_season_fishing_mortality_ids[sarea][sseason] = sid;
                    }
                }
            }

            rit = (*fleet_model).value.FindMember("likelihood_components");

            if (rit != (*fleet_model).value.MemberEnd()) {
                if (!(*rit).value.IsArray()) {
                    std::cout << "Configuration Warning: No Likelihood components defined for fleet \"" << model->id << "\". Default will be used\n";
                    std::cout << "Configuration Warning: No Likelihood components defined for fleet \"" << model->id << "\". Default will be used\n";
                } else {
                    for (int i = 0; i < (*rit).value.Size(); i++) {
                        rapidjson::Value& m = (*rit).value[i];

                        rapidjson::Document::MemberIterator mit = m.FindMember("id");
                        if (mit == m.MemberEnd()) {
                            std::cout << "Configuration Error: Fleets Likelihood component has no model identifier\n";
                            std::cout << "Configuration Error: Fleets Likelihood component has no model identifier\n";
                            this->valid_configuration = false;

                        } else {

                            rapidjson::Document::MemberIterator mit2 = m.FindMember("component");
                            if (mit2 == m.MemberEnd()) {
                                std::cout << "Configuration Error: Fleets Likelihood component has no \"component\" specified.\n";
                                std::cout << "Configuration Error: Fleets Likelihood component has no \"component\" specified.\n";
                                this->valid_configuration = false;

                            } else {
                                std::string c = std::string((*mit2).value.GetString());
                                if (c == "age_comp") {
                                    model->fishery_age_comp_likelihood_component_id = (*mit).value.GetInt();
                                } else if (c == "biomass_comp") {
                                    model->fishery_biomass_likelihood_component_id = (*mit).value.GetInt();
                                } else {
                                    std::cout << "Configuration Error: Unknown  \"component\" of \" " << c << " \" specified for fleet \"" << model->id << "\" likelihood component definition.\n";
                                    std::cout << "Configuration Error: Unknown  \"component\" of \" " << c << " \" specified for fleet \"" << model->id << "\"  likelihood component definition.\n";
                                    this->valid_configuration = false;
                                }
                            }
                        }

                        //                            
                    }


                }
            }




            fleet_iterator fit;
            fit = this->fleets.find(model->id);
            if (fit == this->fleets.end()) {
                this->fleets[model->id] = model;
            } else {
                std::cout << "Configuration Error: More than one fleet with the same identifier defined. Fleets require a unique id.\n";
                mas::mas_log << "Configuration Error: More than one fleet with the same identifier defined. Fleets require a unique id.\n";
            }

        }

        void HandlePopulationModel(rapidjson::Document::MemberIterator & population_model) {
            INFO_DEBUG
            std::shared_ptr<mas::Population<REAL_T> > model(new mas::Population<REAL_T>());


            rapidjson::Document::MemberIterator rit = (*population_model).value.FindMember("id");
            int model_id = 0;
            if (rit == (*population_model).value.MemberEnd()) {
                std::cout << "Configuration Error: Populations are required to have a unique identifier\n";
                mas::mas_log << "Configuration Error: Populations are required to have a unique identifier\n";
                this->valid_configuration = false;
                ;
            } else {
                model_id = (*rit).value.GetInt();
            }
            model->id = model_id;

            rit = (*population_model).value.FindMember("name");
            if (rit != (*population_model).value.MemberEnd()) {
                model->name = std::string((*rit).value.GetString());
            } else {
                model->name = "NA";
            }

            rit = (*population_model).value.FindMember("hcr");
            if (rit != (*population_model).value.MemberEnd()) {
                std::string hcrmodel = std::string((*rit).value.GetString());
                if (hcrmodel == std::string("NPFMC Tier 3 HCR")) {
                    model->harvest_control_rule = std::make_shared<mas::NPFMC_Tier3_HCR<REAL_T> >();
                } else if (hcrmodel == std::string("PFMC HCR")) {
                    model->harvest_control_rule = std::make_shared<mas::PFMC_HCR<REAL_T> >();
                }
            } else {
                std::cout << "Configuration Warning: Population has no Harvest Control Rule selected\n";
                mas::mas_log << "Configuration Warning: Population has no Harvest Control Rule selected\n";
            }



            rit = (*population_model).value.FindMember("sex_ratio");
            if (rit != (*population_model).value.MemberEnd()) {
                model->female_fraction_value = (*rit).value.GetDouble();
                if (model->female_fraction_value > 1.0) {
                    std::cout << "Configuration Error: Population \"sex_fraction\" value is greater than 1.0.\n";
                    mas::mas_log << "Configuration Error: Population \"sex_fraction\" value is greater than 1.0.\n";
                    this->valid_configuration = false;
                }

                if (model->female_fraction_value < 0.0) {
                    std::cout << "Configuration Error: Population \"sex_fraction\" value is less than 0.0.\n";
                    mas::mas_log << "Configuration Error: Population \"sex_fraction\" value is less than 0.0.\n";
                    this->valid_configuration = false;
                }
            }


            rit = (*population_model).value.FindMember("maturity");

            // NOTE:  maturity model should be specified by parameters (similar to selectivity), and
            //        optionally by an empirical vector instead
            if (rit != (*population_model).value.MemberEnd()) {
                //                model->movement_model_id = (*rit).value.GetInt();

                if ((*rit).value.IsArray()) {

                    for (int i = 0; i < (*rit).value.Size(); i++) {
                        rapidjson::Value& v = (*rit).value[i];
                        int sex = 0;
                        int area = -9999;
                        std::vector<REAL_T> values;
                        rapidjson::Document::MemberIterator mit = v.FindMember("sex");
                        if (std::string((*mit).value.GetString()) == std::string("female")) {
                            sex = 1;
                        }

                        mit = v.FindMember("area");

                        if (mit == v.MemberEnd()) {
                            std::cout << "Configuration Error: Population maturity models must define a area.\n";
                            mas::mas_log << "Configuration Error: Population maturity models must define a area.\n";
                            this->valid_configuration = false;
                        } else {
                            area = (*mit).value.GetInt();
                        }

                        mit = v.FindMember("values");
                        if (mit == v.MemberEnd()) {
                            std::cout << "Configuration Error: Population maturity models must define a value vector.\n";
                            mas::mas_log << "Configuration Error: Population maturity models must define a value vector.\n";
                            this->valid_configuration = false;
                        } else {

                            if ((*mit).value.IsArray()) {

                                for (int j = 0; j < (*mit).value.Size(); j++) {
                                    rapidjson::Value& vv = (*mit).value[j];
                                    values.push_back(static_cast<REAL_T> (vv.GetDouble()));
                                }

                            } else {
                                std::cout << "Configuration Error: Population maturity models must define a value vector.\n";
                                mas::mas_log << "Configuration Error: Population maturity models must define a value vector.\n";
                            }

                        }

                        model->maturity_models[area][sex] = values;

                    }

                } else {
                    std::cout << "Configuration Error: Population maturity models must be defined in a vector.\n";
                    mas::mas_log << "Configuration Error: Population maturity models must be defined in a vector.\n";
                    this->valid_configuration = false;
                }

            } else {
                std::cout << "Configuration Error: Population is required to have maturity model defined\n";
                mas::mas_log << "Configuration Error: Population is required to have maturity model defined\n";
                this->valid_configuration = false;

            }



            rit = (*population_model).value.FindMember("movement");
            if (rit != (*population_model).value.MemberEnd()) {

                if ((*rit).value.IsArray()) {

                    for (int i = 0; i < (*rit).value.Size(); i++) {
                        rapidjson::Value& v = (*rit).value[i];
                        int s = 0;
                        int m = 0;


                        rapidjson::Document::MemberIterator vit = v.FindMember("id");
                        if ((vit) != v.MemberEnd()) {
                            m = (*vit).value.GetInt();
                        } else {
                            std::cout << "Configuration Error: Movement model not defined in population.\n";
                            mas::mas_log << "Configuration Error: Movement model not defined in population.\n";
                            this->valid_configuration = false;
                        }


                        rapidjson::Document::MemberIterator mit = v.FindMember("year");
                        if ((mit) != v.MemberEnd()) {
                            s = (*mit).value.GetInt();
                        } else {
                            std::cout << "Configuration Error: Movement year not defined in population.\n";
                            mas::mas_log << "Configuration Error: Movement year not defined in population.\n";
                            this->valid_configuration = false;
                        }

                        mit = v.FindMember("ensemble");
                        if (mit != v.MemberEnd()) {
                            if (!(*mit).value.IsArray()) {
                                std::cout << "Expected array for population movement ensemble unit, ignored.\n";
                                mas_log << "Expected array for population movement ensemble unit, ignored.\n";
                            } else {
                                EnsembleUnit eunit;
                                eunit.type = mas::MOVEMENT;
                                rapidjson::Value& e = (*mit).value;
                                eunit.val = vit;
                                for (int i = 0; i < e.Size(); i++) {
                                    eunit.units.push_back(e[i].GetInt());
                                }
                                this->ensemble_units.push_back(eunit);

                            }
                        }


                        model->movement_models_ids[s] = m;
                    }


                } else {
                    std::cout << "Configuration Error: Population movement models must be defined in a vector.\n";
                    mas::mas_log << "Configuration Error: Population movement models must be defined in a vector.\n";
                    this->valid_configuration = false;
                }

            } else {
                std::cout << "Configuration Error: Population is required to have movement model defined\n";
                mas::mas_log << "Configuration Error: Population is required to have movement model defined\n";
                this->valid_configuration = false;

            }

            rit = (*population_model).value.FindMember("parameters");

            if (rit == (*population_model).value.MemberEnd()) {
                std::cout << "Configuration Error: Population is required to define parameters.";
                mas_log << "Configuration Error: Population is required to define parameters.";
                this->valid_configuration = false;
                return;

            }

            rapidjson::Document::MemberIterator in_it = (*rit).value.FindMember("initial_deviations");
            bool is_random = false;
            /*
             * NOTE:  initial numbers-at-age is specified by R0 and M-at-age vector
             */

            if (in_it != (*rit).value.MemberEnd()) {
                if ((*in_it).value.IsArray()) {

                    rapidjson::Value& deviations = ((*in_it).value);
                    for (int i = 0; i < deviations.Size(); i++) {
                        bool is_bounded = false;
                        REAL_T min = std::numeric_limits<REAL_T>::min();
                        REAL_T max = std::numeric_limits<REAL_T>::max();

                        rapidjson::Value& entry = deviations[i];
                        rapidjson::Document::MemberIterator nit;
                        std::string sex;
                        int area;
                        int phase = 1;
                        bool estimated = false;
                        int sex_ = mas::UNDIFFERENTIATED;
                        nit = entry.FindMember("estimated");

                        if (nit != entry.MemberEnd()) {
                            if (std::string((*nit).value.GetString()) == "true") {
                                estimated = true;
                            }
                        }

                        nit = entry.FindMember("random_effect");

                        if (nit != entry.MemberEnd()) {
                            if (std::string((*nit).value.GetString()) == "true") {
                                is_random = true;
                            }
                        }

                        nit = entry.FindMember("phase");
                        if (nit != entry.MemberEnd()) {
                            phase = (*nit).value.GetInt();
                        }

                        nit = entry.FindMember("min");
                        if (nit != entry.MemberEnd()) {
                            min = (*nit).value.GetDouble();
                            is_bounded = true;
                        }

                        nit = entry.FindMember("max");
                        if (nit != entry.MemberEnd()) {
                            max = (*nit).value.GetDouble();
                            is_bounded = true;
                        }


                        nit = entry.FindMember("sex");
                        if (nit != entry.MemberEnd()) {
                            sex = std::string((*nit).value.GetString());
                            if (sex == "male") {
                                int sex_ = mas::MALE;
                                nit = entry.FindMember("area");
                                if (nit != entry.MemberEnd()) {
                                    area = (*nit).value.GetInt();

                                } else {
                                    std::cout << "Configuration Error: Population initial number deviations is required to define a area";
                                    mas::mas_log << "Configuration Error: Population initial number deviations is required to define a area";

                                }
                                std::vector<variable>& init_deviations = model->initial_deviations_males[area].second;
                                model->initial_deviations_males[area].first = estimated;
                                nit = entry.FindMember("values");

                                if (nit != entry.MemberEnd()) {
                                    rapidjson::Value& values = (*nit).value;
                                    if (values.IsArray()) {
                                        init_deviations.resize(values.Size());
                                        for (int i = 0; i < values.Size(); i++) {
                                            VariableTrait<REAL_T>::SetValue(init_deviations[i], static_cast<REAL_T> (values[i].GetDouble()));

                                            if (is_bounded) {
                                                VariableTrait<REAL_T>::SetMinBoundary(init_deviations[i], min);
                                                VariableTrait<REAL_T>::SetMaxBoundary(init_deviations[i], max);
                                            }
                                            if (estimated) {
                                                std::stringstream ss;
                                                ss << "idevsm_" << model->id << "_a_" << area;
                                                VariableTrait<REAL_T>::SetName(init_deviations[i], ss.str());
                                                if (is_random) {
                                                    model->RegisterRandom(init_deviations[i]);
                                                } else {
                                                    model->Register(init_deviations[i], phase);
                                                }
                                            }

                                        }

                                    }

                                } else {
                                    std::cout << "Configuration Error: Population initial deviations are required to define values";
                                    mas::mas_log << "Configuration Error: Population initial deviations are required to define values";

                                }

                            } else if (sex == "female") {
                                int sex_ = mas::MALE;
                                nit = entry.FindMember("area");
                                if (nit != entry.MemberEnd()) {
                                    area = (*nit).value.GetInt();

                                } else {
                                    std::cout << "Configuration Error: Population initial number deviations is required to define a area";
                                    mas::mas_log << "Configuration Error: Population initial number deviations is required to define a area";

                                }
                                std::vector<variable>& init_deviations = model->initial_deviations_females[area].second;
                                model->initial_deviations_females[area].first = estimated;
                                nit = entry.FindMember("values");

                                if (nit != entry.MemberEnd()) {
                                    rapidjson::Value& values = (*nit).value;
                                    if (values.IsArray()) {
                                        init_deviations.resize(values.Size());
                                        for (int i = 0; i < values.Size(); i++) {

                                            VariableTrait<REAL_T>::SetValue(init_deviations[i], static_cast<REAL_T> (values[i].GetDouble()));
                                            if (estimated) {
                                                std::stringstream ss;
                                                ss << "idevsf_" << model->id << "_a_" << area;
                                                VariableTrait<REAL_T>::SetName(init_deviations[i], ss.str());
                                                if (is_random) {
                                                    model->RegisterRandom(init_deviations[i]);
                                                } else {
                                                    model->Register(init_deviations[i], phase);
                                                }
                                            }

                                        }

                                    }

                                } else {
                                    std::cout << "Configuration Error: Population initial deviations are required to define values";
                                    mas::mas_log << "Configuration Error: Population initial deviations are required to define values";

                                }
                            } else if (sex == "undifferentiated") {

                                int sex_ = mas::UNDIFFERENTIATED;
                                nit = entry.FindMember("area");
                                if (nit != entry.MemberEnd()) {
                                    area = (*nit).value.GetInt();

                                } else {
                                    std::cout << "Configuration Error: Population initial number deviations is required to define a area";
                                    mas::mas_log << "Configuration Error: Population initial number deviations is required to define a area";

                                }
                                std::vector<variable>& init_deviationsm = model->initial_deviations_males[area].second;
                                model->initial_deviations_males[area].first = estimated;
                                std::vector<variable>& init_deviationsf = model->initial_deviations_females[area].second;
                                model->initial_deviations_females[area].first = estimated;
                                nit = entry.FindMember("values");

                                if (nit != entry.MemberEnd()) {
                                    rapidjson::Value& values = (*nit).value;
                                    if (values.IsArray()) {
                                        init_deviationsm.resize(values.Size());
                                        init_deviationsf.resize(values.Size());
                                        for (int i = 0; i < values.Size(); i++) {

                                            VariableTrait<REAL_T>::SetValue(init_deviationsm[i], static_cast<REAL_T> (values[i].GetDouble()));
                                            VariableTrait<REAL_T>::SetValue(init_deviationsf[i], static_cast<REAL_T> (values[i].GetDouble()));

                                            if (estimated) {
                                                std::stringstream ss;
                                                ss << "idevsu_" << model->id << "_a_" << area;
                                                VariableTrait<REAL_T>::SetName(init_deviationsm[i], ss.str());
                                                if (is_random) {
                                                    model->RegisterRandom(init_deviationsm[i]);
                                                } else {
                                                    model->Register(init_deviationsm[i], phase);
                                                }
                                                ss.str("");
                                                ss << "idevsf_" << model->id << "_a_" << area;
                                                VariableTrait<REAL_T>::SetName(init_deviationsf[i], ss.str());
                                                if (is_random) {
                                                    model->RegisterRandom(init_deviationsf[i]);
                                                } else {
                                                    model->Register(init_deviationsf[i], phase);
                                                }
                                            }

                                        }

                                    }

                                } else {
                                    std::cout << "Configuration Error: Population initial deviations are required to define values";
                                    mas::mas_log << "Configuration Error: Population initial deviations are required to define values";

                                }

                            }

                        } else {
                            std::cout << "Configuration Error: Population initial number deviations is required to define a sex";
                            mas::mas_log << "Configuration Error: Population initial number deviations is required to define a sex";

                        }


                    }


                } else {
                    std::cout << "Configuration Error: initial number deviations for population \"" << model->id << "\" must be a vector.\n";
                    mas_log << "Configuration Error: initial number deviations for population \"" << model->id << "\" must be a vector.\n";
                }
            } else {
                std::cout << "Configuration Warning: initial number deviations for population \"" << model->id << "\" not defined.\n";
                mas_log << "Configuration Warning: initial number deviations for population \"" << model->id << "\" not defined.\n";
            }

            in_it = (*rit).value.FindMember("growth");
            if (in_it != (*rit).value.MemberEnd()) {

                rapidjson::Document::MemberIterator git;
                git = (*in_it).value.FindMember("id");
                if (git != (*in_it).value.MemberEnd()) {
                    model->growth_id = (*git).value.GetInt();
                } else {
                    std::cout << "Configuration Warning: Growth model id not defined in population.\n";
                    mas::mas_log << "Configuration Warning: Growth model id not defined in population.\n";
                }
                git = (*in_it).value.FindMember("ensemble");
                if (git != (*in_it).value.MemberEnd()) {
                    if (!(*git).value.IsArray()) {
                        std::cout << "Expected array for population growth ensemble unit, ignored.\n";
                        mas_log << "Expected array for population growth ensemble unit, ignored.\n";
                    } else {
                        EnsembleUnit eunit;
                        eunit.type = mas::RECRUITMENT;
                        rapidjson::Value& e = (*git).value;
                        eunit.val = git;
                        for (int i = 0; i < e.Size(); i++) {
                            eunit.units.push_back(e[i].GetInt());
                        }
                        this->ensemble_units.push_back(eunit);

                    }
                }
            } else {
                std::cout << "Configuration Warning: Growth model not defined in population.\n";
                mas::mas_log << "Configuration Warning: Growth model not defined in population.\n";
            }

            in_it = (*rit).value.FindMember("natural_mortality");
            if (in_it != (*rit).value.MemberEnd()) {

                if ((*in_it).value.IsArray()) {
                    int a = 0;
                    int m = 0;
                    std::string sex = "NA";

                    for (int i = 0; i < (*in_it).value.Size(); i++) {
                        rapidjson::Value& v = (*in_it).value[i];
                        m = 0;
                        sex = "NA";

                        rapidjson::Document::MemberIterator mit = v.FindMember("area");
                        if ((mit) != v.MemberEnd()) {
                            a = (*mit).value.GetInt();
                        } else {
                            std::cout << "Configuration Error: Natural mortality area not defined in population.\n";
                            mas::mas_log << "Configuration Error: Natural mortality area not defined in population.\n";
                            this->valid_configuration = false;
                        }


                        mit = v.FindMember("id");
                        if ((mit) != v.MemberEnd()) {
                            m = (*mit).value.GetInt();
                        } else {
                            std::cout << "Configuration Error: Natural mortality model not defined in population.\n";
                            mas::mas_log << "Configuration Error: Natural mortality model not defined in population.\n";
                            this->valid_configuration = false;
                        }

                        mit = v.FindMember("sex");
                        if ((mit) != v.MemberEnd()) {
                            sex = std::string((*mit).value.GetString());
                        } else {
                            std::cout << "Configuration Warning: Natural mortality sex not defined in population.\n";
                            mas::mas_log << "Configuration Warning: Natural mortality sex not defined in population.\n";
                            //                            this->valid_configuration = false;
                        }


                        if (sex == "female") {
                            model->female_natural_mortality_ids[a] = m;
                        } else {
                            model->male_natural_mortality_ids[a] = m;
                        }
                    }
                }
            }


            in_it = (*rit).value.FindMember("recruitment");
            if (in_it != (*rit).value.MemberEnd()) {

                if ((*in_it).value.IsArray()) {
                    rapidjson::Value& v = (*in_it).value;

                    for (int i = 0; i < v.Size(); i++) {
                        int area = 0;
                        int season = 0;
                        int id = 0;

                        rapidjson::Value& elem = v[i];

                        rapidjson::Document::MemberIterator mit = elem.FindMember("area");

                        if (mit != elem.MemberEnd()) {
                            area = (*mit).value.GetInt();
                        } else {
                            std::cout << "Configuration Error: Population \"" << model->id << "\" has a recruitment/area/season definition that does not provide an area id\n";
                            mas_log << "Configuration Error: Population \"" << model->id << "\" has a recruitment/area/season definition that does not provide an area id\n";
                            this->valid_configuration = false;
                        }

                        mit = elem.FindMember("season");

                        if (mit != elem.MemberEnd()) {
                            season = (*mit).value.GetInt();
                        } else {
                            std::cout << "Configuration Error: Population \"" << model->id << "\" has a recruitment/area/season definition that does not provide an season id\n";
                            mas_log << "Configuration Error: Population \"" << model->id << "\" has a recruitment/area/season definition that does not provide an season id\n";
                            this->valid_configuration = false;
                        }

                        rapidjson::Document::MemberIterator vit = elem.FindMember("id");

                        if (vit != elem.MemberEnd()) {
                            id = (*vit).value.GetInt();
                        } else {
                            std::cout << "Configuration Error: Population \"" << model->id << "\" has a recruitment/area/season definition that does not provide a recruitment model id\n";
                            mas_log << "Configuration Error: Population \"" << model->id << "\" has a recruitment/area/season definition that does not provide a recruitment model id\n";
                            this->valid_configuration = false;
                        }

                        mit = elem.FindMember("ensemble");
                        if (mit != elem.MemberEnd()) {
                            if (!(*mit).value.IsArray()) {
                                std::cout << "Expected array for population recruitment ensemble unit, ignored.\n";
                                mas_log << "Expected array for population recruitment ensemble unit, ignored.\n";
                            } else {
                                EnsembleUnit eunit;
                                eunit.type = mas::RECRUITMENT;
                                rapidjson::Value& e = (*mit).value;
                                eunit.val = vit;
                                for (int i = 0; i < e.Size(); i++) {
                                    eunit.units.push_back(e[i].GetInt());
                                }
                                this->ensemble_units.push_back(eunit);

                            }
                        }


                        model->recruitment_ids[area] = id;
                        model->area_season_recruitment_ids[area][season] = id;
                        model->season_area_recruitment_ids[season][area] = id;

                    }

                }
            } else {
                std::cout << "Configuration Error: Population \"" << model->id << "\" has no recruitment model definitions.\n";
                mas_log << "Configuration Error: Population \"" << model->id << "\" has no recruitment model definitions.\n";

            }

            population_iterator it = this->populations.find(model->id);
            if (it != this->populations.end()) {
                std::cout << "Configuration Error: More than one population with the same identifier defined. Populations require a unique id.\n";
                mas::mas_log << "Configuration Error: More than one population with the same identifier defined. Populations require a unique id.\n";

                this->valid_configuration = false;
                ;
            } else {
                this->populations[model->id] = model;
            }

        }

        void HandleAreaModel(rapidjson::Document::MemberIterator & area_model) {
            INFO_DEBUG
            std::shared_ptr<mas::Area<REAL_T> > model(new mas::Area<REAL_T>());


            rapidjson::Document::MemberIterator rit = (*area_model).value.FindMember("id");
            int model_id = 0;
            if (rit == (*area_model).value.MemberEnd()) {
                std::cout << "Configuration Error: Area is required to have a unique identifier\n";
                mas::mas_log << "Configuration Error: Area is required to have a unique identifier\n";
                this->valid_configuration = false;
                ;
            } else {
                model_id = (*rit).value.GetInt();
            }

            rit = (*area_model).value.FindMember("name");
            if (rit != (*area_model).value.MemberEnd()) {
                model->name = std::string((*rit).value.GetString());
            } else {
                model->name = "NA";
            }



            model->id = model_id;
            area_iterator it = this->areas.find(model->id);
            if (it != this->areas.end()) {
                std::cout << "Configuration Error: More than one area with the same identifier defined. Areas require a unique id.\n";
                mas::mas_log << "Configuration Error: More than one area with the same identifier defined. Areas require a unique id.\n";

                this->valid_configuration = false;
                ;
            } else {
                this->areas[model->id] = model;
            }


        }

        void HandleSelectivityModel(rapidjson::Document::MemberIterator & selectivity_model) {
            INFO_DEBUG
            rapidjson::Document::MemberIterator rit;
            rit = (*selectivity_model).value.FindMember("model");

            if (rit == (*selectivity_model).value.MemberEnd()) {
                std::cout << "Configuration Error: Selectivity is required to have a model specified.\n";
                mas::mas_log << "Configuration Error: Selectivity is required to have a model specified.\n";
                this->valid_configuration = false;
            }


            std::shared_ptr<mas::SelectivityBase<REAL_T> > model(NULL);
            std::string smodel = std::string((*rit).value.GetString());

            rit = (*selectivity_model).value.FindMember("id");
            int model_id = 0;
            if (rit == (*selectivity_model).value.MemberEnd()) {
                std::cout << "Configuration Error: Selectivity is required to have a unique identifier\n";
                mas::mas_log << "Configuration Error: Selectivity is required to have a unique identifier\n";
                this->valid_configuration = false;
                ;
            } else {
                model_id = (*rit).value.GetInt();
            }

            rit = (*selectivity_model).value.FindMember("sigma");
            REAL_T sigma = 0.5;
            if (rit == (*selectivity_model).value.MemberEnd()) {
                std::cout << "Configuration Warning: No \"sigma\" defined for selectivity model \"" << model_id << "\". Default is set to 0.5.\n";
                mas::mas_log << "Configuration Warning: No \"sigma\" defined for selectivity model \"" << model_id << "\". Default is set to 0.5.\n";

            } else {
                sigma = (*rit).value.GetDouble();
            }

            rapidjson::Document::MemberIterator pit = (*selectivity_model).value.FindMember("parameters");

            if (smodel == std::string("logistic")) {

                model = std::make_shared<mas::LogisticSel<REAL_T> >();
                mas::LogisticSel<REAL_T>* l = (mas::LogisticSel<REAL_T>*)model.get();

                std::stringstream ss;
                ss << "logistic_selectivity_a50_" << model_id;
                VariableTrait<REAL_T>::SetName(l->a50, ss.str());
                ss.str("");
                ss << "logistic_selectivity_s_" << model_id;
                VariableTrait<REAL_T>::SetName(l->s, ss.str());

                if (pit == (*selectivity_model).value.MemberEnd()) {
                    std::cout << "Configuration Error: Selectivity model \"Logistic\" has no parameter definitions.\n";
                    mas::mas_log << "Configuration Error: Recruitment model \"Logistic\" has no parameter definitions.\n";
                    this->valid_configuration = false;
                    ;
                } else {

                    rapidjson::Document::MemberIterator ppit = (*pit).value.FindMember("a50");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Selectivity model \"Logistic\" " <<
                                "does not define \"a50\". Model will use the default value of 0 and \"a50\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Selectivity model \"Logistic\" " <<
                                "does not define \"a50\". Model will use the default value of 0 and \"a50\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Selectivity model \"Logistic\" does not provide a initial value for \"a50\".\n";
                            mas::mas_log << "Configuration Warning: Selectivity model \"Logistic\" does not provide a initial value for \"a50\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(l->a50, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(l->a50, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(l->a50, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }





                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }
                        INFO_DEBUG
                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            l->Register(l->a50, phase);

                            REAL_T lambda = 1.0;
                            pm = (*ppit).value.FindMember("lambda");
                            if (pm != (*ppit).value.MemberEnd()) {
                                lambda = (*pm).value.GetDouble();
                            }
                            l->lambdas.push_back(lambda);

                        }

                    }
                    INFO_DEBUG
                    ppit = (*pit).value.FindMember("s");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Selectivity model \"Logistic\" " <<
                                "does not define \"s\". Model will use the default value of 0 and \"s\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Selectivity model \"Logistic\" " <<
                                "does not define \"s\". Model will use the default value of 0 and \"s\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Selectivity model \"Logistic\" does not provide a initial value for \"s\".\n";
                            mas::mas_log << "Configuration Warning: Selectivity model \"Logistic\" does not provide a initial value for \"s\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(l->s, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(l->s, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(l->s, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            l->Register(l->s, phase);

                            REAL_T lambda = 1.0;
                            pm = (*ppit).value.FindMember("lambda");
                            if (pm != (*ppit).value.MemberEnd()) {
                                lambda = (*pm).value.GetDouble();
                            }
                            l->lambdas.push_back(lambda);

                        }

                    }



                }


            } else if (smodel == std::string("double_logistic")) {
                model = std::make_shared<mas::DoubleLogisticSel<REAL_T> >();
                mas::DoubleLogisticSel<REAL_T>* l = (mas::DoubleLogisticSel<REAL_T>*)model.get();

                std::stringstream ss;
                ss << "double_logistic_selectivity_alpha_asc_" << model_id;
                VariableTrait<REAL_T>::SetName(l->alpha_asc, ss.str());

                ss.str("");
                ss << "double_logistic_selectivity_beta_asc_" << model_id;
                VariableTrait<REAL_T>::SetName(l->beta_asc, ss.str());

                ss.str("");
                ss << "double_logistic_selectivity_alpha_desc_" << model_id;
                VariableTrait<REAL_T>::SetName(l->alpha_desc, ss.str());

                ss.str("");
                ss << "double_logistic_selectivity_beta_desc_" << model_id;
                VariableTrait<REAL_T>::SetName(l->beta_desc, ss.str());

                if (pit == (*selectivity_model).value.MemberEnd()) {
                    std::cout << "Configuration Error: Selectivity model \"Logistic\" has no parameter definitions.\n";
                    mas::mas_log << "Configuration Error: Recruitment model \"Logistic\" has no parameter definitions.\n";
                    this->valid_configuration = false;
                    ;
                } else {

                    rapidjson::Document::MemberIterator ppit = (*pit).value.FindMember("alpha_asc");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Selectivity model \"Logistic\" " <<
                                "does not define \"alpha_asc\". Model will use the default value of 0 and \"alpha_asc\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Selectivity model \"Logistic\" " <<
                                "does not define \"alpha_asc\". Model will use the default value of 0 and \"alpha_asc\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Selectivity model \"Logistic\" does not provide a initial value for \"alpha_asc\".\n";
                            mas::mas_log << "Configuration Warning: Selectivity model \"Logistic\" does not provide a initial value for \"alpha_asc\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(l->alpha_asc, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(l->alpha_asc, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(l->alpha_asc, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            l->Register(l->alpha_asc, phase);

                            REAL_T lambda = 1.0;
                            pm = (*ppit).value.FindMember("lambda");
                            if (pm != (*ppit).value.MemberEnd()) {
                                lambda = (*pm).value.GetDouble();
                            }
                            l->lambdas.push_back(lambda);
                        }

                    }

                    ppit = (*pit).value.FindMember("beta_asc");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Selectivity model \"Logistic\" " <<
                                "does not define \"beta_asc\". Model will use the default value of 0 and \"beta_asc\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Selectivity model \"Logistic\" " <<
                                "does not define \"beta_asc\". Model will use the default value of 0 and \"beta_asc\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Selectivity model \"Logistic\" does not provide a initial value for \"beta_asc\".\n";
                            mas::mas_log << "Configuration Warning: Selectivity model \"Logistic\" does not provide a initial value for \"beta_asc\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(l->beta_asc, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(l->beta_asc, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(l->beta_asc, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            l->Register(l->beta_asc, phase);

                            REAL_T lambda = 1.0;
                            pm = (*ppit).value.FindMember("lambda");
                            if (pm != (*ppit).value.MemberEnd()) {
                                lambda = (*pm).value.GetDouble();
                            }
                            l->lambdas.push_back(lambda);
                        }

                    }

                    ppit = (*pit).value.FindMember("alpha_desc");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Selectivity model \"Logistic\" " <<
                                "does not define \"alpha_desc\". Model will use the default value of 0 and \"alpha_desc\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Selectivity model \"Logistic\" " <<
                                "does not define \"alpha_desc\". Model will use the default value of 0 and \"alpha_desc\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Selectivity model \"Logistic\" does not provide a initial value for \"alpha_desc\".\n";
                            mas::mas_log << "Configuration Warning: Selectivity model \"Logistic\" does not provide a initial value for \"alpha_desc\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(l->alpha_desc, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(l->alpha_desc, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(l->alpha_desc, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            l->Register(l->alpha_desc, phase);

                            REAL_T lambda = 1.0;
                            pm = (*ppit).value.FindMember("lambda");
                            if (pm != (*ppit).value.MemberEnd()) {
                                lambda = (*pm).value.GetDouble();
                            }
                            l->lambdas.push_back(lambda);
                        }

                    }

                    ppit = (*pit).value.FindMember("beta_desc");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Selectivity model \"Logistic\" " <<
                                "does not define \"beta_desc\". Model will use the default value of 0 and \"beta_desc\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Selectivity model \"Logistic\" " <<
                                "does not define \"beta_desc\". Model will use the default value of 0 and \"beta_desc\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Selectivity model \"Logistic\" does not provide a initial value for \"beta_desc\".\n";
                            mas::mas_log << "Configuration Warning: Selectivity model \"Logistic\" does not provide a initial value for \"beta_desc\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(l->beta_desc, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(l->beta_desc, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(l->beta_desc, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            l->Register(l->beta_desc, phase);

                            REAL_T lambda = 1.0;
                            pm = (*ppit).value.FindMember("lambda");
                            if (pm != (*ppit).value.MemberEnd()) {
                                lambda = (*pm).value.GetDouble();
                            }
                            l->lambdas.push_back(lambda);
                        }

                    }

                }

            } else if (smodel == std::string("gaussian-rbf")) {

                model = std::make_shared<mas::GaussianRBF<REAL_T> >();
                mas::GaussianRBF<REAL_T>* l = (mas::GaussianRBF<REAL_T>*)model.get();

                std::stringstream ss;
                ss << "gaussian_epsilon_" << model_id;
                VariableTrait<REAL_T>::SetName(l->epsilon, ss.str());

                ss.str("");
                ss << "gaussian_centroid_" << model_id;
                VariableTrait<REAL_T>::SetName(l->centroid, ss.str());
                rapidjson::Document::MemberIterator ppit = (*pit).value.FindMember("epsilon");
                if (ppit == (*pit).value.MemberEnd()) {
                    std::cout << "Configuration Warning: Selectivity model \"Gaussian RBF\" " <<
                            "does not define \"epsilon\". Model will use the default value of 0 and \"epsilon\" will not be estimated.\n";
                    mas::mas_log << "Configuration Warning: Selectivity model \"Gaussian RBF\" " <<
                            "does not define \"epsilon\". Model will use the default value of 0 and \"epsilon\" will not be estimated.\n";
                } else {

                    bool estimated = false;
                    int phase = 1;
                    //1. Get initial value if there is one.
                    rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                    if (pm == (*ppit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Selectivity model \"Gaussian RBF\" does not provide a initial value for \"epsilon\".\n";
                        mas::mas_log << "Configuration Warning: Selectivity model \"Gaussian RBF\" does not provide a initial value for \"epsilon\".\n";
                    } else {
                        VariableTrait<REAL_T>::SetValue(l->epsilon, static_cast<REAL_T> ((*pm).value.GetDouble()));
                    }

                    //2. Get min boundary if there is one.
                    pm = (*ppit).value.FindMember("min");
                    if (pm != (*ppit).value.MemberEnd()) {
                        VariableTrait<REAL_T>::SetMinBoundary(l->epsilon, static_cast<REAL_T> ((*pm).value.GetDouble()));
                    }

                    //3. Get max boundary if there is one.
                    pm = (*ppit).value.FindMember("max");
                    if (pm != (*ppit).value.MemberEnd()) {
                        VariableTrait<REAL_T>::SetMaxBoundary(l->epsilon, static_cast<REAL_T> ((*pm).value.GetDouble()));
                    }

                    pm = (*ppit).value.FindMember("estimated");
                    if (pm != (*ppit).value.MemberEnd()) {
                        std::string e = std::string((*pm).value.GetString());
                        if (e == "true") {
                            estimated = true;
                        }
                    }

                    if (estimated) {
                        phase = 1;
                        pm = (*ppit).value.FindMember("phase");
                        if (pm != (*ppit).value.MemberEnd()) {
                            phase = (*pm).value.GetInt();
                        }
                        //register eplsilon
                        l->Register(l->epsilon, phase);

                        REAL_T lambda = 1.0;
                        pm = (*ppit).value.FindMember("lambda");
                        if (pm != (*ppit).value.MemberEnd()) {
                            lambda = (*pm).value.GetDouble();
                        }
                        l->lambdas.push_back(lambda);
                    }

                }

                //                ppit = (*pit).value.FindMember("centroid");
                //                if (ppit == (*pit).value.MemberEnd()) {
                //                    std::cout << "Configuration Warning: Selectivity model \"Gaussian RBF\" " <<
                //                            "does not define \"centroid\". Model will use the default value of 0 and \"epsilon\" will not be estimated.\n";
                //                    mas::mas_log << "Configuration Warning: Selectivity model \"Gaussian RBF\" " <<
                //                            "does not define \"centroid\". Model will use the default value of 0 and \"epsilon\" will not be estimated.\n";
                //                } else {
                //
                //                    bool estimated = false;
                //                    int phase = 1;
                //                    //1. Get initial value if there is one.
                //                    rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");
                //
                //                    if (pm == (*ppit).value.MemberEnd()) {
                //                        std::cout << "Configuration Warning: Selectivity model \"Gaussian RBF\" does not provide a initial value for \"centroid\".\n";
                //                        mas::mas_log << "Configuration Warning: Selectivity model \"Gaussian RBF\" does not provide a initial value for \"centroid\".\n";
                //                    } else {
                //                        std::cout<<"setting centroid "<<(*pm).value.GetDouble()<<"\n";
                //                        VariableTrait<REAL_T>::SetValue(l->centroid, static_cast<REAL_T> ((*pm).value.GetDouble()));
                //                    }
                //
                //                    //2. Get min boundary if there is one.
                //                    pm = (*ppit).value.FindMember("min");
                //                    if (pm != (*ppit).value.MemberEnd()) {
                //                        VariableTrait<REAL_T>::SetMinBoundary(l->centroid, static_cast<REAL_T> ((*pm).value.GetDouble()));
                //                    }
                //
                //                    //3. Get max boundary if there is one.
                //                    pm = (*ppit).value.FindMember("max");
                //                    if (pm != (*ppit).value.MemberEnd()) {
                //                        VariableTrait<REAL_T>::SetMaxBoundary(l->centroid, static_cast<REAL_T> ((*pm).value.GetDouble()));
                //                    }
                //
                //                    pm = (*ppit).value.FindMember("estimated");
                //                    if (pm != (*ppit).value.MemberEnd()) {
                //                        std::string e = std::string((*pm).value.GetString());
                //                        if (e == "true") {
                //                            estimated = true;
                //                        }
                //                    }
                //
                //                    if (estimated) {
                //                        phase = 1;
                //                        pm = (*ppit).value.FindMember("phase");
                //                        if (pm != (*ppit).value.MemberEnd()) {
                //                            phase = (*pm).value.GetInt();
                //                        }
                //                        //register centroid
                //                        l->Register(l->centroid, phase);
                //
                //                    }
                //
                //                }


                ppit = (*pit).value.FindMember("w");
                if (ppit == (*pit).value.MemberEnd()) {
                    std::cout << "Configuration Warning: Selectivity model \"Gaussian RBF\" " <<
                            "does not define \"w\". Model will use the default value of 0 and \"w\" will not be estimated.\n";
                    mas::mas_log << "Configuration Warning: Selectivity model \"Gaussian  RBF\" " <<
                            "does not define \"w\". Model will use the default value of 0 and \"w\" will not be estimated.\n";
                } else {

                    bool estimated = false;
                    int phase = 1;
                    //1. Get initial value if there is one.
                    rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("values");

                    if (pm == (*ppit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Selectivity model \"Gaussian RBF\" does not provide a initial values for vector \"w\".\n";
                        mas::mas_log << "Configuration Warning: Selectivity model \"Gaussian RBF\" does not provide a initial values for vector \"w\".\n";
                    } else {

                        if ((*pm).value.IsArray()) {
                            rapidjson::Value& v = (*pm).value;
                            l->w.resize(v.Size());
                            for (int i = 0; i < v.Size(); i++) {
                                std::stringstream ss;
                                ss << "gaussian_selectivity_w[" << i << "]_" << model_id;
                                l->w[i].SetName(ss.str());
                                mas::VariableTrait<REAL_T>::SetValue(l->w[i], static_cast<REAL_T> (v[i].GetDouble()));
                            }
                        } else {
                            std::cout << "Configuration Error: \"w\" is expected to be an array in \"Gaussian RBF\" definition. \n";
                            mas::mas_log << "Configuration Error: \"w\" is expected to be an array in \"Gaussian RBF\" definition. \n";
                            this->valid_configuration = false;
                        }

                    }

                    //2. Get min boundary if there is one.
                    pm = (*ppit).value.FindMember("min");
                    if (pm != (*ppit).value.MemberEnd()) {

                        if ((*pm).value.IsArray()) {
                            rapidjson::Value& v = (*pm).value;
                            for (int i = 0; i < v.Size(); i++) {
                                mas::VariableTrait<REAL_T>::SetMinBoundary(l->w[i], static_cast<REAL_T> (v[i].GetDouble()));
                            }
                        }

                    }

                    //3. Get max boundary if there is one.
                    pm = (*ppit).value.FindMember("max");
                    if (pm != (*ppit).value.MemberEnd()) {
                        if ((*pm).value.IsArray()) {
                            rapidjson::Value& v = (*pm).value;
                            for (int i = 0; i < v.Size(); i++) {
                                mas::VariableTrait<REAL_T>::SetMaxBoundary(l->w[i], static_cast<REAL_T> (v[i].GetDouble()));
                            }
                        }
                    }

                    pm = (*ppit).value.FindMember("estimated");
                    if (pm != (*ppit).value.MemberEnd()) {
                        std::string e = std::string((*pm).value.GetString());
                        if (e == "true") {
                            estimated = true;
                        }
                    }

                    if (estimated) {
                        phase = 1;
                        pm = (*ppit).value.FindMember("phase");
                        if (pm != (*ppit).value.MemberEnd()) {
                            phase = (*pm).value.GetInt();
                        }
                        for (int i = 0; i < l->w.size(); i++) {
                            //register alpha
                            l->Register(l->w[i], phase);
                        }

                        pm = (*ppit).value.FindMember("lambdas");
                        if (pm != (*ppit).value.MemberEnd()) {
                            if ((*pm).value.IsArray()) {
                                rapidjson::Value& v = (*pm).value;
                                for (int i = 0; i < v.Size(); i++) {
                                    l->lambdas.push_back(v[i].GetDouble());
                                }
                            }
                        } else {

                            for (int i = 0; i < l->w.size(); i++) {
                                l->lambdas.push_back(1.0);
                            }
                        }

                    }

                }

            } else if (smodel == std::string("inverse_quadratic-rbf")) {

                model = std::make_shared<mas::InverseQuadraticRBF<REAL_T> >();
                mas::InverseQuadraticRBF<REAL_T>* l = (mas::InverseQuadraticRBF<REAL_T>*)model.get();

                std::stringstream ss;
                ss << "inv_quad_epsilon_" << model_id;
                VariableTrait<REAL_T>::SetName(l->epsilon, ss.str());

                ss.str("");
                ss << "inv_quad_centroid_" << model_id;
                VariableTrait<REAL_T>::SetName(l->centroid, ss.str());

                rapidjson::Document::MemberIterator ppit = (*pit).value.FindMember("epsilon");
                if (ppit == (*pit).value.MemberEnd()) {
                    std::cout << "Configuration Warning: Selectivity model \"Inverse Quadratic RBF\" " <<
                            "does not define \"epsilon\". Model will use the default value of 0 and \"epsilon\" will not be estimated.\n";
                    mas::mas_log << "Configuration Warning: Selectivity model \"Inverse Quadratic  RBF\" " <<
                            "does not define \"epsilon\". Model will use the default value of 0 and \"epsilon\" will not be estimated.\n";
                } else {

                    bool estimated = false;
                    int phase = 1;
                    //1. Get initial value if there is one.
                    rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                    if (pm == (*ppit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Selectivity model \"Inverse Quadratic  RBF\" does not provide a initial value for \"epsilon\".\n";
                        mas::mas_log << "Configuration Warning: Selectivity model \"Inverse Quadratic  RBF\" does not provide a initial value for \"epsilon\".\n";
                    } else {
                        VariableTrait<REAL_T>::SetValue(l->epsilon, static_cast<REAL_T> ((*pm).value.GetDouble()));
                    }

                    //2. Get min boundary if there is one.
                    pm = (*ppit).value.FindMember("min");
                    if (pm != (*ppit).value.MemberEnd()) {

                        VariableTrait<REAL_T>::SetMinBoundary(l->epsilon, static_cast<REAL_T> ((*pm).value.GetDouble()));
                    }

                    //3. Get max boundary if there is one.
                    pm = (*ppit).value.FindMember("max");
                    if (pm != (*ppit).value.MemberEnd()) {
                        VariableTrait<REAL_T>::SetMaxBoundary(l->epsilon, static_cast<REAL_T> ((*pm).value.GetDouble()));
                    }

                    pm = (*ppit).value.FindMember("estimated");
                    if (pm != (*ppit).value.MemberEnd()) {
                        std::string e = std::string((*pm).value.GetString());
                        if (e == "true") {
                            estimated = true;
                        }
                    }

                    if (estimated) {
                        phase = 1;
                        pm = (*ppit).value.FindMember("phase");
                        if (pm != (*ppit).value.MemberEnd()) {
                            phase = (*pm).value.GetInt();
                        }
                        //register alpha
                        l->Register(l->epsilon, phase);

                        REAL_T lambda = 1.0;
                        pm = (*ppit).value.FindMember("lambda");
                        if (pm != (*ppit).value.MemberEnd()) {
                            lambda = (*pm).value.GetDouble();
                        }
                        l->lambdas.push_back(lambda);

                    }

                }

                //
                //                ppit = (*pit).value.FindMember("centroid");
                //                if (ppit == (*pit).value.MemberEnd()) {
                //                    std::cout << "Configuration Warning: Selectivity model \"Gaussian RBF\" " <<
                //                            "does not define \"centroid\". Model will use the default value of 0 and \"epsilon\" will not be estimated.\n";
                //                    mas::mas_log << "Configuration Warning: Selectivity model \"Gaussian RBF\" " <<
                //                            "does not define \"centroid\". Model will use the default value of 0 and \"epsilon\" will not be estimated.\n";
                //                } else {
                //
                //                    bool estimated = false;
                //                    int phase = 1;
                //                    //1. Get initial value if there is one.
                //                    rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");
                //
                //                    if (pm == (*ppit).value.MemberEnd()) {
                //                        std::cout << "Configuration Warning: Selectivity model \"Gaussian RBF\" does not provide a initial value for \"centroid\".\n";
                //                        mas::mas_log << "Configuration Warning: Selectivity model \"Gaussian RBF\" does not provide a initial value for \"centroid\".\n";
                //                    } else {
                //                        VariableTrait<REAL_T>::SetValue(l->centroid, static_cast<REAL_T> ((*pm).value.GetDouble()));
                //                    }
                //
                //                    //2. Get min boundary if there is one.
                //                    pm = (*ppit).value.FindMember("min");
                //                    if (pm != (*ppit).value.MemberEnd()) {
                //                        VariableTrait<REAL_T>::SetMinBoundary(l->centroid, static_cast<REAL_T> ((*pm).value.GetDouble()));
                //                    }
                //
                //                    //3. Get max boundary if there is one.
                //                    pm = (*ppit).value.FindMember("max");
                //                    if (pm != (*ppit).value.MemberEnd()) {
                //                        VariableTrait<REAL_T>::SetMaxBoundary(l->centroid, static_cast<REAL_T> ((*pm).value.GetDouble()));
                //                    }
                //
                //                    pm = (*ppit).value.FindMember("estimated");
                //                    if (pm != (*ppit).value.MemberEnd()) {
                //                        std::string e = std::string((*pm).value.GetString());
                //                        if (e == "true") {
                //                            estimated = true;
                //                        }
                //                    }
                //
                //                    if (estimated) {
                //                        phase = 1;
                //                        pm = (*ppit).value.FindMember("phase");
                //                        if (pm != (*ppit).value.MemberEnd()) {
                //                            phase = (*pm).value.GetInt();
                //                        }
                //                        //register centroid
                //                        l->Register(l->centroid, phase);
                //
                //                    }
                //
                //                }


                ppit = (*pit).value.FindMember("w");
                if (ppit == (*pit).value.MemberEnd()) {
                    std::cout << "Configuration Warning: Selectivity model \"Inverse Quadratic RBF\" " <<
                            "does not define \"w\". Model will use the default value of 0 and \"epsilon\" will not be estimated.\n";
                    mas::mas_log << "Configuration Warning: Selectivity model \"Inverse Quadratic  RBF\" " <<
                            "does not define \"w\". Model will use the default value of 0 and \"w\" will not be estimated.\n";
                } else {

                    bool estimated = false;
                    int phase = 1;
                    //1. Get initial value if there is one.
                    rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("values");

                    if (pm == (*ppit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Selectivity model \"Inverse Quadratic  RBF\" does not provide a initial values for vector \"w\".\n";
                        mas::mas_log << "Configuration Warning: Selectivity model \"Inverse Quadratic  RBF\" does not provide a initial values for vector \"w\".\n";
                    } else {

                        if ((*pm).value.IsArray()) {
                            rapidjson::Value& v = (*pm).value;
                            l->w.resize(v.Size());
                            for (int i = 0; i < v.Size(); i++) {
                                std::stringstream ss;
                                ss << "inv_quad_selectivity_w[" << i << "]_" << model_id;
                                l->w[i].SetName(ss.str());
                                mas::VariableTrait<REAL_T>::SetValue(l->w[i], static_cast<REAL_T> (v[i].GetDouble()));
                            }
                        }

                    }

                    //2. Get min boundary if there is one.
                    pm = (*ppit).value.FindMember("min");
                    if (pm != (*ppit).value.MemberEnd()) {

                        if ((*pm).value.IsArray()) {
                            rapidjson::Value& v = (*pm).value;
                            for (int i = 0; i < v.Size(); i++) {
                                mas::VariableTrait<REAL_T>::SetMinBoundary(l->w[i], static_cast<REAL_T> (v[i].GetDouble()));
                            }
                        }

                    }

                    //3. Get max boundary if there is one.
                    pm = (*ppit).value.FindMember("max");
                    if (pm != (*ppit).value.MemberEnd()) {
                        if ((*pm).value.IsArray()) {
                            rapidjson::Value& v = (*pm).value;
                            for (int i = 0; i < v.Size(); i++) {
                                mas::VariableTrait<REAL_T>::SetMaxBoundary(l->w[i], static_cast<REAL_T> (v[i].GetDouble()));
                            }
                        }
                    }

                    pm = (*ppit).value.FindMember("estimated");
                    if (pm != (*ppit).value.MemberEnd()) {
                        std::string e = std::string((*pm).value.GetString());
                        if (e == "true") {
                            estimated = true;
                        }
                    }

                    if (estimated) {
                        phase = 1;
                        pm = (*ppit).value.FindMember("phase");
                        if (pm != (*ppit).value.MemberEnd()) {
                            phase = (*pm).value.GetInt();
                        }
                        for (int i = 0; i < l->w.size(); i++) {
                            //register alpha
                            l->Register(l->w[i], phase);
                        }


                        pm = (*ppit).value.FindMember("lambdas");
                        if (pm != (*ppit).value.MemberEnd()) {
                            if ((*pm).value.IsArray()) {
                                rapidjson::Value& v = (*pm).value;
                                for (int i = 0; i < v.Size(); i++) {
                                    l->lambdas.push_back(v[i].GetDouble());
                                }
                            }
                        } else {

                            for (int i = 0; i < l->w.size(); i++) {
                                l->lambdas.push_back(1.0);
                            }
                        }


                    }

                }

            } else if (smodel == std::string("age_based")) {

                model = std::make_shared<mas::AgeBased<REAL_T> >();
                mas::AgeBased<REAL_T>* l = (mas::AgeBased<REAL_T>*)model.get();

                std::stringstream ss;


                rapidjson::Document::MemberIterator ppit = (*pit).value.FindMember("s");
                if (ppit == (*pit).value.MemberEnd()) {
                    std::cout << "Configuration Warning: Selectivity model \"Age Based\" " <<
                            "does not define \"s\". Model will use the default value of 0.\n";
                    mas::mas_log << "Configuration Warning: Selectivity model \"Age Based\" " <<
                            "does not define \"s\". Model will use the default value of 0.\n";
                } else {

                    bool estimated = false;
                    int phase = 1;
                    //1. Get initial value if there is one.
                    rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("values");

                    if (pm == (*ppit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Selectivity model \"Age Based\" does not provide a initial values for vector \"s\".\n";
                        mas::mas_log << "Configuration Warning: Selectivity model \"Age Based\" does not provide a initial values for vector \"s\".\n";
                    } else {

                        if ((*pm).value.IsArray()) {
                            rapidjson::Value& v = (*pm).value;
                            l->w.resize(v.Size());
                            for (int i = 0; i < v.Size(); i++) {
                                std::stringstream ss;
                                ss << "age_based_s[" << i << "]_" << model_id;
                                l->w[i].SetName(ss.str());
                                mas::VariableTrait<REAL_T>::SetValue(l->w[i], static_cast<REAL_T> (v[i].GetDouble()));
                            }
                        }

                    }

                    //2. Get min boundary if there is one.
                    pm = (*ppit).value.FindMember("min");
                    if (pm != (*ppit).value.MemberEnd()) {

                        if ((*pm).value.IsArray()) {
                            rapidjson::Value& v = (*pm).value;
                            for (int i = 0; i < v.Size(); i++) {
                                mas::VariableTrait<REAL_T>::SetMinBoundary(l->w[i], static_cast<REAL_T> (v[i].GetDouble()));
                            }
                        }

                    }

                    //3. Get max boundary if there is one.
                    pm = (*ppit).value.FindMember("max");
                    if (pm != (*ppit).value.MemberEnd()) {
                        if ((*pm).value.IsArray()) {
                            rapidjson::Value& v = (*pm).value;
                            for (int i = 0; i < v.Size(); i++) {
                                mas::VariableTrait<REAL_T>::SetMaxBoundary(l->w[i], static_cast<REAL_T> (v[i].GetDouble()));
                            }
                        }
                    }

                    pm = (*ppit).value.FindMember("estimated");
                    if (pm != (*ppit).value.MemberEnd()) {
                        std::string e = std::string((*pm).value.GetString());
                        if (e == "true") {
                            estimated = true;
                        }
                    }

                    if (estimated) {
                        phase = 1;
                        pm = (*ppit).value.FindMember("phase");
                        if (pm != (*ppit).value.MemberEnd()) {
                            phase = (*pm).value.GetInt();
                        }
                        for (int i = 0; i < l->w.size(); i++) {
                            //register alpha
                            l->Register(l->w[i], phase);
                        }


                        pm = (*ppit).value.FindMember("lambdas");
                        if (pm != (*ppit).value.MemberEnd()) {
                            if ((*pm).value.IsArray()) {
                                rapidjson::Value& v = (*pm).value;
                                for (int i = 0; i < v.Size(); i++) {
                                    l->lambdas.push_back(v[i].GetDouble());
                                }
                            }
                        } else {

                            for (int i = 0; i < l->w.size(); i++) {
                                l->lambdas.push_back(1.0);
                            }
                        }


                    }

                }

            }




            model->id = model_id;
            model->sigma = sigma;
            model->sigma2 = sigma*sigma;
            selectivity_model_iterator it = this->selectivity_models.find(model->id);
            if (it != this->selectivity_models.end()) {
                std::cout << "Configuration Error: More than one selectivity model with the same identifier defined. Selectivity models require a unique id.\n";
                mas::mas_log << "Configuration Error: More than one selectivity model with the same identifier defined. Selectivity models require a unique id.\n";
                this->valid_configuration = false;
                ;
            } else {
                this->selectivity_models[model->id] = model;
            }


        }

        void HandleGrowthModel(rapidjson::Document::MemberIterator & growth_model) {
            INFO_DEBUG

            std::shared_ptr<mas::WeightFunctorBase<REAL_T> > weight_functor(NULL);
            rapidjson::Document::MemberIterator rit;
            rit = (*growth_model).value.FindMember("model");

            if (rit == (*growth_model).value.MemberEnd()) {
                std::cout << "Configuration Error: Growth is required to have a model specified.\n";
                mas::mas_log << "Configuration Error: Growth is required to have a model specified.\n";
                this->valid_configuration = false;
            }


            std::shared_ptr<mas::GrowthBase<REAL_T> > model(NULL);
            std::string smodel = std::string((*rit).value.GetString());

            rit = (*growth_model).value.FindMember("id");
            int model_id = 0;
            if (rit == (*growth_model).value.MemberEnd()) {
                std::cout << "Configuration Error: Growth is required to have a unique identifier\n";
                mas::mas_log << "Configuration Error: Growth is required to have a unique identifier\n";
                this->valid_configuration = false;

            } else {
                model_id = (*rit).value.GetInt();
            }







            rapidjson::Document::MemberIterator pit = (*growth_model).value.FindMember("parameters");



            if (smodel == std::string("von_bertalanffy_modified")) {
                model = std::make_shared<mas::VonBertalanffyModified<REAL_T> >();
                mas::VonBertalanffyModified<REAL_T>* vb = (mas::VonBertalanffyModified<REAL_T>*)model.get();

                std::stringstream ss;
                ss << "von_bertalanffy_modified_c_" << model_id;
                VariableTrait<REAL_T>::SetName(vb->c, ss.str());
                ss.str("");
                ss << "von_bertalanffy_modified_l_inf_" << model_id;
                VariableTrait<REAL_T>::SetName(vb->l_inf, ss.str());

                ss.str("");
                ss << "von_bertalanffy_modified_amax_" << model_id;
                VariableTrait<REAL_T>::SetName(vb->a_max, ss.str());

                ss.str("");
                ss << "von_bertalanffy_modified_amin_" << model_id;
                VariableTrait<REAL_T>::SetName(vb->a_min, ss.str());

                ss.str("");
                ss << "von_bertalanffy_modified_alpha_m_" << model_id;
                VariableTrait<REAL_T>::SetName(vb->alpha_m, ss.str());

                ss.str("");
                ss << "von_bertalanffy_modified_beta_m_" << model_id;
                VariableTrait<REAL_T>::SetName(vb->beta_m, ss.str());

                ss.str("");
                ss << "von_bertalanffy_modified_alpha_f_" << model_id;
                VariableTrait<REAL_T>::SetName(vb->alpha_f, ss.str());

                ss.str("");
                ss << "von_bertalanffy_modified_beta_f_" << model_id;
                VariableTrait<REAL_T>::SetName(vb->beta_f, ss.str());


                if (pit == (*growth_model).value.MemberEnd()) {
                    std::cout << "Configuration Error: Growth model \"Von Bertalannfy Modified\" has no parameter definitions.\n";
                    mas::mas_log << "Configuration Error: Growth model \"Von Bertalannfy\" has no parameter definitions.\n";
                    this->valid_configuration = false;

                } else {
                    rapidjson::Document::MemberIterator ppit = (*pit).value.FindMember("c");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Growth model \"Von Bertalannfy Modified\" " <<
                                "does not define \"c\". Model will use the default value of 0 and \"c\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Von Bertalannfy Modified\" " <<
                                "does not define \"c\". Model will use the default value of 0 and \"c\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Von Bertalannfy Modified\" does not provide a initial value for \"c\".\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Von Bertalannfy Modified\" does not provide a initial value for \"c\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(vb->c, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(vb->c, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(vb->c, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            vb->Register(vb->c, phase);

                        }

                    }


                    ppit = (*pit).value.FindMember("alpha_m");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Growth model \"Von Bertalannfy Modified\" " <<
                                "does not define \"alpha_m\". Model will use the default value of 0.000025 and \"alpha_m\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Growth model \"Von Bertalannfy Modified\" " <<
                                "does not define \"alpha_m\". Model will use the default value of 0.000025 and \"alpha_m\" will not be estimated.\n";
                    } else {

                        ss.str("");
                        ss << "von_bertalanffy_alpha_m_" << model_id;
                        VariableTrait<REAL_T>::SetName(vb->alpha_m, ss.str());
                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Von Bertalannfy Modified\" does not provide a initial value for \"alpha_m\".\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Von Bertalannfy Modified\" does not provide a initial value for \"alpha_m\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(vb->alpha_m, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(vb->alpha_m, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(vb->alpha_m, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            vb->Register(vb->alpha_m, phase);

                        }

                    }

                    ppit = (*pit).value.FindMember("alpha_f");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Growth model \"Von Bertalannfy Modified\" " <<
                                "does not define \"alpha_m\". Model will use the default value of 0.000025 and \"alpha_f\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Growth model \"Von Bertalannfy Modified\" " <<
                                "does not define \"alpha_f\". Model will use the default value of 0.000025 and \"alpha_f\" will not be estimated.\n";
                    } else {

                        ss.str("");
                        ss << "von_bertalanffy_alpha_f_" << model_id;
                        VariableTrait<REAL_T>::SetName(vb->alpha_f, ss.str());
                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Von Bertalannfy Modified\" does not provide a initial value for \"alpha_f\".\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Von Bertalannfy Modified\" does not provide a initial value for \"alpha_f\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(vb->alpha_f, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(vb->alpha_f, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(vb->alpha_f, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            vb->Register(vb->alpha_f, phase);

                        }

                    }

                    ppit = (*pit).value.FindMember("beta_m");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Growth model \"Von Bertalannfy Modified\" " <<
                                "does not define \"alpha_m\". Model will use the default value of 3.0 and \"beta_m\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Growth model \"Von Bertalannfy Modified\" " <<
                                "does not define \"beta_m\". Model will use the default value of 3.0 and \"beta_m\" will not be estimated.\n";
                    } else {

                        ss.str("");
                        ss << "von_bertalanffy_beta_m_" << model_id;
                        VariableTrait<REAL_T>::SetName(vb->beta_m, ss.str());
                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Von Bertalannfy Modified\" does not provide a initial value for \"beta_m\".\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Von Bertalannfy Modified\" does not provide a initial value for \"beta_m\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(vb->beta_m, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(vb->beta_m, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(vb->beta_m, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            vb->Register(vb->beta_m, phase);

                        }

                    }

                    ppit = (*pit).value.FindMember("beta_f");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Growth model \"Von Bertalannfy Modified\" " <<
                                "does not define \"alpha_m\". Model will use the default value of 3.0 and \"beta_f\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Growth model \"Von Bertalannfy Modified\" " <<
                                "does not define \"beta_f\". Model will use the default value of 3.0 and \"beta_f\" will not be estimated.\n";
                    } else {

                        ss.str("");
                        ss << "von_bertalanffy_beta_f_" << model_id;
                        VariableTrait<REAL_T>::SetName(vb->beta_f, ss.str());
                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Von Bertalannfy Modified\" does not provide a initial value for \"beta_f\".\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Von Bertalannfy Modified\" does not provide a initial value for \"beta_f\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(vb->beta_f, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(vb->beta_f, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(vb->beta_f, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            vb->Register(vb->beta_f, phase);

                        }

                    }


                    ppit = (*pit).value.FindMember("lmin");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Growth model \"Von Bertalannfy Modified\" " <<
                                "does not define \"lmin\". Model will use the default value of 0 and \"lmin\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Growth model \"Von Bertalannfy Modified\" " <<
                                "does not define \"lmin\". Model will use the default value of 0 and \"lmin\" will not be estimated.\n";
                    } else {

                        ss.str("");
                        ss << "von_bertalanffy_lmin_" << model_id;
                        VariableTrait<REAL_T>::SetName(vb->lmin, ss.str());
                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Von Bertalannfy Modified\" does not provide a initial value for \"lmin\".\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Von Bertalannfy Modified\" does not provide a initial value for \"lmin\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(vb->lmin, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(vb->lmin, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(vb->lmin, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            vb->Register(vb->lmin, phase);

                        }

                    }

                    ppit = (*pit).value.FindMember("lmax");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Growth model \"Von Bertalannfy Modified\" " <<
                                "does not define \"lmax\". Model will use the default value of 0 and \"lmax\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Growth model \"Von Bertalannfy Modified\" " <<
                                "does not define \"lmax\". Model will use the default value of 0 and \"lmax\" will not be estimated.\n";
                    } else {

                        ss.str("");
                        ss << "von_bertalanffy_lmax_" << model_id;
                        VariableTrait<REAL_T>::SetName(vb->lmax, ss.str());

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Von Bertalannfy Modified\" does not provide a initial value for \"lmax\".\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Von Bertalannfy Modified\" does not provide a initial value for \"lmax\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(vb->lmax, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(vb->lmax, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(vb->lmax, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            vb->Register(vb->lmax, phase);

                        }

                    }


                    // CHANGE THIS:  the modified von Bertalannfy model should calculate l_inf given the other parameters
                    ppit = (*pit).value.FindMember("linf");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Growth model \"Von Bertalannfy Modified\" " <<
                                "does not define \"l_inf\". Model will use the default value of 0 and \"l_inf\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Von Bertalannfy Modified\" " <<
                                "does not define \"l_inf\". Model will use the default value of 0 and \"l_inf\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Von Bertalannfy Modified\" does not provide a initial value for \"l_inf\".\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Von Bertalannfy Modified\" does not provide a initial value for \"l_inf\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(vb->l_inf, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(vb->l_inf, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(vb->l_inf, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            vb->Register(vb->l_inf, phase);

                        }

                    }

                }

            } else if (smodel == std::string("von_bertalanffy")) {
                model = std::make_shared<mas::VonBertalanffy<REAL_T> >();
                mas::VonBertalanffy<REAL_T>* vb = (mas::VonBertalanffy<REAL_T>*)model.get();

                std::stringstream ss;
                ss << "von_bertalanffy_k_" << model_id;
                VariableTrait<REAL_T>::SetName(vb->k, ss.str());
                ss.str("");
                ss << "von_bertalanffy_linf_" << model_id;
                VariableTrait<REAL_T>::SetName(vb->l_inf, ss.str());

                ss.str("");
                ss << "von_bertalanffy_amax_" << model_id;
                VariableTrait<REAL_T>::SetName(vb->a_max, ss.str());

                ss.str("");
                ss << "von_bertalanffy_amin_" << model_id;
                VariableTrait<REAL_T>::SetName(vb->a_min, ss.str());

                ss.str("");
                ss << "von_bertalanffy_alpha_m_" << model_id;
                VariableTrait<REAL_T>::SetName(vb->alpha_m, ss.str());

                ss.str("");
                ss << "von_bertalanffy_beta_m_" << model_id;
                VariableTrait<REAL_T>::SetName(vb->beta_m, ss.str());

                ss.str("");
                ss << "von_bertalanffy_alpha_f_" << model_id;
                VariableTrait<REAL_T>::SetName(vb->alpha_f, ss.str());

                ss.str("");
                ss << "von_bertalanffy_beta_f_" << model_id;
                VariableTrait<REAL_T>::SetName(vb->beta_f, ss.str());


                if (pit == (*growth_model).value.MemberEnd()) {
                    std::cout << "Configuration Error: Recruitment model \"Beverton-Holt\" has no parameter definitions.\n";
                    mas::mas_log << "Configuration Error: Recruitment model \"Beverton-Holt\" has no parameter definitions.\n";
                    this->valid_configuration = false;

                } else {
                    rapidjson::Document::MemberIterator ppit = (*pit).value.FindMember("k");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Growth model \"Von Bertalannfy\" " <<
                                "does not define \"k\". Model will use the default value of 0 and \"k\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Von Bertalannfy\" " <<
                                "does not define \"k\". Model will use the default value of 0 and \"k\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Von Bertalannfy\" does not provide a initial value for \"k\".\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Von Bertalannfy\" does not provide a initial value for \"k\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(vb->k, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(vb->k, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(vb->k, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            vb->Register(vb->k, phase);

                        }

                    }


                    ppit = (*pit).value.FindMember("alpha_m");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Growth model \"Von Bertalannfy\" " <<
                                "does not define \"alpha_m\". Model will use the default value of 0.000025 and \"alpha_m\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Growth model \"Von Bertalannfy\" " <<
                                "does not define \"alpha_m\". Model will use the default value of 0.000025 and \"alpha_m\" will not be estimated.\n";
                    } else {

                        ss.str("");
                        ss << "von_bertalanffy_alpha_m_" << model_id;
                        VariableTrait<REAL_T>::SetName(vb->alpha_m, ss.str());
                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Von Bertalannfy\" does not provide a initial value for \"alpha_m\".\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Von Bertalannfy\" does not provide a initial value for \"alpha_m\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(vb->alpha_m, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(vb->alpha_m, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(vb->alpha_m, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            vb->Register(vb->alpha_m, phase);

                        }

                    }

                    ppit = (*pit).value.FindMember("alpha_f");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Growth model \"Von Bertalannfy\" " <<
                                "does not define \"alpha_m\". Model will use the default value of 0.000025 and \"alpha_f\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Growth model \"Von Bertalannfy\" " <<
                                "does not define \"alpha_f\". Model will use the default value of 0.000025 and \"alpha_f\" will not be estimated.\n";
                    } else {

                        ss.str("");
                        ss << "von_bertalanffy_alpha_f_" << model_id;
                        VariableTrait<REAL_T>::SetName(vb->alpha_f, ss.str());
                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Von Bertalannfy\" does not provide a initial value for \"alpha_f\".\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Von Bertalannfy\" does not provide a initial value for \"alpha_f\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(vb->alpha_f, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(vb->alpha_f, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(vb->alpha_f, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            vb->Register(vb->alpha_f, phase);

                        }

                    }

                    ppit = (*pit).value.FindMember("beta_m");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Growth model \"Von Bertalannfy\" " <<
                                "does not define \"alpha_m\". Model will use the default value of 3.0 and \"beta_m\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Growth model \"Von Bertalannfy\" " <<
                                "does not define \"beta_m\". Model will use the default value of 3.0 and \"beta_m\" will not be estimated.\n";
                    } else {

                        ss.str("");
                        ss << "von_bertalanffy_beta_m_" << model_id;
                        VariableTrait<REAL_T>::SetName(vb->beta_m, ss.str());
                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Von Bertalanffy\" does not provide a initial value for \"beta_m\".\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Von Bertalanffy\" does not provide a initial value for \"beta_m\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(vb->beta_m, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(vb->beta_m, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(vb->beta_m, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            vb->Register(vb->beta_m, phase);

                        }

                    }

                    ppit = (*pit).value.FindMember("beta_f");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Growth model \"Von Bertalannfy\" " <<
                                "does not define \"alpha_m\". Model will use the default value of 3.0 and \"beta_f\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Growth model \"Von Bertalannfy\" " <<
                                "does not define \"beta_f\". Model will use the default value of 3.0 and \"beta_f\" will not be estimated.\n";
                    } else {

                        ss.str("");
                        ss << "von_bertalanffy_beta_f_" << model_id;
                        VariableTrait<REAL_T>::SetName(vb->beta_f, ss.str());
                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Von Bertalannfy\" does not provide a initial value for \"beta_f\".\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Von Bertalannfy\" does not provide a initial value for \"beta_f\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(vb->beta_f, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(vb->beta_f, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(vb->beta_f, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            vb->Register(vb->beta_f, phase);

                        }

                    }


                    ppit = (*pit).value.FindMember("linf");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Growth model \"Von Bertalannfy\" " <<
                                "does not define \"l_inf\". Model will use the default value of 0 and \"l_inf\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Von Bertalannfy\" " <<
                                "does not define \"l_inf\". Model will use the default value of 0 and \"l_inf\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Von Bertalannfy\" does not provide a initial value for \"l_inf\".\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Von Bertalannfy\" does not provide a initial value for \"l_inf\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(vb->l_inf, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(vb->l_inf, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(vb->l_inf, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            vb->Register(vb->l_inf, phase);

                        }

                    }

                }

            } else if (smodel == std::string("schnute_case_I")) {

                model = std::make_shared<mas::SchnuteCaseI<REAL_T> >();
                mas::SchnuteCaseI<REAL_T>* s = (mas::SchnuteCaseI<REAL_T>*)model.get();

                std::stringstream ss;
                ss << "schnute_case_I_alpha_" << model_id;
                VariableTrait<REAL_T>::SetName(s->alpha, ss.str());

                ss.str("");
                ss << "schnute_case_I_beta_" << model_id;
                VariableTrait<REAL_T>::SetName(s->beta, ss.str());

                ss.str("");
                ss << "schnute_case_I_lmin_" << model_id;
                VariableTrait<REAL_T>::SetName(s->lmin, ss.str());

                ss.str("");
                ss << "schnute_case_I_lmax_" << model_id;
                VariableTrait<REAL_T>::SetName(s->lmax, ss.str());

                ss.str("");
                ss << "schnute_case_I_alpha_m_" << model_id;
                VariableTrait<REAL_T>::SetName(s->alpha_m, ss.str());

                ss.str("");
                ss << "schnute_case_I_beta_m_" << model_id;
                VariableTrait<REAL_T>::SetName(s->beta_m, ss.str());

                ss.str("");
                ss << "schnute_case_I_alpha_f_" << model_id;
                VariableTrait<REAL_T>::SetName(s->alpha_f, ss.str());

                ss.str("");
                ss << "schnute_case_I_beta_f_" << model_id;
                VariableTrait<REAL_T>::SetName(s->beta_f, ss.str());



                if (pit == (*growth_model).value.MemberEnd()) {
                    std::cout << "Configuration Error: Recruitment model \"Schnute Case I\" has no parameter definitions.\n";
                    mas::mas_log << "Configuration Error: Recruitment model \"Schnute Case I\" has no parameter definitions.\n";
                    this->valid_configuration = false;

                } else {
                    rapidjson::Document::MemberIterator ppit = (*pit).value.FindMember("alpha");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Growth model \"Schnute Case I\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Schnute Case I\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case I\" does not provide a initial value for \"alpha\".\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Schnute Case I\" does not provide a initial value for \"alpha\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(s->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(s->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(s->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            s->Register(s->alpha, phase);

                        }

                        ppit = (*pit).value.FindMember("alpha_m");
                        if (ppit == (*pit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case I\"  " <<
                                    "does not define \"alpha_m\". Model will use the default value of 0.000025 and \"alpha_m\" will not be estimated.\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Schnute Case I\"  " <<
                                    "does not define \"alpha_m\". Model will use the default value of 0.000025 and \"alpha_m\" will not be estimated.\n";
                        } else {

                            ss.str("");
                            ss << "von_bertalanffy_alpha_m_" << model_id;
                            VariableTrait<REAL_T>::SetName(s->alpha_m, ss.str());
                            bool estimated = false;
                            int phase = 1;
                            //1. Get initial value if there is one.
                            rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                            if (pm == (*ppit).value.MemberEnd()) {
                                std::cout << "Configuration Warning: Growth model \"Schnute Case I\"  does not provide a initial value for \"alpha_m\".\n";
                                mas::mas_log << "Configuration Warning: Growth model \"Schnute Case I\"  does not provide a initial value for \"alpha_m\".\n";
                            } else {
                                VariableTrait<REAL_T>::SetValue(s->alpha_m, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //2. Get min boundary if there is one.
                            pm = (*ppit).value.FindMember("min");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMinBoundary(s->alpha_m, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //3. Get max boundary if there is one.
                            pm = (*ppit).value.FindMember("max");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMaxBoundary(s->alpha_m, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            pm = (*ppit).value.FindMember("estimated");
                            if (pm != (*ppit).value.MemberEnd()) {
                                std::string e = std::string((*pm).value.GetString());
                                if (e == "true") {
                                    estimated = true;
                                }
                            }

                            if (estimated) {
                                phase = 1;
                                pm = (*ppit).value.FindMember("phase");
                                if (pm != (*ppit).value.MemberEnd()) {
                                    phase = (*pm).value.GetInt();
                                }
                                //register alpha
                                s->Register(s->alpha_m, phase);

                            }

                        }

                        ppit = (*pit).value.FindMember("alpha_f");
                        if (ppit == (*pit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case I\"  " <<
                                    "does not define \"alpha_m\". Model will use the default value of 0.000025 and \"alpha_f\" will not be estimated.\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Schnute Case I\"  " <<
                                    "does not define \"alpha_f\". Model will use the default value of 0.000025 and \"alpha_f\" will not be estimated.\n";
                        } else {

                            ss.str("");
                            ss << "von_bertalanffy_alpha_f_" << model_id;
                            VariableTrait<REAL_T>::SetName(s->alpha_f, ss.str());
                            bool estimated = false;
                            int phase = 1;
                            //1. Get initial value if there is one.
                            rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                            if (pm == (*ppit).value.MemberEnd()) {
                                std::cout << "Configuration Warning: Growth model \"Schnute Case I\"  does not provide a initial value for \"alpha_f\".\n";
                                mas::mas_log << "Configuration Warning: Growth model \"Schnute Case I\"  does not provide a initial value for \"alpha_f\".\n";
                            } else {
                                VariableTrait<REAL_T>::SetValue(s->alpha_f, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //2. Get min boundary if there is one.
                            pm = (*ppit).value.FindMember("min");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMinBoundary(s->alpha_f, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //3. Get max boundary if there is one.
                            pm = (*ppit).value.FindMember("max");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMaxBoundary(s->alpha_f, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            pm = (*ppit).value.FindMember("estimated");
                            if (pm != (*ppit).value.MemberEnd()) {
                                std::string e = std::string((*pm).value.GetString());
                                if (e == "true") {
                                    estimated = true;
                                }
                            }

                            if (estimated) {
                                phase = 1;
                                pm = (*ppit).value.FindMember("phase");
                                if (pm != (*ppit).value.MemberEnd()) {
                                    phase = (*pm).value.GetInt();
                                }
                                //register alpha
                                s->Register(s->alpha_f, phase);

                            }

                        }

                        ppit = (*pit).value.FindMember("beta_m");
                        if (ppit == (*pit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case I\"  " <<
                                    "does not define \"alpha_m\". Model will use the default value of 3.0 and \"beta_m\" will not be estimated.\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Schnute Case I\"  " <<
                                    "does not define \"beta_m\". Model will use the default value of 3.0 and \"beta_m\" will not be estimated.\n";
                        } else {

                            ss.str("");
                            ss << "von_bertalanffy_beta_m_" << model_id;
                            VariableTrait<REAL_T>::SetName(s->beta_m, ss.str());
                            bool estimated = false;
                            int phase = 1;
                            //1. Get initial value if there is one.
                            rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                            if (pm == (*ppit).value.MemberEnd()) {
                                std::cout << "Configuration Warning: Growth model \"Schnute Case I\" does not provide a initial value for \"beta_m\".\n";
                                mas::mas_log << "Configuration Warning: Growth model \"Schnute Case I\"  does not provide a initial value for \"beta_m\".\n";
                            } else {
                                VariableTrait<REAL_T>::SetValue(s->beta_m, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //2. Get min boundary if there is one.
                            pm = (*ppit).value.FindMember("min");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMinBoundary(s->beta_m, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //3. Get max boundary if there is one.
                            pm = (*ppit).value.FindMember("max");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMaxBoundary(s->beta_m, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            pm = (*ppit).value.FindMember("estimated");
                            if (pm != (*ppit).value.MemberEnd()) {
                                std::string e = std::string((*pm).value.GetString());
                                if (e == "true") {
                                    estimated = true;
                                }
                            }

                            if (estimated) {
                                phase = 1;
                                pm = (*ppit).value.FindMember("phase");
                                if (pm != (*ppit).value.MemberEnd()) {
                                    phase = (*pm).value.GetInt();
                                }
                                //register alpha
                                s->Register(s->beta_m, phase);

                            }

                        }

                        ppit = (*pit).value.FindMember("beta_f");
                        if (ppit == (*pit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case I\"  " <<
                                    "does not define \"alpha_m\". Model will use the default value of 3.0 and \"beta_f\" will not be estimated.\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Schnute Case I\"  " <<
                                    "does not define \"beta_f\". Model will use the default value of 3.0 and \"beta_f\" will not be estimated.\n";
                        } else {

                            ss.str("");
                            ss << "von_bertalanffy_beta_f_" << model_id;
                            VariableTrait<REAL_T>::SetName(s->beta_f, ss.str());
                            bool estimated = false;
                            int phase = 1;
                            //1. Get initial value if there is one.
                            rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                            if (pm == (*ppit).value.MemberEnd()) {
                                std::cout << "Configuration Warning: Growth model \"Schnute Case I\"  does not provide a initial value for \"beta_f\".\n";
                                mas::mas_log << "Configuration Warning: Growth model \"Schnute Case I\"  does not provide a initial value for \"beta_f\".\n";
                            } else {
                                VariableTrait<REAL_T>::SetValue(s->beta_f, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //2. Get min boundary if there is one.
                            pm = (*ppit).value.FindMember("min");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMinBoundary(s->beta_f, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //3. Get max boundary if there is one.
                            pm = (*ppit).value.FindMember("max");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMaxBoundary(s->beta_f, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            pm = (*ppit).value.FindMember("estimated");
                            if (pm != (*ppit).value.MemberEnd()) {
                                std::string e = std::string((*pm).value.GetString());
                                if (e == "true") {
                                    estimated = true;
                                }
                            }

                            if (estimated) {
                                phase = 1;
                                pm = (*ppit).value.FindMember("phase");
                                if (pm != (*ppit).value.MemberEnd()) {
                                    phase = (*pm).value.GetInt();
                                }
                                //register alpha
                                s->Register(s->beta_f, phase);

                            }

                        }



                        ppit = (*pit).value.FindMember("beta");
                        if (ppit == (*pit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case I\" " <<
                                    "does not define \"beta\". Model will use the default value of 0 and \"beta\" will not be estimated.\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Schnute Case I\" " <<
                                    "does not define \"beta\". Model will use the default value of 0 and \"beta\" will not be estimated.\n";
                        } else {

                            bool estimated = false;
                            int phase = 1;
                            //1. Get initial value if there is one.
                            rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                            if (pm == (*ppit).value.MemberEnd()) {
                                std::cout << "Configuration Warning: Growth model \"Schnute Case I\" does not provide a initial value for \"beta\".\n";
                                mas::mas_log << "Configuration Warning: Growth model \"Schnute Case I\" does not provide a initial value for \"beta\".\n";
                            } else {
                                VariableTrait<REAL_T>::SetValue(s->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //2. Get min boundary if there is one.
                            pm = (*ppit).value.FindMember("min");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMinBoundary(s->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //3. Get max boundary if there is one.
                            pm = (*ppit).value.FindMember("max");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMaxBoundary(s->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            pm = (*ppit).value.FindMember("estimated");
                            if (pm != (*ppit).value.MemberEnd()) {
                                std::string e = std::string((*pm).value.GetString());
                                if (e == "true") {
                                    estimated = true;
                                }
                            }

                            if (estimated) {
                                phase = 1;
                                pm = (*ppit).value.FindMember("phase");
                                if (pm != (*ppit).value.MemberEnd()) {
                                    phase = (*pm).value.GetInt();
                                }
                                //register alpha
                                s->Register(s->beta, phase);

                            }

                        }

                        ppit = (*pit).value.FindMember("lmin");
                        if (ppit == (*pit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case I\" " <<
                                    "does not define \"lmin\". Model will use the default value of 0 and \"lmin\" will not be estimated.\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Schnute Case I\" " <<
                                    "does not define \"lmin\". Model will use the default value of 0 and \"lmin\" will not be estimated.\n";
                        } else {

                            bool estimated = false;
                            int phase = 1;
                            //1. Get initial value if there is one.
                            rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                            if (pm == (*ppit).value.MemberEnd()) {
                                std::cout << "Configuration Warning: Growth model \"Schnute Case I\" does not provide a initial value for \"lmin\".\n";
                                mas::mas_log << "Configuration Warning: Growth model \"Schnute Case I\" does not provide a initial value for \"lmin\".\n";
                            } else {
                                VariableTrait<REAL_T>::SetValue(s->lmin, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //2. Get min boundary if there is one.
                            pm = (*ppit).value.FindMember("min");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMinBoundary(s->lmin, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //3. Get max boundary if there is one.
                            pm = (*ppit).value.FindMember("max");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMaxBoundary(s->lmin, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            pm = (*ppit).value.FindMember("estimated");
                            if (pm != (*ppit).value.MemberEnd()) {
                                std::string e = std::string((*pm).value.GetString());
                                if (e == "true") {
                                    estimated = true;
                                }
                            }

                            if (estimated) {
                                phase = 1;
                                pm = (*ppit).value.FindMember("phase");
                                if (pm != (*ppit).value.MemberEnd()) {
                                    phase = (*pm).value.GetInt();
                                }
                                //register alpha
                                s->Register(s->lmin, phase);

                            }

                        }

                        ppit = (*pit).value.FindMember("lmax");
                        if (ppit == (*pit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case I\" " <<
                                    "does not define \"lmax\". Model will use the default value of 0 and \"lmax\" will not be estimated.\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Schnute Case I\" " <<
                                    "does not define \"lmax\". Model will use the default value of 0 and \"lmax\" will not be estimated.\n";
                        } else {

                            bool estimated = false;
                            int phase = 1;
                            //1. Get initial value if there is one.
                            rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                            if (pm == (*ppit).value.MemberEnd()) {
                                std::cout << "Configuration Warning: Growth model \"Schnute Case I\" does not provide a initial value for \"lmax\".\n";
                                mas::mas_log << "Configuration Warning: Growth model \"Schnute Case I\" does not provide a initial value for \"lmax\".\n";
                            } else {
                                VariableTrait<REAL_T>::SetValue(s->lmax, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //2. Get min boundary if there is one.
                            pm = (*ppit).value.FindMember("min");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMinBoundary(s->lmax, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //3. Get max boundary if there is one.
                            pm = (*ppit).value.FindMember("max");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMaxBoundary(s->lmax, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            pm = (*ppit).value.FindMember("estimated");
                            if (pm != (*ppit).value.MemberEnd()) {
                                std::string e = std::string((*pm).value.GetString());
                                if (e == "true") {
                                    estimated = true;
                                }
                            }

                            if (estimated) {
                                phase = 1;
                                pm = (*ppit).value.FindMember("phase");
                                if (pm != (*ppit).value.MemberEnd()) {
                                    phase = (*pm).value.GetInt();
                                }
                                //register alpha
                                s->Register(s->lmax, phase);

                            }

                        }
                    }
                }

            } else if (smodel == std::string("schnute_case_II")) {
                model = std::make_shared<mas::SchnuteCaseII<REAL_T> >();
                mas::SchnuteCaseII<REAL_T>* s = (mas::SchnuteCaseII<REAL_T>*)model.get();

                std::stringstream ss;
                ss << "schnute_case_II_alpha_" << model_id;
                VariableTrait<REAL_T>::SetName(s->alpha, ss.str());

                ss.str("");
                ss << "schnute_case_II_lmin_" << model_id;
                VariableTrait<REAL_T>::SetName(s->lmin, ss.str());

                ss.str("");
                ss << "schnute_case_II_lmax_" << model_id;
                VariableTrait<REAL_T>::SetName(s->lmax, ss.str());

                ss.str("");
                ss << "schnute_case_II_alpha_m_" << model_id;
                VariableTrait<REAL_T>::SetName(s->alpha_m, ss.str());

                ss.str("");
                ss << "schnute_case_II_beta_m_" << model_id;
                VariableTrait<REAL_T>::SetName(s->beta_m, ss.str());

                ss.str("");
                ss << "schnute_case_II_alpha_f_" << model_id;
                VariableTrait<REAL_T>::SetName(s->alpha_f, ss.str());

                ss.str("");
                ss << "schnute_case_II_beta_f_" << model_id;
                VariableTrait<REAL_T>::SetName(s->beta_f, ss.str());



                if (pit == (*growth_model).value.MemberEnd()) {
                    std::cout << "Configuration Error: Recruitment model \"Schnute Case II\" has no parameter definitions.\n";
                    mas::mas_log << "Configuration Error: Recruitment model \"Schnute Case II\" has no parameter definitions.\n";
                    this->valid_configuration = false;
                    ;
                } else {
                    rapidjson::Document::MemberIterator ppit = (*pit).value.FindMember("alpha");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Growth model \"Schnute Case II\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Schnute Case II\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case II\" does not provide a initial value for \"alpha\".\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Schnute Case II\" does not provide a initial value for \"alpha\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(s->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(s->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(s->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            s->Register(s->alpha, phase);

                        }

                        ppit = (*pit).value.FindMember("alpha_m");
                        if (ppit == (*pit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case II\"  " <<
                                    "does not define \"alpha_m\". Model will use the default value of 0.000025 and \"alpha_m\" will not be estimated.\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Schnute Case II\"  " <<
                                    "does not define \"alpha_m\". Model will use the default value of 0.000025 and \"alpha_m\" will not be estimated.\n";
                        } else {

                            ss.str("");
                            ss << "von_bertalanffy_alpha_m_" << model_id;
                            VariableTrait<REAL_T>::SetName(s->alpha_m, ss.str());
                            bool estimated = false;
                            int phase = 1;
                            //1. Get initial value if there is one.
                            rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                            if (pm == (*ppit).value.MemberEnd()) {
                                std::cout << "Configuration Warning: Growth model \"Schnute Case II\"  does not provide a initial value for \"alpha_m\".\n";
                                mas::mas_log << "Configuration Warning: Growth model \"Schnute Case II\"  does not provide a initial value for \"alpha_m\".\n";
                            } else {
                                VariableTrait<REAL_T>::SetValue(s->alpha_m, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //2. Get min boundary if there is one.
                            pm = (*ppit).value.FindMember("min");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMinBoundary(s->alpha_m, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //3. Get max boundary if there is one.
                            pm = (*ppit).value.FindMember("max");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMaxBoundary(s->alpha_m, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            pm = (*ppit).value.FindMember("estimated");
                            if (pm != (*ppit).value.MemberEnd()) {
                                std::string e = std::string((*pm).value.GetString());
                                if (e == "true") {
                                    estimated = true;
                                }
                            }

                            if (estimated) {
                                phase = 1;
                                pm = (*ppit).value.FindMember("phase");
                                if (pm != (*ppit).value.MemberEnd()) {
                                    phase = (*pm).value.GetInt();
                                }
                                //register alpha
                                s->Register(s->alpha_m, phase);

                            }

                        }

                        ppit = (*pit).value.FindMember("alpha_f");
                        if (ppit == (*pit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case II\"  " <<
                                    "does not define \"alpha_m\". Model will use the default value of 0.000025 and \"alpha_f\" will not be estimated.\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Schnute Case II\"  " <<
                                    "does not define \"alpha_f\". Model will use the default value of 0.000025 and \"alpha_f\" will not be estimated.\n";
                        } else {

                            ss.str("");
                            ss << "von_bertalanffy_alpha_f_" << model_id;
                            VariableTrait<REAL_T>::SetName(s->alpha_f, ss.str());
                            bool estimated = false;
                            int phase = 1;
                            //1. Get initial value if there is one.
                            rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                            if (pm == (*ppit).value.MemberEnd()) {
                                std::cout << "Configuration Warning: Growth model \"Schnute Case II\"  does not provide a initial value for \"alpha_f\".\n";
                                mas::mas_log << "Configuration Warning: Growth model \"Schnute Case II\"  does not provide a initial value for \"alpha_f\".\n";
                            } else {
                                VariableTrait<REAL_T>::SetValue(s->alpha_f, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //2. Get min boundary if there is one.
                            pm = (*ppit).value.FindMember("min");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMinBoundary(s->alpha_f, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //3. Get max boundary if there is one.
                            pm = (*ppit).value.FindMember("max");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMaxBoundary(s->alpha_f, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            pm = (*ppit).value.FindMember("estimated");
                            if (pm != (*ppit).value.MemberEnd()) {
                                std::string e = std::string((*pm).value.GetString());
                                if (e == "true") {
                                    estimated = true;
                                }
                            }

                            if (estimated) {
                                phase = 1;
                                pm = (*ppit).value.FindMember("phase");
                                if (pm != (*ppit).value.MemberEnd()) {
                                    phase = (*pm).value.GetInt();
                                }
                                //register alpha
                                s->Register(s->alpha_f, phase);

                            }

                        }

                        ppit = (*pit).value.FindMember("beta_m");
                        if (ppit == (*pit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case II\"  " <<
                                    "does not define \"alpha_m\". Model will use the default value of 3.0 and \"beta_m\" will not be estimated.\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Schnute Case II\"  " <<
                                    "does not define \"beta_m\". Model will use the default value of 3.0 and \"beta_m\" will not be estimated.\n";
                        } else {

                            ss.str("");
                            ss << "von_bertalanffy_beta_m_" << model_id;
                            VariableTrait<REAL_T>::SetName(s->beta_m, ss.str());
                            bool estimated = false;
                            int phase = 1;
                            //1. Get initial value if there is one.
                            rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                            if (pm == (*ppit).value.MemberEnd()) {
                                std::cout << "Configuration Warning: Growth model \"Schnute Case II\" does not provide a initial value for \"beta_m\".\n";
                                mas::mas_log << "Configuration Warning: Growth model \"Schnute Case II\"  does not provide a initial value for \"beta_m\".\n";
                            } else {
                                VariableTrait<REAL_T>::SetValue(s->beta_m, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //2. Get min boundary if there is one.
                            pm = (*ppit).value.FindMember("min");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMinBoundary(s->beta_m, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //3. Get max boundary if there is one.
                            pm = (*ppit).value.FindMember("max");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMaxBoundary(s->beta_m, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            pm = (*ppit).value.FindMember("estimated");
                            if (pm != (*ppit).value.MemberEnd()) {
                                std::string e = std::string((*pm).value.GetString());
                                if (e == "true") {
                                    estimated = true;
                                }
                            }

                            if (estimated) {
                                phase = 1;
                                pm = (*ppit).value.FindMember("phase");
                                if (pm != (*ppit).value.MemberEnd()) {
                                    phase = (*pm).value.GetInt();
                                }
                                //register alpha
                                s->Register(s->beta_m, phase);

                            }

                        }

                        ppit = (*pit).value.FindMember("beta_f");
                        if (ppit == (*pit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case II\"  " <<
                                    "does not define \"alpha_m\". Model will use the default value of 3.0 and \"beta_f\" will not be estimated.\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Schnute Case II\"  " <<
                                    "does not define \"beta_f\". Model will use the default value of 3.0 and \"beta_f\" will not be estimated.\n";
                        } else {

                            ss.str("");
                            ss << "von_bertalanffy_beta_f_" << model_id;
                            VariableTrait<REAL_T>::SetName(s->beta_f, ss.str());
                            bool estimated = false;
                            int phase = 1;
                            //1. Get initial value if there is one.
                            rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                            if (pm == (*ppit).value.MemberEnd()) {
                                std::cout << "Configuration Warning: Growth model \"Schnute Case II\"  does not provide a initial value for \"beta_f\".\n";
                                mas::mas_log << "Configuration Warning: Growth model \"Schnute Case II\"  does not provide a initial value for \"beta_f\".\n";
                            } else {
                                VariableTrait<REAL_T>::SetValue(s->beta_f, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //2. Get min boundary if there is one.
                            pm = (*ppit).value.FindMember("min");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMinBoundary(s->beta_f, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //3. Get max boundary if there is one.
                            pm = (*ppit).value.FindMember("max");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMaxBoundary(s->beta_f, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            pm = (*ppit).value.FindMember("estimated");
                            if (pm != (*ppit).value.MemberEnd()) {
                                std::string e = std::string((*pm).value.GetString());
                                if (e == "true") {
                                    estimated = true;
                                }
                            }

                            if (estimated) {
                                phase = 1;
                                pm = (*ppit).value.FindMember("phase");
                                if (pm != (*ppit).value.MemberEnd()) {
                                    phase = (*pm).value.GetInt();
                                }
                                //register alpha
                                s->Register(s->beta_f, phase);

                            }

                        }


                        ppit = (*pit).value.FindMember("lmin");
                        if (ppit == (*pit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case II\" " <<
                                    "does not define \"lmin\". Model will use the default value of 0 and \"lmin\" will not be estimated.\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Schnute Case II\" " <<
                                    "does not define \"lmin\". Model will use the default value of 0 and \"lmin\" will not be estimated.\n";
                        } else {

                            bool estimated = false;
                            int phase = 1;
                            //1. Get initial value if there is one.
                            rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                            if (pm == (*ppit).value.MemberEnd()) {
                                std::cout << "Configuration Warning: Growth model \"Schnute Case II\" does not provide a initial value for \"lmin\".\n";
                                mas::mas_log << "Configuration Warning: Growth model \"Schnute Case II\" does not provide a initial value for \"lmin\".\n";
                            } else {
                                VariableTrait<REAL_T>::SetValue(s->lmin, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //2. Get min boundary if there is one.
                            pm = (*ppit).value.FindMember("min");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMinBoundary(s->lmin, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //3. Get max boundary if there is one.
                            pm = (*ppit).value.FindMember("max");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMaxBoundary(s->lmin, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            pm = (*ppit).value.FindMember("estimated");
                            if (pm != (*ppit).value.MemberEnd()) {
                                std::string e = std::string((*pm).value.GetString());
                                if (e == "true") {
                                    estimated = true;
                                }
                            }

                            if (estimated) {
                                phase = 1;
                                pm = (*ppit).value.FindMember("phase");
                                if (pm != (*ppit).value.MemberEnd()) {
                                    phase = (*pm).value.GetInt();
                                }
                                //register alpha
                                s->Register(s->lmin, phase);

                            }

                        }

                        ppit = (*pit).value.FindMember("lmax");
                        if (ppit == (*pit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case II\" " <<
                                    "does not define \"lmax\". Model will use the default value of 0 and \"lmax\" will not be estimated.\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Schnute Case II\" " <<
                                    "does not define \"lmax\". Model will use the default value of 0 and \"lmax\" will not be estimated.\n";
                        } else {

                            bool estimated = false;
                            int phase = 1;
                            //1. Get initial value if there is one.
                            rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                            if (pm == (*ppit).value.MemberEnd()) {
                                std::cout << "Configuration Warning: Growth model \"Schnute Case II\" does not provide a initial value for \"lmax\".\n";
                                mas::mas_log << "Configuration Warning: Growth model \"Schnute Case II\" does not provide a initial value for \"lmax\".\n";
                            } else {
                                VariableTrait<REAL_T>::SetValue(s->lmax, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //2. Get min boundary if there is one.
                            pm = (*ppit).value.FindMember("min");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMinBoundary(s->lmax, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //3. Get max boundary if there is one.
                            pm = (*ppit).value.FindMember("max");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMaxBoundary(s->lmax, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            pm = (*ppit).value.FindMember("estimated");
                            if (pm != (*ppit).value.MemberEnd()) {
                                std::string e = std::string((*pm).value.GetString());
                                if (e == "true") {
                                    estimated = true;
                                }
                            }

                            if (estimated) {
                                phase = 1;
                                pm = (*ppit).value.FindMember("phase");
                                if (pm != (*ppit).value.MemberEnd()) {
                                    phase = (*pm).value.GetInt();
                                }
                                //register alpha
                                s->Register(s->lmax, phase);

                            }

                        }
                    }
                }

            } else if (smodel == std::string("schnute_case_III")) {
                model = std::make_shared<mas::SchnuteCaseIII<REAL_T> >();
                mas::SchnuteCaseIII<REAL_T>* s = (mas::SchnuteCaseIII<REAL_T>*)model.get();

                std::stringstream ss;
                ss << "schnute_case_III_alpha_" << model_id;
                VariableTrait<REAL_T>::SetName(s->alpha, ss.str());

                ss.str("");
                ss << "schnute_case_III_beta_" << model_id;
                VariableTrait<REAL_T>::SetName(s->beta, ss.str());

                ss.str("");
                ss << "schnute_case_III_lmin_" << model_id;
                VariableTrait<REAL_T>::SetName(s->lmin, ss.str());

                ss.str("");
                ss << "schnute_case_III_lmax_" << model_id;
                VariableTrait<REAL_T>::SetName(s->lmax, ss.str());

                ss.str("");
                ss << "schnute_case_III_alpha_m_" << model_id;
                VariableTrait<REAL_T>::SetName(s->alpha_m, ss.str());

                ss.str("");
                ss << "schnute_case_III_beta_m_" << model_id;
                VariableTrait<REAL_T>::SetName(s->beta_m, ss.str());

                ss.str("");
                ss << "schnute_case_III_alpha_f_" << model_id;
                VariableTrait<REAL_T>::SetName(s->alpha_f, ss.str());

                ss.str("");
                ss << "schnute_case_III_beta_f_" << model_id;
                VariableTrait<REAL_T>::SetName(s->beta_f, ss.str());



                if (pit == (*growth_model).value.MemberEnd()) {
                    std::cout << "Configuration Error: Recruitment model \"Schnute Case III\" has no parameter definitions.\n";
                    mas::mas_log << "Configuration Error: Recruitment model \"Schnute Case III\" has no parameter definitions.\n";
                    this->valid_configuration = false;
                    ;
                } else {
                    rapidjson::Document::MemberIterator ppit = (*pit).value.FindMember("alpha");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Growth model \"Schnute Case III\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Schnute Case III\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case III\" does not provide a initial value for \"alpha\".\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Schnute Case III\" does not provide a initial value for \"alpha\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(s->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(s->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(s->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            s->Register(s->alpha, phase);

                        }

                        ppit = (*pit).value.FindMember("beta");
                        if (ppit == (*pit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case III\" " <<
                                    "does not define \"beta\". Model will use the default value of 0 and \"beta\" will not be estimated.\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Schnute Case III\" " <<
                                    "does not define \"beta\". Model will use the default value of 0 and \"beta\" will not be estimated.\n";
                        } else {

                            bool estimated = false;
                            int phase = 1;
                            //1. Get initial value if there is one.
                            rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                            if (pm == (*ppit).value.MemberEnd()) {
                                std::cout << "Configuration Warning: Growth model \"Schnute Case III\" does not provide a initial value for \"beta\".\n";
                                mas::mas_log << "Configuration Warning: Growth model \"Schnute Case III\" does not provide a initial value for \"beta\".\n";
                            } else {
                                VariableTrait<REAL_T>::SetValue(s->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //2. Get min boundary if there is one.
                            pm = (*ppit).value.FindMember("min");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMinBoundary(s->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //3. Get max boundary if there is one.
                            pm = (*ppit).value.FindMember("max");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMaxBoundary(s->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            pm = (*ppit).value.FindMember("estimated");
                            if (pm != (*ppit).value.MemberEnd()) {
                                std::string e = std::string((*pm).value.GetString());
                                if (e == "true") {
                                    estimated = true;
                                }
                            }

                            if (estimated) {
                                phase = 1;
                                pm = (*ppit).value.FindMember("phase");
                                if (pm != (*ppit).value.MemberEnd()) {
                                    phase = (*pm).value.GetInt();
                                }
                                //register alpha
                                s->Register(s->beta, phase);

                            }

                        }

                        ppit = (*pit).value.FindMember("alpha_m");
                        if (ppit == (*pit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case III\"  " <<
                                    "does not define \"alpha_m\". Model will use the default value of 0.000025 and \"alpha_m\" will not be estimated.\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Schnute Case III\"  " <<
                                    "does not define \"alpha_m\". Model will use the default value of 0.000025 and \"alpha_m\" will not be estimated.\n";
                        } else {

                            ss.str("");
                            ss << "von_bertalanffy_alpha_m_" << model_id;
                            VariableTrait<REAL_T>::SetName(s->alpha_m, ss.str());
                            bool estimated = false;
                            int phase = 1;
                            //1. Get initial value if there is one.
                            rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                            if (pm == (*ppit).value.MemberEnd()) {
                                std::cout << "Configuration Warning: Growth model \"Schnute Case III\"  does not provide a initial value for \"alpha_m\".\n";
                                mas::mas_log << "Configuration Warning: Growth model \"Schnute Case III\"  does not provide a initial value for \"alpha_m\".\n";
                            } else {
                                VariableTrait<REAL_T>::SetValue(s->alpha_m, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //2. Get min boundary if there is one.
                            pm = (*ppit).value.FindMember("min");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMinBoundary(s->alpha_m, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //3. Get max boundary if there is one.
                            pm = (*ppit).value.FindMember("max");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMaxBoundary(s->alpha_m, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            pm = (*ppit).value.FindMember("estimated");
                            if (pm != (*ppit).value.MemberEnd()) {
                                std::string e = std::string((*pm).value.GetString());
                                if (e == "true") {
                                    estimated = true;
                                }
                            }

                            if (estimated) {
                                phase = 1;
                                pm = (*ppit).value.FindMember("phase");
                                if (pm != (*ppit).value.MemberEnd()) {
                                    phase = (*pm).value.GetInt();
                                }
                                //register alpha
                                s->Register(s->alpha_m, phase);

                            }

                        }

                        ppit = (*pit).value.FindMember("alpha_f");
                        if (ppit == (*pit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case III\"  " <<
                                    "does not define \"alpha_m\". Model will use the default value of 0.000025 and \"alpha_f\" will not be estimated.\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Schnute Case III\"  " <<
                                    "does not define \"alpha_f\". Model will use the default value of 0.000025 and \"alpha_f\" will not be estimated.\n";
                        } else {

                            ss.str("");
                            ss << "von_bertalanffy_alpha_f_" << model_id;
                            VariableTrait<REAL_T>::SetName(s->alpha_f, ss.str());
                            bool estimated = false;
                            int phase = 1;
                            //1. Get initial value if there is one.
                            rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                            if (pm == (*ppit).value.MemberEnd()) {
                                std::cout << "Configuration Warning: Growth model \"Schnute Case III\"  does not provide a initial value for \"alpha_f\".\n";
                                mas::mas_log << "Configuration Warning: Growth model \"Schnute Case III\"  does not provide a initial value for \"alpha_f\".\n";
                            } else {
                                VariableTrait<REAL_T>::SetValue(s->alpha_f, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //2. Get min boundary if there is one.
                            pm = (*ppit).value.FindMember("min");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMinBoundary(s->alpha_f, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //3. Get max boundary if there is one.
                            pm = (*ppit).value.FindMember("max");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMaxBoundary(s->alpha_f, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            pm = (*ppit).value.FindMember("estimated");
                            if (pm != (*ppit).value.MemberEnd()) {
                                std::string e = std::string((*pm).value.GetString());
                                if (e == "true") {
                                    estimated = true;
                                }
                            }

                            if (estimated) {
                                phase = 1;
                                pm = (*ppit).value.FindMember("phase");
                                if (pm != (*ppit).value.MemberEnd()) {
                                    phase = (*pm).value.GetInt();
                                }
                                //register alpha
                                s->Register(s->alpha_f, phase);

                            }

                        }

                        ppit = (*pit).value.FindMember("beta_m");
                        if (ppit == (*pit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case III\"  " <<
                                    "does not define \"alpha_m\". Model will use the default value of 3.0 and \"beta_m\" will not be estimated.\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Schnute Case III\"  " <<
                                    "does not define \"beta_m\". Model will use the default value of 3.0 and \"beta_m\" will not be estimated.\n";
                        } else {

                            ss.str("");
                            ss << "von_bertalanffy_beta_m_" << model_id;
                            VariableTrait<REAL_T>::SetName(s->beta_m, ss.str());
                            bool estimated = false;
                            int phase = 1;
                            //1. Get initial value if there is one.
                            rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                            if (pm == (*ppit).value.MemberEnd()) {
                                std::cout << "Configuration Warning: Growth model \"Schnute Case III\" does not provide a initial value for \"beta_m\".\n";
                                mas::mas_log << "Configuration Warning: Growth model \"Schnute Case III\"  does not provide a initial value for \"beta_m\".\n";
                            } else {
                                VariableTrait<REAL_T>::SetValue(s->beta_m, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //2. Get min boundary if there is one.
                            pm = (*ppit).value.FindMember("min");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMinBoundary(s->beta_m, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //3. Get max boundary if there is one.
                            pm = (*ppit).value.FindMember("max");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMaxBoundary(s->beta_m, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            pm = (*ppit).value.FindMember("estimated");
                            if (pm != (*ppit).value.MemberEnd()) {
                                std::string e = std::string((*pm).value.GetString());
                                if (e == "true") {
                                    estimated = true;
                                }
                            }

                            if (estimated) {
                                phase = 1;
                                pm = (*ppit).value.FindMember("phase");
                                if (pm != (*ppit).value.MemberEnd()) {
                                    phase = (*pm).value.GetInt();
                                }
                                //register alpha
                                s->Register(s->beta_m, phase);

                            }

                        }

                        ppit = (*pit).value.FindMember("beta_f");
                        if (ppit == (*pit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case III\"  " <<
                                    "does not define \"alpha_m\". Model will use the default value of 3.0 and \"beta_f\" will not be estimated.\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Schnute Case III\"  " <<
                                    "does not define \"beta_f\". Model will use the default value of 3.0 and \"beta_f\" will not be estimated.\n";
                        } else {

                            ss.str("");
                            ss << "von_bertalanffy_beta_f_" << model_id;
                            VariableTrait<REAL_T>::SetName(s->beta_f, ss.str());
                            bool estimated = false;
                            int phase = 1;
                            //1. Get initial value if there is one.
                            rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                            if (pm == (*ppit).value.MemberEnd()) {
                                std::cout << "Configuration Warning: Growth model \"Schnute Case III\"  does not provide a initial value for \"beta_f\".\n";
                                mas::mas_log << "Configuration Warning: Growth model \"Schnute Case III\"  does not provide a initial value for \"beta_f\".\n";
                            } else {
                                VariableTrait<REAL_T>::SetValue(s->beta_f, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //2. Get min boundary if there is one.
                            pm = (*ppit).value.FindMember("min");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMinBoundary(s->beta_f, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //3. Get max boundary if there is one.
                            pm = (*ppit).value.FindMember("max");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMaxBoundary(s->beta_f, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            pm = (*ppit).value.FindMember("estimated");
                            if (pm != (*ppit).value.MemberEnd()) {
                                std::string e = std::string((*pm).value.GetString());
                                if (e == "true") {
                                    estimated = true;
                                }
                            }

                            if (estimated) {
                                phase = 1;
                                pm = (*ppit).value.FindMember("phase");
                                if (pm != (*ppit).value.MemberEnd()) {
                                    phase = (*pm).value.GetInt();
                                }
                                //register alpha
                                s->Register(s->beta_f, phase);

                            }

                        }

                        ppit = (*pit).value.FindMember("lmin");
                        if (ppit == (*pit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case III\" " <<
                                    "does not define \"lmin\". Model will use the default value of 0 and \"lmin\" will not be estimated.\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Schnute Case III\" " <<
                                    "does not define \"lmin\". Model will use the default value of 0 and \"lmin\" will not be estimated.\n";
                        } else {

                            bool estimated = false;
                            int phase = 1;
                            //1. Get initial value if there is one.
                            rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                            if (pm == (*ppit).value.MemberEnd()) {
                                std::cout << "Configuration Warning: Growth model \"Schnute Case III\" does not provide a initial value for \"lmin\".\n";
                                mas::mas_log << "Configuration Warning: Growth model \"Schnute Case III\" does not provide a initial value for \"lmin\".\n";
                            } else {
                                VariableTrait<REAL_T>::SetValue(s->lmin, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //2. Get min boundary if there is one.
                            pm = (*ppit).value.FindMember("min");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMinBoundary(s->lmin, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //3. Get max boundary if there is one.
                            pm = (*ppit).value.FindMember("max");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMaxBoundary(s->lmin, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            pm = (*ppit).value.FindMember("estimated");
                            if (pm != (*ppit).value.MemberEnd()) {
                                std::string e = std::string((*pm).value.GetString());
                                if (e == "true") {
                                    estimated = true;
                                }
                            }

                            if (estimated) {
                                phase = 1;
                                pm = (*ppit).value.FindMember("phase");
                                if (pm != (*ppit).value.MemberEnd()) {
                                    phase = (*pm).value.GetInt();
                                }
                                //register alpha
                                s->Register(s->lmin, phase);

                            }

                        }

                        ppit = (*pit).value.FindMember("lmax");
                        if (ppit == (*pit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case III\" " <<
                                    "does not define \"lmax\". Model will use the default value of 0 and \"lmax\" will not be estimated.\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Schnute Case III\" " <<
                                    "does not define \"lmax\". Model will use the default value of 0 and \"lmax\" will not be estimated.\n";
                        } else {

                            bool estimated = false;
                            int phase = 1;
                            //1. Get initial value if there is one.
                            rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                            if (pm == (*ppit).value.MemberEnd()) {
                                std::cout << "Configuration Warning: Growth model \"Schnute Case III\" does not provide a initial value for \"lmax\".\n";
                                mas::mas_log << "Configuration Warning: Growth model \"Schnute Case III\" does not provide a initial value for \"lmax\".\n";
                            } else {
                                VariableTrait<REAL_T>::SetValue(s->lmax, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //2. Get min boundary if there is one.
                            pm = (*ppit).value.FindMember("min");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMinBoundary(s->lmax, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //3. Get max boundary if there is one.
                            pm = (*ppit).value.FindMember("max");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMaxBoundary(s->lmax, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            pm = (*ppit).value.FindMember("estimated");
                            if (pm != (*ppit).value.MemberEnd()) {
                                std::string e = std::string((*pm).value.GetString());
                                if (e == "true") {
                                    estimated = true;
                                }
                            }

                            if (estimated) {
                                phase = 1;
                                pm = (*ppit).value.FindMember("phase");
                                if (pm != (*ppit).value.MemberEnd()) {
                                    phase = (*pm).value.GetInt();
                                }
                                //register alpha
                                s->Register(s->lmax, phase);

                            }

                        }
                    }
                }


            } else if (smodel == std::string("schnute_case_IV")) {
                model = std::make_shared<mas::SchnuteCaseIV<REAL_T> >();
                mas::SchnuteCaseIV<REAL_T>* s = (mas::SchnuteCaseIV<REAL_T>*)model.get();

                std::stringstream ss;
                ss << "schnute_case_IV_alpha_" << model_id;
                VariableTrait<REAL_T>::SetName(s->alpha, ss.str());

                ss.str("");
                ss << "schnute_case_IV_beta_" << model_id;
                VariableTrait<REAL_T>::SetName(s->beta, ss.str());

                ss.str("");
                ss << "schnute_case_IV_lmin_" << model_id;
                VariableTrait<REAL_T>::SetName(s->lmin, ss.str());

                ss.str("");
                ss << "schnute_case_IV_lmax_" << model_id;
                VariableTrait<REAL_T>::SetName(s->lmax, ss.str());

                ss.str("");
                ss << "schnute_case_III_lmax_" << model_id;
                VariableTrait<REAL_T>::SetName(s->lmax, ss.str());

                ss.str("");
                ss << "schnute_case_IV_alpha_m_" << model_id;
                VariableTrait<REAL_T>::SetName(s->alpha_m, ss.str());

                ss.str("");
                ss << "schnute_case_IV_beta_m_" << model_id;
                VariableTrait<REAL_T>::SetName(s->beta_m, ss.str());

                ss.str("");
                ss << "schnute_case_IV_alpha_f_" << model_id;
                VariableTrait<REAL_T>::SetName(s->alpha_f, ss.str());

                ss.str("");
                ss << "schnute_case_IV_beta_f_" << model_id;
                VariableTrait<REAL_T>::SetName(s->beta_f, ss.str());


                if (pit == (*growth_model).value.MemberEnd()) {
                    std::cout << "Configuration Error: Recruitment model \"Schnute Case IV\" has no parameter definitions.\n";
                    mas::mas_log << "Configuration Error: Recruitment model \"Schnute Case IV\" has no parameter definitions.\n";
                    this->valid_configuration = false;
                    ;
                } else {
                    rapidjson::Document::MemberIterator ppit = (*pit).value.FindMember("alpha");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Growth model \"Schnute Case IV\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Schnute Case IV\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case IV\" does not provide a initial value for \"alpha\".\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Schnute Case IV\" does not provide a initial value for \"alpha\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(s->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(s->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(s->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            s->Register(s->alpha, phase);

                        }

                        ppit = (*pit).value.FindMember("alpha_m");
                        if (ppit == (*pit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case IV\"  " <<
                                    "does not define \"alpha_m\". Model will use the default value of 0.000025 and \"alpha_m\" will not be estimated.\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Schnute Case IV\"  " <<
                                    "does not define \"alpha_m\". Model will use the default value of 0.000025 and \"alpha_m\" will not be estimated.\n";
                        } else {

                            ss.str("");
                            ss << "von_bertalanffy_alpha_m_" << model_id;
                            VariableTrait<REAL_T>::SetName(s->alpha_m, ss.str());
                            bool estimated = false;
                            int phase = 1;
                            //1. Get initial value if there is one.
                            rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                            if (pm == (*ppit).value.MemberEnd()) {
                                std::cout << "Configuration Warning: Growth model \"Schnute Case IV\"  does not provide a initial value for \"alpha_m\".\n";
                                mas::mas_log << "Configuration Warning: Growth model \"Schnute Case IV\"  does not provide a initial value for \"alpha_m\".\n";
                            } else {
                                VariableTrait<REAL_T>::SetValue(s->alpha_m, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //2. Get min boundary if there is one.
                            pm = (*ppit).value.FindMember("min");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMinBoundary(s->alpha_m, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //3. Get max boundary if there is one.
                            pm = (*ppit).value.FindMember("max");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMaxBoundary(s->alpha_m, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            pm = (*ppit).value.FindMember("estimated");
                            if (pm != (*ppit).value.MemberEnd()) {
                                std::string e = std::string((*pm).value.GetString());
                                if (e == "true") {
                                    estimated = true;
                                }
                            }

                            if (estimated) {
                                phase = 1;
                                pm = (*ppit).value.FindMember("phase");
                                if (pm != (*ppit).value.MemberEnd()) {
                                    phase = (*pm).value.GetInt();
                                }
                                //register alpha
                                s->Register(s->alpha_m, phase);

                            }

                        }

                        ppit = (*pit).value.FindMember("alpha_f");
                        if (ppit == (*pit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case IV\"  " <<
                                    "does not define \"alpha_m\". Model will use the default value of 0.000025 and \"alpha_f\" will not be estimated.\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Schnute Case IV\"  " <<
                                    "does not define \"alpha_f\". Model will use the default value of 0.000025 and \"alpha_f\" will not be estimated.\n";
                        } else {

                            ss.str("");
                            ss << "von_bertalanffy_alpha_f_" << model_id;
                            VariableTrait<REAL_T>::SetName(s->alpha_f, ss.str());
                            bool estimated = false;
                            int phase = 1;
                            //1. Get initial value if there is one.
                            rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                            if (pm == (*ppit).value.MemberEnd()) {
                                std::cout << "Configuration Warning: Growth model \"Schnute Case IV\"  does not provide a initial value for \"alpha_f\".\n";
                                mas::mas_log << "Configuration Warning: Growth model \"Schnute Case IV\"  does not provide a initial value for \"alpha_f\".\n";
                            } else {
                                VariableTrait<REAL_T>::SetValue(s->alpha_f, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //2. Get min boundary if there is one.
                            pm = (*ppit).value.FindMember("min");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMinBoundary(s->alpha_f, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //3. Get max boundary if there is one.
                            pm = (*ppit).value.FindMember("max");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMaxBoundary(s->alpha_f, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            pm = (*ppit).value.FindMember("estimated");
                            if (pm != (*ppit).value.MemberEnd()) {
                                std::string e = std::string((*pm).value.GetString());
                                if (e == "true") {
                                    estimated = true;
                                }
                            }

                            if (estimated) {
                                phase = 1;
                                pm = (*ppit).value.FindMember("phase");
                                if (pm != (*ppit).value.MemberEnd()) {
                                    phase = (*pm).value.GetInt();
                                }
                                //register alpha
                                s->Register(s->alpha_f, phase);

                            }

                        }

                        ppit = (*pit).value.FindMember("beta_m");
                        if (ppit == (*pit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case IV\"  " <<
                                    "does not define \"alpha_m\". Model will use the default value of 3.0 and \"beta_m\" will not be estimated.\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Schnute Case IV\"  " <<
                                    "does not define \"beta_m\". Model will use the default value of 3.0 and \"beta_m\" will not be estimated.\n";
                        } else {

                            ss.str("");
                            ss << "von_bertalanffy_beta_m_" << model_id;
                            VariableTrait<REAL_T>::SetName(s->beta_m, ss.str());
                            bool estimated = false;
                            int phase = 1;
                            //1. Get initial value if there is one.
                            rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                            if (pm == (*ppit).value.MemberEnd()) {
                                std::cout << "Configuration Warning: Growth model \"Schnute Case IV\" does not provide a initial value for \"beta_m\".\n";
                                mas::mas_log << "Configuration Warning: Growth model \"Schnute Case IV\"  does not provide a initial value for \"beta_m\".\n";
                            } else {
                                VariableTrait<REAL_T>::SetValue(s->beta_m, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //2. Get min boundary if there is one.
                            pm = (*ppit).value.FindMember("min");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMinBoundary(s->beta_m, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //3. Get max boundary if there is one.
                            pm = (*ppit).value.FindMember("max");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMaxBoundary(s->beta_m, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            pm = (*ppit).value.FindMember("estimated");
                            if (pm != (*ppit).value.MemberEnd()) {
                                std::string e = std::string((*pm).value.GetString());
                                if (e == "true") {
                                    estimated = true;
                                }
                            }

                            if (estimated) {
                                phase = 1;
                                pm = (*ppit).value.FindMember("phase");
                                if (pm != (*ppit).value.MemberEnd()) {
                                    phase = (*pm).value.GetInt();
                                }
                                //register alpha
                                s->Register(s->beta_m, phase);

                            }

                        }

                        ppit = (*pit).value.FindMember("beta_f");
                        if (ppit == (*pit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case IV\"  " <<
                                    "does not define \"alpha_m\". Model will use the default value of 3.0 and \"beta_f\" will not be estimated.\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Schnute Case IV\"  " <<
                                    "does not define \"beta_f\". Model will use the default value of 3.0 and \"beta_f\" will not be estimated.\n";
                        } else {

                            ss.str("");
                            ss << "von_bertalanffy_beta_f_" << model_id;
                            VariableTrait<REAL_T>::SetName(s->beta_f, ss.str());
                            bool estimated = false;
                            int phase = 1;
                            //1. Get initial value if there is one.
                            rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                            if (pm == (*ppit).value.MemberEnd()) {
                                std::cout << "Configuration Warning: Growth model \"Schnute Case IV\"  does not provide a initial value for \"beta_f\".\n";
                                mas::mas_log << "Configuration Warning: Growth model \"Schnute Case IV\"  does not provide a initial value for \"beta_f\".\n";
                            } else {
                                VariableTrait<REAL_T>::SetValue(s->beta_f, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //2. Get min boundary if there is one.
                            pm = (*ppit).value.FindMember("min");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMinBoundary(s->beta_f, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //3. Get max boundary if there is one.
                            pm = (*ppit).value.FindMember("max");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMaxBoundary(s->beta_f, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            pm = (*ppit).value.FindMember("estimated");
                            if (pm != (*ppit).value.MemberEnd()) {
                                std::string e = std::string((*pm).value.GetString());
                                if (e == "true") {
                                    estimated = true;
                                }
                            }

                            if (estimated) {
                                phase = 1;
                                pm = (*ppit).value.FindMember("phase");
                                if (pm != (*ppit).value.MemberEnd()) {
                                    phase = (*pm).value.GetInt();
                                }
                                //register alpha
                                s->Register(s->beta_f, phase);

                            }

                        }

                        ppit = (*pit).value.FindMember("beta");
                        if (ppit == (*pit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case IV\" " <<
                                    "does not define \"beta\". Model will use the default value of 0 and \"beta\" will not be estimated.\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Schnute Case IV\" " <<
                                    "does not define \"beta\". Model will use the default value of 0 and \"beta\" will not be estimated.\n";
                        } else {

                            bool estimated = false;
                            int phase = 1;
                            //1. Get initial value if there is one.
                            rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                            if (pm == (*ppit).value.MemberEnd()) {
                                std::cout << "Configuration Warning: Growth model \"Schnute Case IV\" does not provide a initial value for \"beta\".\n";
                                mas::mas_log << "Configuration Warning: Growth model \"Schnute Case IV\" does not provide a initial value for \"beta\".\n";
                            } else {
                                VariableTrait<REAL_T>::SetValue(s->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //2. Get min boundary if there is one.
                            pm = (*ppit).value.FindMember("min");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMinBoundary(s->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //3. Get max boundary if there is one.
                            pm = (*ppit).value.FindMember("max");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMaxBoundary(s->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            pm = (*ppit).value.FindMember("estimated");
                            if (pm != (*ppit).value.MemberEnd()) {
                                std::string e = std::string((*pm).value.GetString());
                                if (e == "true") {
                                    estimated = true;
                                }
                            }

                            if (estimated) {
                                phase = 1;
                                pm = (*ppit).value.FindMember("phase");
                                if (pm != (*ppit).value.MemberEnd()) {
                                    phase = (*pm).value.GetInt();
                                }
                                //register alpha
                                s->Register(s->beta, phase);

                            }

                        }

                        ppit = (*pit).value.FindMember("lmin");
                        if (ppit == (*pit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case IV\" " <<
                                    "does not define \"lmin\". Model will use the default value of 0 and \"lmin\" will not be estimated.\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Schnute Case IV\" " <<
                                    "does not define \"lmin\". Model will use the default value of 0 and \"lmin\" will not be estimated.\n";
                        } else {

                            bool estimated = false;
                            int phase = 1;
                            //1. Get initial value if there is one.
                            rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                            if (pm == (*ppit).value.MemberEnd()) {
                                std::cout << "Configuration Warning: Growth model \"Schnute Case IV\" does not provide a initial value for \"lmin\".\n";
                                mas::mas_log << "Configuration Warning: Growth model \"Schnute Case IV\" does not provide a initial value for \"lmin\".\n";
                            } else {
                                VariableTrait<REAL_T>::SetValue(s->lmin, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //2. Get min boundary if there is one.
                            pm = (*ppit).value.FindMember("min");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMinBoundary(s->lmin, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //3. Get max boundary if there is one.
                            pm = (*ppit).value.FindMember("max");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMaxBoundary(s->lmin, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            pm = (*ppit).value.FindMember("estimated");
                            if (pm != (*ppit).value.MemberEnd()) {
                                std::string e = std::string((*pm).value.GetString());
                                if (e == "true") {
                                    estimated = true;
                                }
                            }

                            if (estimated) {
                                phase = 1;
                                pm = (*ppit).value.FindMember("phase");
                                if (pm != (*ppit).value.MemberEnd()) {
                                    phase = (*pm).value.GetInt();
                                }
                                //register alpha
                                s->Register(s->lmin, phase);

                            }

                        }

                        ppit = (*pit).value.FindMember("lmax");
                        if (ppit == (*pit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case IV\" " <<
                                    "does not define \"lmax\". Model will use the default value of 0 and \"lmax\" will not be estimated.\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Schnute Case IV\" " <<
                                    "does not define \"lmax\". Model will use the default value of 0 and \"lmax\" will not be estimated.\n";
                        } else {

                            bool estimated = false;
                            int phase = 1;
                            //1. Get initial value if there is one.
                            rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                            if (pm == (*ppit).value.MemberEnd()) {
                                std::cout << "Configuration Warning: Growth model \"Schnute Case IV\" does not provide a initial value for \"lmax\".\n";
                                mas::mas_log << "Configuration Warning: Growth model \"Schnute Case IV\" does not provide a initial value for \"lmax\".\n";
                            } else {
                                VariableTrait<REAL_T>::SetValue(s->lmax, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //2. Get min boundary if there is one.
                            pm = (*ppit).value.FindMember("min");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMinBoundary(s->lmax, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //3. Get max boundary if there is one.
                            pm = (*ppit).value.FindMember("max");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMaxBoundary(s->lmax, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            pm = (*ppit).value.FindMember("estimated");
                            if (pm != (*ppit).value.MemberEnd()) {
                                std::string e = std::string((*pm).value.GetString());
                                if (e == "true") {
                                    estimated = true;
                                }
                            }

                            if (estimated) {
                                phase = 1;
                                pm = (*ppit).value.FindMember("phase");
                                if (pm != (*ppit).value.MemberEnd()) {
                                    phase = (*pm).value.GetInt();
                                }
                                //register alpha
                                s->Register(s->lmax, phase);

                            }

                        }
                    }
                }

            }

            rapidjson::Document::MemberIterator amin_it = (*pit).value.FindMember("amin");

            //            REAL_T amin = static_cast<REAL_T> (0.0);
            //            REAL_T amax = static_cast<REAL_T> (0.0);

            if (amin_it == (*pit).value.MemberEnd()) {
                std::cout << "Configuration Error: Growth is required to have a amin specified.\n";
                mas::mas_log << "Configuration Error: Growth is required to have a amin specified.\n";
                this->valid_configuration = false;

            } else {
                bool estimated = false;
                int phase = 1;
                rapidjson::Document::MemberIterator pm = (*amin_it).value.FindMember("value");

                if (pm == (*amin_it).value.MemberEnd()) {
                    std::cout << "Configuration Warning: Growth model does not provide a initial value for \"amin\".\n";
                    mas::mas_log << "Configuration Warning: Growth model  does not provide a initial value for \"amin\".\n";
                } else {
                    VariableTrait<REAL_T>::SetValue(model->a_min, static_cast<REAL_T> ((*pm).value.GetDouble()));
                }

                //2. Get min boundary if there is one.
                pm = (*amin_it).value.FindMember("min");
                if (pm != (*amin_it).value.MemberEnd()) {
                    VariableTrait<REAL_T>::SetMinBoundary(model->a_min, static_cast<REAL_T> ((*pm).value.GetDouble()));
                }

                //3. Get max boundary if there is one.
                pm = (*amin_it).value.FindMember("max");
                if (pm != (*amin_it).value.MemberEnd()) {
                    VariableTrait<REAL_T>::SetMaxBoundary(model->a_min, static_cast<REAL_T> ((*pm).value.GetDouble()));
                }

                pm = (*amin_it).value.FindMember("estimated");
                if (pm != (*amin_it).value.MemberEnd()) {
                    std::string e = std::string((*pm).value.GetString());
                    if (e == "true") {
                        estimated = true;
                    }
                }

                if (estimated) {
                    phase = 1;
                    pm = (*amin_it).value.FindMember("phase");
                    if (pm != (*amin_it).value.MemberEnd()) {
                        phase = (*pm).value.GetInt();
                    }
                    //register alpha
                    model->Register(model->a_min, phase);

                }
            }

            rapidjson::Document::MemberIterator amax_it = (*pit).value.FindMember("amax");

            if (amax_it == (*pit).value.MemberEnd()) {
                std::cout << "Configuration Error: Growth is required to have a amax specified.\n";
                mas::mas_log << "Configuration Error: Growth is required to have a amax specified.\n";
                this->valid_configuration = false;
                ;
            } else {
                bool estimated = false;
                int phase = 1;
                rapidjson::Document::MemberIterator pm = (*amax_it).value.FindMember("value");

                if (pm == (*amax_it).value.MemberEnd()) {
                    std::cout << "Configuration Warning: Growth model does not provide a initial value for \"amax\".\n";
                    mas::mas_log << "Configuration Warning: Growth model does not provide a initial value for \"amax\".\n";
                } else {
                    VariableTrait<REAL_T>::SetValue(model->a_max, static_cast<REAL_T> ((*pm).value.GetDouble()));
                }

                //2. Get min boundary if there is one.
                pm = (*amax_it).value.FindMember("min");
                if (pm != (*amax_it).value.MemberEnd()) {
                    VariableTrait<REAL_T>::SetMinBoundary(model->a_max, static_cast<REAL_T> ((*pm).value.GetDouble()));
                }

                //3. Get max boundary if there is one.
                pm = (*amax_it).value.FindMember("max");
                if (pm != (*amax_it).value.MemberEnd()) {
                    VariableTrait<REAL_T>::SetMaxBoundary(model->a_max, static_cast<REAL_T> ((*pm).value.GetDouble()));
                }

                pm = (*amax_it).value.FindMember("estimated");
                if (pm != (*amax_it).value.MemberEnd()) {
                    std::string e = std::string((*pm).value.GetString());
                    if (e == "true") {
                        estimated = true;
                    }
                }

                if (estimated) {
                    phase = 1;
                    pm = (*amax_it).value.FindMember("phase");
                    if (pm != (*amax_it).value.MemberEnd()) {
                        phase = (*pm).value.GetInt();
                    }
                    //register alpha
                    model->Register(model->a_max, phase);

                }


            }



            rapidjson::Document::MemberIterator ewaa_it = (*growth_model).value.FindMember("empirical_weight_at_age");

            if (ewaa_it != (*growth_model).value.MemberEnd()) {
                std::vector<int> check = {0, 0, 0, 0};
                size_t ages = this->ages.size();
                size_t years = this->nyears;
                size_t seasons = this->nseasons;

                weight_functor = std::make_shared<mas::EmpiricalWeightFunctor<REAL_T> >();
                mas::EmpiricalWeightFunctor<REAL_T>* eg = (mas::EmpiricalWeightFunctor<REAL_T>*)weight_functor.get();

                for (ewaa_it = (*growth_model).value.MemberBegin(); ewaa_it != (*growth_model).value.MemberEnd(); ++ewaa_it) {

                    if ((*ewaa_it).value.IsArray()) {
                        rapidjson::Value& v = (*ewaa_it).value;
                        for (int ii = 0; ii < v.Size(); ii++) {
                            std::shared_ptr<mas::DataObject<REAL_T> > data_object(new mas::DataObject<REAL_T>());
                            rapidjson::Document::MemberIterator t_it = v[ii].FindMember("data_object_type");
                            mas::DataObjectType type = mas::DataObject<double>::GetType((*t_it).value.GetString());
                            t_it = v[ii].FindMember("sex");
                            mas::FishSexType stype = mas::DataObject<double>::GetSex((*t_it).value.GetString());
                            data_object->type = type;
                            data_object->sex_type = stype;
                            int i, j, k;
                            switch (type) {

                                case mas::CATCH_MEAN_WEIGHT_AT_AGE:
                                    INFO_DEBUG
                                    data_object->imax = years;
                                    data_object->jmax = seasons;
                                    data_object->kmax = ages;
                                    data_object->dimensions = 3;

                                    break;


                                case mas::SURVEY_MEAN_WEIGHT_AT_AGE:
                                    INFO_DEBUG
                                    data_object->imax = years;
                                    data_object->jmax = seasons;
                                    data_object->kmax = ages;
                                    data_object->dimensions = 3;

                                    break;

                                case mas::MEAN_WEIGHT_AT_AGE_SEASON_START:
                                    INFO_DEBUG
                                    data_object->imax = years;
                                    data_object->jmax = seasons;
                                    data_object->kmax = ages;
                                    data_object->dimensions = 3;

                                    break;
                                case mas::MEAN_WEIGHT_AT_AGE_SPAWNING:
                                    INFO_DEBUG
                                    data_object->imax = years;
                                    data_object->jmax = 1;
                                    data_object->kmax = ages;
                                    data_object->dimensions = 3;

                                    break;

                            }



                            t_it = v[ii].FindMember("values");

                            if ((*t_it).value.IsArray()) {
                                rapidjson::Value& v = (*t_it).value;
                                switch (type) {

                                    case mas::CATCH_MEAN_WEIGHT_AT_AGE:
                                        check[0]++;
                                        std::cout << "reading CATCH_MEAN_WEIGHT_AT_AGE\n";
                                        INFO_DEBUG
                                        data_object->imax = years;
                                        data_object->jmax = seasons;
                                        data_object->kmax = ages;
                                        data_object->dimensions = 3;


                                        INFO_DEBUG
                                        data_object->imax = v.Size();
                                        INFO_DEBUG
                                        for (i = 0; i < v.Size(); i++) {
                                            if (!v[i].IsArray()) {
                                                std::cout << "Data Warning: Data Object \"values\" for catch_mean_weight_at_age expect as a 3 dimensional array.\n";
                                                mas::mas_log << "Data Warning: Data Object \"values\" for catch_mean_weight_at_age expect as a 3 dimensional array.\n";
                                            } else {
                                                data_object->jmax = v[i].Size();
                                                for (j = 0; j < v[i].Size(); j++) {
                                                    if (!v[i][j].IsArray()) {
                                                        std::cout << "Data Warning: Data Object \"values\" for catch_mean_weight_at_age expect as a 3 dimensional array.\n";
                                                        mas::mas_log << "Data Warning: Data Object \"values\" for catch_mean_weight_at_age expect as a 3 dimensional array.\n";
                                                    } else {
                                                        data_object->kmax = v[i][j].Size();
                                                        for (k = 0; k < v[i][j].Size(); k++) {
                                                            INFO_DEBUG
                                                            data_object->data.push_back(static_cast<REAL_T> (v[i][j][k].GetDouble()));
                                                        }
                                                    }
                                                }
                                            }
                                        }



                                        INFO_DEBUG
                                        break;


                                    case mas::SURVEY_MEAN_WEIGHT_AT_AGE:
                                        check[1]++;
                                        std::cout << "reading SURVEY_MEAN_WEIGHT_AT_AGE\n";
                                        INFO_DEBUG
                                        data_object->imax = years;
                                        data_object->jmax = seasons;
                                        data_object->kmax = ages;
                                        data_object->dimensions = 3;



                                        INFO_DEBUG
                                        data_object->imax = v.Size();
                                        for (i = 0; i < v.Size(); i++) {
                                            if (!v[i].IsArray()) {
                                                std::cout << "Data Warning: Data Object \"values\" for survey_mean_weight_at_age expect as a 3 dimensional array.\n";
                                                mas::mas_log << "Data Warning: Data Object \"values\" for survey_mean_weight_at_age expect as a 3 dimensional array.\n";
                                            } else {
                                                data_object->jmax = v[i].Size();
                                                for (j = 0; j < v[i].Size(); j++) {
                                                    if (!v[i][j].IsArray()) {
                                                        std::cout << "Data Warning: Data Object \"values\" for survey_mean_weight_at_age expect as a 3 dimensional array.\n";
                                                        mas::mas_log << "Data Warning: Data Object \"values\" for survey_mean_weight_at_age expect as a 3 dimensional array.\n";
                                                    } else {
                                                        data_object->kmax = v[i][j].Size();
                                                        for (k = 0; k < v[i][j].Size(); k++) {
                                                            data_object->data.push_back(static_cast<REAL_T> (v[i][j][k].GetDouble()));
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        break;

                                    case mas::MEAN_WEIGHT_AT_AGE_SEASON_START:
                                        check[2]++;
                                        std::cout << "reading MEAN_WEIGHT_AT_AGE_SEASON_START\n";

                                        INFO_DEBUG
                                        data_object->imax = years;
                                        data_object->jmax = seasons;
                                        data_object->kmax = ages;
                                        data_object->dimensions = 3;
                                        INFO_DEBUG
                                        data_object->imax = v.Size();
                                        for (i = 0; i < v.Size(); i++) {
                                            if (!v[i].IsArray()) {
                                                std::cout << "Data Warning: Data Object \"values\" for empirical_mean_weight_at_age_season_start expect as a 2 dimensional array.\n";
                                                mas::mas_log << "Data Warning: Data Object \"values\" for empirical_mean_weight_at_age_season_start expect as a 2 dimensional array.\n";
                                            } else {
                                                //                                                data_object->jmax = v[i].Size();
                                                for (j = 0; j < v[i].Size(); j++) {
                                                    data_object->data.push_back(static_cast<REAL_T> (v[i][j].GetDouble()));
                                                }
                                            }
                                        }
                                        break;
                                    case mas::MEAN_WEIGHT_AT_AGE_SPAWNING:
                                        check[3]++;
                                        std::cout << "reading MEAN_WEIGHT_AT_AGE_SPAWNING\n";

                                        INFO_DEBUG
                                        data_object->imax = years;
                                        data_object->jmax = 1;
                                        data_object->kmax = ages;
                                        data_object->dimensions = 3;
                                        INFO_DEBUG
                                        data_object->imax = v.Size();
                                        for (i = 0; i < v.Size(); i++) {
                                            if (!v[i].IsArray()) {
                                                std::cout << "Data Warning: Data Object \"values\" for empirical_mean_weight_at_age_spawning expect as a 2 dimensional array.\n";
                                                mas::mas_log << "Data Warning: Data Object \"values\" for empirical_mean_weight_at_age_spawning expect as a 2 dimensional array.\n";
                                            } else {
                                                //                                                data_object->jmax = v[i].Size();
                                                for (j = 0; j < v[i].Size(); j++) {
                                                    data_object->data.push_back(static_cast<REAL_T> (v[i][j].GetDouble()));
                                                }
                                            }
                                        }
                                        break;

                                }

                            }
                            INFO_DEBUG
                            mas::EmpricalDataStructure<REAL_T> eds;
                            eds.empirical_data_at_age = data_object;
                            mas::GrowthBase<REAL_T>::Do3DInterpolation(eds.empirical_data_at_age, eds.interpolated_data_at_age);
                            INFO_DEBUG
                            switch (data_object->sex_type) {
                                case mas::FEMALE:
                                    eg->weight_at_age_data[data_object->type][mas::FEMALE] = eds;
                                    break;
                                case mas::MALE:
                                    eg->weight_at_age_data[data_object->type][mas::MALE] = eds;
                                    break;
                                case mas::UNDIFFERENTIATED:
                                    eg->weight_at_age_data[data_object->type][mas::FEMALE] = eds;
                                    eg->weight_at_age_data[data_object->type][mas::MALE] = eds;
                                    break;
                            }
                        }
                    }
                }

                int check_value = 0;
                for (int i = 0; i < check.size(); i++) {
                    check_value += check[i];
                }
                if (check_value != 4) {
                    //error here
                }

            } else {

                //instantiate default here
                weight_functor = std::make_shared<mas::DefaultWeightFunctor<REAL_T> >(model->alpha_f, model->alpha_m, model->beta_f, model->beta_f);
            }



            model->weight_functor = weight_functor;
            model->id = model_id;
            growth_model_iterator it = this->growth_models.find(model->id);
            if (it != this->growth_models.end()) {
                std::cout << "Configuration Error: More than one growth model with the same identifier defined. Growth models require a unique id.\n";
                mas::mas_log << "Configuration Error: More than one growth model with the same identifier defined. Growth models require a unique id.\n";

                this->valid_configuration = false;

            } else {
                this->growth_models[model->id] = model;
            }


        }

        void HandleRecruitmentModel(rapidjson::Document::MemberIterator & recruitment_model) {
            INFO_DEBUG
            rapidjson::Document::MemberIterator rit;
            rit = (*recruitment_model).value.FindMember("model");

            if (rit == (*recruitment_model).value.MemberEnd()) {
                std::cout << "Configuration Error: Recruitment is required to have a model specified.\n";
                mas::mas_log << "Configuration Error: Recruitment is required to have a model specified.\n";
                this->valid_configuration = false;

            }

            std::shared_ptr<mas::RecruitmentBase<REAL_T> > model(NULL);
            std::string smodel = std::string((*rit).value.GetString());

            rit = (*recruitment_model).value.FindMember("id");
            int model_id = 0;
            if (rit == (*recruitment_model).value.MemberEnd()) {
                std::cout << "Configuration Error: Recruitment is required to have a unique identifier\n";
                mas::mas_log << "Configuration Error: Recruitment is required to have a unique identifier\n";
                this->valid_configuration = false;
            } else {
                model_id = (*rit).value.GetInt();
            }

            rapidjson::Document::MemberIterator pit = (*recruitment_model).value.FindMember("parameters");


            if (smodel == std::string("beverton_holt")) {
                model = std::make_shared<mas::BevertonHolt<REAL_T> >();
                mas::BevertonHolt<REAL_T>* bh = (mas::BevertonHolt<REAL_T>*)model.get();
                model->id = model_id;
                std::stringstream ss;

                ss << "beverton_holt_log_R0_" << model_id;
                VariableTrait<REAL_T>::SetName(bh->log_R0, ss.str());

                ss.str("");
                ss << "beverton_holt_h_" << model_id;
                VariableTrait<REAL_T>::SetName(bh->h, ss.str());

                ss.str("");
                ss << "beverton_holt_rho_" << model_id;
                VariableTrait<REAL_T>::SetName(bh->rho, ss.str());

                ss.str("");
                ss << "beverton_holt_sigma_r_" << model_id;
                VariableTrait<REAL_T>::SetName(bh->sigma_r, ss.str());

                ss.str("");
                ss << "beverton_holt_alpha_" << model_id;
                VariableTrait<REAL_T>::SetName(bh->alpha, ss.str());

                ss.str("");
                ss << "beverton_holt_beta_" << model_id;
                VariableTrait<REAL_T>::SetName(bh->beta, ss.str());

                if (pit == (*recruitment_model).value.MemberEnd()) {
                    std::cout << "Configuration Error: Recruitment model \"Beverton-Holt\" has no parameter definitions.\n";
                    mas::mas_log << "Configuration Error: Recruitment model \"Beverton-Holt\" has no parameter definitions.\n";
                    this->valid_configuration = false;

                } else {
                    rapidjson::Document::MemberIterator ppit = (*pit).value.FindMember("R0");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Error: Recruitment model \"Beverton-Holt\" \"" << model_id <<
                                "\" does not define \"R0\".\n";
                        mas::mas_log << "Configuration Error: Recruitment model \"Beverton-Holt\" \"" << model_id <<
                                "\" does not define \"R0\".\n";
                        this->valid_configuration = false;
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Error: Recruitment model \"Beverton-Holt\" does not provide a initial value for \"R0\".\n";
                            mas::mas_log << "Configuration Error: Recruitment model \"Beverton-Holt\" does not provide a initial value for \"R0\".\n";
                            this->valid_configuration = false;
                        } else {
                            VariableTrait<REAL_T>::SetValue(bh->log_R0, std::log(static_cast<REAL_T> ((*pm).value.GetDouble())));
                            VariableTrait<REAL_T>::SetValue(bh->R0, (static_cast<REAL_T> ((*pm).value.GetDouble())));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(bh->log_R0, std::log(static_cast<REAL_T> ((*pm).value.GetDouble())));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(bh->log_R0, std::log(static_cast<REAL_T> ((*pm).value.GetDouble())));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register R0
                            bh->Register(bh->log_R0, phase);

                        }

                    }


                    ppit = (*pit).value.FindMember("h");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Recruitment model \"Beverton-Holt\" " <<
                                "does not define \"h\". Model will use the default value of 0.6 and \"h\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Beverton-Holt\" " <<
                                "does not define \"h\". Model will use the default value of 0.6 and \"h\" will not be estimated.\n";

                        VariableTrait<REAL_T>::SetValue(bh->h, static_cast<REAL_T> (0.6));
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Recruitment model \"Beverton-Holt\" does not provide a initial value for \"h\".\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Beverton-Holt\" does not provide a initial value for \"h\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(bh->h, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(bh->h, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(bh->h, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register h
                            bh->Register(bh->h, phase);

                        }

                    }


                    ppit = (*pit).value.FindMember("rho");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Recruitment model \"Beverton-Holt Alt\" " <<
                                "does not define \"rho\". Model will use the default value of 0 and \"rho\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Beverton-Holt\" " <<
                                "does not define \"rho\". Model will use the default value of 0 and \"rho\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Recruitment model \"Beverton-Holt\" does not provide a initial value for \"rho\".\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Beverton-Holt\" does not provide a initial value for \"rho\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(bh->rho, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(bh->rho, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(bh->rho, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register rho
                            bh->Register(bh->rho, phase);

                        }

                    }


                    ppit = (*pit).value.FindMember("sigma_r");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Recruitment model \"Beverton-Holt\" " <<
                                "does not define \"sigma_r\". Model will use the default value of 0 and \"sigma_r\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Beverton-Holt\" " <<
                                "does not define \"sigma_r\". Model will use the default value of 0 and \"sigma_r\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Recruitment model \"Beverton-Holt\" does not provide a initial value for \"sigma_r\".\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Beverton-Holt\" does not provide a initial value for \"sigma_r\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(bh->sigma_r, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(bh->sigma_r, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(bh->sigma_r, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register sigma_r
                            bh->Register(bh->sigma_r, phase);

                        }

                    }

                    ppit = (*pit).value.FindMember("alpha");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Recruitment model \"Beverton-Holt\" " <<
                                "does not define \"sigma_r\". Model will use the default value of 1 and \"alpha\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Beverton-Holt\" " <<
                                "does not define \"sigma_r\". Model will use the default value of 1 and \"alpha\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Recruitment model \"Beverton-Holt\" does not provide a initial value for \"alpha\".\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Beverton-Holt\" does not provide a initial value for \"alpha\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(bh->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(bh->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(bh->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register sigma_r
                            bh->Register(bh->alpha, phase);

                        }

                    }

                    ppit = (*pit).value.FindMember("beta");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Recruitment model \"Beverton-Holt\" " <<
                                "does not define \"sigma_r\". Model will use the default value of 1 and \"beta\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Beverton-Holt\" " <<
                                "does not define \"sigma_r\". Model will use the default value of 1 and \"beta\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Recruitment model \"Beverton-Holt\" does not provide a initial value for \"beta\".\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Beverton-Holt\" does not provide a initial value for \"beta\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(bh->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(bh->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(bh->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register sigma_r
                            bh->Register(bh->beta, phase);

                        }

                    }


                }

            } else if (smodel == std::string("beverton_holt_alt")) {
                model = std::make_shared<mas::BevertonHoltAlt<REAL_T> >();
                mas::BevertonHoltAlt<REAL_T>* bh = (mas::BevertonHoltAlt<REAL_T>*)model.get();
                model->id = model_id;
                std::stringstream ss;

                ss << "beverton_holt_alt__log_R0_" << model_id;
                VariableTrait<REAL_T>::SetName(bh->log_R0, ss.str());

                ss.str("");
                ss << "beverton_holt_alt_h_" << model_id;
                VariableTrait<REAL_T>::SetName(bh->h, ss.str());

                ss.str("");
                ss << "beverton_holt_alt_rho_" << model_id;
                VariableTrait<REAL_T>::SetName(bh->rho, ss.str());

                ss.str("");
                ss << "beverton_holt_alt_sigma_r_" << model_id;
                VariableTrait<REAL_T>::SetName(bh->sigma_r, ss.str());

                if (pit == (*recruitment_model).value.MemberEnd()) {
                    std::cout << "Configuration Error: Recruitment model \"Beverton-Holt Alt\" has no parameter definitions.\n";
                    mas::mas_log << "Configuration Error: Recruitment model \"Beverton-Holt Alt\" has no parameter definitions.\n";
                    this->valid_configuration = false;
                } else {
                    rapidjson::Document::MemberIterator ppit = (*pit).value.FindMember("R0");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Error: Recruitment model \"Beverton-Holt Alt\" \"" << model_id <<
                                "\" does not define \"R0\".\n";
                        mas::mas_log << "Configuration Error: Recruitment model \"Beverton-Holt Alt\" \"" << model_id <<
                                "\" does not define \"R0\".\n";
                        this->valid_configuration = false;
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Error: Recruitment model \"Beverton-Holt Alt\" does not provide a initial value for \"R0\".\n";
                            mas::mas_log << "Configuration Error: Recruitment model \"Beverton-Holt Alt\" does not provide a initial value for \"R0\".\n";
                            this->valid_configuration = false;
                        } else {
                            VariableTrait<REAL_T>::SetValue(bh->log_R0, std::log(static_cast<REAL_T> ((*pm).value.GetDouble())));
                            VariableTrait<REAL_T>::SetValue(bh->R0, (static_cast<REAL_T> ((*pm).value.GetDouble())));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(bh->log_R0, std::log(static_cast<REAL_T> ((*pm).value.GetDouble())));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(bh->log_R0, std::log(static_cast<REAL_T> ((*pm).value.GetDouble())));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register R0
                            bh->Register(bh->log_R0, phase);

                        }

                    }


                    ppit = (*pit).value.FindMember("h");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Recruitment model \"Beverton-Holt Alt\" " <<
                                "does not define \"h\". Model will use the default value of 0.6 and \"h\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Beverton-Holt\" " <<
                                "does not define \"h\". Model will use the default value of 0.6 and \"h\" will not be estimated.\n";

                        VariableTrait<REAL_T>::SetValue(bh->h, static_cast<REAL_T> (0.6));
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Recruitment model \"Beverton-Holt Alt\" does not provide a initial value for \"h\".\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Beverton-Holt Alt\" does not provide a initial value for \"h\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(bh->h, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(bh->h, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(bh->h, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register h
                            bh->Register(bh->h, phase);

                        }

                    }


                    ppit = (*pit).value.FindMember("rho");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Recruitment model \"Beverton-Holt Alt\" " <<
                                "does not define \"rho\". Model will use the default value of 0 and \"rho\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Beverton-Holt\" " <<
                                "does not define \"rho\". Model will use the default value of 0 and \"rho\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Recruitment model \"Beverton-Holt Alt\" does not provide a initial value for \"rho\".\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Beverton-Holt Alt\" does not provide a initial value for \"rho\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(bh->rho, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(bh->rho, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(bh->rho, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register rho
                            bh->Register(bh->rho, phase);

                        }

                    }


                    ppit = (*pit).value.FindMember("sigma_r");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Recruitment model \"Beverton-Holt Alt\" " <<
                                "does not define \"sigma_r\". Model will use the default value of 0 and \"sigma_r\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Beverton-Holt\" " <<
                                "does not define \"sigma_r\". Model will use the default value of 0 and \"sigma_r\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Recruitment model \"Beverton-Holt Alt\" does not provide a initial value for \"sigma_r\".\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Beverton-Holt Alt\" does not provide a initial value for \"sigma_r\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(bh->sigma_r, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(bh->sigma_r, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(bh->sigma_r, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register sigma_r
                            bh->Register(bh->sigma_r, phase);

                        }

                    }
                }
            } else if (smodel == std::string("beverton_holt_dep")) {
                model = std::make_shared<mas::BevertonHoltDep<REAL_T> >();
                mas::BevertonHoltDep<REAL_T>* bh = (mas::BevertonHoltDep<REAL_T>*)model.get();
                model->id = model_id;
                std::stringstream ss;
                ss << "beverton_holt_dep_log_R0_" << model_id;
                VariableTrait<REAL_T>::SetName(bh->log_R0, ss.str());

                ss << "beverton_holt_dep_alpha_" << model_id;
                VariableTrait<REAL_T>::SetName(bh->alpha, ss.str());
                ss.str("");
                ss << "beverton_holt_dep_beta_" << model_id;
                VariableTrait<REAL_T>::SetName(bh->beta, ss.str());
                ss.str("");
                ss << "beverton_holt_dep_c_" << model_id;
                VariableTrait<REAL_T>::SetName(bh->c, ss.str());


                if (pit == (*recruitment_model).value.MemberEnd()) {
                    std::cout << "Configuration Error: Recruitment model \"Beverton-Holt Dep\" has no parameter definitions.\n";
                    mas::mas_log << "Configuration Error: Recruitment model \"Beverton-Holt Dep\" has no parameter definitions.\n";
                    this->valid_configuration = false;
                    ;
                } else {

                    rapidjson::Document::MemberIterator ppit = (*pit).value.FindMember("R0");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Error: Recruitment model \"Beverton-Holt Dep\" \"" << model_id <<
                                "\" does not define \"R0\".\n";
                        mas::mas_log << "Configuration Error: Recruitment model \"Beverton-Holt Dep\" \"" << model_id <<
                                "\" does not define \"R0\".\n";
                        this->valid_configuration = false;
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Error: Recruitment model \"Beverton-Holt Alt\" does not provide a initial value for \"R0\".\n";
                            mas::mas_log << "Configuration Error: Recruitment model \"Beverton-Holt Alt\" does not provide a initial value for \"R0\".\n";
                            this->valid_configuration = false;
                        } else {
                            VariableTrait<REAL_T>::SetValue(bh->log_R0, std::log(static_cast<REAL_T> ((*pm).value.GetDouble())));
                            VariableTrait<REAL_T>::SetValue(bh->R0, (static_cast<REAL_T> ((*pm).value.GetDouble())));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(bh->log_R0, std::log(static_cast<REAL_T> ((*pm).value.GetDouble())));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(bh->log_R0, std::log(static_cast<REAL_T> ((*pm).value.GetDouble())));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register R0
                            bh->Register(bh->log_R0, phase);

                        }

                    }

                    ppit = (*pit).value.FindMember("alpha");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Recruitment model \"Beverton-Holt Dep\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Beverton-Holt Dep\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Recruitment model \"Beverton-Holt\" does not provide a initial value for \"alpha\".\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Beverton-Holt\" does not provide a initial value for \"alpha\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(bh->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(bh->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(bh->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            bh->Register(bh->alpha, phase);

                        }

                    }

                    ppit = (*pit).value.FindMember("beta");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Recruitment model \"Beverton-Holt Dep\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Beverton-Holt Dep\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Recruitment model \"Beverton-Holt Dep\" does not provide a initial value for \"alpha\".\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Beverton-Holt Dep\" does not provide a initial value for \"alpha\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(bh->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(bh->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(bh->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register beta
                            bh->Register(bh->beta, phase);

                        }

                    }

                    ppit = (*pit).value.FindMember("c");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Recruitment model \"Beverton-Holt Dep\" " <<
                                "does not define \"c\". Model will use the default value of 0 and \"c\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Beverton-Holt Dep\" " <<
                                "does not define \"c\". Model will use the default value of 0 and \"c\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Recruitment model \"Beverton-Holt Dep\" does not provide a initial value for \"c\".\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Beverton-Holt Dep\" does not provide a initial value for \"c\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(bh->c, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(bh->c, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(bh->c, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register c
                            bh->Register(bh->c, phase);

                        }

                    }
                }

            } else if (smodel == std::string("ricker")) {
                model = std::make_shared<mas::Ricker<REAL_T> >();
                mas::Ricker<REAL_T>* r = (mas::Ricker<REAL_T>*)model.get();
                model->id = model_id;
                std::stringstream ss;
                ss << "ricker_alpha_" << model_id;
                VariableTrait<REAL_T>::SetName(r->alpha, ss.str());
                ss.str("");
                ss << "ricker_beta_" << model_id;
                VariableTrait<REAL_T>::SetName(r->beta, ss.str());


                if (pit == (*recruitment_model).value.MemberEnd()) {
                    std::cout << "Configuration Error: Recruitment model \"Ricker\" has no parameter definitions.\n";
                    mas::mas_log << "Configuration Error: Recruitment model \"Ricker\" has no parameter definitions.\n";
                    this->valid_configuration = false;

                } else {
                    rapidjson::Document::MemberIterator ppit = (*pit).value.FindMember("alpha");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Recruitment model \"Ricker\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Ricker\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Recruitment model \"Ricker\" does not provide a initial value for \"alpha\".\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Ricker\" does not provide a initial value for \"alpha\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(r->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(r->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(r->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            r->Register(r->alpha, phase);

                        }

                    }

                    ppit = (*pit).value.FindMember("beta");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Recruitment model \"Ricker\" " <<
                                "does not define \"beta\". Model will use the default value of 0 and \"beta\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Ricker\" " <<
                                "does not define \"beta\". Model will use the default value of 0 and \"beta\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Recruitment model \"Ricker\" does not provide a initial value for \"beta\".\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Ricker\" does not provide a initial value for \"beta\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(r->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(r->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(r->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register beta
                            r->Register(r->beta, phase);

                        }

                    }
                }
            } else if (smodel == std::string("ricker_alt")) {
                model = std::make_shared<mas::RickerAlt<REAL_T> >();
                mas::RickerAlt<REAL_T>* ra = (mas::RickerAlt<REAL_T>*)model.get();
                model->id = model_id;
                std::stringstream ss;
                ss << "ricker_alt_log_R0_" << model_id;
                VariableTrait<REAL_T>::SetName(ra->log_R0, ss.str());
                //                ss.str("");
                //                ss << "ricker_alt_phi0_" << model_id;
                //                VariableTrait<REAL_T>::SetName(ra->phi0, ss.str());
                //
                //                ss.str("");
                //                ss << "ricker_alt_A_" << model_id;
                //                VariableTrait<REAL_T>::SetName(ra->A, ss.str());


                ss.str("");
                ss << "ricker_alt_h_" << model_id;
                VariableTrait<REAL_T>::SetName(ra->h, ss.str());

                if (pit == (*recruitment_model).value.MemberEnd()) {
                    std::cout << "Configuration Error: Recruitment model \"Ricker Alt\" has no parameter definitions.\n";
                    mas::mas_log << "Configuration Error: Recruitment model \"Ricker Alt\" has no parameter definitions.\n";
                    this->valid_configuration = false;

                } else {
                    rapidjson::Document::MemberIterator ppit = (*pit).value.FindMember("R0");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Recruitment model \"Ricker Alt\" " <<
                                "does not define \"Ro\". Model will use the default value of 0 and \"Ro\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Ricker Alt\" " <<
                                "does not define \"Ro\". Model will use the default value of 0 and \"Ro\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Recruitment model \"Ricker Alt\" does not provide a initial value for \"Ro\".\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Ricker Alt\" does not provide a initial value for \"Ro\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(ra->log_R0, std::log(static_cast<REAL_T> ((*pm).value.GetDouble())));
                            VariableTrait<REAL_T>::SetValue(ra->R0, (static_cast<REAL_T> ((*pm).value.GetDouble())));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(ra->log_R0, std::log(static_cast<REAL_T> ((*pm).value.GetDouble())));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(ra->log_R0, std::log(static_cast<REAL_T> ((*pm).value.GetDouble())));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register R0
                            ra->Register(ra->log_R0, phase);

                        }

                    }

                    //                    ppit = (*pit).value.FindMember("phi0");
                    //                    if (ppit == (*pit).value.MemberEnd()) {
                    //                        std::cout << "Configuration Warning: Recruitment model \"Ricker Alt\" " <<
                    //                                "does not define \"phi0\". Model will use the default value of 0 and \"phi0\" will not be estimated.\n";
                    //                        mas::mas_log << "Configuration Warning: Recruitment model \"Ricker Alt\" " <<
                    //                                "does not define \"phi0\". Model will use the default value of 0 and \"phi0\" will not be estimated.\n";
                    //                    } else {
                    //
                    //                        bool estimated = false;
                    //                        int phase = 1;
                    //                        //1. Get initial value if there is one.
                    //                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");
                    //
                    //                        if (pm == (*ppit).value.MemberEnd()) {
                    //                            std::cout << "Configuration Warning: Recruitment model \"Ricker Alt\" does not provide a initial value for \"phi0\".\n";
                    //                            mas::mas_log << "Configuration Warning: Recruitment model \"Ricker Alt\" does not provide a initial value for \"phi0\".\n";
                    //                        } else {
                    //                            VariableTrait<REAL_T>::SetValue(ra->phi0, static_cast<REAL_T> ((*pm).value.GetDouble()));
                    //                        }
                    //
                    //                        //2. Get min boundary if there is one.
                    //                        pm = (*ppit).value.FindMember("min");
                    //                        if (pm != (*ppit).value.MemberEnd()) {
                    //                            VariableTrait<REAL_T>::SetMinBoundary(ra->phi0, static_cast<REAL_T> ((*pm).value.GetDouble()));
                    //                        }
                    //
                    //                        //3. Get max boundary if there is one.
                    //                        pm = (*ppit).value.FindMember("max");
                    //                        if (pm != (*ppit).value.MemberEnd()) {
                    //                            VariableTrait<REAL_T>::SetMaxBoundary(ra->phi0, static_cast<REAL_T> ((*pm).value.GetDouble()));
                    //                        }
                    //
                    //                        pm = (*ppit).value.FindMember("estimated");
                    //                        if (pm != (*ppit).value.MemberEnd()) {
                    //                            std::string e = std::string((*pm).value.GetString());
                    //                            if (e == "true") {
                    //                                estimated = true;
                    //                            }
                    //                        }
                    //
                    //                        if (estimated) {
                    //                            phase = 1;
                    //                            pm = (*ppit).value.FindMember("phase");
                    //                            if (pm != (*ppit).value.MemberEnd()) {
                    //                                phase = (*pm).value.GetInt();
                    //                            }
                    //                            //register phi0
                    //                            ra->Register(ra->phi0, phase);
                    //
                    //                        }
                    //
                    //                    }

                    //                    ppit = (*pit).value.FindMember("A");
                    //                    if (ppit == (*pit).value.MemberEnd()) {
                    //                        std::cout << "Configuration Warning: Recruitment model \"Ricker Alt\" " <<
                    //                                "does not define \"A\". Model will use the default value of 0 and \"A\" will not be estimated.\n";
                    //                        mas::mas_log << "Configuration Warning: Recruitment model \"Ricker Alt\" " <<
                    //                                "does not define \"A\". Model will use the default value of 0 and \"A\" will not be estimated.\n";
                    //                    } else {
                    //
                    //                        bool estimated = false;
                    //                        int phase = 1;
                    //                        //1. Get initial value if there is one.
                    //                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");
                    //
                    //                        if (pm == (*ppit).value.MemberEnd()) {
                    //                            std::cout << "Configuration Warning: Recruitment model \"Ricker Alt\" does not provide a initial value for \"A\".\n";
                    //                            mas::mas_log << "Configuration Warning: Recruitment model \"Ricker Alt\" does not provide a initial value for \"A\".\n";
                    //                        } else {
                    //                            VariableTrait<REAL_T>::SetValue(ra->A, static_cast<REAL_T> ((*pm).value.GetDouble()));
                    //                        }
                    //
                    //                        //2. Get min boundary if there is one.
                    //                        pm = (*ppit).value.FindMember("min");
                    //                        if (pm != (*ppit).value.MemberEnd()) {
                    //                            VariableTrait<REAL_T>::SetMinBoundary(ra->A, static_cast<REAL_T> ((*pm).value.GetDouble()));
                    //                        }
                    //
                    //                        //3. Get max boundary if there is one.
                    //                        pm = (*ppit).value.FindMember("max");
                    //                        if (pm != (*ppit).value.MemberEnd()) {
                    //                            VariableTrait<REAL_T>::SetMaxBoundary(ra->A, static_cast<REAL_T> ((*pm).value.GetDouble()));
                    //                        }
                    //
                    //                        pm = (*ppit).value.FindMember("estimated");
                    //                        if (pm != (*ppit).value.MemberEnd()) {
                    //                            std::string e = std::string((*pm).value.GetString());
                    //                            if (e == "true") {
                    //                                estimated = true;
                    //                            }
                    //                        }
                    //
                    //                        if (estimated) {
                    //                            phase = 1;
                    //                            pm = (*ppit).value.FindMember("phase");
                    //                            if (pm != (*ppit).value.MemberEnd()) {
                    //                                phase = (*pm).value.GetInt();
                    //                            }
                    //                            //register A
                    //                            ra->Register(ra->A, phase);
                    //
                    //                        }
                    //                    }

                    ppit = (*pit).value.FindMember("h");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Recruitment model \"Ricker Alt\" " <<
                                "does not define \"A\". Model will use the default value of 0 and \"h\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Ricker Alt\" " <<
                                "does not define \"A\". Model will use the default value of 0 and \"h\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Recruitment model \"Ricker Alt\" does not provide a initial value for \"h\".\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Ricker Alt\" does not provide a initial value for \"h\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(ra->h, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(ra->h, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(ra->h, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register A
                            ra->Register(ra->h, phase);

                        }
                    }




                }
            } else if (smodel == std::string("shepard")) {
                //                model = std::make_shared<mas::Shepherd<REAL_T> >();
                //                mas::Shepherd<REAL_T>* s = (mas::Shepherd<REAL_T>*)model.get();
                //                model->id = model_id;
                //                std::stringstream ss;
                //                
                //                ss << "shepard_log_R0_" << model_id;
                //                VariableTrait<REAL_T>::SetName(s->log_R0, ss.str());
                //                
                //                ss << "shepard_alpha_" << model_id;
                //                VariableTrait<REAL_T>::SetName(s->alpha, ss.str());
                //                ss.str("");
                //                ss << "shepard_beta_" << model_id;
                //                VariableTrait<REAL_T>::SetName(s->beta, ss.str());
                //                ss.str("");
                //                ss << "shepard_c_" << model_id;
                //                VariableTrait<REAL_T>::SetName(s->c, ss.str());
                //
                //
                //                if (pit == (*recruitment_model).value.MemberEnd()) {
                //                    std::cout << "Configuration Error: Recruitment model \"Shepherd\" has no parameter definitions.\n";
                //                    mas::mas_log << "Configuration Error: Recruitment model \"Shepherd\" has no parameter definitions.\n";
                //                    this->valid_configuration = false;
                //                    ;
                //                } else {
                //                    rapidjson::Document::MemberIterator ppit = (*pit).value.FindMember("R0");
                //                    if (ppit == (*pit).value.MemberEnd()) {
                //                        std::cout << "Configuration Error: Recruitment model \"Beverton-Holt Alt\" \"" << model_id <<
                //                                "\" does not define \"R0\".\n";
                //                        mas::mas_log << "Configuration Error: Recruitment model \"Beverton-Holt Alt\" \"" << model_id <<
                //                                "\" does not define \"R0\".\n";
                //                        this->valid_configuration = false;
                //                    } else {
                //
                //                        bool estimated = false;
                //                        int phase = 1;
                //                        //1. Get initial value if there is one.
                //                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");
                //
                //                        if (pm == (*ppit).value.MemberEnd()) {
                //                            std::cout << "Configuration Error: Recruitment model \"Beverton-Holt Alt\" does not provide a initial value for \"R0\".\n";
                //                            mas::mas_log << "Configuration Error: Recruitment model \"Beverton-Holt Alt\" does not provide a initial value for \"R0\".\n";
                //                            this->valid_configuration = false;
                //                        } else {
                //                            VariableTrait<REAL_T>::SetValue(bh->log_R0, std::log(static_cast<REAL_T> ((*pm).value.GetDouble())));
                //                            VariableTrait<REAL_T>::SetValue(bh->R0, (static_cast<REAL_T> ((*pm).value.GetDouble())));
                //                        }
                //
                //                        //2. Get min boundary if there is one.
                //                        pm = (*ppit).value.FindMember("min");
                //                        if (pm != (*ppit).value.MemberEnd()) {
                //                            VariableTrait<REAL_T>::SetMinBoundary(bh->log_R0, std::log(static_cast<REAL_T> ((*pm).value.GetDouble())));
                //                        }
                //
                //                        //3. Get max boundary if there is one.
                //                        pm = (*ppit).value.FindMember("max");
                //                        if (pm != (*ppit).value.MemberEnd()) {
                //                            VariableTrait<REAL_T>::SetMaxBoundary(bh->log_R0, std::log(static_cast<REAL_T> ((*pm).value.GetDouble())));
                //                        }
                //
                //                        pm = (*ppit).value.FindMember("estimated");
                //                        if (pm != (*ppit).value.MemberEnd()) {
                //                            std::string e = std::string((*pm).value.GetString());
                //                            if (e == "true") {
                //                                estimated = true;
                //                            }
                //                        }
                //
                //                        if (estimated) {
                //                            phase = 1;
                //                            pm = (*ppit).value.FindMember("phase");
                //                            if (pm != (*ppit).value.MemberEnd()) {
                //                                phase = (*pm).value.GetInt();
                //                            }
                //                            //register R0
                //                            bh->Register(bh->log_R0, phase);
                //
                //                        }
                //
                //                    }
                //                    
                //                    rapidjson::Document::MemberIterator ppit = (*pit).value.FindMember("alpha");
                //                    if (ppit == (*pit).value.MemberEnd()) {
                //                        std::cout << "Configuration Warning: Recruitment model \"Shepherd\" " <<
                //                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                //                        mas::mas_log << "Configuration Warning: Recruitment model \"Shepherd\" " <<
                //                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                //                    } else {
                //
                //                        bool estimated = false;
                //                        int phase = 1;
                //                        //1. Get initial value if there is one.
                //                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");
                //
                //                        if (pm == (*ppit).value.MemberEnd()) {
                //                            std::cout << "Configuration Warning: Recruitment model \"Shepherd\" does not provide a initial value for \"alpha\".\n";
                //                            mas::mas_log << "Configuration Warning: Recruitment model \"Shepherd\" does not provide a initial value for \"alpha\".\n";
                //                        } else {
                //                            VariableTrait<REAL_T>::SetValue(s->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                //                        }
                //
                //                        //2. Get min boundary if there is one.
                //                        pm = (*ppit).value.FindMember("min");
                //                        if (pm != (*ppit).value.MemberEnd()) {
                //                            VariableTrait<REAL_T>::SetMinBoundary(s->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                //                        }
                //
                //                        //3. Get max boundary if there is one.
                //                        pm = (*ppit).value.FindMember("max");
                //                        if (pm != (*ppit).value.MemberEnd()) {
                //                            VariableTrait<REAL_T>::SetMaxBoundary(s->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                //                        }
                //
                //                        pm = (*ppit).value.FindMember("estimated");
                //                        if (pm != (*ppit).value.MemberEnd()) {
                //                            std::string e = std::string((*pm).value.GetString());
                //                            if (e == "true") {
                //                                estimated = true;
                //                            }
                //                        }
                //
                //                        if (estimated) {
                //                            phase = 1;
                //                            pm = (*ppit).value.FindMember("phase");
                //                            if (pm != (*ppit).value.MemberEnd()) {
                //                                phase = (*pm).value.GetInt();
                //                            }
                //                            //register alpha
                //                            s->Register(s->alpha, phase);
                //
                //                        }
                //
                //                    }
                //
                //                    ppit = (*pit).value.FindMember("beta");
                //                    if (ppit == (*pit).value.MemberEnd()) {
                //                        std::cout << "Configuration Warning: Recruitment model \"Shepherd\" " <<
                //                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                //                        mas::mas_log << "Configuration Warning: Recruitment model \"Shepherd\" " <<
                //                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                //                    } else {
                //
                //                        bool estimated = false;
                //                        int phase = 1;
                //                        //1. Get initial value if there is one.
                //                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");
                //
                //                        if (pm == (*ppit).value.MemberEnd()) {
                //                            std::cout << "Configuration Warning: Recruitment model \"Shepherd\" does not provide a initial value for \"alpha\".\n";
                //                            mas::mas_log << "Configuration Warning: Recruitment model \"Shepherd\" does not provide a initial value for \"alpha\".\n";
                //                        } else {
                //                            VariableTrait<REAL_T>::SetValue(s->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                //                        }
                //
                //                        //2. Get min boundary if there is one.
                //                        pm = (*ppit).value.FindMember("min");
                //                        if (pm != (*ppit).value.MemberEnd()) {
                //                            VariableTrait<REAL_T>::SetMinBoundary(s->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                //                        }
                //
                //                        //3. Get max boundary if there is one.
                //                        pm = (*ppit).value.FindMember("max");
                //                        if (pm != (*ppit).value.MemberEnd()) {
                //                            VariableTrait<REAL_T>::SetMaxBoundary(s->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                //                        }
                //
                //                        pm = (*ppit).value.FindMember("estimated");
                //                        if (pm != (*ppit).value.MemberEnd()) {
                //                            std::string e = std::string((*pm).value.GetString());
                //                            if (e == "true") {
                //                                estimated = true;
                //                            }
                //                        }
                //
                //                        if (estimated) {
                //                            phase = 1;
                //                            pm = (*ppit).value.FindMember("phase");
                //                            if (pm != (*ppit).value.MemberEnd()) {
                //                                phase = (*pm).value.GetInt();
                //                            }
                //                            //register beta
                //                            s->Register(s->beta, phase);
                //
                //                        }
                //
                //                    }
                //
                //                    ppit = (*pit).value.FindMember("c");
                //                    if (ppit == (*pit).value.MemberEnd()) {
                //                        std::cout << "Configuration Warning: Recruitment model \"Shepherd\" " <<
                //                                "does not define \"c\". Model will use the default value of 0 and \"c\" will not be estimated.\n";
                //                        mas::mas_log << "Configuration Warning: Recruitment model \"Shepherd\" " <<
                //                                "does not define \"c\". Model will use the default value of 0 and \"c\" will not be estimated.\n";
                //                    } else {
                //
                //                        bool estimated = false;
                //                        int phase = 1;
                //                        //1. Get initial value if there is one.
                //                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");
                //
                //                        if (pm == (*ppit).value.MemberEnd()) {
                //                            std::cout << "Configuration Warning: Recruitment model \"Shepherd\" does not provide a initial value for \"c\".\n";
                //                            mas::mas_log << "Configuration Warning: Recruitment model \"Shepherd\" does not provide a initial value for \"c\".\n";
                //                        } else {
                //                            VariableTrait<REAL_T>::SetValue(s->c, static_cast<REAL_T> ((*pm).value.GetDouble()));
                //                        }
                //
                //                        //2. Get min boundary if there is one.
                //                        pm = (*ppit).value.FindMember("min");
                //                        if (pm != (*ppit).value.MemberEnd()) {
                //                            VariableTrait<REAL_T>::SetMinBoundary(s->c, static_cast<REAL_T> ((*pm).value.GetDouble()));
                //                        }
                //
                //                        //3. Get max boundary if there is one.
                //                        pm = (*ppit).value.FindMember("max");
                //                        if (pm != (*ppit).value.MemberEnd()) {
                //                            VariableTrait<REAL_T>::SetMaxBoundary(s->c, static_cast<REAL_T> ((*pm).value.GetDouble()));
                //                        }
                //
                //                        pm = (*ppit).value.FindMember("estimated");
                //                        if (pm != (*ppit).value.MemberEnd()) {
                //                            std::string e = std::string((*pm).value.GetString());
                //                            if (e == "true") {
                //                                estimated = true;
                //                            }
                //                        }
                //
                //                        if (estimated) {
                //                            phase = 1;
                //                            pm = (*ppit).value.FindMember("phase");
                //                            if (pm != (*ppit).value.MemberEnd()) {
                //                                phase = (*pm).value.GetInt();
                //                            }
                //                            //register c
                //                            s->Register(s->c, phase);
                //
                //                        }
                //
                //                    }
                //                }
            } else if (smodel == std::string("deriso")) {
                model = std::make_shared<mas::Deriso<REAL_T> >();
                mas::Deriso<REAL_T>* d = (mas::Deriso<REAL_T>*)model.get();
                model->id = model_id;
                std::stringstream ss;
                ss << "deriso_alpha_" << model_id;
                VariableTrait<REAL_T>::SetName(d->alpha, ss.str());
                ss.str("");
                ss << "deriso_beta_" << model_id;
                VariableTrait<REAL_T>::SetName(d->beta, ss.str());
                ss.str("");
                ss << "deriso_c_" << model_id;
                VariableTrait<REAL_T>::SetName(d->c, ss.str());


                if (pit == (*recruitment_model).value.MemberEnd()) {
                    std::cout << "Configuration Error: Recruitment model \"Deriso\" has no parameter definitions.\n";
                    mas::mas_log << "Configuration Error: Recruitment model \"Deriso\" has no parameter definitions.\n";
                    this->valid_configuration = false;
                    ;
                } else {
                    rapidjson::Document::MemberIterator ppit = (*pit).value.FindMember("alpha");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Recruitment model \"Deriso\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Deriso\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Recruitment model \"Deriso\" does not provide a initial value for \"alpha\".\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Deriso\" does not provide a initial value for \"alpha\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(d->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(d->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(d->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            d->Register(d->alpha, phase);

                        }

                    }

                    ppit = (*pit).value.FindMember("beta");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Recruitment model \"Deriso\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Deriso\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Recruitment model \"Deriso\" does not provide a initial value for \"alpha\".\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Deriso\" does not provide a initial value for \"alpha\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(d->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(d->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(d->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register beta
                            d->Register(d->beta, phase);

                        }

                    }

                    ppit = (*pit).value.FindMember("c");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Recruitment model \"Deriso\" " <<
                                "does not define \"c\". Model will use the default value of 0 and \"c\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Deriso\" " <<
                                "does not define \"c\". Model will use the default value of 0 and \"c\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Recruitment model \"Deriso\" does not provide a initial value for \"c\".\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Deriso\" does not provide a initial value for \"c\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(d->c, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(d->c, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(d->c, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register c
                            d->Register(d->c, phase);

                        }

                    }
                }
            }


            if (pit == (*recruitment_model).value.MemberEnd()) {
                std::cout << "Configuration Error: Recruitment model \"" << model_id << "\" has no parameter definitions.\n";
                mas_log << "Configuration Error: Recruitment model \"" << model_id << "\" has no parameter definitions.\n";
                this->valid_configuration = false;
                return;
            } else {

                rapidjson::Document::MemberIterator sr_it = (*pit).value.FindMember("sigma_r");
                if (sr_it != (*pit).value.MemberEnd()) {

                    rapidjson::Document::MemberIterator srv_it = (*sr_it).value.FindMember("value");
                    if (srv_it != (*sr_it).value.MemberEnd()) {
                        mas::VariableTrait<REAL_T>::SetValue(model->sigma_r, static_cast<REAL_T> ((*srv_it).value.GetDouble()));
                    }
                    std::string estimated = "false";
                    rapidjson::Document::MemberIterator sre_it = (*sr_it).value.FindMember("estimated");
                    if (sre_it != (*sr_it).value.MemberEnd()) {
                        estimated = std::string((*sre_it).value.GetString());
                        if (estimated == "true") {
                            REAL_T min = std::numeric_limits<REAL_T>::min();
                            REAL_T max = std::numeric_limits<REAL_T>::max();
                            int phase = 1;

                            rapidjson::Document::MemberIterator phase_it = (*sr_it).value.FindMember("phase");
                            if (phase_it != (*sr_it).value.MemberEnd()) {
                                phase = (*phase_it).value.GetInt();
                            }

                            rapidjson::Document::MemberIterator min_it = (*sr_it).value.FindMember("min");
                            if (min_it != (*sr_it).value.MemberEnd()) {
                                min = static_cast<REAL_T> ((*min_it).value.GetDouble());
                                mas::VariableTrait<REAL_T>::SetMinBoundary(model->sigma_r, static_cast<REAL_T> ((*min_it).value.GetDouble()));
                            }

                            rapidjson::Document::MemberIterator max_it = (*sr_it).value.FindMember("max");
                            if (max_it != (*sr_it).value.MemberEnd()) {
                                max = static_cast<REAL_T> ((*min_it).value.GetDouble());
                                mas::VariableTrait<REAL_T>::SetMaxBoundary(model->sigma_r, static_cast<REAL_T> ((*max_it).value.GetDouble()));
                            }

                            std::stringstream ss;
                            ss << "sigma_r_" << model->id;
                            mas::VariableTrait<REAL_T>::SetName(model->sigma_r, ss.str());

                            model->Register(model->sigma_r, phase);
                        }
                    }


                } else {
                    std::cout << "Configuration Error: Recruitment model \"" << model_id << "\" has no sigma_r definition.\n";
                    mas_log << "Configuration Error: Recruitment model \"" << model_id << "\" has no sigma_r definition.\n";
                    this->valid_configuration = false;
                }


                /*
                rapidjson::Document::MemberIterator rho_it = (*pit).value.FindMember("rho");
                if (rho_it != (*pit).value.MemberEnd()) {

                    rapidjson::Document::MemberIterator rhov_it = (*rho_it).value.FindMember("value");
                    if (rhov_it != (*rho_it).value.MemberEnd()) {
                        mas::VariableTrait<REAL_T>::SetValue(model->rho, static_cast<REAL_T> ((*rhov_it).value.GetDouble()));
                    }
                    std::string estimated = "false";
                    rapidjson::Document::MemberIterator sre_it = (*rho_it).value.FindMember("estimated");
                    if (sre_it != (*rho_it).value.MemberEnd()) {
                        estimated = std::string((*sre_it).value.GetString());
                        if (estimated == "true") {
                            REAL_T min = std::numeric_limits<REAL_T>::min();
                            REAL_T max = std::numeric_limits<REAL_T>::max();
                            int phase = 1;

                            rapidjson::Document::MemberIterator phase_it = (*rho_it).value.FindMember("phase");
                            if (phase_it != (*rho_it).value.MemberEnd()) {
                                phase = (*phase_it).value.GetInt();
                            }

                            rapidjson::Document::MemberIterator min_it = (*rho_it).value.FindMember("min");
                            if (min_it != (*rho_it).value.MemberEnd()) {
                                min = static_cast<REAL_T> ((*min_it).value.GetDouble());
                                mas::VariableTrait<REAL_T>::SetMinBoundary(model->rho, static_cast<REAL_T> ((*min_it).value.GetDouble()));
                            }

                            rapidjson::Document::MemberIterator max_it = (*rho_it).value.FindMember("max");
                            if (max_it != (*rho_it).value.MemberEnd()) {
                                max = static_cast<REAL_T> ((*min_it).value.GetDouble());
                                mas::VariableTrait<REAL_T>::SetMaxBoundary(model->rho, static_cast<REAL_T> ((*max_it).value.GetDouble()));
                            }

                            std::stringstream ss;
                            ss << "rho_" << model->id;
                            mas::VariableTrait<REAL_T>::SetName(model->rho, ss.str());

                            model->Register(model->rho, phase);
                        }
                    }

                }
                 */


                rapidjson::Document::MemberIterator rdevs_it = (*pit).value.FindMember("recruitment_deviations");
                bool constrained = false;
                bool is_random = false;

                if (rdevs_it == (*pit).value.MemberEnd()) {
                    std::cout << "Configuration Error: Recruitment model \"" << model_id << "\" has no deviation definition.\n";
                    mas_log << "Configuration Error: Recruitment model \"" << model_id << "\" has no deviation definition.\n";
                    this->valid_configuration = false;
                } else {
                    rapidjson::Document::MemberIterator vit = (*rdevs_it).value.FindMember("values");

                    if (!(*vit).value.IsArray()) {
                        std::cout << "Configuration Error: Recruitment model \"" << model_id << "\" deviations must be a vector.\n";
                        mas_log << "Configuration Error: Recruitment model \"" << model_id << "\" deviations must be a vector.\n";
                        this->valid_configuration = false;
                    } else {
                        rapidjson::Value& v = (*vit).value;
                        model->recruitment_deviations.resize(v.Size());
                        for (int i = 0; i < v.Size(); i++) {
                            mas::VariableTrait<REAL_T>::SetValue(model->recruitment_deviations[i], static_cast<REAL_T> (v[i].GetDouble()));
                        }

                    }

                    rapidjson::Document::MemberIterator e_it = (*rdevs_it).value.FindMember("estimated");

                    if (e_it != (*rdevs_it).value.MemberEnd()) {


                        if (std::string((*e_it).value.GetString()) == "true") {
                            int phase = 1;
                            rapidjson::Document::MemberIterator c_it = (*rdevs_it).value.FindMember("constrained");
                            if (c_it != (*rdevs_it).value.MemberEnd()) {
                                if (std::string((*c_it).value.GetString()) == "true") {
                                    constrained = true;
                                }
                            }
                            rapidjson::Document::MemberIterator re_it = (*rdevs_it).value.FindMember("random_effect");
                            if (re_it != (*rdevs_it).value.MemberEnd()) {
                                if (std::string((*re_it).value.GetString()) == "true") {
                                    is_random = true;
                                }
                            }
                            rapidjson::Document::MemberIterator p_it = (*rdevs_it).value.FindMember("phase");

                            if (p_it != (*rdevs_it).value.MemberEnd()) {
                                phase = (*p_it).value.GetInt();
                            }
                            bool bounded = false;
                            REAL_T min = std::numeric_limits<REAL_T>::min();
                            rapidjson::Document::MemberIterator min_it = (*rdevs_it).value.FindMember("min");

                            if (min_it != (*rdevs_it).value.MemberEnd()) {
                                bounded = true;
                                min = static_cast<REAL_T> ((*min_it).value.GetDouble());
                            }

                            REAL_T max = std::numeric_limits<REAL_T>::max();
                            rapidjson::Document::MemberIterator max_it = (*rdevs_it).value.FindMember("max");

                            if (max_it != (*rdevs_it).value.MemberEnd()) {
                                bounded = true;
                                max = static_cast<REAL_T> ((*max_it).value.GetDouble());
                            }

                            if (is_random) {
                                bounded = false;
                            }
                            if (bounded) {
                                for (int i = 0; i < model->recruitment_deviations.size(); i++) {
                                    mas::VariableTrait<REAL_T>::SetMinBoundary(model->recruitment_deviations[i], min);
                                    mas::VariableTrait<REAL_T>::SetMaxBoundary(model->recruitment_deviations[i], max);
                                    std::stringstream ss;
                                    ss << "recruitment_deviations[" << i << "]_" << model->id;
                                    mas::VariableTrait<REAL_T>::SetName(model->recruitment_deviations[i], ss.str());
                                    model->Register(model->recruitment_deviations[i], phase);
                                }
                            }

                            if (is_random) {
                                for (int i = 0; i < model->recruitment_deviations.size(); i++) {
                                    std::stringstream ss;
                                    ss << "recruitment_deviations[" << i << "]_" << model->id;
                                    mas::VariableTrait<REAL_T>::SetName(model->recruitment_deviations[i], ss.str());
                                    model->RegisterRandom(model->recruitment_deviations[i]);
                                }
                            } else {
                                for (int i = 0; i < model->recruitment_deviations.size(); i++) {
                                    std::stringstream ss;
                                    ss << "recruitment_deviations[" << i << "]_" << model->id;
                                    mas::VariableTrait<REAL_T>::SetName(model->recruitment_deviations[i], ss.str());
                                    model->Register(model->recruitment_deviations[i], phase);
                                }
                            }

                        }
                    }

                }
            }



            model->id = model_id;
            recruitment_model_iterator it = this->recruitment_models.find(model->id);
            if (it != this->recruitment_models.end()) {
                std::cout << "Configuration Error: More than one recruitment model with the same identifier defined. Recruitment models require a unique id.\n";
                mas::mas_log << "Configuration Error: More than one recruitment model with the same identifier defined. Recruitment models require a unique id.\n";

                this->valid_configuration = false;

            } else {
                this->recruitment_models[model->id] = model;
            }

        }

        void ParseData(const std::string & path) {
            INFO_DEBUG
            std::stringstream ss;
            std::ifstream config;
            config.open(path.c_str());
            if (!config.good()) {
                std::cerr << "MAS Data file \"" << path << "\" not found.\n";
                mas::mas_log << "MAS Data file \"" << path << "\" not found.\n";
                this->valid_configuration = false;
            }

            while (config.good()) {
                std::string line;
                std::getline(config, line);
                ss << line << "\n";
            }
            this->ParseData(ss);
        }

        void ParseData(const std::stringstream & ss) {
            INFO_DEBUG
            rapidjson::Document document;
            document.Parse(ss.str().c_str());
            this->ParseData(document);
        }

        void ParseData(rapidjson::Document& document) {

            rapidjson::Document::MemberIterator dit;

            int years = this->nyears;
            int seasons = this->nseasons;
            int ages = this->ages.size();


            //            for (dit = document.MemberBegin(); dit != document.MemberEnd(); ++dit) {
            //                if (std::string((*dit).name.GetString()) == "years") {
            //                    INFO_DEBUG
            //                    years = (*dit).value.GetInt();
            //                }
            //
            //                if (std::string((*dit).name.GetString()) == "seasons") {
            //                    INFO_DEBUG
            //                    seasons = (*dit).value.GetInt();
            //                }
            //
            //                if (std::string((*dit).name.GetString()) == "ages") {
            //                    INFO_DEBUG
            //                    ages = (*dit).value.GetInt();
            //                }
            //            }

            INFO_DEBUG
            for (dit = document.MemberBegin(); dit != document.MemberEnd(); ++dit) {
                if (std::string((*dit).name.GetString()) == "data_object") {
                    rapidjson::Document::MemberIterator mit;
                    std::shared_ptr<DataObject<REAL_T> > data_object(new DataObject<REAL_T>());
                    INFO_DEBUG
                    //                    data_object->imax = years;
                    //                    data_object->jmax = seasons;
                    //                    data_object->kmax = ages;

                    mit = (*dit).value.FindMember("data_object_type");
                    if (mit != (*dit).value.MemberEnd()) {
                        data_object->type = DataObject<REAL_T>::GetType(std::string((*mit).value.GetString()));

                        switch (data_object->type) {
                            case mas::CATCH_BIOMASS:
                                INFO_DEBUG
                                data_object->imax = years;
                                data_object->jmax = seasons;
                                data_object->dimensions = 2;
                                break;

                            case mas::SURVEY_BIOMASS:
                                INFO_DEBUG
                                data_object->imax = years;
                                data_object->jmax = seasons;
                                data_object->dimensions = 2;
                                break;

                            case mas::CATCH_PROPORTION_AT_AGE_N:
                                INFO_DEBUG
                                data_object->imax = years;
                                data_object->jmax = seasons;
                                data_object->dimensions = 2;
                                break;

                            case mas::CATCH_PROPORTION_AT_AGE:
                                INFO_DEBUG
                                data_object->imax = years;
                                data_object->jmax = seasons;
                                data_object->kmax = ages;
                                data_object->dimensions = 3;
                                break;

                                //                            case mas::CATCH_MEAN_WEIGHT_AT_AGE:
                                //                                INFO_DEBUG
                                //                                data_object->imax = years;
                                //                                data_object->jmax = seasons;
                                //                                data_object->kmax = ages;
                                //                                data_object->dimensions = 3;
                                //                                break;
                                //
                                //                            case mas::MEAN_WEIGHT_AT_AGE_SPAWNING:
                                //                                INFO_DEBUG
                                //                                data_object->imax = years;
                                //                                data_object->jmax = 1;
                                //                                data_object->kmax = ages;
                                //                                data_object->dimensions = 3;
                                //                                break;
                                //                            case mas::MEAN_WEIGHT_AT_AGE_SEASON_START:
                                //                                INFO_DEBUG
                                //                                data_object->imax = years;
                                //                                data_object->jmax = 1;
                                //                                data_object->kmax = ages;
                                //                                data_object->dimensions = 3;
                                //                                break;

                            case mas::SURVEY_PROPORTION_AT_AGE_N:
                                INFO_DEBUG
                                data_object->imax = years;
                                data_object->jmax = seasons;
                                data_object->dimensions = 2;
                                break;

                            case mas::SURVEY_PROPORTION_AT_AGE:
                                INFO_DEBUG
                                data_object->imax = years;
                                data_object->jmax = seasons;
                                data_object->kmax = ages;
                                data_object->dimensions = 3;
                                break;
                                //
                                //                            case mas::SURVEY_MEAN_WEIGHT_AT_AGE:
                                //                                data_object->imax = years;
                                //                                data_object->jmax = seasons;
                                //                                data_object->kmax = ages;
                                //                                data_object->dimensions = 3;
                                //                                break;

                            default:
                                std::cout << "Unknown Data type\n";
                        }

                    } else {
                        std::cout << "Data Warning: Data Object has no \"data_object_type\" specified.\n";
                        mas_log << "Data Warning: Data Object has no \"data_object_type\" specified.\n";

                    }

                    mit = (*dit).value.FindMember("units");
                    if (mit != (*dit).value.MemberEnd()) {
                        data_object->units = DataObject<REAL_T>::GetUnits(std::string((*mit).value.GetString()));
                    } else {
                        std::cout << "Data Warning: Data Object has no \"units\" specified.\n";
                        mas_log << "Data Warning: Data Object has no \"units\" specified.\n";

                    }

                    mit = (*dit).value.FindMember("id");
                    if (mit != (*dit).value.MemberEnd()) {
                        data_object->id = (*mit).value.GetInt();
                    } else {
                        std::cout << "Data Warning: Data Object has no \"id\" specified.\n";
                        mas_log << "Data Warning: Data Object has no \"id\" specified.\n";

                    }

                    mit = (*dit).value.FindMember("sex");
                    if (mit != (*dit).value.MemberEnd()) {
                        data_object->sex_type = DataObject<float>::GetSex(std::string((*mit).value.GetString()));
                    } else {
                        std::cout << "Data Warning: Data Object has no \"sex\" specified.\n";
                        mas_log << "Data Warning: Data Object has no \"sex\" specified.\n";

                    }



                    mit = (*dit).value.FindMember("missing_values");
                    if (mit != (*dit).value.MemberEnd()) {
                        data_object->missing_value = static_cast<REAL_T> ((*mit).value.GetDouble());
                    } else {
                        std::cout << "Data Warning: Data Object has no \"missing_values\" specified.\n";
                        mas_log << "Data Warning: Data Object has no \"missing_values\" specified.\n";

                    }

                    mit = (*dit).value.FindMember("name");
                    if (mit != (*dit).value.MemberEnd()) {
                        data_object->name = (*mit).value.GetString();
                    } else {
                        std::cout << "Data Warning: Data Object has no \"name\" specified.\n";
                        mas_log << "Data Warning: Data Object has no \"name\" specified.\n";

                    }

                    //                    data_object->data.resize(years * seasons*ages, data_object->missing_value);

                    int counter = 0;
                    mit = (*dit).value.FindMember("values");
                    if (mit != (*dit).value.MemberEnd()) {
                        if ((*mit).value.IsArray()) {
                            int i, j, k;
                            rapidjson::Value& v = (*mit).value;
                            switch (data_object->type) {
                                case mas::CATCH_BIOMASS:
                                    INFO_DEBUG
                                    data_object->imax = v.Size();
                                    for (i = 0; i < v.Size(); i++) {
                                        if (!v[i].IsArray()) {
                                            std::cout << "Data Warning: Data Object \"values\" for catch_biomass expect as a 2 dimensional array.\n";
                                            mas_log << "Data Warning: Data Object \"values\" for catch_biomass expect as a 2 dimensional array.\n";
                                            this->valid_configuration = false;
                                        } else {
                                            data_object->jmax = v[i].Size();
                                            for (int j = 0; j < v[i].Size(); j++) {
                                                data_object->data.push_back(static_cast<REAL_T> (v[i][j].GetDouble()));
                                            }
                                        }
                                    }
                                    if (data_object->imax != this->nyears) {
                                        mas::mas_log << "Catch Biomass for entry id " << data_object->id << " data size is " << data_object->imax << " and it should equal " << this->nyears;
                                        std::cout << "Catch Biomass for entry id " << data_object->id << " data size is " << data_object->imax << " and it should equal " << this->nyears;
                                        this->valid_configuration = false;
                                    }
                                    break;

                                case mas::SURVEY_BIOMASS:
                                    INFO_DEBUG
                                    data_object->imax = v.Size();
                                    for (i = 0; i < v.Size(); i++) {
                                        if (!v[i].IsArray()) {
                                            std::cout << "Data Warning: Data Object \"values\" for survey_biomass expect as a 2 dimensional array.\n";
                                            mas_log << "Data Warning: Data Object \"values\" for survey_biomass expect as a 2 dimensional array.\n";
                                            this->valid_configuration = false;
                                        } else {
                                            data_object->jmax = v[i].Size();
                                            for (int j = 0; j < v[i].Size(); j++) {
                                                data_object->data.push_back(static_cast<REAL_T> (v[i][j].GetDouble()));
                                            }
                                        }
                                    }
                                    if (data_object->imax != this->nyears) {
                                        mas::mas_log << "Survey Biomass for entry id " << data_object->id << " data size is " << data_object->imax << " and it should equal " << this->nyears;
                                        std::cout << "Survey Biomass for entry id " << data_object->id << " data size is " << data_object->imax << " and it should equal " << this->nyears;
                                        this->valid_configuration = false;
                                    }
                                    break;

                                case mas::CATCH_PROPORTION_AT_AGE_N:
                                    INFO_DEBUG
                                    data_object->imax = v.Size();
                                    for (i = 0; i < v.Size(); i++) {
                                        data_object->data.push_back(static_cast<REAL_T> (v[i].GetDouble()));
                                    }

                                    break;

                                case mas::CATCH_PROPORTION_AT_AGE:
                                    INFO_DEBUG
                                    counter = 0;
                                    data_object->imax = v.Size();
                                    for (i = 0; i < v.Size(); i++) {
                                        if (!v[i].IsArray()) {
                                            std::cout << "Data Warning: Data Object \"values\" for catch_proportion_at_age expect as a 3 dimensional array.\n";
                                            mas_log << "Data Warning: Data Object \"values\" for catch_proportion_at_age expect as a 3 dimensional array.\n";
                                        } else {
                                            data_object->jmax = v[i].Size();
                                            for (j = 0; j < v[i].Size(); j++) {
                                                if (!v[i][j].IsArray()) {
                                                    std::cout << "Data Warning: Data Object \"values\" for catch_proportion_at_age expect as a 3 dimensional array.\n";
                                                    mas_log << "Data Warning: Data Object \"values\" for catch_proportion_at_age expect as a 3 dimensional array.\n";
                                                } else {
                                                    data_object->kmax = v[i][j].Size();
                                                    for (k = 0; k < v[i][j].Size(); k++) {
                                                        counter++;
                                                        data_object->data.push_back(static_cast<REAL_T> (v[i][j][k].GetDouble()));
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    std::cout << "catch proportion at age " << counter << " elements pushed\n";

                                    break;

                                    //                                case mas::CATCH_MEAN_WEIGHT_AT_AGE:
                                    //                                    INFO_DEBUG
                                    //                                    data_object->imax = v.Size();
                                    //                                    for (i = 0; i < v.Size(); i++) {
                                    //                                        if (!v[i].IsArray()) {
                                    //                                            std::cout << "Data Warning: Data Object \"values\" for catch_mean_weight_at_age expect as a 3 dimensional array.\n";
                                    //                                            mas_log << "Data Warning: Data Object \"values\" for catch_mean_weight_at_age expect as a 3 dimensional array.\n";
                                    //                                        } else {
                                    //                                            data_object->jmax = v[i].Size();
                                    //                                            for (j = 0; j < v[i].Size(); j++) {
                                    //                                                if (!v[i][j].IsArray()) {
                                    //                                                    std::cout << "Data Warning: Data Object \"values\" for catch_mean_weight_at_age expect as a 3 dimensional array.\n";
                                    //                                                    mas_log << "Data Warning: Data Object \"values\" for catch_mean_weight_at_age expect as a 3 dimensional array.\n";
                                    //                                                } else {
                                    //                                                    data_object->kmax = v[i][j].Size();
                                    //                                                    for (k = 0; k < v[i][j].Size(); k++) {
                                    //                                                        data_object->data.push_back(static_cast<REAL_T> (v[i][j][k].GetDouble()));
                                    //                                                    }
                                    //                                                }
                                    //                                            }
                                    //                                        }
                                    //                                    }
                                    //                                    break;
                                    //
                                    //                                case mas::MEAN_WEIGHT_AT_AGE_SPAWNING:
                                    //                                    INFO_DEBUG
                                    //                                    data_object->imax = v.Size();
                                    //                                    for (i = 0; i < v.Size(); i++) {
                                    //                                        if (!v[i].IsArray()) {
                                    //                                            std::cout << "Data Warning: Data Object \"values\" for empirical_mean_weight_at_age_spawning expect as a 2 dimensional array.\n";
                                    //                                            mas_log << "Data Warning: Data Object \"values\" for empirical_mean_weight_at_age_spawning expect as a 2 dimensional array.\n";
                                    //                                        } else {
                                    //                                            data_object->jmax = v[i].Size();
                                    //                                            for (j = 0; j < v[i].Size(); j++) {
                                    //                                                data_object->data.push_back(static_cast<REAL_T> (v[i][j].GetDouble()));
                                    //                                            }
                                    //                                        }
                                    //                                    }
                                    //                                    break;
                                    //                                case mas::MEAN_WEIGHT_AT_AGE_SEASON_START:
                                    //                                    INFO_DEBUG
                                    //                                    data_object->imax = v.Size();
                                    //                                    for (i = 0; i < v.Size(); i++) {
                                    //                                        if (!v[i].IsArray()) {
                                    //                                            std::cout << "Data Warning: Data Object \"values\" for empirical_mean_weight_at_age_season_start expect as a 2 dimensional array.\n";
                                    //                                            mas_log << "Data Warning: Data Object \"values\" for empirical_mean_weight_at_age_season_start expect as a 2 dimensional array.\n";
                                    //                                        } else {
                                    //                                            data_object->jmax = v[i].Size();
                                    //                                            for (j = 0; j < v[i].Size(); j++) {
                                    //                                                data_object->data.push_back(static_cast<REAL_T> (v[i][j].GetDouble()));
                                    //                                            }
                                    //                                        }
                                    //                                    }
                                    //                                    break;

                                case mas::SURVEY_PROPORTION_AT_AGE_N:
                                    INFO_DEBUG
                                    data_object->imax = v.Size();
                                    for (i = 0; i < v.Size(); i++) {
                                        data_object->data.push_back(static_cast<REAL_T> (v[i].GetDouble()));
                                    }
                                    break;

                                case mas::SURVEY_PROPORTION_AT_AGE:
                                    INFO_DEBUG
                                    data_object->imax = v.Size();
                                    for (i = 0; i < v.Size(); i++) {
                                        if (!v[i].IsArray()) {
                                            std::cout << "Data Warning: Data Object \"values\" for catch_proportion_at_age expect as a 3 dimensional array.\n";
                                            mas_log << "Data Warning: Data Object \"values\" for catch_proportion_at_age expect as a 3 dimensional array.\n";
                                        } else {
                                            data_object->jmax = v[i].Size();
                                            for (j = 0; j < v[i].Size(); j++) {
                                                if (!v[i][j].IsArray()) {
                                                    std::cout << "Data Warning: Data Object \"values\" for catch_proportion_at_age expect as a 3 dimensional array.\n";
                                                    mas_log << "Data Warning: Data Object \"values\" for catch_proportion_at_age expect as a 3 dimensional array.\n";
                                                } else {
                                                    data_object->kmax = v[i][j].Size();
                                                    for (k = 0; k < v[i][j].Size(); k++) {
                                                        data_object->data.push_back(static_cast<REAL_T> (v[i][j][k].GetDouble()));
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    break;

                                    //
                                    //                                case mas::SURVEY_MEAN_WEIGHT_AT_AGE:
                                    //                                    INFO_DEBUG
                                    //                                    data_object->imax = v.Size();
                                    //                                    for (i = 0; i < v.Size(); i++) {
                                    //                                        if (!v[i].IsArray()) {
                                    //                                            std::cout << "Data Warning: Data Object \"values\" for survey_mean_weight_at_age expect as a 3 dimensional array.\n";
                                    //                                            mas_log << "Data Warning: Data Object \"values\" for survey_mean_weight_at_age expect as a 3 dimensional array.\n";
                                    //                                        } else {
                                    //                                            data_object->jmax = v[i].Size();
                                    //                                            for (j = 0; j < v[i].Size(); j++) {
                                    //                                                if (!v[i][j].IsArray()) {
                                    //                                                    std::cout << "Data Warning: Data Object \"values\" for survey_mean_weight_at_age expect as a 3 dimensional array.\n";
                                    //                                                    mas_log << "Data Warning: Data Object \"values\" for survey_mean_weight_at_age expect as a 3 dimensional array.\n";
                                    //                                                } else {
                                    //                                                    data_object->kmax = v[i][j].Size();
                                    //                                                    for (k = 0; k < v[i][j].Size(); k++) {
                                    //                                                        data_object->data.push_back(static_cast<REAL_T> (v[i][j][k].GetDouble()));
                                    //                                                    }
                                    //                                                }
                                    //                                            }
                                    //                                        }
                                    //                                    }
                                    //                                    break;
                            }


                        } else {
                            std::cout << "Data Warning: Data Object \"values\" expected to be a vector.\n";
                            mas_log << "Data Warning: Data Object  \"values\" expected to be a vector.\n";
                        }


                    } else {
                        std::cout << "Data Warning: Data Object has no \"values\" specified.\n";
                        mas_log << "Data Warning: Data Object has no \"values\" specified.\n";

                    }


                    mit = (*dit).value.FindMember("sample_size");
                    if (mit != (*dit).value.MemberEnd()) {
                        if ((*mit).value.IsArray()) {
                            rapidjson::Value& v = (*mit).value;
                            for (int i = 0; i < v.Size(); i++) {
                                if (!v[i].IsArray()) {
                                    std::cout << "Data Warning: Data Object \"sample_size\"  expect as a 2 dimensional array.\n";
                                    mas_log << "Data Warning: Data Object \"sample_size\"  expect as a 2 dimensional array.\n";
                                    this->valid_configuration = false;
                                } else {
                                    data_object->jmax = v[i].Size();
                                    for (int j = 0; j < v[i].Size(); j++) {
                                        data_object->sample_size.push_back(static_cast<REAL_T> (v[i][j].GetDouble()));
                                    }
                                }
                            }
                        }
                    }

                    mit = (*dit).value.FindMember("observation_error");
                    if (mit != (*dit).value.MemberEnd()) {
                        if ((*mit).value.IsArray()) {
                            int i, j, k;
                            rapidjson::Value& v = (*mit).value;
                            switch (data_object->type) {
                                case mas::CATCH_BIOMASS:
                                    INFO_DEBUG
                                    for (i = 0; i < v.Size(); i++) {
                                        if (!v[i].IsArray()) {
                                            std::cout << "Data Warning: Data Object \"observation_error\" for catch_biomass expect as a 2 dimensional array.\n";
                                            mas_log << "Data Warning: Data Object \"observation_error\" for catch_biomass expect as a 2 dimensional array.\n";
                                            this->valid_configuration = false;
                                        } else {
                                            data_object->jmax = v[i].Size();
                                            for (j = 0; j < v[i].Size(); j++) {
                                                data_object->observation_error.push_back(static_cast<REAL_T> (v[i][j].GetDouble()));
                                            }
                                        }
                                    }
                                    break;

                                case mas::SURVEY_BIOMASS:
                                    INFO_DEBUG
                                    for (i = 0; i < v.Size(); i++) {
                                        if (!v[i].IsArray()) {
                                            std::cout << "Data Warning: Data Object \"observation_error\" for survey_biomass expect as a 2 dimensional array.\n";
                                            mas_log << "Data Warning: Data Object \"observation_error\" for survey_biomass expect as a 2 dimensional array.\n";
                                            this->valid_configuration = false;
                                        } else {
                                            data_object->jmax = v[i].Size();
                                            for (j = 0; j < v[i].Size(); j++) {
                                                data_object->observation_error.push_back(static_cast<REAL_T> (v[i][j].GetDouble()));
                                            }
                                        }
                                    }
                                    break;

                                case mas::CATCH_PROPORTION_AT_AGE_N:
                                    INFO_DEBUG
                                    for (i = 0; i < v.Size(); i++) {
                                        data_object->observation_error.push_back(static_cast<REAL_T> (v[i].GetDouble()));
                                    }
                                    break;

                                case mas::CATCH_PROPORTION_AT_AGE:
                                    INFO_DEBUG
                                    for (i = 0; i < v.Size(); i++) {
                                        if (!v[i].IsArray()) {
                                            std::cout << "Data Warning: Data Object \"observation_error\" for catch_proportion_at_age expect as a 3 dimensional array.\n";
                                            mas_log << "Data Warning: Data Object \"observation_error\" for catch_proportion_at_age expect as a 3 dimensional array.\n";
                                        } else {
                                            for (j = 0; j < v[i].Size(); j++) {
                                                if (!v[i][j].IsArray()) {
                                                    std::cout << "Data Warning: Data Object \"observation_error\" for catch_proportion_at_age expect as a 3 dimensional array.\n";
                                                    mas_log << "Data Warning: Data Object \"observation_error\" for catch_proportion_at_age expect as a 3 dimensional array.\n";
                                                } else {
                                                    for (k = 0; k < v[i][j].Size(); k++) {
                                                        data_object->observation_error.push_back(static_cast<REAL_T> (v[i][j][k].GetDouble()));
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    break;
                                    ////
                                    ////                                case mas::CATCH_MEAN_WEIGHT_AT_AGE:
                                    ////                                    INFO_DEBUG
                                    ////                                    for (i = 0; i < v.Size(); i++) {
                                    ////                                        if (!v[i].IsArray()) {
                                    ////                                            std::cout << "Data Warning: Data Object \"values\" for catch_mean_weight_at_age expect as a 3 dimensional array.\n";
                                    ////                                            mas_log << "Data Warning: Data Object \"values\" for catch__mean_weight_at_age error expect as a 3 dimensional array.\n";
                                    ////                                        } else {
                                    ////                                            for (j = 0; j < v[i].Size(); j++) {
                                    ////                                                if (!v[i][j].IsArray()) {
                                    ////                                                    std::cout << "Data Warning: Data Object \"observation_error\" for catch__mean_weight_at_age expect as a 3 dimensional array.\n";
                                    ////                                                    mas_log << "Data Warning: Data Object \"observation_error\" for catch_mean_weight_at_age expect as a 3 dimensional array.\n";
                                    ////                                                } else {
                                    ////                                                    for (k = 0; k < v[i][j].Size(); k++) {
                                    ////                                                        data_object->observation_error.push_back(static_cast<REAL_T> (v[i][j][k].GetDouble()));
                                    ////                                                    }
                                    ////                                                }
                                    ////                                            }
                                    ////                                        }
                                    ////                                    }
                                    ////                                    break;
                                    //
                                    //                                case mas::MEAN_WEIGHT_AT_AGE_SPAWNING:
                                    //                                    INFO_DEBUG
                                    //                                    for (i = 0; i < v.Size(); i++) {
                                    //                                        if (!v[i].IsArray()) {
                                    //                                            std::cout << "Data Warning: Data Object \"observation_error\" for empirical_mean_weight_at_age_spawning expect as a 2 dimensional array.\n";
                                    //                                            mas_log << "Data Warning: Data Object \"observation_error\" for empirical_mean_weight_at_age_spawning expect as a 2 dimensional array.\n";
                                    //                                        } else {
                                    //                                            for (j = 0; j < v[i].Size(); j++) {
                                    //                                                data_object->observation_error.push_back(static_cast<REAL_T> (v[i][j].GetDouble()));
                                    //                                            }
                                    //                                        }
                                    //                                    }
                                    //                                    break;
                                    //                                case mas::MEAN_WEIGHT_AT_AGE_SEASON_START:
                                    //                                    INFO_DEBUG
                                    //                                    for (i = 0; i < v.Size(); i++) {
                                    //                                        if (!v[i].IsArray()) {
                                    //                                            std::cout << "Data Warning: Data Object \"observation_error\" for empirical_mean_weight_at_age_season_start expect as a 2 dimensional array.\n";
                                    //                                            mas_log << "Data Warning: Data Object \"observation_error\" for empirical_mean_weight_at_age_season expect as a 2 dimensional array.\n";
                                    //                                        } else {
                                    //                                            for (j = 0; j < v[i].Size(); j++) {
                                    //                                                data_object->observation_error.push_back(static_cast<REAL_T> (v[i][j].GetDouble()));
                                    //                                            }
                                    //                                        }
                                    //                                    }
                                    //                                    break;

                                case mas::SURVEY_PROPORTION_AT_AGE_N:
                                    INFO_DEBUG
                                    for (i = 0; i < v.Size(); i++) {
                                        data_object->observation_error.push_back(static_cast<REAL_T> (v[i].GetDouble()));
                                    }
                                    break;

                                case mas::SURVEY_PROPORTION_AT_AGE:
                                    INFO_DEBUG
                                    for (i = 0; i < v.Size(); i++) {
                                        if (!v[i].IsArray()) {
                                            std::cout << "Data Warning: Data Object \"observation_error\" for catch_proportion_at_age expect as a 3 dimensional array.\n";
                                            mas_log << "Data Warning: Data Object \"observation_error\" for catch_proportion_at_age expect as a 3 dimensional array.\n";
                                        } else {
                                            for (j = 0; j < v[i].Size(); j++) {
                                                if (!v[i][j].IsArray()) {
                                                    std::cout << "Data Warning: Data Object \"observation_error\" for catch_proportion_at_age expect as a 3 dimensional array.\n";
                                                    mas_log << "Data Warning: Data Object \"observation_error\" for catch_proportion_at_age expect as a 3 dimensional array.\n";
                                                } else {
                                                    for (k = 0; k < v[i][j].Size(); k++) {
                                                        data_object->observation_error.push_back(static_cast<REAL_T> (v[i][j][k].GetDouble()));
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    break;
                                    //
                                    //                                case mas::SURVEY_MEAN_WEIGHT_AT_AGE:
                                    //                                    INFO_DEBUG
                                    //                                    for (i = 0; i < v.Size(); i++) {
                                    //                                        if (!v[i].IsArray()) {
                                    //                                            std::cout << "Data Warning: Data Object \"observation_error\" for survey_mean_weight_at_age expect as a 3 dimensional array.\n";
                                    //                                            mas_log << "Data Warning: Data Object \"observation_error\" for survey_mean_weight_at_age expect as a 3 dimensional array.\n";
                                    //                                        } else {
                                    //                                            for (j = 0; j < v[i].Size(); j++) {
                                    //                                                if (!v[i][j].IsArray()) {
                                    //                                                    std::cout << "Data Warning: Data Object \"values\" for survey_mean_weight_at_age observation error expect as a 3 dimensional array.\n";
                                    //                                                    mas_log << "Data Warning: Data Object \"values\" for survey_mean_weight_at_age observation error expect as a 3 dimensional array.\n";
                                    //                                                } else {
                                    //                                                    for (k = 0; k < v[i][j].Size(); k++) {
                                    //                                                        data_object->observation_error.push_back(static_cast<REAL_T> (v[i][j][k].GetDouble()));
                                    //                                                    }
                                    //                                                }
                                    //                                            }
                                    //                                        }
                                    //                                    }
                                    //                                    break;

                            }


                        } else {
                            std::cout << "Data Warning: Data Object \"cv\" expected to be a vector.\n";
                            mas_log << "Data Warning: Data Object  \"cv\" expected to be a vector.\n";
                        }


                    } else {
                        std::cout << "Data Warning: Data Object has no \"cv\" specified.\n";
                        mas_log << "Data Warning: Data Object has no \"cv\" specified.\n";

                    }
#warning remove this
                    if (data_object->observation_error.size() != data_object->data.size()) {
                        std::cout << "Data Error: Observation error and data size mismatch for data object \"" << data_object->name << "\".\n";
                        mas_log << "Data Error: Observation error and data size mismatch for data object \"" << data_object->name << "\".\n";
                        this->valid_configuration = false;
                    }

                    data_object->Validate();
                    data_dictionary[data_object->id].push_back(data_object);
                    this->data.push_back(data_object);


                }

            }
            INFO_DEBUG
        }

        void ValidateData() {

            int onedsize = this->nyears;
            int twodsize = this->nyears * this->ages.size();
            int threedsize = this->nyears * this->nseasons * this->ages.size();

            for (int i = 0; i < this->data.size(); i++) {

                switch (data[i]->type) {
                    case CATCH_BIOMASS:
                        if (data[i]->data.size() != this->nyears) {
                            mas::mas_log << "Catch Biomass data dimensions not correct for entry " << data[i]->id << ". Size should equal " << this->nyears << ", but input equals " << data[i]->data.size() << "\n";
                            std::cout << "Catch Biomass data dimensions not correct for entry " << data[i]->id << ". Size should equal " << this->nyears << ", but input equals " << data[i]->data.size() << "\n";
                            this->valid_configuration = false;
                        } else {
                            data[i]->imax = this->nyears;
                            data[i]->jmax = 1;
                            data[i]->kmax = 1;
                        }

                        if (data[i]->observation_error.size() != this->nyears) {
                            mas::mas_log << "Catch Biomass observed error dimensions not correct for entry " << data[i]->id << ". Size should equal " << this->nyears << ", but input equals " << data[i]->observation_error.size() << "\n";
                            std::cout << "Catch Biomass observed error dimensions not correct for entry " << data[i]->id << ". Size should equal " << this->nyears << ", but input equals " << data[i]->observation_error.size() << "\n";
                            this->valid_configuration = false;
                        } else {
                            data[i]->imax = this->nyears;
                            data[i]->jmax = 1;
                            data[i]->kmax = 1;
                        }

                        break;
                    case CATCH_PROPORTION_AT_AGE_N:
                        break;
                    case CATCH_PROPORTION_AT_AGE:
                        if (data[i]->data.size() != threedsize) {
                            std::cout << data[i]->imax << " " << data[i]->jmax << " " << data[i]->kmax << "\n";
                            mas::mas_log << "Catch Proportion at Age data dimensions not correct for entry " << data[i]->id << ". Flattened size should equal " << threedsize << ", but input equals " << data[i]->data.size() << "\n";
                            std::cout << "Catch Proportion at Age data dimensions not correct for entry " << data[i]->id << ". Flattened size should equal " << threedsize << ", but input equals " << data[i]->data.size() << "\n";
                            this->valid_configuration = false;
                        } else {
                            data[i]->imax = this->nyears;
                            data[i]->jmax = this->nseasons;
                            data[i]->kmax = this->ages.size();
                        }

                        if (data[i]->sample_size.size() != this->nyears * this->nseasons) {
                            //                            mas::mas_log << "Catch Proportion at Age observed error dimensions not correct for entry " << data[i]->id << ". Flattened size should equal " << threedsize << ", but input equals " << data[i]->observation_error.size() << "\n";
                            //                            std::cout << "Catch Proportion at Age observed error dimensions not correct for entry " << data[i]->id << ". Flattened size should equal " << threedsize << ", but input equals " << data[i]->observation_error.size() << "\n";
                            //                            this->valid_configuration = false;
                        } else {
                            data[i]->imax = this->nyears;
                            data[i]->jmax = this->nseasons;
                            data[i]->kmax = this->ages.size();
                        }
                        break;
                    case CATCH_PROPORTION_AT_LENGTH_N:
                        break;
                    case CATCH_PROPORTION_AT_LENGTH:
                        break;
                    case CATCH_MEAN_SIZE_AT_AGE:
                        break;
                    case SURVEY_BIOMASS:
                        if (data[i]->imax != this->nyears) {
                            if (data[i]->data.size() != this->nyears) {
                                mas::mas_log << "Survey Biomass data dimensions not correct. Size should equal " << this->nyears << ", but input equals " << data[i]->data.size() << "\n";
                                std::cout << "Survey Biomass data dimensions not correct. Size should equal " << this->nyears << ", but input equals " << data[i]->data.size() << "\n";
                                this->valid_configuration = false;
                            } else {
                                data[i]->imax = this->nyears;
                            }
                        }

                        if (data[i]->imax != this->nyears) {
                            if (data[i]->observation_error.size() != this->nyears) {
                                mas::mas_log << "Survey Biomass observed error dimensions not correct. Size should equal " << this->nyears << ", but input equals " << data[i]->observation_error.size() << "\n";
                                std::cout << "Survey Biomass observed error dimensions not correct. Size should equal " << this->nyears << ", but input equals " << data[i]->observation_error.size() << "\n";
                                this->valid_configuration = false;
                            } else {
                                data[i]->imax = this->nyears;
                            }
                        }
                        break;
                    case SURVEY_PROPORTION_AT_AGE_N:
                        break;
                    case SURVEY_PROPORTION_AT_AGE:
                        if (data[i]->data.size() != threedsize) {
                            mas::mas_log << "Survey Proportion at Age data dimensions not correct for entry " << data[i]->id << ". Flattened size should equal " << threedsize << ", but input equals " << data[i]->data.size() << "\n";
                            std::cout << "Survey Proportion at Age data dimensions not correct for entry " << data[i]->id << ". Flattened size should equal " << threedsize << ", but input equals " << data[i]->data.size() << "\n";
                            this->valid_configuration = false;
                        } else {
                            data[i]->imax = this->nyears;
                            data[i]->jmax = this->nseasons;
                            data[i]->kmax = this->ages.size();
                        }

                        //                        if (data[i]->observation_error.size() != threedsize) {
                        //                            mas::mas_log << "Survey Proportion at Age observed error dimensions not correct for entry " << data[i]->id << ". Flattened size should equal " << threedsize << ", but input equals " << data[i]->observation_error.size() << "\n";
                        //                            std::cout << "Survey Proportion at Age observed error dimensions not correct for entry " << data[i]->id << ". Flattened size should equal " << threedsize << ", but input equals " << data[i]->observation_error.size() << "\n";
                        //                            this->valid_configuration = false;
                        //                        } else {
                        //                            data[i]->imax = this->nyears;
                        //                            data[i]->jmax = this->nseasons;
                        //                            data[i]->kmax = this->ages.size();
                        //                        }
                        break;
                    case SURVEY_PROPORTION_AT_LENGTH_N:
                        break;
                    case SURVEY_PROPORTION_AT_LENGTH:
                        break;
                    case SURVEY_MEAN_SIZE_AT_AGE:
                        break;
                }
            }
        }

        void ShowData() {
            for (int i = 0; i < this->data.size(); i++) {
                std::cout << *this->data[i] << "\n\n";
            }
        }

        void CreateModel() {
            INFO_DEBUG
                    int uid = 0;
            area_iterator it;
            for (it = this->areas.begin(); it != this->areas.end(); ++it) {

                mas::Area<REAL_T>* area = (*it).second.get();

                area->Initialize(nyears, nseasons, ages.size());

                /*

                growth_model_iterator git = this->growth_models.find(area->growth_model_id);
                if (git == this->growth_models.end()) {
                    std::cout << "Configuration Error: Growth model " << area->growth_model_id << " has not been defined.\n";
                    mas_log << "Configuration Error: Growth model " << area->growth_model_id << " has not been defined.\n";
                    this->valid_configuration = false;
                } else {
                    area->growth_model = (*git).second;
                }

                recruitment_model_iterator rit = this->recruitment_models.find(area->recruitment_model_id);
                if (rit == this->recruitment_models.end()) {
                    std::cout << "Configuration Error: Recruitment model " << area->recruitment_model_id << " has not been defined.\n";
                    mas_log << "Configuration Error: Recruitment model " << area->recruitment_model_id << " has not been defined.\n";
                    this->valid_configuration = false;

                } else {
                    area->recruitment_model = (*rit).second;
                }

                natural_mortality_model_iterator mit = this->natural_mortality_models.find(area->mortality_model_id);
                if (mit == this->natural_mortality_models.end()) {
                    std::cout << "Configuration Error: Mortality model " << area->mortality_model_id << " has not been defined.\n";
                    mas_log << "Configuration Error: Mortality model " << area->mortality_model_id << " has not been defined.\n";
                    this->valid_configuration = false;
                } else {
                    area->mortality_model = (*mit).second;
                }

                 */

            }


            population_iterator pit;
            INFO_DEBUG
            for (pit = this->populations.begin(); pit != this->populations.end(); ++pit) {


                mas::Population<REAL_T>* population = (*pit).second.get();
                population->ages = this->ages.size();
                population->years = this->nyears;
                population->seasons = this->nseasons;

                //                population->initial_population_males.resize(this->ages.size());
                //                population->initial_population_females.resize(this->ages.size());
                //                if (population->initial_population_males.size() < this->ages.size()) {
                //                    std::cout << "Configuration Error: Initial population vector size of "<<population->initial_population_males.size()<<" for population " << population->natal_area_id << " not equal to declared ages of size " << this->ages.size() << ".\n";
                //                    mas_log << "Configuration Error: Initial population vector size for population " << population->natal_area_id << " not equal to declared ages of size " << this->ages.size() << ".\n";
                //                    this->valid_configuration = false;
                //                }

                area_iterator ait; // = this->areas.find(population->natal_area_id);
                //
                //                if (ait != this->areas.end()) {
                //                    population->natal_area = (*ait).second;
                //                } else {
                //                    std::cout << "Configuration Error: Natal area " << population->natal_area_id << " has not been defined.\n";
                //                    mas_log << "Configuration Error: Natal area " << population->natal_area_id << " has not been defined.\n";
                //                    this->valid_configuration = false;
                //                }
                INFO_DEBUG
                for (ait = this->areas.begin(); ait != this->areas.end(); ++ait) {

                    population->areas_list.push_back((*ait).second);

                    std::cout << "Adding pop/area info for males...." << std::flush;
                    mas::AreaPopulationInfo<REAL_T>& male_pop_info = population->males[(*ait).second->id];
                    male_pop_info.sex = mas::MALE;
                    male_pop_info.natal_area = population->natal_area;
                    male_pop_info.natal_population = population;
                    male_pop_info.area = (*ait).second;
                    male_pop_info.uid = uid++;
                    male_pop_info.catch_season_offset = this->catch_fraction_of_year;
                    male_pop_info.survey_season_offset = this->survey_fraction_of_year;
                    male_pop_info.spawning_season_offset = this->spawning_season_offset;

                    //                    male_pop_info.maturity_vector = population->maturity_models[male_pop_info.area->id][0];
                    //                    if (male_pop_info.maturity_vector.size() == 0 || male_pop_info.maturity_vector.size() < this->ages.size()) {
                    //                        std::cout << "Configuration Error: Maturity vector for population " << population->id << " has wrong size.\n";
                    //                        std::cout << "Configuration Error: Maturity vector for population " << population->id << " has wrong size.\n";
                    //                        this->valid_configuration = false;
                    //                    }
                    growth_model_iterator git = this->growth_models.find(population->growth_id);
                    if (git != this->growth_models.end()) {
                        male_pop_info.growth_model = this->growth_models[population->growth_id];
                        male_pop_info.growth_model->used = true;
                    } else {
                        std::cout << "Configuration Error: Growth for population " << population->id << " has not been defined.\n";
                        mas_log << "Configuration Error: Growth for population " << population->id << " has not been defined.\n";
                        this->valid_configuration = false;
                    }


                    typename mas::Population<REAL_T>::male_natural_mortality_ids_iterator mit;
                    mit = population->male_natural_mortality_ids.find(male_pop_info.area->id);

                    if (mit != population->male_natural_mortality_ids.end()) {
                        male_pop_info.natural_mortality_model = this->natural_mortality_models[(*mit).second];
                        male_pop_info.natural_mortality_model->used = true;
                    } else {
                        std::cout << "Configuration Error: Male natural mortality for population " << population->id << " in area " << male_pop_info.area->id << " has not been defined.\n";
                        mas_log << "Configuration Error: Male natural mortality for population " << population->id << " in area " << male_pop_info.area->id << " has not been defined.\n";
                        this->valid_configuration = false;
                    }


                    male_pop_info.id = population->id;
                    male_pop_info.years = this->nyears;
                    male_pop_info.seasons = this->nseasons;
                    male_pop_info.ages = this->ages;
                    male_pop_info.males = true;
                    //                    male_pop_info.Initialize();

                    std::cout << "Done\n" << std::flush;
                    std::cout << "Adding pop/area info for females...." << std::flush;
                    mas::AreaPopulationInfo<REAL_T>& female_pop_info = population->females[(*ait).second->id];
                    female_pop_info.sex = mas::FEMALE;
                    female_pop_info.natal_area = population->natal_area;
                    female_pop_info.natal_population = population;
                    female_pop_info.area = (*ait).second;
                    female_pop_info.uid = uid++;
                    female_pop_info.catch_season_offset = this->catch_fraction_of_year;
                    female_pop_info.survey_season_offset = this->survey_fraction_of_year;
                    female_pop_info.spawning_season_offset = this->spawning_season_offset;
                    //                    female_pop_info.maturity_vector = population->maturity_models[female_pop_info.area->id][1];
                    //                    if (female_pop_info.maturity_vector.size() == 0 || female_pop_info.maturity_vector.size() < this->ages.size()) {
                    //                        std::cout << "Configuration Error: Maturity vector for population " << population->id << " has wrong size.\n";
                    //                        std::cout << "Configuration Error: Maturity vector for population " << population->id << " has wrong size.\n";
                    //                        this->valid_configuration = false;
                    //                    }
                    if (git != this->growth_models.end()) {
                        female_pop_info.growth_model = this->growth_models[population->growth_id];
                        female_pop_info.growth_model->used = true;
                    } else {
                        std::cout << "Configuration Error: Growth for population " << population->id << " has not been defined.\n";
                        mas_log << "Configuration Error: Growth for population " << population->id << " has not been defined.\n";
                        this->valid_configuration = false;
                    }

                    typename mas::Population<REAL_T>::female_natural_mortality_ids_iterator fit;
                    fit = population->female_natural_mortality_ids.find(female_pop_info.area->id);

                    if (fit != population->female_natural_mortality_ids.end()) {
                        female_pop_info.natural_mortality_model = this->natural_mortality_models[(*fit).second];
                        female_pop_info.natural_mortality_model->used = true;
                    } else {
                        std::cout << "Configuration Error: Female natural mortality for population " << population->id << " in area " << male_pop_info.area->id << " has not been defined.\n";
                        mas_log << "Configuration Error: Female natural mortality for population " << population->id << " in area " << male_pop_info.area->id << " has not been defined.\n";
                        this->valid_configuration = false;
                    }


                    female_pop_info.id = population->id;
                    female_pop_info.years = this->nyears;
                    female_pop_info.seasons = this->nseasons;
                    female_pop_info.ages = this->ages;
                    female_pop_info.males = false;


                    typename mas::Population<REAL_T>::recruitment_ids_iterator rit;
                    rit = population->recruitment_ids.find(female_pop_info.area->id);

                    if (rit != population->recruitment_ids.end()) {
                        male_pop_info.recruitment_model = this->recruitment_models[(*rit).second];
                        male_pop_info.recruitment_model->used = true;
                        female_pop_info.recruitment_model = this->recruitment_models[(*rit).second];
                        female_pop_info.recruitment_model->used = true;
                    } else {
                        std::cout << "Configuration Warning: Recruitment for population " << population->id << " in area " << female_pop_info.area->id << " has not been defined.\n";
                        std::cout << "Configuration Warning: Recruitment for population " << population->id << " in area " << female_pop_info.area->id << " has not been defined.\n";
                        this->valid_configuration = false;
                    }
                    std::cout << "Done\n" << std::flush;
                    //                    female_pop_info.Initialize();
                }

                INFO_DEBUG

                        //                movement_model_iterator move_it = this->movement_models.find(population->movement_model_id);
                        //                if (move_it != this->movement_models.end()) {
                        //                    population->movement_model = (*move_it).second;
                        //                } else {
                        //                    std::cout << "Configuration Error:  Movement model " << population->movement_model_id << " has not been defined.\n";
                        //                    mas_log << "Configuration Error:  Movement model " << population->movement_model_id << " has not been defined.\n";
                        //                }

                        typename mas::Population<REAL_T>::movement_model_id_iterator mmit;
                for (mmit = population->movement_models_ids.begin(); mmit != population->movement_models_ids.end(); ++mmit) {

                    int year = (*mmit).first;
                    int model = (*mmit).second;

                    movement_model_iterator mm = this->movement_models.find(model);
                    if (mm != this->movement_models.end()) {
                        population->movement_models[year] = (*mm).second;
                        population->movement_models[year]->used = true;
                    } else {
                        std::cout << "Configuration Error:  Movement model " << model << " has not been defined.\n";
                        mas_log << "Configuration Error:  Movement model " << model << " has not been defined.\n";
                        this->valid_configuration = false;
                    }


                }

            }
            INFO_DEBUG
            for (pit = this->populations.begin(); pit != this->populations.end(); ++pit) {

                typename Population<REAL_T>::season_area_id_iterator sait;
                for (sait = (*pit).second->area_season_recruitment_ids.begin(); sait != (*pit).second->area_season_recruitment_ids.end(); ++sait) {
                    int area = (*sait).first;
                    typename std::unordered_map<int, int>::iterator it;
                    for (it = (*sait).second.begin(); it != (*sait).second.end(); ++it) {
                        int season = (*it).first;
                        int id = (*it).second;
                        (*pit).second->area_season_recruitment[area][season] = this->recruitment_models[id];
                        (*pit).second->area_season_recruitment[area][season]->used = true;
                        (*pit).second->season_area_recruitment[season][area] = this->recruitment_models[id];
                        (*pit).second->season_area_recruitment[season][area]->used = true;
                    }


                }

                fleet_iterator fit;
                for (fit = this->fleets.begin(); fit != this->fleets.end(); ++fit) {

                    (*pit).second->active_fleets.insert((*fit).second->id);
                }


                //                (*pit).second->Initialize();
            }

            //            this->nseasons = this->seasons.size();

            INFO_DEBUG
            fleet_iterator fit;
            for (fit = this->fleets.begin(); fit != this->fleets.end(); ++fit) {

                (*fit).second->Initialize(nyears, nseasons, this->ages.size());

                int nll = (*fit).second->fishery_age_comp_likelihood_component_id;


                if (nll == -999) {
                    (*fit).second->fishery_age_comp_likelihood_component = std::make_shared < mas::Multinomial<REAL_T> >();
                } else {

                    likelihood_components_iterator comp =
                            this->likelihood_components.find(nll);
                    if (comp == this->likelihood_components.end()) {
                        std::cout << "Configuration Error: Likelihood component \"" << nll << "\" not found  \n";
                        std::cout << "Configuration Error: Likelihood component \"" << nll << "\" not found  \n";
                        this->valid_configuration = false;
                    } else {
                        (*fit).second->fishery_age_comp_likelihood_component = (*comp).second;
                        (*fit).second->fishery_age_comp_likelihood_component->used = true;
                    }

                }

                nll = (*fit).second->fishery_biomass_likelihood_component_id;

                INFO_DEBUG
                if (nll == -999) {
                    (*fit).second->fishery_biomass_likelihood_component = std::make_shared < mas::Multinomial<REAL_T> >();
                } else {

                    likelihood_components_iterator comp =
                            this->likelihood_components.find(nll);
                    if (comp == this->likelihood_components.end()) {
                        std::cout << "Configuration Error: Likelihood component \"" << nll << "\" not found  \n";
                        std::cout << "Configuration Error: Likelihood component \"" << nll << "\" not found  \n";
                        this->valid_configuration = false;
                    } else {
                        (*fit).second->fishery_biomass_likelihood_component = (*comp).second;
                        (*fit).second->fishery_biomass_likelihood_component->used = true;
                    }

                }

                INFO_DEBUG
                        typename Fleet<REAL_T>::season_area_id_iterator sit;
                for (sit = (*fit).second->season_area_selectivity_ids.begin(); sit != (*fit).second->season_area_selectivity_ids.end(); ++sit) {
                    typename Fleet<REAL_T>::area_id_iterator ait;
                    int season = (*sit).first;
                    for (ait = (*sit).second.begin(); ait != (*sit).second.end(); ++ait) {
                        int area = (*ait).first;
                        int id = (*ait).second;
                        (*fit).second->area_season_selectivity[area][season] = this->selectivity_models[id];
                        (*fit).second->area_season_selectivity[area][season]->used = true;
                        (*fit).second->season_area_selectivity[season][area] = this->selectivity_models[id];
                        (*fit).second->season_area_selectivity[season][area]->used = true;
                        this->areas[area]->active_fleets.insert((*fit).second);
                    }
                }

                INFO_DEBUG
                for (sit = (*fit).second->season_area_fishing_mortality_ids.begin(); sit != (*fit).second->season_area_fishing_mortality_ids.end(); ++sit) {
                    typename Fleet<REAL_T>::area_id_iterator ait;
                    int season = (*sit).first;
                    for (ait = (*sit).second.begin(); ait != (*sit).second.end(); ++ait) {
                        int area = (*ait).first;
                        int id = (*ait).second;

                        fishing_mortality_model_iterator fmit = this->fishing_mortality_models.find(id);

                        if (fmit != this->fishing_mortality_models.end()) {
                            (*fit).second->area_season_fishing_mortality[area][season] = this->fishing_mortality_models[id];
                            (*fit).second->area_season_fishing_mortality[area][season]->used = true;
                            (*fit).second->season_area_fishing_mortality[season][area] = this->fishing_mortality_models[id];
                            (*fit).second->season_area_fishing_mortality[season][area]->used = true;
                            this->areas[area]->seasonal_fleet_operations[season].push_back((*fit).second);

                        } else {
                            std::cout << "Configuration Error: Fishing Mortality Model \"" << id << "\" not found  \n";
                            std::cout << "Configuration Error: Fishing Mortality Model \"" << id << "\" not found  \n";
                            this->valid_configuration = false;
                        }
                    }
                }

                INFO_DEBUG
                for (int i = 0; i <this->data.size(); i++) {
                    if (data[i]->id == (*fit).second->id) {

                        switch (data[i]->type) {
                            case mas::CATCH_BIOMASS:
                                (*fit).second->catch_biomass_data = data[i];
                                (*fit).second->data_objects.push_back(data[i]);
                                break;
                            case mas::CATCH_PROPORTION_AT_AGE_N:
                                (*fit).second->catch_proportion_at_age_data_N = data[i];
                                (*fit).second->data_objects.push_back(data[i]);
                                break;
                            case mas::CATCH_PROPORTION_AT_AGE:
                                (*fit).second->catch_proportion_at_age_data = data[i];
                                (*fit).second->data_objects.push_back(data[i]);
                                break;
                            case mas::CATCH_PROPORTION_AT_LENGTH_N:
                                (*fit).second->catch_proportion_at_length_data_N = data[i];
                                (*fit).second->data_objects.push_back(data[i]);
                                break;
                            case mas::CATCH_PROPORTION_AT_LENGTH:
                                (*fit).second->catch_proportion_at_length_data = data[i];
                                (*fit).second->data_objects.push_back(data[i]);
                                break;
                            case mas::CATCH_MEAN_SIZE_AT_AGE:
                                (*fit).second->catch_mean_size_at_age_data = data[i];
                                (*fit).second->data_objects.push_back(data[i]);
                                break;
                            case mas::CATCH_MEAN_WEIGHT_AT_AGE:
                                (*fit).second->catch_mean_weight_at_age_data = data[i];
                                (*fit).second->data_objects.push_back(data[i]);
                                break;
                        }
                    }
                }


                (*fit).second->InitializeNLLComponents();

            }
            INFO_DEBUG
            survey_model_iterator ssit;
            for (ssit = this->survey_models.begin(); ssit != this->survey_models.end(); ++ssit) {
                typename Survey<REAL_T>::season_area_id_iterator sit;

                (*ssit).second->Initialize(this->nyears, this->nseasons, this->ages.size());

                int nll = (*ssit).second->survey_age_comp_likelihood_component_id;


                if (nll == -999) {
                    (*ssit).second->survey_age_comp_likelihood_component = std::make_shared < mas::Multinomial<REAL_T> >();
                } else {

                    likelihood_components_iterator comp =
                            this->likelihood_components.find(nll);
                    if (comp == this->likelihood_components.end()) {
                        std::cout << "Configuration Error: Likelihood component \"" << nll << "\" not found  \n";
                        std::cout << "Configuration Error: Likelihood component \"" << nll << "\" not found  \n";
                        this->valid_configuration = false;
                    } else {
                        (*ssit).second->survey_age_comp_likelihood_component = (*comp).second;
                        (*ssit).second->survey_age_comp_likelihood_component->used = true;
                    }

                }

                nll = (*ssit).second->survey_biomass_likelihood_component_id;


                if (nll == -999) {
                    (*ssit).second->survey_biomass_likelihood_component = std::make_shared < mas::Multinomial<REAL_T> >();
                } else {

                    likelihood_components_iterator comp =
                            this->likelihood_components.find(nll);
                    if (comp == this->likelihood_components.end()) {
                        std::cout << "Configuration Error: Likelihood component \"" << nll << "\" not found  \n";
                        std::cout << "Configuration Error: Likelihood component \"" << nll << "\" not found  \n";
                        this->valid_configuration = false;
                    } else {
                        (*ssit).second->survey_biomass_likelihood_component = (*comp).second;
                        (*ssit).second->survey_biomass_likelihood_component->used = true;
                    }

                }


                for (sit = (*ssit).second->season_area_selectivity_ids.begin(); sit != (*ssit).second->season_area_selectivity_ids.end(); ++sit) {
                    typename Survey<REAL_T>::area_id_iteraor ait;
                    int season = (*sit).first;
                    for (ait = (*sit).second.begin(); ait != (*sit).second.end(); ++ait) {
                        int area = (*ait).first;
                        int id = (*ait).second;
                        (*ssit).second->area_season_selectivity[area][season] = this->selectivity_models[id];
                        (*ssit).second->area_season_selectivity[area][season]->used = true;
                        (*ssit).second->season_area_selectivity[season][area] = this->selectivity_models[id];
                        (*ssit).second->season_area_selectivity[season][area]->used = true;
                        (*ssit).second->seasonal_areas_of_operation[season][area] = this->areas[area];
                        this->areas[area]->seasonal_survey_operations[season].push_back((*ssit).second);
                        //                        this->areas[area]->seasonal_survey_operations[season].push_back((*ssit).second);
                        //                        std::cout << "setting survey " << id << ""

                        //find area
                        //                        area_iterator ait = this->areas.find(area);
                        //                        if (ait != this->areas.end()) {
                        //                            (*ait).second->survey_models[(*ssit).second->population][season] = (*ssit).second;
                        //                        }
                        //add population by season

                    }
                }
                INFO_DEBUG
                for (int i = 0; i <this->data.size(); i++) {
                    if (data[i]->id == (*ssit).second->id) {

                        switch (data[i]->type) {
                            case mas::SURVEY_BIOMASS:
                                (*ssit).second->survey_biomass_data = data[i];
                                (*ssit).second->data_objects.push_back(data[i]);
                                break;
                            case mas::SURVEY_PROPORTION_AT_AGE_N:
                                (*ssit).second->survey_proportion_at_age_data_N = data[i];
                                (*ssit).second->data_objects.push_back(data[i]);
                                break;
                            case mas::SURVEY_PROPORTION_AT_AGE:
                                (*ssit).second->survey_proportion_at_age_data = data[i];
                                (*ssit).second->data_objects.push_back(data[i]);
                                break;
                            case mas::SURVEY_PROPORTION_AT_LENGTH_N:
                                (*ssit).second->survey_proportion_at_length_data_N = data[i];
                                (*ssit).second->data_objects.push_back(data[i]);
                                break;
                            case mas::SURVEY_PROPORTION_AT_LENGTH:
                                (*ssit).second->survey_proportion_at_length_data = data[i];
                                (*ssit).second->data_objects.push_back(data[i]);
                                break;
                            case mas::SURVEY_MEAN_SIZE_AT_AGE:
                                (*ssit).second->survey_mean_size_at_age_data = data[i];
                                (*ssit).second->data_objects.push_back(data[i]);
                                break;
                            case mas::SURVEY_MEAN_WEIGHT_AT_AGE:
                                (*ssit).second->survey_mean_weight_at_age_data = data[i];
                                (*ssit).second->data_objects.push_back(data[i]);
                                break;
                        }
                    }


                }
                (*ssit).second->InitializeNLLComponents();
                typename mas::ModelObject<REAL_T>::estimable_parameter_iterator it;
                for (it = (*ssit).second->estimated_parameters_map.begin(); it != (*ssit).second->estimated_parameters_map.end(); ++it) {
                    mas_log << (*it).first->GetName() << "\t\t\t\t\t\t" << (*it).first->GetValue() << "\t\t\t\t\t\t" << (*it).second << "\n";
                    std::cout << (*it).first->GetName() << "\t\t\t\t\t\t" << (*it).first->GetValue() << "\t\t\t\t\t\t" << (*it).second << "\n";

                    this->Register(*(*it).first, (*it).second);
                }
            }

            //            typename likelihood_components_iterator lit;
            //            for (lit = this->likelihood_components.begin(); lit != this->likelihood_components.end(); ++lit) {
            //
            //
            //                
            //                
            //            }

            //
            //            this->ShowData();
            INFO_DEBUG
                    this->ValidateData();

            //            data_iterator dit;
            //            for (dit = this->data_dictionary.begin(); dit != this->data_dictionary.end(); ++dit) {
            //                int area = (*dit).first;
            //
            //                area_iterator ait = this->areas.find(area);
            //                if (ait == this->areas.end()) {
            //                    std::cout << "Configuration Error: Data area id not found  \n";
            //                    mas_log << "Configuration Error: Data area id not found  \n";
            //
            //
            //                } else {
            //                    for (int i = 0; i < (*dit).second.size(); i++) {
            //
            //                        switch ((*dit).second[i]->type) {
            //
            //                            case CATCH_BIOMASS:
            //                                (*ait).second->catch_biomass_data.push_back((*dit).second[i]);
            //                                break;
            //                            case CATCH_PROPORTION_AT_AGE_N:
            //                                (*ait).second->catch_proportion_data_N.push_back((*dit).second[i]);
            //                                break;
            //                            case CATCH_PROPORTION_AT_AGE:
            //                                (*ait).second->catch_proportion_data.push_back((*dit).second[i]);
            //                                break;
            //                            case CATCH_PROPORTION_AT_LENGTH_N:
            //                                (*ait).second->catch_proportion_length_data_N.push_back((*dit).second[i]);
            //                                break;
            //                            case CATCH_PROPORTION_AT_LENGTH:
            //                                (*ait).second->catch_proportion_length_data.push_back((*dit).second[i]);
            //                                break;
            //                            case CATCH_MEAN_SIZE_AT_AGE:
            //                                (*ait).second->catch_mean_size_data.push_back((*dit).second[i]);
            //                                break;
            //                            case CATCH_MEAN_WEIGHT_AT_AGE:
            //                                (*ait).second->catch_mean_weight_at_age_data.push_back((*dit).second[i]);
            //                                break;
            //                            case SURVEY_BIOMASS:
            //                                (*ait).second->survey_biomass_data.push_back((*dit).second[i]);
            //                                break;
            //                            case SURVEY_PROPORTION_AT_AGE_N:
            //                                (*ait).second->survey_proportion_data_N.push_back((*dit).second[i]);
            //                                break;
            //                            case SURVEY_PROPORTION_AT_AGE:
            //                                (*ait).second->survey_proportion_data.push_back((*dit).second[i]);
            //                                break;
            //                            case SURVEY_PROPORTION_AT_LENGTH_N:
            //                                (*ait).second->survey_proportion_length_data_N.push_back((*dit).second[i]);
            //                                break;
            //                            case SURVEY_PROPORTION_AT_LENGTH:
            //                                (*ait).second->survey_proportion_length_data.push_back((*dit).second[i]);
            //                                break;
            //                            case SURVEY_MEAN_SIZE_AT_AGE:
            //                                (*ait).second->survey_mean_size_data.push_back((*dit).second[i]);
            //                                break;
            //                            default:
            //                                std::cout << "name = " << (*dit).second[i]->name << "\n";
            //                                std::cout << "Unknown data type found!\n" << *(*dit).second[i];
            //                                mas_log << "Unknown data type found!\n";
            //                        }
            //                    }
            //                }
            //
            //
            //            }

            //            for (fit = this->fleets.begin(); fit != this->fleets.end(); ++fit) {
            //                (*fit).second->InitializeFishingMortality();
            //            }

            // exit(0);

            //            for (fit = this->fleets.begin(); fit != this->fleets.end(); ++fit) {
            //                typename Fleet<REAL_T>::season_area_selectivity_iterator sait;
            //                for (sait = (*fit).second->season_area_selectivity.begin(); sait != (*fit).second->season_area_selectivity.end(); ++sait) {
            //                    int season = (*sait).first;
            //                    typename Fleet<REAL_T>::area_sectivity_iterator ait;
            //                    for (ait = (*sait).second.begin(); ait != (*sait).second.begin(); ++ait) {
            //                        int area = (*ait).first;
            //                        this->areas[area]->seasonal_fleet_operations[season].push_back((*fit).second);
            //                    }
            //                }
            //
            //                typename Fleet<REAL_T>::season_area_fishing_mortality_iterator sait;
            //                for (sait = (*fit).second->season_area_selectivity.begin(); sait != (*fit).second->season_area_selectivity.end(); ++sait) {
            //                    int season = (*sait).first;
            //                    typename Fleet<REAL_T>::area_sectivity_iterator ait;
            //                    for (ait = (*sait).second.begin(); ait != (*sait).second.begin(); ++ait) {
            //                        int area = (*ait).first;
            //                        this->areas[area]->seasonal_fleet_operations[season].push_back((*fit).second);
            //                    }
            //                }
            //            }


            //            std::cout << "Estimated parameters:\n";
            //            typename mas::ModelObject<REAL_T>::estimable_parameter_iterator eit;
            //            for (eit = this->estimated_parameters_map.begin(); eit != this->estimated_parameters_map.end(); ++eit) {
            //                std::cout << (*eit).first->GetName() << " " << (*eit).second << "\n";
            //            }



            //            for (int i = 0; i < this->survey_models.size(); i++) {
            //                std::cout << *this->survey_models[i] << "\n";
            //
            //                selectivity_model_iterator sit = this->selectivity_models.find(this->survey_models[i]->selectivity_model_id);
            //                if (sit != this->selectivity_models.end()) {
            //                    this->survey_models[i]->selectivity = (*sit).second;
            //                } else {
            //                    std::cout << "Configuration Error:  Survey lists a selectivity model that does not exist.\n";
            //                    mas_log << "Configuration Error:  Survey lists a selectivity model that does not exist.\n";
            //                    this->valid_configuration = false;
            //                }
            //
            //                for (int j = 0; j < this->survey_models[i]->area_ids.size(); j++) {
            //                    area_iterator it = this->areas.find(survey_models[i]->area_ids[j]);
            //
            //                    if (it != this->areas.end()) {
            //                        (*it).second->survey_models[survey_models[i]->population][survey_models[i]->id] = survey_models[i];
            //
            //                    } else {
            //                        std::cout << "Configuration Error:  Survey lists an area identifier that does not exist.\n";
            //                        mas_log << "Configuration Error:  Survey lists an area identifier that does not exist.\n";
            //                        this->valid_configuration = false;
            //                    }
            //                }
            //            }


            INFO_DEBUG
            mas_log << "\n\nEstimated Parameters\t\t\t\t\t\tInitial Value\t\t\t\t\t\tPhase\n";
            mas_log << "\nRecruitment:\n";
            recruitment_model_iterator rit;
            for (rit = this->recruitment_models.begin(); rit != this->recruitment_models.end(); ++rit) {
                typename mas::ModelObject<REAL_T>::estimable_parameter_iterator it;

                if ((*rit).second->used) {
                    for (it = (*rit).second->estimated_parameters_map.begin(); it != (*rit).second->estimated_parameters_map.end(); ++it) {
                        mas_log << (*it).first->GetName() << "\t\t\t\t\t\t" << (*it).first->GetValue() << "\t\t\t\t\t\t" << (*it).second << "\n";
                        this->Register(*(*it).first, (*it).second);
                    }


                }
                for (int i = 0; i < (*rit).second->random_variables.size(); i++) {
                    this->RegisterRandom(*(*rit).second->random_variables[i]);
                }

            }
            mas_log << "\nGrowth:\n";
            growth_model_iterator git;
            for (git = this->growth_models.begin(); git != this->growth_models.end(); ++git) {
                typename mas::ModelObject<REAL_T>::estimable_parameter_iterator it;
                if ((*git).second->used) {
                    for (it = (*git).second->estimated_parameters_map.begin(); it != (*git).second->estimated_parameters_map.end(); ++it) {
                        mas_log << (*it).first->GetName() << "\t\t\t\t\t\t" << (*it).first->GetValue() << "\t\t\t\t\t\t" << (*it).second << "\n";
                        this->Register(*(*it).first, (*it).second);
                    }
                }

            }

            INFO_DEBUG
            mas_log << "\nNLL Component:\n";
            likelihood_components_iterator lit;
            for (lit = this->likelihood_components.begin(); lit != this->likelihood_components.end(); ++lit) {

                if ((*lit).second->lambda->data.size() == 0) {

                }
                //                (*lit).second->years = this->nyears;
                //                (*lit).second->seasons = this->nseasons;
                //                (*lit).second->ages = this->ages.size();
                typename mas::ModelObject<REAL_T>::estimable_parameter_iterator it;
                if ((*lit).second->used) {
                    for (it = (*lit).second->estimated_parameters_map.begin(); it != (*lit).second->estimated_parameters_map.end(); ++it) {
                        mas_log << (*it).first->GetName() << "\t\t\t\t\t\t" << (*it).first->GetValue() << "\t\t\t\t\t\t" << (*it).second << "\n";
                        this->Register(*(*it).first, (*it).second);

                    }
                }
            }
            INFO_DEBUG
            mas_log << "\nSelectivity:\n";
            selectivity_model_iterator sit;
            for (sit = this->selectivity_models.begin(); sit != this->selectivity_models.end(); ++sit) {
                typename mas::ModelObject<REAL_T>::estimable_parameter_iterator it;
                if ((*sit).second->used) {
                    for (it = (*sit).second->estimated_parameters_map.begin(); it != (*sit).second->estimated_parameters_map.end(); ++it) {
                        mas_log << (*it).first->GetName() << "\t\t\t\t\t\t" << (*it).first->GetValue() << "\t\t\t\t\t\t" << (*it).second << "\n";
                        this->Register(*(*it).first, (*it).second);
                    }
                }

            }
            INFO_DEBUG
            mas_log << "\nMovement:\n";
            movement_model_iterator mit;
            for (mit = this->movement_models.begin(); mit != this->movement_models.end(); ++mit) {
                typename mas::ModelObject<REAL_T>::estimable_parameter_iterator it;
                if ((*mit).second->used) {
                    for (it = (*mit).second->estimated_parameters_map.begin(); it != (*mit).second->estimated_parameters_map.end(); ++it) {
                        mas_log << (*it).first->GetName() << "\t\t\t\t\t\t" << (*it).first->GetValue() << "\t\t\t\t\t\t" << (*it).second << "\n";
                        this->Register(*(*it).first, (*it).second);
                    }
                }
            }
            INFO_DEBUG
            mas_log << "\nNatural Mortality:\n";
            natural_mortality_model_iterator mmit;
            for (mmit = this->natural_mortality_models.begin(); mmit != this->natural_mortality_models.end(); ++mmit) {
                if ((*mmit).second->mortality_vector.size() != this->ages.size()) {
                    this->valid_configuration = false;
                    std::cout << "Configuration Error: Natural mortality vector for model \"" << (*mmit).second->id << "\" not eqaul to " << ages.size() << "\n";
                    mas_log << "Configuration Error: Natural mortality vector for model \"" << (*mmit).second->id << "\" not eqaul to " << ages.size() << "\n";

                }
                typename mas::ModelObject<REAL_T>::estimable_parameter_iterator it;
                if ((*mmit).second->used) {
                    for (it = (*mmit).second->estimated_parameters_map.begin(); it != (*mmit).second->estimated_parameters_map.end(); ++it) {
                        mas_log << (*it).first->GetName() << "\t\t\t\t\t\t" << (*it).first->GetValue() << "\t\t\t\t\t\t" << (*it).second << "\n";
                        this->Register(*(*it).first, (*it).second);
                    }


                }



                //            for (int a = 0; a <this->ages.size(); a++) {
                //                (*mmit).second->male_mortality[this->ages[a]] = (*mmit).second->male_mortality_vector[a];
                //                (*mmit).second->female_mortality[this->ages[a]] = (*mmit).second->female_mortality_vector[a];
                //
                //            }

            }



            INFO_DEBUG
            fishing_mortality_model_iterator fmmit;
            for (fmmit = this->fishing_mortality_models.begin(); fmmit != this->fishing_mortality_models.end(); ++fmmit) {
                typename mas::ModelObject<REAL_T>::estimable_parameter_iterator it;

                if ((*fmmit).second->used) {
                    //                    if ((*fmmit).second->fishing_mortality_type == mas::ESTIMATED) {
                    for (it = (*fmmit).second->estimated_parameters_map.begin(); it != (*fmmit).second->estimated_parameters_map.end(); ++it) {
                        mas_log << (*it).first->GetName() << "\t\t\t\t\t\t" << (*it).first->GetValue() << "\t\t\t\t\t\t" << (*it).second << "\n";
                        this->Register(*(*it).first, (*it).second);
                    }

                    for (int i = 0; i < (*fmmit).second->random_variables.size(); i++) {
                        this->RegisterRandom(*(*fmmit).second->random_variables[i]);
                    }
                    //                    }
                    //                    else {
                    //                        area_iterator ait;
                    //                        population_iterator pit;
                    //                        for (ait = this->areas.begin(); ait != this->areas.end(); ++ait) {
                    //                            for (pit = this->populations.begin(); pit != this->populations.end(); ++pit) {
                    //                                std::vector<std::vector<variable> > & fm = (*fmmit).second->f_delta_method_at_age[(*ait).second->id][(*pit).second->id];
                    //                                fm.resize(this->ages.size());
                    //                                for (int y = 0; y < this->nyears; y++) {
                    //                                    for (int s = 0; s < this->nseasons; s++) {
                    //                                        fm[y][s] = (*fmmit).second->fishing_mortality[y][s];
                    //                                    }
                    //                                }
                    //
                    //                            }
                    //                        }
                    //
                    //
                    //                    }
                } else {
                    std::cout << "Fishing mortality model is not used :{\n";
                }
            }
            INFO_DEBUG
            for (pit = this->populations.begin(); pit != this->populations.end(); ++pit) {
                (*pit).second->Initialize();
                typename mas::ModelObject<REAL_T>::estimable_parameter_iterator it;
                for (it = (*pit).second->estimated_parameters_map.begin(); it != (*pit).second->estimated_parameters_map.end(); ++it) {
                    mas_log << (*it).first->GetName() << "\t\t\t\t\t\t" << (*it).first->GetValue() << "\t\t\t\t\t\t" << (*it).second << "\n";
                    this->Register(*(*it).first, (*it).second);
                }

                for (int i = 0; i < (*pit).second->random_variables.size(); i++) {
                    this->RegisterRandom(*(*pit).second->random_variables[i]);
                }


            }



            INFO_DEBUG
            //            //Push EWAA and ESWAA to Growth module
            //            for (int i = 0; i <this->data.size(); i++) {
            //
            //                switch (data[i]->type) {
            //
            //                    case mas::CATCH_MEAN_WEIGHT_AT_AGE:
            //
            //                        switch (data[i]->sex_type) {
            //                            case mas::FEMALE:
            //                                mas::GrowthBase<REAL_T>::empirical_weight_at_age_female_fleet[data[i]->id] = data[i];
            //                                break;
            //
            //                            case mas::MALE:
            //                                mas::GrowthBase<REAL_T>::empirical_weight_at_age_male_fleet[data[i]->id] = data[i];
            //                                break;
            //
            //                            case mas::UNDIFFERENTIATED:
            //                                mas::GrowthBase<REAL_T>::empirical_weight_at_age_female_fleet[data[i]->id] = data[i];
            //                                mas::GrowthBase<REAL_T>::empirical_weight_at_age_male_fleet[data[i]->id] = data[i];
            //                                break;
            //                        }
            //
            //                        break;
            //
            //                    case mas::SURVEY_MEAN_WEIGHT_AT_AGE:
            //
            //                        switch (data[i]->sex_type) {
            //                            case mas::FEMALE:
            //                                mas::GrowthBase<REAL_T>::empirical_weight_at_age_female_survey[data[i]->id] = data[i];
            //                                break;
            //
            //                            case mas::MALE:
            //                                mas::GrowthBase<REAL_T>::empirical_weight_at_age_male_survey[data[i]->id] = data[i];
            //                                break;
            //
            //                            case mas::UNDIFFERENTIATED:
            //                                mas::GrowthBase<REAL_T>::empirical_weight_at_age_female_survey[data[i]->id] = data[i];
            //                                mas::GrowthBase<REAL_T>::empirical_weight_at_age_male_survey[data[i]->id] = data[i];
            //                                break;
            //                        }
            //
            //                        break;
            //
            //                    case mas::MEAN_WEIGHT_AT_AGE_SPAWNING:
            //
            //                        switch (data[i]->sex_type) {
            //                            case mas::FEMALE:
            //                                mas::GrowthBase<REAL_T>::empirical_weight_at_spawning_female[data[i]->id] = data[i];
            //                                break;
            //
            //                            case mas::MALE:
            //                                mas::GrowthBase<REAL_T>::empirical_weight_at_spawning_male[data[i]->id] = data[i];
            //                                break;
            //
            //                            case mas::UNDIFFERENTIATED:
            //                                mas::GrowthBase<REAL_T>::empirical_weight_at_spawning_female[data[i]->id] = data[i];
            //                                mas::GrowthBase<REAL_T>::empirical_weight_at_spawning_male[data[i]->id] = data[i];
            //                                break;
            //                        }
            //
            //                        break;
            //                    case mas::MEAN_WEIGHT_AT_AGE_SEASON_START:
            //
            //                        switch (data[i]->sex_type) {
            //                            case mas::FEMALE:
            //                                mas::GrowthBase<REAL_T>::empirical_weight_at_season_start_female[data[i]->id] = data[i];
            //                                break;
            //
            //                            case mas::MALE:
            //                                mas::GrowthBase<REAL_T>::empirical_weight_at_season_start_male[data[i]->id] = data[i];
            //                                break;
            //
            //                            case mas::UNDIFFERENTIATED:
            //                                mas::GrowthBase<REAL_T>::empirical_weight_at_season_start_female[data[i]->id] = data[i];
            //                                mas::GrowthBase<REAL_T>::empirical_weight_at_season_start_male[data[i]->id] = data[i];
            //                                break;
            //                        }
            //
            //                        break;
            //
            //                }
            //
            //
            //
            //
            //            }
            //
            //
            //
            // INFO_DEBUG
            //            mas::GrowthBase<REAL_T>::InterpolateEmpiricalWeightData();

            INFO_DEBUG
            if (!this->valid_configuration) {
                std::cout << "Configuration Error:  Invalid model configuration. See mas.log for errors.\n";
                mas_log << "Configuration Error:  Invalid model configuration. See mas.log for errors.\n";
                exit(0);
            }

        }

        std::unordered_map<int, std::shared_ptr<mas::Population<REAL_T> > >& GetPopulations() {
            return populations;
        }

        void DumpFishingMortality(std::string file) {
            std::ofstream out(file.c_str());

            typename std::unordered_map<int, std::shared_ptr<mas::FishingMortality<REAL_T> > >::iterator it;
            for (it = this->fishing_mortality_models.begin(); it != this->fishing_mortality_models.end(); ++it) {
                out << (*it).second->ToString() << "\n\n";
            }
        }

        void DumpSelectivity(std::string file) {
            std::ofstream out(file.c_str());

            selectivity_model_iterator sit;
            for (sit = this->selectivity_models.begin(); sit != this->selectivity_models.end(); ++sit) {
                typename mas::ModelObject<REAL_T>::estimable_parameter_iterator it;
                if ((*sit).second->used) {
                    out << (*sit).second->ToString() << "\n";

                    for (int i = 0; i < this->ages.size(); i++) {
                        out << this->ages[i] << ", " << (*sit).second->Evaluate(this->ages[i]) << "\n";
                    }
                }
                out << "\n\n\n";
            }


        }




    private:

    };


}


#endif /* MAS_INFORMATION_HPP */

