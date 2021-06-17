/* 
 * File:   Fecundity.hpp
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

#ifndef MAS_FECUNDITY_HPP
#define MAS_FECUNDITY_HPP

#include "Common.hpp"

namespace mas {

    template<typename REAL_T>
    struct FecundityBase : mas::ModelObject<REAL_T> {

        typedef typename VariableTrait<REAL_T>::variable variable;

        virtual const variable Evaluate(const int& sex, const variable& age) = 0;

        virtual const std::string Name() {
            return "FecundityBase";
        }

    };


    template<typename REAL_T>
    struct LogisticFec : FecundityBase<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;
        variable a50; //age of 50% maturity
        variable s; //the rate of increase in maturity at a50

        /**
         * Age based logistic maturity
         *
         * @param sex
         * @param age
         * @return fraction_mature
         */
        virtual const variable Evaluate(const int& sex, const variable& age) {
            return static_cast<REAL_T> (1.0) / (static_cast<REAL_T> (1.0) + std::exp(-1.0*(age - a50)/s));
        }

        virtual const std::string Name() {
            return "Logistic Fecundity";
        }

    };

    template<typename REAL_T>
    struct DoubleLogisticFec : FecundityBase<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;
        variable alpha_asc; //ascending alpha
        variable beta_asc; // ascending beta
        variable alpha_desc; // descending alpha
        variable beta_desc; // descending beta

        /**
         * Age based double logistic maturity
         *
         * @param sex
         * @param age
         * @return fraction_mature
         */
        virtual const variable Evaluate(const int& sex, const variable& age) {
            return (static_cast<REAL_T> (1.0) /
                    (static_cast<REAL_T> (1.0) +
                    std::exp(-beta_asc * (age - alpha_asc)))) *
                    (static_cast<REAL_T> (1.0) - (static_cast<REAL_T> (1.0) /
                    (static_cast<REAL_T> (1.0) +
                    std::exp(-beta_desc * (age - alpha_desc)))));
        }

        virtual const std::string Name() {
            return "Double Logistic Fecundity";
        }

    };


}


#endif /* MAS_FECUNDITY_HPP */




