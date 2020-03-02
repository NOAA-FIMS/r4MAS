/* 
 * File:   Mortality.hpp
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

#ifndef MAS_MORTALITY_HPP
#define MAS_MORTALITY_HPP

#include "Common.hpp"


namespace mas {

    template<typename REAL_T>
    struct NaturalMortality : mas::ModelObject<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;
        std::vector<variable> mortality_vector;

        std::unordered_map<REAL_T, variable> mortality;

        typedef typename std::unordered_map<REAL_T, variable>::iterator mortality_iterator;

        const variable Evaluate(const int& age_index) {
            return mortality_vector[age_index];

        }

        virtual const std::string ToJSONString() {
            std::stringstream ss;

            ss << "\"natural_mortality\" : {";
            ss << "\"id\" : " << this->id << ",\n";

            ss << "\"parameters\" : {\n";

            if (this->estimated_parameters.size() > 0) {
                ss << "\"estimated\" : \"true\",\n";
            }
            if (this->mortality_vector.size() > 0) {
                ss << "\"values\" : [" << this->mortality_vector[0];
            }
            for (int i = 1; i < this->mortality_vector.size(); i++) {
                ss << ", " << this->mortality_vector[i];
            }


            if (this->mortality_vector.size() > 0 && this->mortality_vector[0].bounded_m) {
                ss << "],\n";
                ss << "\"min\" : [" << this->mortality_vector[0].GetMinBoundary();

                for (int i = 1; i < this->mortality_vector.size(); i++) {
                    ss << ", " << this->mortality_vector[i].GetMinBoundary();
                }
                ss << "],\n";

                ss << "\"max\" : [" << this->mortality_vector[0].GetMaxBoundary();

                for (int i = 1; i < this->mortality_vector.size(); i++) {
                    ss << ", " << this->mortality_vector[i].GetMaxBoundary();
                }
                ss << "]\n";
            } else {
                ss << "]\n";
            }
            ss << "}\n}";
            return ss.str();
        }

        virtual const std::string Name() {
            return "Natural Mortality";
        }

    };

    enum FishingMortalityType {
        ESTIMATED = 0,
        FMETHOD
    };

    template<typename REAL_T>
    struct FishingMortality : mas::ModelObject<REAL_T> {
        FishingMortalityType fishing_mortality_type = ESTIMATED;
        bool needs_delta_method_update = false; // assumes estimated
        REAL_T contributions = 0.0; //contributions to the fmethod calc, used to average results
        typedef typename VariableTrait<REAL_T>::variable variable;
        std::vector<std::vector<variable> > fishing_mortality;
        std::unordered_map<int, std::unordered_map<int, std::vector<std::vector<variable> > > > f_delta_method_at_age; //area,pop,f mortality

        const variable Evaluate(const int& year, const int& season) {
            return fishing_mortality[year][season];
        }

        variable& Get(const int& year, const int& season) {
            return fishing_mortality[year][season];
        }

        void AverageFmethodContributions() {
//            if (this->contributions != 0) {
//                bool recording = variable::tape.recording;
//                variable::tape.recording = false;
//                for (int i = 0; i < this->fishing_mortality.size(); i++) {
//                    for (int j = 0; j < this->fishing_mortality[i].size(); j++) {
//
//                        this->fishing_mortality[i][j] /= this->contributions;
//
//                    }
//                }
//                this->contributions = 0.0;
//                variable::tape.recording = recording;
//            }
        }

        virtual std::string ToString() {
            std::stringstream ss;
            ss.setf(std::ios::fixed, std::ios::floatfield);
            
            ss<<"Fishing Mortality: "<<this->id<<"\n";
            for(int i =0; i < this->fishing_mortality.size(); i++){
                for(int j=0; j < this->fishing_mortality[i].size(); j++){
                    ss<<std::setw(12)<<std::left<<this->fishing_mortality[i][j];
                }
                ss<<std::endl;
                
            }
            return ss.str();
        }

        virtual const std::string ToJSONString() {
            std::stringstream ss;
            ss.setf(std::ios::fixed, std::ios::floatfield);
            ss << "\"fishing_mortality\" : {";
            ss << "\"id\" : " << this->id << ",\n";

            ss << "\"parameters\" : {\n";

            if (this->estimated_parameters.size() > 0) {
                ss << "\"estimated\" : \"true\",\n";
            }
            if (this->fishing_mortality.size() > 0) {
                if (this->fishing_mortality[0].size() > 0) {
                    ss << "\"values\" : [\n";

                    for (int i = 0; i < this->fishing_mortality.size(); i++) {
                        ss << "[";
                        for (int j = 0; j < this->fishing_mortality[i].size(); j++) {
                            ss << this->fishing_mortality[i][j];
                            if (j < this->fishing_mortality[i].size() - 1) {
                                ss << ",";
                            }
                        }
                        ss << "]";
                        if (i < this->fishing_mortality.size() - 1) {
                            ss << ",";
                        }
                        ss << "\n";
                    }


                }
            }


            if (this->fishing_mortality.size() > 0) {
                if (this->fishing_mortality[0].size() > 0) {
                    if (this->fishing_mortality[0][0].bounded_m) {
                        ss << "],\n";
                        ss << "\"min\" : [" << this->fishing_mortality[0][0].GetMinBoundary();

                        for (int i = 0; i < this->fishing_mortality.size(); i++) {
                            for (int j = 1; j < this->fishing_mortality[0].size(); j++) {
                                ss << ", " << this->fishing_mortality[i][j].GetMinBoundary();
                            }
                        }
                        ss << "],\n";

                        ss << "\"max\" : [" << this->fishing_mortality[0][0].GetMaxBoundary();

                        for (int i = 0; i < this->fishing_mortality.size(); i++) {
                            for (int j = 1; j < this->fishing_mortality[0].size(); j++) {
                                ss << ", " << this->fishing_mortality[i][j].GetMaxBoundary();
                            }
                        }
                        ss << "]\n";
                    } else {
                        ss << "]\n";
                    }
                }
            }
            ss << "}\n}";
            return ss.str();
        }

        virtual const std::string Name() {
            return "Natural Mortality";
        }

    };

    template<typename REAL_T >
    std::ostream& operator<<(std::ostream& out, const FishingMortality<REAL_T>& fm) {
        out << fm.ToString();
        return out;
    }

}


#endif /* MAS_MORTALITY_HPP */

