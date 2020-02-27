/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   EnsembleEngine.hpp
 * Author: mattadmin
 *
 * Created on April 4, 2019, 11:47 AM
 */

#ifndef ENSEMBLEENGINE_HPP
#define ENSEMBLEENGINE_HPP

#include <vector>
#include <map>
#include <set>
#include <iomanip>
#include "MAS.hpp"
#include "ObjectiveFunction.hpp"
#include "Options.hpp"

//#define MAS_ENSEMBLE

#ifdef MAS_ENSEMBLE

#include <mpi.h>

namespace mas {

    std::ostream& operator<<(std::ostream& out, const std::vector<int>& v) {
        for (int i = 0; i < v.size(); i++) {
            out << v[i] << " ";
        }
        //        std::cout << "\n";
        return out;
    }

    /**
     *Sorts by metric value.
     */
    template<typename REAL_T>
    struct less_accepted_model {

        bool operator()(const std::pair<REAL_T, std::vector<int> >& lhs, const std::pair<REAL_T, std::vector<int> >& rhs) const {
            return lhs.first < rhs.first;
        }
    };

    template<typename REAL_T>
    class EnsembleEngine {
        std::string config;
        std::string data;

        int ensemble_world_size;
        int ensemble_world_rank;
        int current = 0;
        int count;
        int current_child = 1;
        std::vector<int> working;
        std::vector<std::vector<int> > ensemble;
        REAL_T metric_threshold; //max goodness of fit value
        size_t ensemble_total_size = 10; //the total number of models to keep in the ensemble
        std::vector<int> original_model;
        std::vector<std::pair<REAL_T, std::vector<int> > > accepted_models;
        mas::Options<REAL_T> options;
        mas::Information<REAL_T> info;
        //        std::vector<std::pair<int, std::vector<int> > > current_running_models; //child, model
        std::map<int, std::vector<int> > current_running_models;
        std::vector<EnsembleUnit> models;
        std::stack<int> available_nodes;

        enum EnsembleCode {
            ERROR = -1,
            SUCCESS,
            DISCARD,
            KEEP,
            METRIC,
            QUIT,
            CANDIDATE,
            ACCEPTED_CANDIDATE,
            RECIEVE_DATA,
            RECIEVE_CONFIG
        };

        rapidjson::Document config_doc;

