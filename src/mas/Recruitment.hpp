/*
 * File:   Recruitment.hpp
 *
 * Author: Matthew Supernaw
 * National Oceanic and Atmospheric Administration
 * National Marine Fisheries Service
 * Sustainable Fisheries Division
 * St. Petersburg, FL, 33701
 *
 * Created on September 16, 2016, 12:32 PM
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


#ifndef RECRUITMENT_HPP
#define RECRUITMENT_HPP
#include "Common.hpp"

#define SQUARE(x) ((x)*(x))

namespace mas {

    template<typename REAL_T>
    struct RecruitmentBase : mas::ModelObject<REAL_T> {
        typedef typename mas::VariableTrait<REAL_T>::variable variable;
        variable R0; // unfished equilibrium recruits
        variable log_R0;
        variable h; // steepness
        variable sigma_r; // recruitment standard deviation
        variable rho; // correlation coefficient
        bool use_bias_correction = false;

        variable bias_correction = 0.0;

        std::unordered_map<int, std::unordered_map<int, variable> > SB0; // unfished equilibrium female spawning biomass, by population and area
        variable phi0; // unfished equilibrium spawning biomass per recruit, SB0 / R0

        std::vector<variable> recruitment_deviations;
        bool recruitment_deviations_random_effects = false;
        bool estimating_recruitment_deviations = false;
        bool recruitment_deviations_constrained = false;

        virtual ~RecruitmentBase() {
        }

        virtual const variable Evaluate(const int& pop_id, const int& area_id, const variable& s) = 0;

        /**
         * Used for unit testing.
         * 
         * @param SB0
         * @param s
         * @return 
         */
        virtual const variable Evaluate(const variable& SB0, const variable& s) = 0;

        //        virtual const variable GetR0() = 0;

        virtual const REAL_T GetAlpha() {
            return 0.0;
        }

        virtual const REAL_T GetBeta() {
            return 0.0;
        }

        virtual variable GetBiasCorrection() {
            return this->bias_correction;
        }

        virtual const REAL_T CalculateEquilibriumSpawningBiomass(REAL_T spawing_biomass_per_recruit) {
            return 0.0;
        }

        virtual const REAL_T CalculateEquilibriumRecruitment(REAL_T spr) {
            return 0.0;
        }

        virtual const variable CalculateEquilibriumRecruitment(variable spr, variable spr_F0) {
            return 0.0;
        }

        std::string RecruitDeviationsToJSON() {

#ifdef USE_ATL_AS_ESTIMATION_ENGINE

            std::stringstream ss;
            ss.setf(std::ios::fixed, std::ios::floatfield);
            ss << "\"recruitment_deviations\":{";
            if (this->estimating_recruitment_deviations) {
                ss << "\"estimated\": \"true\",\n";
                if (this->recruitment_deviations.size() > 0) {
                    ss << "\"phase\": " << this->estimated_parameters_map[&recruitment_deviations[0]] << ",\n";
                }
            } else {
                ss << "\"estimated\": \"false\",\n";
            }
            ss << "\"values\":[\n";
            for (int i = 0; i < this->recruitment_deviations.size() - 1; i++) {
                ss << this->recruitment_deviations[i].GetValue() << ",\n";
            }
            ss << this->recruitment_deviations[this->recruitment_deviations.size() - 1].GetValue() << "]\n}\n";
            return ss.str();
#endif
        }

        virtual const std::string Name() {
            return "RecruitmentBase";
        }

        variable LikelihoodComponent() {
            variable likelihood; // = mas::log(this->sigma_r);
            variable norm2 = 0.0;
            variable nll1;
            variable nll2;
            for (int i = 0; i < this->recruitment_deviations.size(); i++) {
                nll1 += mas::log(this->sigma_r);
                nll2 += SQUARE((this->recruitment_deviations[i] / this->sigma_r) + 0.5 * this->sigma_r);
                //                norm2 += (this->recruitment_deviations[i] + 1e-5) * (this->recruitment_deviations[i] + 1e-5);

            }
            likelihood = nll1 + 0.5 * nll2; //+= norm2 / (2.0 * this->sigma_r * this->sigma_r);
            return likelihood;
        }

        void Prepare() {


            if (estimating_recruitment_deviations && recruitment_deviations_constrained) {
                variable sum = static_cast<REAL_T> (0.0);

                for (int i = 0; i < this->recruitment_deviations.size(); i++) {
                    sum += this->recruitment_deviations[i];
                }



                for (int i = 0; i < this->recruitment_deviations.size(); i++) {
                    this->recruitment_deviations[i] -= sum / static_cast<REAL_T> (this->recruitment_deviations.size());
                }


            }
            if (this->use_bias_correction) {
                this->bias_correction = -0.5 * this->sigma_r * this->sigma_r; //bias correction
            } else {
                this->bias_correction = 0.0;
            }
        }


    };

    template<typename REAL_T>
    struct Ricker : RecruitmentBase<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;
        variable alpha;
        variable beta;

        const variable Evaluate(const int& pop_id, const int& area_id, const variable& s) {

            return alpha * s * mas::mfexp(static_cast<REAL_T> (-1.0) * beta * s) *
                    mas::mfexp(-0.5 * this->sigma_r * this->sigma_r);
        }

        /**
         * Used for unit testing.
         * 
         * @param SB0
         * @param s
         * @return 
         */
        virtual const variable Evaluate(const variable& SB0, const variable& s) {
            return alpha * s * mas::mfexp(static_cast<REAL_T> (-1.0) * beta * s)
                    * mas::mfexp(-0.5 * this->sigma_r * this->sigma_r);
        }

        virtual const REAL_T GetAlpha() {
            return this->alpha;
        }

        virtual const REAL_T GetBeta() {
            return this->beta;
        }

        virtual const REAL_T CalculateEquilibriumSpawningBiomass(REAL_T spawing_biomass_per_recruit) {
            return (1.0 / this->GetBeta())*
                    (mas::log(spawing_biomass_per_recruit) * this->GetAlpha());
        }

        virtual const REAL_T CalculateEquilibriumRecruitment(REAL_T equilibrium_spawning_biomass) {
            return this->GetAlpha() * equilibrium_spawning_biomass *
                    mas::exp(-1.0 * this->GetBeta() * equilibrium_spawning_biomass);
        }

        virtual const variable CalculateEquilibriumRecruitment(variable spr, variable spr_F0) {

            return mas::exp(this->log_R0) * (1.0 + mas::log(mas::exp(this->bias_correction) *
                    spr / spr_F0) / this->h) / (spr / spr_F0);
        }

        virtual const std::string ToJSONString() {

#ifdef USE_ATL_AS_ESTIMATION_ENGINE
            std::stringstream ss;
            ss.setf(std::ios::fixed, std::ios::floatfield);
            ss << "\"recruitment\": {\n";
            ss << "\"id\":" << this->id << ",\n";
            ss << "\"model\":\"" << this->Name() << "\",\n";
            ss << "\"parameters\":{\n";
            ss << this->JSONParameter(alpha, "alpha") << ",\n";
            ss << this->JSONParameter(beta, "beta") << ",\n";
            ss << this->JSONParameter(this->R0, "R0") << ",\n";
            ss << this->JSONParameter(this->rho, "rho") << ",\n";
            ss << this->JSONParameter(this->h, "h") << ",\n";
            ss << this->JSONParameter(this->sigma_r, "sigma_r") << ",\n";
            ss << this->RecruitDeviationsToJSON();
            ss << "}\n";
            ss << "}";
            return ss.str();
#endif
        }

        virtual const std::string Name() {
            return "Ricker";
        }
    };

    template<typename REAL_T>
    struct RickerAlt : RecruitmentBase<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;
        variable phi0; //unfished spawning biomass or abundance per recruit
        variable A; //adjustment factor
        variable h;

        /**
         * Alternative Ricker Spawn-Recruit relationship
         * @param s - spawning biomass or abundance
         * @return
         */
        const variable Evaluate(const int& pop_id, const int& area_id, const variable& s) {
            return ((this->R0 * this->SB0[pop_id][area_id]) /
                    this->SB0[pop_id][area_id])*mas::mfexp(this->h * (1.0 - s /
                    this->SB0[pop_id][area_id])) * mas::mfexp(-0.5 * this->sigma_r * this->sigma_r);
        }

        /**
         * Used for unit testing.
         * 
         * @param SB0
         * @param s
         * @return 
         */
        virtual const variable Evaluate(const variable& SB0, const variable& s) {
            return ((this->R0 * SB0) /
                    SB0)*mas::mfexp(this->h * (1.0 - s /
                    SB0)) * mas::mfexp(-0.5 * this->sigma_r * this->sigma_r);
        }

        virtual const REAL_T CalculateEquilibriumSpawningBiomass(REAL_T spawing_biomass_per_recruit) {
            return 0.0;
        }

        virtual const variable CalculateEquilibriumRecruitment(variable spr, variable spr_F0) {

            return mas::exp(this->log_R0) * (1.0 + mas::log(mas::exp(this->bias_correction) *
                    spr / spr_F0) / this->h) / (spr / spr_F0);
        }

        virtual const std::string ToJSONString() {
#ifdef USE_ATL_AS_ESTIMATION_ENGINE
            std::stringstream ss;
            ss.setf(std::ios::fixed, std::ios::floatfield);
            ss << "\"recruitment\": {\n";
            ss << "\"id\":" << this->id << ",\n";
            ss << "\"model\":\"" << this->Name() << "\",\n";
            ss << "\"parameters\":{\n";
            ss << this->JSONParameter(phi0, "phi0") << ",\n";
            ss << this->JSONParameter(A, "A") << ",\n";
            ss << this->JSONParameter(this->R0, "R0") << ",\n";
            ss << this->JSONParameter(this->rho, "rho") << ",\n";
            ss << this->JSONParameter(this->h, "h") << ",\n";
            ss << this->JSONParameter(this->sigma_r, "sigma_r") << ",\n";
            ss << this->RecruitDeviationsToJSON();
            ss << "}\n";
            ss << "}";
            return ss.str();
#endif
        }

        virtual const std::string Name() {
            return "Ricker Alt";
        }
    };

    template<typename REAL_T>
    struct BevertonHolt : RecruitmentBase<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;
        variable alpha; //maximum recruitment
        variable beta; //the stock level needed to produce the half of maximum recruitment

        virtual void PrepareChild() {
            if (this->use_bias_correction) {
                this->bias_correction = -0.5 * this->sigma_r * this->sigma_r; //bias correction
            } else {
                this->bias_correction = 0.0;
            }
        }

        /**
         * Beverton-Holt Spawn-Recruit relationship
         *
         * @param s - spawning biomass or abundance
         * @return
         */
        const variable Evaluate(const int& pop_id, const int& area_id, const variable& sb) {

            alpha = 4.0 * this->h * mas::exp(this->log_R0) / (5.0 * this->h - 1.0);
            beta = (this->SB0[pop_id][area_id] * (1.0 - this->h)) / (5.0 * this->h - 1.0);

            return ( 4.0 * this->h * mas::exp(this->log_R0) * sb) / (this->SB0[pop_id][area_id]*(1.0 - this->h) + (sb * (5.0 * this->h - 1.0)));
        }

        /**
         * Used for unit testing.
         * 
         * @param SB0
         * @param s
         * @return 
         */
        virtual const variable Evaluate(const variable& SB0, const variable& sb) {
            alpha = 4.0 * this->h * mas::exp(this->log_R0) / (5.0 * this->h - 1.0);
            beta = (SB0 * (1.0 - this->h)) / (5.0 * this->h - 1.0);
            return ( 4.0 * this->h * mas::exp(this->log_R0) * sb) / (SB0 * (1.0 - this->h) + sb * (5.0 * this->h - 1.0));
        }

        virtual const REAL_T GetAlpha() {
            return this->alpha;
        }

        virtual const REAL_T GetBeta() {
            return this->beta;
        }

        virtual const REAL_T CalculateEquilibriumSpawningBiomass(REAL_T spawing_biomass_per_recruit) {
            return this->GetAlpha() * spawing_biomass_per_recruit - this->GetBeta();
        }

        virtual const REAL_T CalculateEquilibriumRecruitment(REAL_T equilibrium_spawning_biomass) {
            //                                                R_eq[i] = (R0 / ((5.0 * steep - 1.0) * spr[i]))*
            //                                                        (BC * 4.0 * steep * spr[i] - spr_F0 * (1.0 - steep));
            //             (mas::exp(this->log_R0) / ((5.0 * this->h - 1.0) * spr[i]))*
            //                                                                       (BC * 4.0 * this->h * spr[i] - spr_F0 * (1.0 - this->h));
            return (this->GetAlpha() * equilibrium_spawning_biomass) /
                    (this->GetBeta() + equilibrium_spawning_biomass);
        }

        //         virtual const variable CalculateEquilibriumRecruitment(variable spr, variable spr_F0) {

        //             return (mas::exp(this->log_R0) / ((5.0 * this->h - 1.0) * spr))*
        //                     (/*mas::exp(this->bias_correction)*/4.0 * this->h * spr - spr_F0 * (1.0 - this->h));
        //         }

        virtual const variable CalculateEquilibriumRecruitment(
                const variable spawning_biomass_per_recruit_unfished,
                const variable spawning_biomass_per_recruit_f) {
            //=R0*((4*h*R52-(1-h)*$B$7)/((5*h-1)*R52))
            //            std::cout<<spawning_biomass_per_recruit_unfished<<"\n"<<spawning_biomass_per_recruit_f<<"\n";
            variable ret = mas::exp(this->log_R0)*((4.0 * this->h *
                    spawning_biomass_per_recruit_f - (1.0 - this->h) *
                    spawning_biomass_per_recruit_unfished) / (((5.0 * this->h) - 1.0)
                    * spawning_biomass_per_recruit_f));
            return ret;
        }

        virtual const std::string ToJSONString() {
#ifdef USE_ATL_AS_ESTIMATION_ENGINE
            std::stringstream ss;
            ss.setf(std::ios::fixed, std::ios::floatfield);
            ss << "\"recruitment\": {\n";
            ss << "\"id\":" << this->id << ",\n";
            ss << "\"model\":\"" << this->Name() << "\",\n";
            ss << "\"parameters\":{\n";
            ss << this->JSONParameter(alpha, "alpha") << ",\n";
            ss << this->JSONParameter(beta, "beta") << ",\n";
            ss << this->JSONParameter(this->R0, "R0") << ",\n";
            ss << this->JSONParameter(this->rho, "rho") << ",\n";
            ss << this->JSONParameter(this->h, "h") << ",\n";
            ss << this->JSONParameter(this->sigma_r, "sigma_r") << ",\n";
            ss << this->RecruitDeviationsToJSON();
            ss << "}\n";
            ss << "}";
            return ss.str();
#endif
        }

        virtual const std::string Name() {
            return "Beverton-Holt";
        }
    };

    template<typename REAL_T>
    struct BevertonHoltAlt : RecruitmentBase<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;

        /**
         * Alternative Beverton-Holt S-R relationship
         *
         * @param s - spawning biomass or abundance
         * @return
         */
        const variable Evaluate(const int& pop_id, const int& area_id, const variable& s) {
            variable rr;

            rr = 4.0 * (this->h * mas::exp(this->log_R0) * s / (this->SB0[pop_id][area_id]*(1.0 - this->h) +
                    s * (5.0 * this->h - 1.0)));
            return rr;
        }

        /**
         * Used for unit testing.
         * 
         * @param SB0
         * @param s
         * @return 
         */
        virtual const variable Evaluate(const variable& SB0, const variable& s) {
            variable rr;
            rr = 4.0 * (this->h * mas::exp(this->log_R0) * s / (SB0 * (1.0 - this->h) +
                    s * (5.0 * this->h - 1.0)));

            return rr;
        }

        virtual const variable CalculateEquilibriumRecruitment(variable spr, variable spr_F0) {

            return (mas::exp(this->log_R0) / ((5.0 * this->h - 1.0) * spr))*
                    (/*mas::exp(this->bias_correction)*/4.0 * this->h * spr - spr_F0 * (1.0 - this->h));
        }

        virtual const std::string ToJSONString() {
#ifdef USE_ATL_AS_ESTIMATION_ENGINE
            std::stringstream ss;
            ss << "\"recruitment\": {\n";
            ss.setf(std::ios::fixed, std::ios::floatfield);
            ss << "\"id\":" << this->id << ",\n";
            ss << "\"model\":\"" << this->Name() << "\",\n";
            ss << "\"parameters\":{\n";
            ss << this->JSONParameter(this->R0, "R0") << ",\n";
            ss << this->JSONParameter(this->rho, "rho") << ",\n";
            ss << this->JSONParameter(this->h, "h") << ",\n";
            ss << this->JSONParameter(this->sigma_r, "sigma_r") << ",\n";
            ss << this->RecruitDeviationsToJSON();
            ss << "}\n";
            ss << "}";
            return ss.str();
#endif
        }

        virtual const std::string Name() {
            return "beverton_holt_alt";
        }
    };

    template<typename REAL_T>
    struct BevertonHoltDep : RecruitmentBase<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;
        variable alpha; // maximum recruitment
        variable beta; // the stock level needed to produce the half of maximum recruitment
        variable c; //exponent

        /**
         * Depensatory Beverton-Holt S-R relationship
         *
         * @param s - spawning biomass or abundance
         * @return
         */
        const variable Evaluate(const int& pop_id, const int& area_id, const variable& sb) {

            alpha = 4.0 * this->h * sb;
            beta = sb * (1.0 - this->h) + sb * (5.0 * this->h - 1.0);
            return (alpha / beta)* mas::exp(-0.5 * mas::pow(this->sigma_r, 2.0)); ///(alpha*s)/(beta + s);
        }

        /**
         * Used for unit testing.
         * 
         * @param SB0
         * @param s
         * @return 
         */
        virtual const variable Evaluate(const variable& SB0, const variable& sb) {
            alpha = 4.0 * this->h * sb; //mas::exp(this->log_R0) / (5.0 * this->h - 1.0);
            beta = sb * (1.0 - this->h) + sb * (5.0 * this->h - 1.0); //mas::exp(this->log_R0)  * (1.0 - this->h ) / (5.0 * this->h  - 1.0);
            return (alpha / beta)* mas::exp(-0.5 * mas::pow(this->sigma_r, 2.0)); ///(alpha*s)/(beta + s);
        }

        virtual const REAL_T CalculateEquilibriumSpawningBiomass(REAL_T spawing_biomass_per_recruit) {
            return this->GetAlpha() * spawing_biomass_per_recruit - this->GetBeta();
        }

        virtual const std::string ToJSONString() {

#ifdef USE_ATL_AS_ESTIMATION_ENGINE
            std::stringstream ss;
            ss.setf(std::ios::fixed, std::ios::floatfield);
            ss << "\"recruitment\": {\n";
            ss << "\"id\":" << this->id << ",\n";
            ss << "\"model\":\"" << this->Name() << "\",\n";
            ss << "\"parameters\":{\n";
            ss << this->JSONParameter(alpha, "alpha") << ",\n";
            ss << this->JSONParameter(beta, "beta") << ",\n";
            ss << this->JSONParameter(c, "c") << ",\n";
            ss << this->JSONParameter(this->R0, "R0") << ",\n";
            ss << this->JSONParameter(this->rho, "rho") << ",\n";
            ss << this->JSONParameter(this->h, "h") << ",\n";
            ss << this->JSONParameter(this->sigma_r, "sigma_r") << ",\n";
            ss << this->RecruitDeviationsToJSON();
            ss << "}\n";
            ss << "}";
            return ss.str();
#endif
        }

        virtual const std::string Name() {
            return "Beverton-Holt Dep";
        }
    };

    template<typename REAL_T>
    struct Shepherd : RecruitmentBase<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;
        variable alpha;
        variable beta;
        variable c;

        const variable Evaluate(const int& pop_id, const int& area_id, const variable& s) {
            return (alpha * s) / (static_cast<REAL_T> (1.0) + mas::pow((s / beta), c));
        }

        /**
         * Used for unit testing.
         * 
         * @param SB0
         * @param s
         * @return 
         */
        virtual const variable Evaluate(const variable& SB0, const variable& s) {
            return (alpha * s) / (static_cast<REAL_T> (1.0) + mas::pow((s / beta), c));

        }

        virtual const std::string ToJSONString() {

#ifdef USE_ATL_AS_ESTIMATION_ENGINE
            std::stringstream ss;
            ss.setf(std::ios::fixed, std::ios::floatfield);
            ss << "\"recruitment\": {\n";
            ss << "\"id\":" << this->id << ",\n";
            ss << "\"model\":\"" << this->Name() << "\",\n";
            ss << "\"parameters\":{\n";
            ss << this->JSONParameter(alpha, "alpha") << ",\n";
            ss << this->JSONParameter(beta, "beta") << ",\n";
            ss << this->JSONParameter(c, "c") << ",\n";
            ss << this->JSONParameter(this->R0, "R0") << ",\n";
            ss << this->JSONParameter(this->rho, "rho") << "\n";
            ss << this->JSONParameter(this->h, "h") << ",\n";
            ss << this->JSONParameter(this->sigma_r, "sigma_r") << ",\n";
            ss << this->RecruitDeviationsToJSON();
            ss << "}\n";
            ss << "}";
            return ss.str();
#endif
        }

        virtual const std::string Name() {
            return "Shepard";
        }
    };

    template<typename REAL_T>
    struct Deriso : RecruitmentBase<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;
        variable alpha;
        variable beta;
        variable c;

        const variable Evaluate(const int& pop_id, const int& area_id, const variable& s) {
            return (alpha * s) * mas::pow((static_cast<REAL_T> (1.0) - beta * c * s), static_cast<REAL_T> (1.0) / c);
        }

        /**
         * Used for unit testing.
         * 
         * @param SB0
         * @param s
         * @return 
         */
        virtual const variable Evaluate(const variable& SB0, const variable& s) {
            return (alpha * s) * mas::pow((static_cast<REAL_T> (1.0) - beta * c * s), static_cast<REAL_T> (1.0) / c);
        }

        virtual const std::string ToJSONString() {

#ifdef USE_ATL_AS_ESTIMATION_ENGINE
            std::stringstream ss;
            ss.setf(std::ios::fixed, std::ios::floatfield);
            ss << "\"recruitment\": {\n";
            ss << "\"id\":" << this->id << ",\n";
            ss << "\"model\":\"" << this->Name() << "\",\n";
            ss << "\"parameters\":{\n";
            ss << this->JSONParameter(alpha, "alpha") << ",\n";
            ss << this->JSONParameter(beta, "beta") << ",\n";
            ss << this->JSONParameter(c, "c") << ",\n";
            ss << this->JSONParameter(this->R0, "R0") << ",\n";
            ss << this->JSONParameter(this->rho, "rho") << ",\n";
            ss << this->JSONParameter(this->h, "h") << ",\n";
            ss << this->JSONParameter(this->sigma_r, "sigma_r") << ",\n";
            ss << this->RecruitDeviationsToJSON();
            ss << "}\n";
            ss << "}";
            return ss.str();
#endif
        }

        virtual const std::string Name() {
            return "Deriso";
        }
    };



}


#endif /* RECRUITMENT_HPP */




