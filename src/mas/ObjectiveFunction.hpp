/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ObjectiveFunction.hpp
 * Author: mattadmin
 *
 * Created on March 28, 2019, 9:32 AM
 */

#ifndef OBJECTIVEFUNCTION_HPP
#define OBJECTIVEFUNCTION_HPP


#include "third_party/ATL/lib/Optimization.hpp"
#include "Output.hpp"

namespace mas {

    template<typename REAL_T>
    class MASObjectiveFunction : public atl::ObjectiveFunction<REAL_T> {
    public:

        mas::MAS<REAL_T> mas_instance;

        std::string data_path = "";
        std::string config_path = "";
        std::string ouput_path = "mas_output.json";
	int references = 0;
	
	

        typedef typename mas::VariableTrait<REAL_T>::variable variable;

        virtual void Initialize() {
#ifdef MAS_COMMAND_TOOL
            mas_instance.Initialize(config_path, data_path);
#endif
            for (int i = 0; i < mas_instance.info.estimated_parameters.size(); i++) {
                this->RegisterParameter(*mas_instance.info.estimated_parameters[i], mas_instance.info.estimated_phase[i]);
            }

            for (int i = 0; i < mas_instance.info.random_variables.size(); i++) {
                this->RegisterRandomVariable(*(mas_instance.info.random_variables[i]));
            }
        }

        void SetVarianceCovariance() {
            mas_instance.variance_covaiance = this->GetVarianceCovariance();
            mas_instance.std_dev.Resize(mas_instance.variance_covaiance.GetRows());
            for (int i = 0; i < mas_instance.std_dev.GetSize(); i++) {
                mas_instance.std_dev(i) = std::sqrt(mas_instance.variance_covaiance(i, i));
            }

        }
	
	
    	void CalculateVarianceOfDerivedParameters(){
    		typename std::unordered_map<int, atl::intrusive_ptr<mas::Population<REAL_T> > >::iterator pit;

    		for (pit = this->mas_instance.info.populations.begin(); pit != this->mas_instance.info.populations.end();
    				++pit) {

    			mas::Population<REAL_T> *population = (*pit).second.get();
    			typename std::unordered_map<int, Subpopulation<REAL_T> >::iterator spit;

    			for(spit = population->females.begin(); spit != population->females.end(); ++spit){

    				for(int i =0; i < (*spit).second.spawning_stock_biomass.size(); i++){
    					std::cout<<"spawning stock biomass variance for subpopulation "<<(*spit).second.id<<"\n";
    				std::vector<uint32_t> pid;


					for (int j = 0;j< (*spit).second.growth_model->estimated_parameters.size(); j++) {

							pid.push_back((*spit).second.growth_model->estimated_parameters[j]->info->id);
				        }
					
					for (int j = 0;j< (*spit).second.recruitment_model->estimated_parameters.size(); j++) {

							pid.push_back((*spit).second.recruitment_model->estimated_parameters[j]->info->id);
				        }
					std::cout<<this->GetVarianceOfDerivedValue((*spit).second.spawning_stock_biomass[i].info->id, pid)<<"\n";

    				}
    			}

    		}
		}

        virtual void Finalize() {
           
            this->SetVarianceCovariance();
            mas_instance.Finalize();
	    this->CalculateVarianceOfDerivedParameters();
//            mas_instance.Report();
            mas::JSONOutputGenerator<REAL_T> json;
            std::ofstream output(this->ouput_path.data());
            output << json.GenerateOutput(mas_instance);

        }

        virtual void OutputVarCovar() {
            std::ofstream out;
            out.open("variance_covariance.txt");
            std::ofstream csv_out;
            csv_out.open("variance_covariance.csv");
            std::cout.precision(6);
            std::cout.unsetf(std::ios_base::fixed);

            if (mas_instance.info.estimated_parameters.size() > 1) {

                auto Mat_varcovar = this->GetVarianceCovariance();

                std::cout << "\nVariance-Covariance Matrix for the Estimated Parameters\n";
                std::cout << std::setw(20) << std::left << "Parameter" << " " << std::setw(40) << std::left << "Name" << std::setw(20) << "Value" << std::setw(20) << "StdDev";

                out << "\nVariance-Covariance Matrix for the Estimated Parameters\n";
                out << std::setw(20) << std::left << "Parameter" << " " << std::setw(40) << std::left << "Name" << std::setw(20) << "Value" << std::setw(20) << "StdDev";

                csv_out << "\nVariance-Covariance Matrix for the Estimated Parameters\n";
                csv_out << "Parameter ,Name,Value,StdDev,";


                for (int i = 1; i <= mas_instance.info.estimated_parameters.size(); ++i) {
                    std::cout << std::setw(20) << i;
                    out << std::setw(20) << i;
                    csv_out << i << ",";
                }
                std::cout << "\n";
                out << "\n";
                csv_out << "\n";

                for (int i = 0; i < mas_instance.info.estimated_parameters.size(); i++) {
                    std::cout << std::setw(20) << std::left << (i + 1) << " ";
                    std::cout << std::setw(40) << std::left << mas_instance.info.estimated_parameters[i]->GetName();
                    std::cout << std::setw(20) << mas_instance.info.estimated_parameters[i]->info->value;
                    std::cout << std::setw(20) << std::sqrt(Mat_varcovar(i, i));

                    out << std::setw(20) << std::left << (i + 1) << " ";

                    out << std::setw(40) << std::left << mas_instance.info.estimated_parameters[i]->GetName();

                    out << std::setw(20) << mas_instance.info.estimated_parameters[i]->info->value;

                    out << std::setw(20) << std::sqrt(Mat_varcovar(i, i));

                    // std::cout << std::setw(20) << atl::Variable<double>::tape.Value(mas.info.estimated_parameters[i]->info->id);
                    csv_out << (i + 1) << ",";
                    csv_out << mas_instance.info.estimated_parameters[i]->GetName() << ",";
                    csv_out << mas_instance.info.estimated_parameters[i]->info->value << ",";
                    csv_out << std::sqrt(Mat_varcovar(i, i)) << ",";
                    for (int j = 0; j < mas_instance.info.estimated_parameters.size(); ++j) {
                        std::cout << std::setw(20) << Mat_varcovar(i, j);
                        out << std::setw(20) << Mat_varcovar(i, j);
                        csv_out << Mat_varcovar(i, j) << ",";
                    }
                    std::cout << "\n";
                    out << "\n";
                    csv_out << "\n";
                }

                std::cout << "\n\n";
                out << "\n\n";

            }
        }

        virtual const atl::Variable<REAL_T> Evaluate() {
            variable f;
            mas_instance.phase = this->phase_m;
            mas_instance.Run(f);

            return f;
        }

        virtual void Objective_Function(atl::Variable<REAL_T>& f) {
            mas_instance.phase = this->phase_m;
            mas_instance.Run(f);
        }



    };



}

#endif /* OBJECTIVEFUNCTION_HPP */




