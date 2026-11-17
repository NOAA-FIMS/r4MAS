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


#include "QuadraObjective.hpp"
#include "Output.hpp"

namespace mas {

    template<typename REAL_T>
    class MASObjectiveFunction : public mas::backend::ObjectiveFunction<REAL_T> {
    public:

        mas::MAS<REAL_T> mas_instance;

        std::string data_path = "";
        std::string config_path = "";
        std::string ouput_path = "mas_output.json";
        int references = 0;
        bool compute_variance_for_derived_quantities =  true;



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
            mas_instance.fit_converged=this->converged;
            mas_instance.optimizer_status=this->optimizer_status;
            mas_instance.covariance_status=this->covariance_status;
            mas_instance.covariance_rank=this->covariance_rank;
            mas_instance.fixed_effect_report=this->fixed_effect_report;
            mas_instance.parameter_gradients.clear();
            for (unsigned id : this->fixed_ids)
                mas_instance.parameter_gradients.push_back(mas::backend::tape.Value(id));
            mas_instance.std_dev.Resize(mas_instance.variance_covaiance.GetRows());
            mas_instance.std_dev.setConstant(std::numeric_limits<REAL_T>::quiet_NaN());
            for (std::size_t i=0;i<this->fixed_effect_report.rows_m.size();++i)
                mas_instance.std_dev(i)=this->fixed_effect_report.rows_m[i].std_error_m;
        }

        void CalculateVarianceOfDerivedParameters() {
            std::vector<std::pair<std::string,variable*>> quantities;
            std::vector<REAL_T*> destinations;
            auto collect_series=[&](const std::string& prefix,const std::string& name,
                    std::vector<variable>& values,std::vector<REAL_T>& variances) {
                variances.resize(values.size());
                for (std::size_t i=0;i<values.size();++i) {
                    quantities.emplace_back(prefix+"/year_"+std::to_string(i+1)+"/"+name,&values[i]);
                    destinations.push_back(&variances[i]);
                }
            };
            for (auto& population_entry:mas_instance.info.populations) {
                auto& population=*population_entry.second;
                auto collect_sex=[&](std::unordered_map<int,Subpopulation<REAL_T>>& groups,
                        const std::string& sex) {
                    for (auto& area_entry:groups) {
                        auto& sub=area_entry.second;
                        const std::string prefix="population_"+std::to_string(population_entry.first)
                                +"/"+sex+"/area_"+std::to_string(area_entry.first);
                        collect_series(prefix,"spawning_stock_biomass",sub.spawning_stock_biomass,sub.spawning_stock_biomass_variance);
                        collect_series(prefix,"biomass",sub.biomass_total,sub.biomass_variance);
                        collect_series(prefix,"recruitment",sub.recruitment,sub.recruitment_variance);
                        collect_series(prefix,"F_over_F_msy",sub.F_over_F_msy,sub.F_over_F_msy_variance);
                    }
                };
                collect_sex(population.females,"female");
                collect_sex(population.males,"male");
            }
            const auto variances=this->BuildDerivedReport(quantities);
            for (std::size_t i=0;i<variances.size();++i) *destinations[i]=variances[i];
            mas_instance.derived_report=this->derived_report;
            mas_instance.unrecorded_derived_quantities=this->unrecorded_derived_quantities;
        }

        virtual void Finalize() {

            this->SetVarianceCovariance();
            mas_instance.Finalize();
            if (compute_variance_for_derived_quantities) {
                this->CalculateVarianceOfDerivedParameters();
            } else {
                mas_instance.derived_report=quadra::LaplaceProfiledDerivedReport();
                mas_instance.derived_report.message_m="Derived uncertainty reporting is disabled.";
                 typename std::unordered_map<int, mas::backend::intrusive_ptr<mas::Population<REAL_T> > >::iterator pit;
                for (pit = this->mas_instance.info.populations.begin();
                        pit != this->mas_instance.info.populations.end();
                        ++pit) {
                    (*pit).second->ComputeBiologicalReferencePoints();
                }

            }
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
                    const REAL_T std_error=i<static_cast<int>(this->fixed_effect_report.rows_m.size())
                            ? this->fixed_effect_report.rows_m[i].std_error_m
                            : std::numeric_limits<REAL_T>::quiet_NaN();
                    std::cout << std::setw(20) << std::left << (i + 1) << " ";
                    std::cout << std::setw(40) << std::left << mas_instance.info.estimated_parameters[i]->GetName();
                    std::cout << std::setw(20) << mas_instance.info.estimated_parameters[i]->info->value;
                    std::cout << std::setw(20) << std_error;

                    out << std::setw(20) << std::left << (i + 1) << " ";

                    out << std::setw(40) << std::left << mas_instance.info.estimated_parameters[i]->GetName();

                    out << std::setw(20) << mas_instance.info.estimated_parameters[i]->info->value;

                    out << std::setw(20) << std_error;

                    // std::cout << std::setw(20) << mas::backend::Variable<double>::tape.Value(mas.info.estimated_parameters[i]->info->id);
                    csv_out << (i + 1) << ",";
                    csv_out << mas_instance.info.estimated_parameters[i]->GetName() << ",";
                    csv_out << mas_instance.info.estimated_parameters[i]->info->value << ",";
                    csv_out << std_error << ",";
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

        virtual mas::backend::Variable<REAL_T> Evaluate() {
            variable f;
            mas_instance.phase = this->phase_m;
            mas_instance.Run(f);

            return f;
        }

        virtual void Objective_Function(mas::backend::Variable<REAL_T>& f) {
            mas_instance.phase = this->phase_m;
            mas_instance.Run(f);
        }



    };



}

#endif /* OBJECTIVEFUNCTION_HPP */