        bool ValidEnemble() {

            if (info.ensemble_units.size() == 0) {
                mas_log << "MAS ERROR: No Ensemble Units Found." << std::endl;
                return false;
            }
            int i, j;
            bool ret = true;
            typename mas::Information<REAL_T>::growth_model_iterator git;
            typename mas::Information<REAL_T>::recruitment_model_iterator rit;
            typename mas::Information<REAL_T>::natural_mortality_model_iterator nmit;
            typename mas::Information<REAL_T>::selectivity_model_iterator sit;
            typename mas::Information<REAL_T>::movement_model_iterator mit;
            this->ensemble.resize(info.ensemble_units.size());
            for (int i = 0; i < info.ensemble_units.size(); i++) {
                std::set<int> s;
                std::set<int>::iterator it;
                for (j = 0; j < info.ensemble_units[i].units.size(); j++) {
                    s.insert(info.ensemble_units[i].units[j]); //get rid of any double entries
                }

                switch (info.ensemble_units[i].type) {

                    case mas::GROWTH:
                        for (it = s.begin(); it != s.end(); ++it) {
                            this->ensemble[i].push_back((*it));
                            git = this->info.growth_models.find((*it));
                            if (git == this->info.growth_models.end()) {
                                std::cout << "MAS ERROR: Growth Ensemble Unit " << (*it) << " Not Found." << std::endl;
                                mas_log << "MAS ERROR: Growth Ensemble Unit " << (*it) << " Not Found." << std::endl;
                                ret = false;
                            }
                        }
                        break;
                    case mas::RECRUITMENT:
                        for (it = s.begin(); it != s.end(); ++it) {
                            this->ensemble[i].push_back((*it));
                            rit = this->info.recruitment_models.find((*it));
                            if (rit == this->info.recruitment_models.end()) {
                                std::cout << "MAS ERROR: Recruitment Ensemble Unit " << (*it) << " Not Found." << std::endl;
                                mas_log << "MAS ERROR: Recruitment Ensemble Unit " << (*it) << " Not Found." << std::endl;
                                ret = false;
                            }
                        }
                        break;
                    case mas::MORTALITY:
                        for (it = s.begin(); it != s.end(); ++it) {
                            this->ensemble[i].push_back((*it));
                            nmit = this->info.natural_mortality_models.find((*it));
                            if (nmit == this->info.natural_mortality_models.end()) {
                                std::cout << "MAS ERROR: Natural Mortality Ensemble Unit " << (*it) << " Not Found." << std::endl;
                                mas_log << "MAS ERROR: Natural Mortality Ensemble Unit " << (*it) << " Not Found." << std::endl;
                                ret = false;
                            }
                        }
                        break;
                    case mas::SELECTIVITY:
                        for (it = s.begin(); it != s.end(); ++it) {
                            this->ensemble[i].push_back((*it));
                            sit = this->info.selectivity_models.find((*it));
                            if (sit == this->info.selectivity_models.end()) {
                                std::cout << "MAS ERROR: Selectivity Ensemble Unit " << (*it) << " Not Found." << std::endl;
                                mas_log << "MAS ERROR: Selectivity Ensemble Unit " << (*it) << " Not Found." << std::endl;
                                ret = false;
                            }
                        }
                        break;
                    case mas::MOVEMENT:
                        for (it = s.begin(); it != s.end(); ++it) {
                            this->ensemble[i].push_back((*it));
                            mit = this->info.movement_models.find((*it));
                            if (mit == this->info.movement_models.end()) {
                                std::cout << "MAS ERROR: Movement Ensemble Unit " << (*it) << " Not Found." << std::endl;
                                mas_log << "MAS ERROR: Movement Ensemble Unit " << (*it) << " Not Found." << std::endl;
                                ret = false;
                            }
                        }
                        break;
                }
            }
            std::cout << std::endl;
            return true; //ret;
        }

        const std::string ConfigToString() {

        }

        /**
         * Creates new models from the possible combinations in the ensemble and sends them to child instances for model run. If the 
         * model completes the metric value is reviewed, if the metric falls within the metric threshold the output is preserved, 
         * otherwise the output is discarded.
         * 
         * @param count
         * @param current
         * @param working
         * @param ensemble
         */
        void EnsembleHandler(int& count, int current, std::vector<int>& working, std::vector<std::vector<int> >& ensemble) {

            if (ensemble.size() == 0) {
                return;
            }
            if (current == 0) {
                for (int i = 0; i < ensemble[0].size(); i++) {
                    std::vector<int> v;
                    v.push_back(ensemble[0][i]);
                    EnsembleHandler(count, current + 1, v, ensemble);
                }
            } else if (current < ensemble.size() - 1) {
                std::vector<int> v = working;
                v.push_back(0);
                for (int i = 0; i < ensemble[current].size(); i++) {
                    v[v.size() - 1] = ensemble[current][i];
                    EnsembleHandler(count, current + 1, v, ensemble);
                }
            } else {

                std::vector<int> v = working;
                v.push_back(0);
                for (int i = 0; i < ensemble[current].size(); i++) {
                    v[v.size() - 1] = ensemble[current][i];

                    //send to next child and increment working counter
                    //                    if ((this->current_running_models.size() == this->ensemble_world_size - 1) || (current_child == this->ensemble_world_size)) {
                    while (this->available_nodes.empty()) {

                        REAL_T m = 0.0;
                        for (int i = 1; i < this->ensemble_world_size; i++) {
                            MPI_Status status;
                            // Probe for an incoming message from process zero
                            MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                            if (status.count_lo) {

                                m = 0.0;
                                //collect metrics
                                MPI_Recv(&m, 1, MPI_DOUBLE, status.MPI_SOURCE, 0, MPI_COMM_WORLD,
                                        MPI_STATUS_IGNORE);
                                std::cout.precision(10);
                                std::cout << std::setw(6) << status.MPI_SOURCE << " goodness of fit = "  << m << std::endl;
                                this->accepted_models.push_back(std::pair<REAL_T, std::vector<int> >(m, this->current_running_models[i]));
                                this->available_nodes.push(status.MPI_SOURCE);
                            }
                        }

                        if (this->accepted_models.size()> this->ensemble_total_size) {
                            std::sort(this->accepted_models.begin(), this->accepted_models.end(), mas::less_accepted_model<REAL_T>());
                            this->accepted_models.resize(this->ensemble_total_size);

                            std::cout << "\nCurrent Ensemble:\n";
                            for (int i = 0; i < this->accepted_models.size(); i++) {
                                std::cout << std::setw(3) << " " << this->accepted_models[i].first << std::endl;
                            }
                            std::cout << "\n\n";
                        }

                        //                        this->current_running_models.clear();
                    }

                    int message[] = {CANDIDATE, count, (int) v.size() + 1, 0};
                    current_child = this->available_nodes.top();
                    this->available_nodes.pop();
                    this->current_running_models[current_child] = v; //.push_back(std::pair<int, std::vector<int> >(current_child, v));
                    MPI_Send(&message, 4, MPI_INT, current_child, 0, MPI_COMM_WORLD);
                    MPI_Send(v.data(), (int) v.size(), MPI_INT, current_child, 0, MPI_COMM_WORLD);
                    //                    current_child++;
                    count++;
                }

            }
        }

