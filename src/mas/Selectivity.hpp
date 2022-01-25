/* 
 * File:   Selectivity.hpp
 * 
 * Author: Matthew Supernaw
 * National Oceanic and Atmospheric Administration
 * National Marine Fisheries Service 
 * Sustainable Fisheries Division
 * St. Petersburg, FL, 33701
 * 
 * Created on September 16, 2016, 12:33 PM
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

#ifndef MAS_SELECTIVITY_HPP
#define MAS_SELECTIVITY_HPP

#include "Common.hpp"
#include <cassert>
#include <vector>

#define SELX_SQUARE(x) ((x)*(x))

namespace mas {

    template<typename REAL_T>
    struct SelectivityBase : mas::ModelObject<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;
        std::vector<REAL_T> lambdas;
        variable sigma = 0.85;
        variable sigma2 = 0.7225;
        variable cv = 0.01;

        std::vector<variable> selectivity;

        void Update(const std::vector<variable>& ages) {
            if (this->selectivity.size() != ages.size()) {
                this->selectivity.resize(ages.size());
            }
            for (size_t i = 0; i < this->selectivity.size(); i++) {
                this->selectivity[i] = this->Evaluate(ages, i);
            }
        }

        virtual const variable& Get(const std::vector<variable>& ages, size_t index) {
            return selectivity[index];
        }

        virtual const variable Evaluate(const variable& age) = 0;

        virtual const variable Evaluate(const std::vector<variable>& ages, size_t index) = 0;

        virtual const std::string Name() {
            return "SelectivityBase";
        }

        virtual variable LikelihoodComponent(int phase) {

            assert(this->initial_parameter_values.size() == this->estimated_parameters.size());
            variable ret = 0.0;
            for (int i = 0; i < this->estimated_parameters.size(); i++) {
                if (this->estimated_phase[i] <= phase) {

                    variable p = *this->estimated_parameters[i];
                    if (p > 0.0) {

                        this->sigma2 = mas::log(this->cv * this->cv + 1.0);
                        this->sigma = mas::pow(this->sigma2, 0.5);
                        ret += 0.5 * std::log(2.0 * M_PI) + std::log(this->initial_parameter_values[i]);
                        ret += (mas::log(sigma) + 0.5 * (SELX_SQUARE(std::log(this->initial_parameter_values[i]) - mas::log(p))) / sigma2);
                    } else {
                        std::cout << "Warning:  cannot do prior in log space for parm with min <= 0.0\n";
                    }
                } else {
                    //                    ret = 1.0;
                }
            }

            return ret;
        }

    };

    template<typename REAL_T>
    struct LogisticSel : SelectivityBase<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;
        variable a50; //age of 50% selectivity 
        variable s; //the rate of increase in selectivity at a50

        /**
         * Age based logistic selectivity
         * @param a - age
         * @return 
         */
        virtual const variable Evaluate(const variable& a) {
            return static_cast<REAL_T> (1.0) / (static_cast<REAL_T> (1.0) + mas::exp(-1.0 * (a - a50) / s));
        }

        virtual const variable Evaluate(const std::vector<variable>& ages, size_t index) {
            return static_cast<REAL_T> (1.0) / (static_cast<REAL_T> (1.0) + mas::exp(-1.0 * (ages[index] - a50) / s));
        }

        virtual const std::string ToJSONString() {
            std::stringstream ss;
            ss.setf(std::ios::fixed, std::ios::floatfield);
            ss << "\"selectivity\": {\n";
            ss << "\"id\":" << this->id << ",\n";
            ss << "\"model\":\"" << this->Name() << "\",\n";
            ss << "\"parameters\":{\n";
            ss << this->JSONParameter(a50, "a50") << ",\n";
            ss << this->JSONParameter(s, "s") << "\n";
            ss << "}\n}";
            return ss.str();
        }

        virtual const std::string Name() {
            return "logistic";
        }

        virtual std::string ToString() {
            std::stringstream ss;
            ss << " " << this->id;
            return this->Name() + ss.str();
        }

    };

    template<typename REAL_T>
    struct DoubleLogisticSel : SelectivityBase<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;
        variable alpha_asc; //ascending alpha
        variable beta_asc; // ascending beta
        variable alpha_desc; // descending alpha
        variable beta_desc; // descending beta

        /**
         * Age based logistic selectivity
         * @param a - age
         * @return 
         */
        virtual const variable Evaluate(const variable& a) {

            variable a_ = 1.0 / (1.0 + mas::exp(-1.0 * (a - this->alpha_asc) / this->beta_asc));
            variable b_ = 1.0 - (1.0 / (1.0 + mas::exp(-1.0 * (a - this->alpha_desc) / this->beta_desc)));

            return a_*b_;
            //            return (static_cast<REAL_T> (1.0) /
            //                    (static_cast<REAL_T> (1.0) +
            //                    mas::mfexp(-beta_asc * (a - alpha_asc)))) *
            //                    (static_cast<REAL_T> (1.0) - (static_cast<REAL_T> (1.0) /
            //                    (static_cast<REAL_T> (1.0) +
            //                    mas::mfexp(-beta_desc * (a - alpha_desc)))));
        }

        virtual const variable Evaluate(const std::vector<variable>& ages, size_t index) {
            variable a_ = 1.0 / (1.0 + mas::exp(-1.0 * (ages[index] - this->alpha_asc) / this->beta_asc));
            variable b_ = 1.0 - (1.0 / (1.0 + mas::exp(-1.0 * (ages[index] - this->alpha_desc) / this->beta_desc)));

            return a_*b_;
            //            return (static_cast<REAL_T> (1.0) /
            //                    (static_cast<REAL_T> (1.0) +
            //                    mas::mfexp(-beta_asc * (ages[index] - alpha_asc)))) *
            //                    (static_cast<REAL_T> (1.0) - (static_cast<REAL_T> (1.0) /
            //                    (static_cast<REAL_T> (1.0) +
            //                    mas::mfexp(-beta_desc * (ages[index] - alpha_desc)))));
        }

        virtual const std::string ToJSONString() {
            std::stringstream ss;
            ss.setf(std::ios::fixed, std::ios::floatfield);
            ss << "\"selectivity\": {\n";
            ss << "\"id\":" << this->id << ",\n";
            ss << "\"model\":\"" << this->Name() << "\",\n";
            ss << "\"parameters\":{\n";
            ss << this->JSONParameter(alpha_asc, "alpha_asc") << ",\n";
            ss << this->JSONParameter(beta_asc, "beta_asc") << ",\n";
            ss << this->JSONParameter(alpha_desc, "alpha_desc") << ",\n";
            ss << this->JSONParameter(beta_desc, "beta_desc") << "\n";
            ss << "}\n}";
            return ss.str();
        }

        virtual const std::string Name() {
            return "double_logistic";
        }

        virtual std::string ToString() {
            std::stringstream ss;
            ss << " " << this->id;
            return this->Name() + ss.str();
        }

    };

    template<typename REAL_T>
    struct AgeBased : SelectivityBase<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;
        std::map<REAL_T, variable> selex; //age keyed
        std::vector<variable> w;

        virtual const variable Evaluate(const variable& a) {
            return selex[a];
        }

        virtual const variable Evaluate(const std::vector<variable>& ages,
                size_t index) {
            return w[index];
        }

    };
    //
    //    template<typename REAL_T>
    //    struct LogisticSel : SelectivityBase<REAL_T> {
    //        typedef typename VariableTrait<REAL_T>::variable variable;
    //        variable a50; //age of 50% selectivity 
    //        variable s; //the rate of increase in selectivity at a50
    //
    //        /**
    //         * Age based logistic selectivity
    //         * @param a - age
    //         * @return 
    //         */
    //        virtual const variable Evaluate(const variable& a) {
    //            return static_cast<REAL_T> (1.0) / (static_cast<REAL_T> (1.0) + mas::mfexp(-1.0 * (a - a50) / s));
    //        }
    //
    //        virtual const std::string ToJSONString() {
    //            std::stringstream ss;
    //            ss.setf(std::ios::fixed, std::ios::floatfield);
    //            ss << "\"selectivity\": {\n";
    //            ss << "\"id\":" << this->id << ",\n";
    //            ss << "\"model\":\"" << this->Name() << "\",\n";
    //            ss << "\"parameters\":{\n";
    //            ss << this->JSONParameter(a50, "a50") << ",\n";
    //            ss << this->JSONParameter(s, "s") << "\n";
    //            ss << "}\n}";
    //            return ss.str();
    //        }
    //
    //        virtual const std::string Name() {
    //            return "logistic";
    //        }
    //
    //    };

    template<typename REAL_T>
    struct GaussianRBF : SelectivityBase<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;
        variable centroid = 1.1;
        variable epsilon = 1.0;
        std::vector<variable> w;

        inline const variable Distance(const variable& a) {
            return mas::pow((centroid - a)*(centroid - a), 0.5);
        }

        /**
         * Age based gaussian rbf selectivity
         * @param a - age
         * @return 
         */
        virtual const variable Evaluate(const variable& a) {
            return mas::mfexp(-1.0 * mas::pow(this->epsilon * Distance(a), 2.0));
        }

        virtual const variable Evaluate(const std::vector<variable>& ages, size_t index) {

            this->centroid = ages[index] + 1e-8;
            variable ret;
            for (int i = 0; i < ages.size(); i++) {
                ret += w[i] * this->Evaluate(ages[i]);
            }

            return ret;

        }

        virtual const std::string ToJSONString() {
            std::stringstream ss;
            ss.setf(std::ios::fixed, std::ios::floatfield);
            ss << "\"selectivity\": {\n";
            ss << "\"id\":" << this->id << ",\n";
            ss << "\"model\":\"" << this->Name() << "\",\n";
            ss << "\"parameters\":{\n";

            ss << "}\n}";
            return ss.str();
        }

        virtual const std::string Name() {
            return "gaussian-rbf";
        }

        virtual std::string ToString() {
            return this->Name();
        }

    };

    template<typename REAL_T>
    struct InverseQuadraticRBF : SelectivityBase<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;
        variable centroid = 1.1;
        variable epsilon = 1.0;
        std::vector<variable> w;

        inline const variable Distance(const variable& a) {
            return mas::pow((centroid - a)*(centroid - a), 0.5);
        }

        /**
         * Age based inverse quadratic rbf selectivity
         * @param a - age
         * @return 
         */
        virtual const variable Evaluate(const variable& a) {
            return 1.0 / (1.0 + mas::pow(this->epsilon * Distance(a), 2.0));
        }

        virtual const variable Evaluate(const std::vector<variable>& ages, size_t index) {
            this->centroid = ages[index] + 1e-8;
            variable ret;
            for (int i = 0; i < ages.size(); i++) {
                ret += w[i] * this->Evaluate(ages[i]);
            }

            return ret;

        }

        virtual const std::string ToJSONString() {
            std::stringstream ss;
            ss.setf(std::ios::fixed, std::ios::floatfield);
            ss << "\"selectivity\": {\n";
            ss << "\"id\":" << this->id << ",\n";
            ss << "\"model\":\"" << this->Name() << "\",\n";
            ss << "\"parameters\":{\n";

            ss << "}\n}";
            return ss.str();
        }

        virtual const std::string Name() {
            return "inverse_quadratic-rbf";
        }

        virtual std::string ToString() {
            return this->Name();
        }

    };




}


#endif /* MAS_SELECTIVITY_HPP */




