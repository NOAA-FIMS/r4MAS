/* 
 * File:   Movement.hpp
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

#ifndef MAS_MOVEMENT_HPP
#define MAS_MOVEMENT_HPP

#include "Common.hpp"

namespace mas {

    /**
     *Movement probabilities for a box-transfer model for a given year.
     * 
     * Probability information is defined by season and areas.
     */
    template<typename REAL_T>
    struct Movement : mas::ModelObject<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;
        std::vector<std::vector<std::vector<variable> > > recruit_connectivity;
        std::vector<std::vector<std::vector<variable> > > male_connectivity;
        std::vector<std::vector<std::vector<variable> > > female_connectivity;

        virtual const std::string Name() {
            return "Box Transfer";
        }

        void Prepare() {
            if (this->estimated_parameters.size()) {
                for (int i = 0; i < recruit_connectivity.size(); i++) {
                    for (int j = 0; j < recruit_connectivity[i].size(); j++) {
                        variable r = 0.0;
                        variable f = 0.0;
                        variable m = 0.0;
                        for (int k = 0; k < recruit_connectivity[i][j].size(); k++) {
                            r += recruit_connectivity[i][j][k];
                            f += female_connectivity[i][j][k];
                            m += male_connectivity[i][j][k];
                        }
                        for (int k = 0; k < recruit_connectivity[i][j].size(); k++) {
                            recruit_connectivity[i][j][k] /= r;
                            female_connectivity[i][j][k] /= f;
                            male_connectivity[i][j][k] /= m;
                        }
                    }
                }
            }
        }

        virtual const std::string ToJSONString() {
            std::stringstream ss;
            ss.setf(std::ios::fixed, std::ios::floatfield);
            ss << "\"movement\": {\n";
            ss << "\"id\":" << this->id << ",\n";
            ss << "\"recruits\" : [\n";

            for (int i = 0; i < this->recruit_connectivity.size(); i++) {
                ss << "[";
                for (int j = 0; j < this->recruit_connectivity[i].size(); j++) {
                    ss << "[";
                    for (int k = 0; k < this->recruit_connectivity[i][j].size(); k++) {
                        ss << this->recruit_connectivity[i][j][k];
                        if (k != this->recruit_connectivity[i][j].size() - 1) {
                            ss << ",";
                        } else {
                            ss << "]";
                        }
                    }
                    if (j != this->recruit_connectivity[i].size() - 1) {
                        ss << ",\n";
                    } else {
                        ss << "]\n";
                    }
                }
                //                ss<<"]"; 
            }
            ss << "],\n";

            ss << "\"male\" : [\n";

            for (int i = 0; i < this->male_connectivity.size(); i++) {
                ss << "[";
                for (int j = 0; j < this->male_connectivity[i].size(); j++) {
                    ss << "[";
                    for (int k = 0; k < this->male_connectivity[i][j].size(); k++) {
                        ss << this->male_connectivity[i][j][k];
                        if (k != this->male_connectivity[i][j].size() - 1) {
                            ss << ",";
                        } else {
                            ss << "]";
                        }
                    }
                    if (j != this->male_connectivity[i].size() - 1) {
                        ss << ",\n";
                    } else {
                        ss << "]\n";
                    }
                }
                //                ss<<"]"; 
            }
            ss << "],\n";

            ss << "\"female\" : [\n";

            for (int i = 0; i < this->female_connectivity.size(); i++) {
                ss << "[";
                for (int j = 0; j < this->female_connectivity[i].size(); j++) {
                    ss << "[";
                    for (int k = 0; k < this->female_connectivity[i][j].size(); k++) {
                        ss << this->female_connectivity[i][j][k];
                        if (k != this->female_connectivity[i][j].size() - 1) {
                            ss << ",";
                        } else {
                            ss << "]";
                        }
                    }
                    if (j != this->female_connectivity[i].size() - 1) {
                        ss << ",\n";
                    } else {
                        ss << "]\n";
                    }
                }
                //                ss<<"]"; 
            }
            ss << "]\n";

            ss << "}";
            return ss.str();
        }

    };


}


#endif /* MAS_MOVEMENT_HPP */