        void RunParent() {

            std::cout.setstate(std::ios_base::failbit);

            for (int i = 1; i < ensemble_world_size; i++) {
                this->available_nodes.push(i);
            }


            //            this->info.ParseConfig(options.config);
            std::ifstream data_in;
            data_in.open(options.data.c_str());
            std::stringstream ss;
            std::string line;
            if (!data_in.good()) {
                std::cout << " data not good" << std::endl;
                exit(0);
            }

            while (data_in.good()) {
                std::getline(data_in, line);
                ss << line << std::endl;
            }
            this->data = ss.str();

            std::ifstream config_in;
            config_in.open(options.config.c_str());
            ss.str("");
            line = "";
            while (config_in.good()) {
                std::getline(config_in, line);
                ss << line << std::endl;
            }
            this->config = ss.str();
            this->config_doc.Parse(this->config.c_str());
            this->info.ParseConfig(this->config_doc);

            std::cout.clear();

            if (this->ValidEnemble()) {

                int message[] = {0, 0, 0, 0};



                //send config and data to children
                for (int i = 1; i < this->ensemble_world_size; i++) {
                    int dl = this->data.size();
                    message[0] = RECIEVE_DATA;
                    MPI_Send(&message, 4, MPI_INT, i, 0, MPI_COMM_WORLD);
                    MPI_Send(&dl, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                    MPI_Send(&data[0], (int) this->data.size(), MPI_CHAR, i, 0, MPI_COMM_WORLD);

                    int cl = this->config.size();
                    message[0] = RECIEVE_CONFIG;
                    MPI_Send(&message, 4, MPI_INT, i, 0, MPI_COMM_WORLD);
                    MPI_Send(&cl, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                    MPI_Send(&config[0], (int) this->config.size(), MPI_CHAR, i, 0, MPI_COMM_WORLD);
                }



                for (int i = 0; i < this->models.size(); i++) {
                    this->original_model.push_back((*this->models[i].val).value.GetInt());
                }

                //send data to all children
                int current = 0;
                std::vector<int> working;

                std::cout << "Starting...\n";
                this->EnsembleHandler(count, current, working, ensemble);

                while (this->available_nodes.size() < this->ensemble_world_size - 1) {
                    REAL_T m = 0.0;
                    //                    for (int i = 1; i < this->ensemble_world_size; i++) {
                    MPI_Status status;
                    // Probe for an incoming message from process zero
                    MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                    if (status.count_lo) {

                        m = 0.0;
                        //collect metrics
                        MPI_Recv(&m, 1, MPI_DOUBLE, status.MPI_SOURCE, 0, MPI_COMM_WORLD,
                                MPI_STATUS_IGNORE);
                        std::cout << std::setw(6) << status.MPI_SOURCE << " goodness of fit = " << m << std::endl;
                        this->accepted_models.push_back(std::pair<REAL_T, std::vector<int> >(m, this->current_running_models[status.MPI_SOURCE]));
                        this->available_nodes.push(status.MPI_SOURCE);
                    }
                    //                    }
                }

                if (this->accepted_models.size()> this->ensemble_total_size) {
                    std::sort(this->accepted_models.begin(), this->accepted_models.end(), mas::less_accepted_model<REAL_T>());
                    this->accepted_models.resize(this->ensemble_total_size);
                    std::cout << "\nCurrent Ensemble:\n";
                    for (int i = 0; i < this->accepted_models.size(); i++) {
                        std::cout << std::setw(3) << " " << this->accepted_models[i].first << std::endl;
                    }
                    std::cout << "\n\n";
                }

                std::cout << "Collecting output from top " << this->ensemble_total_size << " best fitting models...\n";
                std::vector<std::string> accepted_ensemble_outputs;
                int current_child = 1;

                //rerun best in ensemble
                for (int i = 0; i < this->accepted_models.size(); i++) {

                    while (this->available_nodes.empty()) {

                        for (int j = 1; j < this->ensemble_world_size; j++) {
                            MPI_Status status;
                            MPI_Probe(j, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                            if (status.count_lo) {
                                //                                std::cout << "recieved output of size " << status.count << " from node " << j << std::endl;
                                std::string accepted_output;
                                int length;
                                MPI_Recv(&length, 1, MPI_INT, j, 0, MPI_COMM_WORLD,
                                        MPI_STATUS_IGNORE);

                                accepted_output.resize(length + 1024);

                                MPI_Recv((void*) &accepted_output[0], length + 1000000, MPI_CHAR, j, 0,
                                        MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                                accepted_ensemble_outputs.push_back(accepted_output);
                                this->available_nodes.push(j);
                            }
                        }

                    }
                    current_child = this->available_nodes.top();
                    this->available_nodes.pop();
                    int message[] = {ACCEPTED_CANDIDATE, count, (int) this->accepted_models[i].second.size() + 1, 0};
                    MPI_Send(&message, 4, MPI_INT, current_child, 0, MPI_COMM_WORLD);
                    MPI_Send(this->accepted_models[i].second.data(), (int) this->accepted_models[i].second.size(), MPI_INT, current_child, 0, MPI_COMM_WORLD);
                    this->current_running_models[current_child] = this->accepted_models[i].second;

                }

                //gather output

                //mpi probe hangs here, so we'll just build a set of working 
                //nodes and retrieve output
                std::set<int> working_set;
                for (int i = 1; i < this->ensemble_world_size; i++) {
                    working_set.insert(i);
                }
                std::set<int>::iterator it;
                while (!this->available_nodes.empty()) {
                    it = working_set.find(this->available_nodes.top());
                    working_set.erase(it);
                    this->available_nodes.pop();
                }

                for (it = working_set.begin(); it != working_set.end(); ++it) {
                    int i = (*it);

                    std::string accepted_output;
                    int length;

                    MPI_Recv(&length, 1, MPI_INT, i, 0, MPI_COMM_WORLD,
                            MPI_STATUS_IGNORE);

                    accepted_output.resize(length + 1024);

                    MPI_Recv((void*) &accepted_output[0], length + 1000000, MPI_CHAR, i, 0,
                            MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    accepted_ensemble_outputs.push_back(accepted_output);

                }


                std::cout << "done!." << std::endl;
                std::vector<rapidjson::Document> output_docs;
                rapidjson::Document doc;
                std::stringstream ss;
                ss << "{\n\"ensemble\" : [\n";
                for (int i = 0; i < accepted_ensemble_outputs.size(); i++) {
                    ss << accepted_ensemble_outputs[i] << "";
                    if (i < accepted_ensemble_outputs.size() - 1) {
                        ss << ","<<std::endl;
                    }
                }
                ss << "]\n}";
                doc.Parse(ss.str().c_str());
                rapidjson::StringBuffer buffer;
                rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
                doc.Accept(writer);

                std::ofstream out;
                out.open("mas_ensemble_output.json");

                out << ss.str(); //buffer.GetString();

            } else {
                std::cout << "MAS ERROR: Invalid Ensemble Definition." << std::endl;
                mas_log << "MAS ERROR: Invalid Ensemble Definition." << std::endl;
            }


            //shutdown children
            int message[] = {QUIT, 0, 0, 0};
            for (int i = 1; i < this->ensemble_world_size; i++) {
                MPI_Send(&message, 4, MPI_INT, i, 0, MPI_COMM_WORLD);
            }
            std::cout << "Done. " << count << " possible models analyzed.\n";




        }

        REAL_T RunChildModel(const std::vector<int>& candidate) {

            atl::VariableIdGenerator::instance()->reset();
            atl::Variable<REAL_T>::tape.Reset();
            atl::Variable<REAL_T>::tape.recording = true;


            for (int i = 0; i < this->info.ensemble_units.size(); i++) {
                this->info.ensemble_units[i].val->value.SetInt(candidate[i]);
            }


            mas::MASObjectiveFunction<REAL_T> objective_function;
            std::stringstream ss;
            std::stringstream dd;
            ss << config;
            objective_function.mas_instance.info.ParseConfig(this->config_doc);
            dd << this->data;
            objective_function.mas_instance.info.ParseData(dd);

            objective_function.mas_instance.info.CreateModel();


            if (objective_function.mas_instance.info.valid_configuration) {

                objective_function.Initialize();
                //create an instance of a L-BFGS minimizer
                atl::LBFGS<double> fm;
                fm.max_line_searches = 1000;
                fm.SetPrintWidth(2);
                fm.SetTolerance(options.tol);


                //set the objective function
                fm.SetObjectiveFunction(&objective_function);
                //                std::cout.clear();
                //run the minimizer
                fm.Run();

                //return Pearson's chi-squared test
                REAL_T gof = objective_function.mas_instance.ComputeGoodnessOfFit();

                if (gof == gof) {
                    return gof;
                } else {
                    return std::numeric_limits<REAL_T>::max();
                }

            } else {
                return std::numeric_limits<REAL_T>::max();
            }

        }

        std::string RunAcceptedChildModel(const std::vector<int>& candidate) {


            atl::VariableIdGenerator::instance()->reset();
            atl::Variable<REAL_T>::tape.Reset();
            atl::Variable<REAL_T>::tape.recording = true;

            for (int i = 0; i < this->info.ensemble_units.size(); i++) {
                this->info.ensemble_units[i].val->value.SetInt(candidate[i]);
            }


            mas::MASObjectiveFunction<REAL_T> objective_function;
            std::stringstream ss;
            std::stringstream dd;

            ss << config;
            objective_function.mas_instance.info.ParseConfig(config_doc);

            dd << this->data;
            objective_function.mas_instance.info.ParseData(dd);

            objective_function.mas_instance.info.CreateModel();

            objective_function.Initialize();


            //create an instance of a L-BFGS minimizer
            atl::LBFGS<double> fm;
            fm.SetPrintWidth(2);
            fm.SetTolerance(options.tol);


            //set the objective function
            fm.SetObjectiveFunction(&objective_function);

            //run the minimizer
            fm.Run();

            objective_function.mas_instance.variance_covaiance = objective_function.GetVarianceCovariance();
            objective_function.mas_instance.std_dev.Resize(objective_function.mas_instance.variance_covaiance.rows);
            for (int i = 0; i < objective_function.mas_instance.std_dev.GetSize(); i++) {
                objective_function.mas_instance.std_dev(i) =
                        std::sqrt(objective_function.mas_instance.variance_covaiance(i, i));
            }
            mas::JSONOutputGenerator<REAL_T> out;
            return out.GenerateOutput(objective_function.mas_instance);

        }

        void RunChild() {
            //turn off io on children
            std::cout.setstate(std::ios_base::failbit);

            int code[] = {-999, -999, -999, -999};
            int candidate_id;
            std::vector<int> mconfig;
            int clength;
            REAL_T metric_value;
            std::string accepted_output;
            int data_length;
            int config_length;


            while (code[0] != QUIT) {

                MPI_Recv(&code, 4, MPI_INT, 0, 0, MPI_COMM_WORLD,
                        MPI_STATUS_IGNORE);
                if (code[0] == QUIT) {
                    return;
                }
                switch (code[0]) {

                    case RECIEVE_DATA:
                        MPI_Recv(&data_length, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,
                                MPI_STATUS_IGNORE);
                        this->data.resize(data_length + 1024);
                        MPI_Recv((void*) &data[0], data_length + 1000000, MPI_CHAR, 0, 0,
                                MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                        break;

                    case RECIEVE_CONFIG:
                        MPI_Recv(&config_length, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,
                                MPI_STATUS_IGNORE);
                        this->config.resize(config_length + 1024);
                        MPI_Recv((void*) &config[0], config_length + 100000, MPI_CHAR, 0, 0, MPI_COMM_WORLD,
                                MPI_STATUS_IGNORE);

                        this->config_doc.Parse(this->config.c_str());
                        this->info.ParseConfig(this->config_doc);
                        break;

                    case CANDIDATE:

                        candidate_id = code[1];
                        clength = code[2];
                        mconfig.resize(clength);
                        MPI_Recv(&mconfig[0], clength, MPI_INT, 0, 0, MPI_COMM_WORLD,
                                MPI_STATUS_IGNORE);
                        //run model by calling RunChildModel
                        metric_value = 0.0;
                        metric_value = this->RunChildModel(mconfig);
                        MPI_Send(&metric_value, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);

                        break;


                    case ACCEPTED_CANDIDATE:
                        candidate_id = code[1];
                        clength = code[2];
                        mconfig.resize(clength);
                        MPI_Recv(&mconfig[0], clength, MPI_INT, 0, 0, MPI_COMM_WORLD,
                                MPI_STATUS_IGNORE);
                        //run model by calling RunChildModel
                        accepted_output = this->RunAcceptedChildModel(mconfig);
                        clength = accepted_output.size();
                        MPI_Send(&clength, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
                        MPI_Send(&accepted_output[0], (int) accepted_output.size(), MPI_CHAR, 0, 0, MPI_COMM_WORLD);
                        break;

                    default:
                        break;



                }



            }
        }

    public:

        void SetOptions(const mas::Options<REAL_T>& options) {
            this->options = options;
        }

        void Run() {
            //        this->ensemble_id_builder(count, current, this->working, this->ensemble);

            // Initialize the MPI environment
            MPI_Init(NULL, NULL);

            // Get the number of processes   
            MPI_Comm_size(MPI_COMM_WORLD, &ensemble_world_size);

            // Get the rank of the process
            MPI_Comm_rank(MPI_COMM_WORLD, &ensemble_world_rank);


            if (ensemble_world_rank == 0) {

                std::cout << std::endl;
                std::cout << util::center("                ███╗ ███╗  █████╗  ███████╗", 60) << std::endl;
                std::cout << util::center("                ████╗ ████║██╔══██╗██╔════╝", 60) << std::endl;
                std::cout << util::center("                ██╔████╔██║███████║███████╗", 60) << std::endl;
                std::cout << util::center("                ██║╚██╔╝██║██╔══██║╚════██║", 60) << std::endl;
                std::cout << util::center("                ██║ ╚═╝ ██║██║  ██║███████║", 60) << std::endl;
                std::cout << util::center("Meta-Population Assessment System", 60) << std::endl;
                std::cout << util::center("National Oceanic and Atmospheric Administration", 60) << std::endl;
                std::cout << util::center("National Marine Fisheries Service", 60) << std::endl;
                std::cout << util::center("Ensemble Engine", 60) << std::endl;
                this->RunParent();


            } else {
                this->RunChild();
            }

            MPI_Finalize();

        }


    };
}

#endif

#endif /* ENSEMBLEENGINE_HPP */

