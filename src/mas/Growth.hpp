/*
 * File:   Growth.hpp
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

#ifndef MAS_GROWTH_HPP
#define MAS_GROWTH_HPP

#ifdef MAS_DEBUG
#define GROWTH_DEBUG std::cout<<"Growth.hpp: "<<__func__<<": "<<__LINE__<<std::endl;
#else
#define GROWTH_DEBUG //std::cout<<"Growth.hpp: "<<__func__<<": "<<__LINE__<<std::endl;
#endif


#include "Common.hpp"
#include "Information.hpp"
#include <memory>
#include <unordered_set>
#include <map>
#include "third_party/ATL/lib//Utilities/flat_map.hpp"

namespace mas {


    //Empirical Weight At Age Lookup Type

    enum EMPIRICAL_DATA_TYPE {
        CATCH_EWAA = 0,
        SURVEY_EWAA,
        FLEET_SPAWNING_EWAA,
        SURVEY_SPAWNING_EWAA,
        SPAWNING_EWAA,
        SEASON_START_EWAA,
        CATCH_ELAA = 0,
        SURVEY_ELAA,
        FLEET_SPAWNING_ELAA,
        SURVEY_SPAWNING_ELAA,
        SPAWNING_ELAA,
        SEASON_START_ELAA

    };

    /**
     * Data structure to hold empirical weight or length at age.
     */
    template<typename REAL_T>
    struct EmpricalDataStructure {
        std::shared_ptr<mas::DataObject<REAL_T> > empirical_data_at_age;
        std::unordered_map<int, std::unordered_map<int, std::unordered_map<REAL_T, REAL_T> > > interpolated_data_at_age;
    };

    template <typename REAL_T>
    struct WeightFunctorBase {
        typedef typename VariableTrait<REAL_T>::variable variable;

        WeightFunctorBase() {
        }

        virtual inline void Evaluate(int year,
                int season,
                const variable& length,
                const REAL_T& age,
                const int& sex,
                const DataObjectType& ew_type, variable& ret) = 0;
    };

    template<typename REAL_T>
    struct DefaultWeightFunctor : public WeightFunctorBase<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;
        variable& alpha_f;
        variable& alpha_m;
        variable& beta_f;
        variable& beta_m;

        //        

        DefaultWeightFunctor(variable& alpha_f, variable& alpha_m, variable& beta_f, variable& beta_m) :
        alpha_f(alpha_f), alpha_m(alpha_m), beta_f(beta_f), beta_m(beta_m) {
        }

        DefaultWeightFunctor(const DefaultWeightFunctor<REAL_T>& other) :
        alpha_f(other.alpha_f), alpha_m(other.alpha_m), beta_f(other.beta_f), beta_m(other.beta_m) {
        }

        virtual inline void Evaluate(int year,
                int season,
                const variable& length,
                const REAL_T& age,
                const int& sex,
                const DataObjectType& ew_type, variable& ret) {
            //
            ret = sex == 0 ? alpha_f * mas::pow(length, beta_f) : alpha_m * mas::pow(length, beta_m);
            //            return ret < variable(0.0) ? variable(0.0) : ret;

        }


    };

    template<typename REAL_T>
    struct EmpiricalWeightFunctor : public WeightFunctorBase<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;

        //indexed by empirical weight type and sex
        flat_map<mas::DataObjectType, flat_map<int, mas::EmpricalDataStructure<REAL_T> > > weight_at_age_data;
        typedef typename flat_map<mas::DataObjectType, flat_map<int, mas::EmpricalDataStructure<REAL_T> > >::iterator waa_it;
        typedef typename flat_map<int, mas::EmpricalDataStructure<REAL_T> >::iterator sex_it;

        EmpiricalWeightFunctor() {
        }

        EmpiricalWeightFunctor(const EmpiricalWeightFunctor<REAL_T>& other) :
        weight_at_age_data(other.weight_at_age_data) {
        }

        virtual inline void Evaluate(int year,
                int season,
                const variable& length,
                const REAL_T& age,
                const int& sex,
                const DataObjectType& ew_type, variable& ret) {

            ret = (ew_type == mas::MEAN_WEIGHT_AT_AGE_SPAWNING) ?
                    weight_at_age_data[ew_type][sex].interpolated_data_at_age[year][0][age] :
                    weight_at_age_data[ew_type][sex].interpolated_data_at_age[year][season][age];

        }

    };

    template<typename REAL_T>
    struct GrowthBase : mas::ModelObject<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;
        variable a_min;
        variable a_max;

        variable alpha_f = 0.000025;
        variable alpha_m = 0.000025;
        variable beta_f = 3.0;
        variable beta_m = 3.0;

        std::shared_ptr<mas::WeightFunctorBase<REAL_T> > weight_functor;

        //global list of ages to interpolate.
        static std::set<REAL_T> ages_to_intrpolate;
        typedef typename std::set<REAL_T>::iterator ages_iterator;

        static std::vector<REAL_T> ages;

        const variable dnorm(const variable& x,
                const variable& mean,
                const variable& sd, int give_log = 0) {
            variable SD = sd;
            if (sd.GetValue() == 0.0) {
                SD += 1e-8;
                throw std::overflow_error("Divide by zero exception");
            }

            variable logres = static_cast<double> (-1.0) * mas::log(double(std::sqrt(2 * M_PI)) * SD) - static_cast<double> (.5) * mas::pow((x - mean) / SD, static_cast<double> (2.0));
            if (give_log)return logres;
            else return mas::exp(logres);
        }

        static REAL_T PolynomialInterpolator(REAL_T x,
                const std::vector<REAL_T>& xi,
                const std::vector<REAL_T>& yi,
                int imax) {

            REAL_T y;
            int j;
            // if x is ouside the xi[] interval
            if (x <= xi[0]) return y = yi[0];
            if (x >= xi[imax - 1]) return y = yi[imax - 1];
            // loop to find j so that x[j-1] < x < x[j]
            j = 0;
            while (j <= imax - 1) {
                if (xi[j] >= x) break;
                j = j + 1;
            }
            y = yi[j - 1]+(yi[j] - yi[j - 1])*(x - xi[j - 1]) / (xi[j] - xi[j - 1]);
            return y;
        }

        static void Do2DInterpolation(int id, const std::shared_ptr<mas::DataObject<REAL_T> > & data,
                std::unordered_map<int, std::unordered_map<int, std::unordered_map<REAL_T, REAL_T> > >& results) {

            size_t years = data->imax;
            size_t ages = data->kmax;
            std::vector<REAL_T> x(ages);
            REAL_T to_kg = data->Convert(mas::KG);
            for (size_t y = 0; y < years; y++) {
                for (size_t a = 0; a < ages; a++) {
                    x[a] = data->get(y, a);
                    results[id][y][mas::GrowthBase<REAL_T>::ages[a]] = x[a] * to_kg;
                }

                for (ages_iterator ait = mas::GrowthBase<REAL_T>::ages_to_intrpolate.begin();
                        ait != mas::GrowthBase<REAL_T>::ages_to_intrpolate.end();
                        ++ait) {
                    REAL_T xi = mas::GrowthBase<REAL_T>::PolynomialInterpolator((*ait), mas::GrowthBase<REAL_T>::ages, x, x.size());
                    results[id][y][(*ait)] = xi;
                }
            }

        }

        static void Do2DInterpolation(const std::shared_ptr<mas::DataObject<REAL_T> > & data,
                std::unordered_map<int, std::unordered_map<int, std::unordered_map<REAL_T, REAL_T> > >& results) {

            size_t years = data->imax;
            size_t ages = data->kmax;
            std::vector<REAL_T> x(ages);
            REAL_T to_kg = data->Convert(mas::KG);
            for (size_t y = 0; y < years; y++) {
                for (size_t a = 0; a < ages; a++) {
                    x[a] = data->get(y, a);
                    results[y][mas::GrowthBase<REAL_T>::ages[a]] = x[a] * to_kg;
                }

                for (ages_iterator ait = mas::GrowthBase<REAL_T>::ages_to_intrpolate.begin();
                        ait != mas::GrowthBase<REAL_T>::ages_to_intrpolate.end();
                        ++ait) {
                    REAL_T xi = mas::GrowthBase<REAL_T>::PolynomialInterpolator((*ait), mas::GrowthBase<REAL_T>::ages, x, x.size());
                    results[y][(*ait)] = xi;
                }
            }

        }

        static void Do3DInterpolation(int id, const std::shared_ptr<mas::DataObject<REAL_T> > & data,
                std::unordered_map<int, std::unordered_map<int, std::unordered_map<int, std::unordered_map<REAL_T, REAL_T> > > >& results) {

            size_t years = data->imax;
            size_t seasons = data->jmax;
            size_t ages = mas::GrowthBase<REAL_T>::ages.size();
            std::vector<REAL_T> x(ages);
            REAL_T to_kg = data->Convert(mas::KG);
            for (size_t y = 0; y < years; y++) {
                for (size_t s = 0; s < seasons; s++) {
                    for (size_t a = 0; a < ages; a++) {
                        x[a] = data->get(y, s, a);
                        results[id][y][s][mas::GrowthBase<REAL_T>::ages[a]] = x[a] * to_kg;

                    }

                    for (ages_iterator ait = mas::GrowthBase<REAL_T>::ages_to_intrpolate.begin();
                            ait != mas::GrowthBase<REAL_T>::ages_to_intrpolate.end();
                            ++ait) {
                        REAL_T xi = mas::GrowthBase<REAL_T>::PolynomialInterpolator((*ait), mas::GrowthBase<REAL_T>::ages, x, x.size());
                        results[id][y][s][(*ait)] = xi;
                    }

                }
            }

        }

        static void Do3DInterpolation(const std::shared_ptr<mas::DataObject<REAL_T> > & data,
                std::unordered_map<int, std::unordered_map<int, std::unordered_map<REAL_T, REAL_T> > >& results) {
            GROWTH_DEBUG
            size_t years = data->imax;
            size_t seasons = data->jmax;
            size_t ages = mas::GrowthBase<REAL_T>::ages.size();
            std::cout << "#ages " << ages << "\n";
            std::vector<REAL_T> x(ages);
            GROWTH_DEBUG
            REAL_T to_kg = data->Convert(mas::KG);
            GROWTH_DEBUG
            for (size_t y = 0; y < years; y++) {
                GROWTH_DEBUG
                for (size_t s = 0; s < seasons; s++) {
                    GROWTH_DEBUG
                    for (size_t a = 0; a < ages; a++) {
                        GROWTH_DEBUG
                        x[a] = data->get(y, s, a);
                        results[y][s][mas::GrowthBase<REAL_T>::ages[a]] = x[a] * to_kg;

                    }
                    GROWTH_DEBUG
                    for (ages_iterator ait = mas::GrowthBase<REAL_T>::ages_to_intrpolate.begin();
                            ait != mas::GrowthBase<REAL_T>::ages_to_intrpolate.end();
                            ++ait) {
                        GROWTH_DEBUG
                        REAL_T xi = mas::GrowthBase<REAL_T>::PolynomialInterpolator((*ait), mas::GrowthBase<REAL_T>::ages, x, x.size());
                        results[y][s][(*ait)] = xi;
                    }

                }
            }
            GROWTH_DEBUG
        }

        virtual void Initialize() {

        }

        /**
         * Computes the length of a fish at age by sex or uses a empirical weight
         * at age look up by fleet, year, and season supplied as data from a 
         * fleet. If a fleet look up does not exist, the weight is computed by
         * estimated length.
         * 
         * @param age
         * @param sex
         * @return length
         */
        virtual const variable Evaluate(const variable& age, const int& sex) = 0;

        virtual void GetWeight(int year,
                int season,
                const variable& length,
                const REAL_T& age,
                const int& sex,
                const int& id,
                const mas::DataObjectType& ew_type, variable& ret) {

            this->weight_functor->Evaluate(year, season, length, age, sex, ew_type, ret);
        }

        /**
         * Default weight at age functon.
         * 
         * \f$
         *  waa = alpha * length^{beta}
         * \f$
         * @param length
         * @param age
         * @param sex
         * @return 
         */
        inline const variable Weight(
                const variable& length,
                const variable& age,
                const int& sex) {

            variable ret = sex == 0 ? alpha_f * mas::pow(length, beta_f) : alpha_m * mas::pow(length, beta_m);
            return ret < variable(0.0) ? variable(0.0) : ret;
        }

        virtual const variable Variance(const variable& age, const int& sex) {
            return 1.0;
        }

        virtual const std::string Name() {
            return "GrowthBase";
        }

        static void Show3InterpolatedValues(std::unordered_map<int, std::unordered_map<int, std::unordered_map<int, std::unordered_map<REAL_T, REAL_T> > > >& results) {

            typename std::unordered_map<int,
                    std::unordered_map<int,
                    std::unordered_map<int,
                    std::unordered_map<REAL_T, REAL_T> > > >::iterator fit;
            typename std::unordered_map<int,
                    std::unordered_map<int,
                    std::unordered_map<REAL_T, REAL_T> > >::iterator yit;

            typename std::unordered_map<int,
                    std::unordered_map<REAL_T, REAL_T> >::iterator sit;

            typename std::unordered_map<REAL_T, REAL_T>::iterator ait;

            for (fit = results.begin(); fit != results.end(); ++fit) {
                std::cout << "ID: " << (*fit).first << "\n";
                for (yit = (*fit).second.begin(); yit != (*fit).second.end(); ++yit) {
                    for (sit = (*yit).second.begin(); sit != (*yit).second.end(); ++sit) {
                        for (ait = (*sit).second.begin(); ait != (*sit).second.end(); ++ait) {
                            std::cout << (*ait).second << "  ";
                        }
                    }
                    std::cout << "\n";
                }
            }
        }

    };

    template<typename REAL_T>
    std::set<REAL_T> mas::GrowthBase<REAL_T>::ages_to_intrpolate;

    template<typename REAL_T>
    std::vector<REAL_T> mas::GrowthBase<REAL_T>::ages;
    //      
    //    template<typename REAL_T>
    //    bool mas::GrowthBase<REAL_T>::requires_interpolation = true;
    //
    //  
    //
    //    template<typename REAL_T>
    //    std::unordered_map<int, std::shared_ptr<mas::DataObject<REAL_T> > > mas::GrowthBase<REAL_T>::empirical_weight_at_age_female_fleet;
    //    template<typename REAL_T>
    //    std::unordered_map<int, std::shared_ptr<mas::DataObject<REAL_T> > > mas::GrowthBase<REAL_T>::empirical_weight_at_age_male_fleet;
    //    template<typename REAL_T>
    //    std::unordered_map<int, std::shared_ptr<mas::DataObject<REAL_T> > > mas::GrowthBase<REAL_T>::empirical_weight_at_spawning_female_fleet;
    //    template<typename REAL_T>
    //    std::unordered_map<int, std::shared_ptr<mas::DataObject<REAL_T> > > mas::GrowthBase<REAL_T>::empirical_weight_at_spawning_male_fleet;
    //
    //
    //    template<typename REAL_T>
    //    std::unordered_map<int, std::shared_ptr<mas::DataObject<REAL_T> > > mas::GrowthBase<REAL_T>::empirical_weight_at_age_female_survey;
    //    template<typename REAL_T>
    //    std::unordered_map<int, std::shared_ptr<mas::DataObject<REAL_T> > > mas::GrowthBase<REAL_T>::empirical_weight_at_age_male_survey;
    //    template<typename REAL_T>
    //    std::unordered_map<int, std::shared_ptr<mas::DataObject<REAL_T> > > mas::GrowthBase<REAL_T>::empirical_weight_at_spawning_female_survey;
    //    template<typename REAL_T>
    //    std::unordered_map<int, std::shared_ptr<mas::DataObject<REAL_T> > > mas::GrowthBase<REAL_T>::empirical_weight_at_spawning_male_survey;
    //    template<typename REAL_T>
    //    std::unordered_map<int, std::shared_ptr<mas::DataObject<REAL_T> > > mas::GrowthBase<REAL_T>::empirical_weight_at_spawning_female;
    //    template<typename REAL_T>
    //    std::unordered_map<int, std::shared_ptr<mas::DataObject<REAL_T> > > mas::GrowthBase<REAL_T>::empirical_weight_at_spawning_male;
    //    template<typename REAL_T>
    //    std::unordered_map<int, std::shared_ptr<mas::DataObject<REAL_T> > > mas::GrowthBase<REAL_T>::empirical_weight_at_season_start_female;
    //    template<typename REAL_T>
    //    std::unordered_map<int, std::shared_ptr<mas::DataObject<REAL_T> > > mas::GrowthBase<REAL_T>::empirical_weight_at_season_start_male;
    //
    //    template<typename REAL_T>
    //    std::unordered_map<int, std::unordered_map<int, std::unordered_map<int, std::unordered_map<REAL_T, REAL_T> > > > mas::GrowthBase<REAL_T>::interpolated_empirical_weight_at_age_female_fleet;
    //    template<typename REAL_T>
    //    std::unordered_map<int, std::unordered_map<int, std::unordered_map<int, std::unordered_map<REAL_T, REAL_T> > > > mas::GrowthBase<REAL_T>::interpolated_empirical_weight_at_age_male_fleet;
    //    template<typename REAL_T>
    //    std::unordered_map<int, std::unordered_map<int, std::unordered_map<int, std::unordered_map<REAL_T, REAL_T> > > > mas::GrowthBase<REAL_T>::interpolated_empirical_weight_at_age_female_survey;
    //    template<typename REAL_T>
    //    std::unordered_map<int, std::unordered_map<int, std::unordered_map<int, std::unordered_map<REAL_T, REAL_T> > > > mas::GrowthBase<REAL_T>::interpolated_empirical_weight_at_age_male_survey;
    //
    //    template<typename REAL_T>
    //    std::unordered_map<int, std::unordered_map<int, std::unordered_map<REAL_T, REAL_T> > > mas::GrowthBase<REAL_T>::interpolated_empirical_weight_at_spawning_female_fleet;
    //    template<typename REAL_T>
    //    std::unordered_map<int, std::unordered_map<int, std::unordered_map<REAL_T, REAL_T> > > mas::GrowthBase<REAL_T>::interpolated_empirical_weight_at_spawning_female_survey;
    //    template<typename REAL_T>
    //    std::unordered_map<int, std::unordered_map<int, std::unordered_map<REAL_T, REAL_T> > > mas::GrowthBase<REAL_T>::interpolated_empirical_weight_at_spawning_male_fleet;
    //    template<typename REAL_T>
    //    std::unordered_map<int, std::unordered_map<int, std::unordered_map<REAL_T, REAL_T> > > mas::GrowthBase<REAL_T>::interpolated_empirical_weight_at_spawning_male_survey;
    //    template<typename REAL_T>
    //    std::unordered_map<int, std::unordered_map<int, std::unordered_map<REAL_T, REAL_T> > > mas::GrowthBase<REAL_T>::interpolated_weight_at_spawning_female;
    //    template<typename REAL_T>
    //    std::unordered_map<int, std::unordered_map<int, std::unordered_map<REAL_T, REAL_T> > > mas::GrowthBase<REAL_T>::interpolated_weight_at_spawning_male;
    //    template<typename REAL_T>
    //    std::unordered_map<int, std::unordered_map<int, std::unordered_map<REAL_T, REAL_T> > > mas::GrowthBase<REAL_T>::interpolated_weight_at_season_start_female;
    //    template<typename REAL_T>
    //    std::unordered_map<int, std::unordered_map<int, std::unordered_map<REAL_T, REAL_T> > > mas::GrowthBase<REAL_T>::interpolated_weight_at_season_start_male;

    template<typename REAL_T>
    struct VonBertalanffy : GrowthBase<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;
        variable k;
        variable l_inf;

        /**
         * Length at age.
         * 
         * \f$
         * length\_at\_age = l\_inf*(1.0 - e^{(-k*(age - a\_min))})
         * \f$
         * 
         * @param age
         * @param sex
         * @return length\_at\_age 
         */
        virtual const variable Evaluate(const variable& age, const int& sex) {
            variable ret = l_inf * (static_cast<REAL_T> (1.0) - mas::mfexp(-k * (age - this->a_min)));
            return ret < variable(0.0) ? variable(0.0) : ret;
        }

        virtual const std::string Name() {
            return "von_bertalanffy";
        }

        virtual const std::string ToJSONString() {
            std::stringstream ss;
            ss << "\"growth\": {\n";
            ss << "\"id\":" << this->id << ",\n";
            ss << "\"model\":\"" << this->Name() << "\",\n";
            ss << "\"parameters\":{\n";
            ss << this->JSONParameter(l_inf, "linf") << ",\n";
            ss << this->JSONParameter(this->a_min, "amin") << ",\n";
            ss << this->JSONParameter(this->a_max, "amax") << ",\n";
            ss << this->JSONParameter(this->alpha_f, "alpha_f") << ",\n";
            ss << this->JSONParameter(this->beta_f, "beta_f") << ",\n";
            ss << this->JSONParameter(this->alpha_m, "alpha_m") << ",\n";
            ss << this->JSONParameter(this->beta_m, "beta_m") << "\n";
            ss << "}\n";
            ss << "}";
            return ss.str();
        }

        virtual std::string ToString() {
            std::stringstream ss;
            ss << "Von Bertalanffy Growth:\n";
            ss << "k = " << k << "\n";
            ss << "l_inf = " << l_inf << "\n";
             ss << "a_min = " << this->a_min << "\n";
            return ss.str();
        }
    };

    template<typename REAL_T>
    struct VonBertalanffyModified : GrowthBase<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;
        variable lmin;
        variable lmax;
        variable l_inf;
        variable c;

        /**
         * \f$
         * 
         * length\_at\_age = lmin + (lmax - lmin)*\frac{(1.0 -
                    c^ {(age - a\_min)}))}{(1.0  - c^{(a\_max - a\_min)})}
         * 
         * \f$
         * 
         * @param age
         * @param sex
         * @return length\_at\_age 
         */
        virtual const variable Evaluate(const variable& age, const int& sex) {
            variable ret = lmin + (lmax - lmin)*((static_cast<REAL_T> (1.0) -
                    (mas::pow(c, age - this->a_min))) / (static_cast<REAL_T> (1.0) - mas::pow(c, this->a_max - this->a_min)));

            return ret < variable(0.0) ? variable(0.0) : ret;
        }

        virtual const std::string ToJSONString() {
            std::stringstream ss;
            ss << "\"growth\": {\n";
            ss << "\"id\":" << this->id << ",\n";
            ss << "\"model\":\"" << this->Name() << "\",\n";
            ss << "\"parameters\":{\n";
            ss << this->JSONParameter(lmin, "lmin") << ",\n";
            ss << this->JSONParameter(lmax, "lmax") << ",\n";
            ss << this->JSONParameter(this->a_min, "amin") << ",\n";
            ss << this->JSONParameter(this->a_max, "amax") << ",\n";
            ss << this->JSONParameter(this->alpha_f, "alpha_f") << ",\n";
            ss << this->JSONParameter(this->beta_f, "beta_f") << ",\n";
            ss << this->JSONParameter(this->alpha_m, "alpha_m") << ",\n";
            ss << this->JSONParameter(this->beta_m, "beta_m") << ",\n";
            ss << this->JSONParameter(c, "c") << "\n";
            ss << "}\n";
            ss << "}";
            return ss.str();
        }

        virtual const std::string Name() {
            return "von_bertalanffy_modified";
        }

        virtual std::string ToString() {
            std::stringstream ss;
            ss << "Modified Von Bertalanffy Growth:\n";
            ss << "lmin = " << lmin << "\n";
            ss << "lmax = " << lmax << "\n";
            ss << "c = " << c << "\n";
            ss << "l_inf = " << l_inf << "\n";
            return ss.str();
        }
    };

    template<typename REAL_T>
    struct SchnuteCaseI : GrowthBase<REAL_T > {
        typedef typename VariableTrait<REAL_T>::variable variable;
        variable alpha;
        variable beta;
        variable lmin;
        variable lmax;

        /**
         * \f$
         * 
         * length\_at\_age = ((lmin + (lmax - lmin)*\frac{(1.0- e^{(-alpha * (age - a\_min))})}{(1.0 - e^{(-alpha * (a\_max - a\_min))})})^{\farc{1.0}{beta}}
         * 
         * \f$
         * @param age
         * @param sex
         * @return length\_at\_age
         */
        virtual const variable Evaluate(const variable& age, const int& sex) {
            variable ret = mas::pow((lmin + (lmax - lmin))*
                    ((static_cast<REAL_T> (1.0) - mas::mfexp(-alpha * (age - this->a_min))) /
                    (static_cast<REAL_T> (1.0) - mas::mfexp(-alpha * (this->a_max - this->a_min)))),
                    static_cast<REAL_T> (1.0) / beta);
            return ret < variable(0.0) ? variable(0.0) : ret;
        }

        virtual const std::string ToJSONString() {
            std::stringstream ss;
            ss << "\"growth\": {\n";
            ss << "\"id\":" << this->id << ",\n";
            ss << "\"model\":\"" << this->Name() << "\",\n";
            ss << "\"parameters\":{\n";
            ss << this->JSONParameter(lmin, "lmin") << ",\n";
            ss << this->JSONParameter(lmax, "lmax") << ",\n";
            ss << this->JSONParameter(this->a_min, "amin") << ",\n";
            ss << this->JSONParameter(this->a_max, "amax") << ",\n";
            ss << this->JSONParameter(this->alpha_f, "alpha_f") << ",\n";
            ss << this->JSONParameter(this->beta_f, "beta_f") << ",\n";
            ss << this->JSONParameter(this->alpha_m, "alpha_m") << ",\n";
            ss << this->JSONParameter(this->beta_m, "beta_m") << ",\n";
            ss << this->JSONParameter(alpha, "alpha") << ",\n";
            ss << this->JSONParameter(beta, "beta") << "\n";
            ss << "}\n";
            ss << "}";
            return ss.str();
        }

        virtual const std::string Name() {
            return "Schnute Case I";
        }

        virtual std::string ToString() {
            std::stringstream ss;
            ss << "Scnute Case I Growth:\n";
            ss << "alpha = " << alpha << "\n";
            ss << "beta = " << beta << "\n";
            ss << "lmin = " << lmin << "\n";
            ss << "lmax = " << lmax << "\n";
            return ss.str();
        }
    };

    template<typename REAL_T>
    struct SchnuteCaseII : GrowthBase<REAL_T > {
        typedef typename VariableTrait<REAL_T>::variable variable;
        variable alpha;
        variable lmin;
        variable lmax;

        /**
         * Length at age.
         * 
         * \f$
         * length\_at\_age = lmin*e^{ln(\frac{lmax}{lmin}) * ((1.0 - e^{-alpha*(age - a\_min)})/(1.0 - e^{-alpha*(a\_max - a\_min)}))}
         * \f$
         * 
         * @param age
         * @param sex
         * @return length\_at\_age
         */
        virtual const variable Evaluate(const variable& age, const int& sex) {
            variable ret = lmin * mas::mfexp(mas::log(lmax / lmin)*
                    ((static_cast<REAL_T> (1.0) - mas::mfexp(-alpha * (age - this->a_min))) /
                    (static_cast<REAL_T> (1.0) - mas::mfexp(-alpha * (this->a_max - this->a_min)))));
            return ret < variable(0.0) ? variable(0.0) : ret;
        }

        virtual const std::string ToJSONString() {
            std::stringstream ss;
            ss << "\"growth\": {\n";
            ss << "\"id\":" << this->id << ",\n";
            ss << "\"model\":\"" << this->Name() << "\",\n";
            ss << "\"parameters\":{\n";
            ss << this->JSONParameter(lmin, "lmin") << ",\n";
            ss << this->JSONParameter(lmax, "lmax") << ",\n";
            ss << this->JSONParameter(this->a_min, "amin") << ",\n";
            ss << this->JSONParameter(this->a_max, "amax") << ",\n";
            ss << this->JSONParameter(this->alpha_f, "alpha_f") << ",\n";
            ss << this->JSONParameter(this->beta_f, "beta_f") << ",\n";
            ss << this->JSONParameter(this->alpha_m, "alpha_m") << ",\n";
            ss << this->JSONParameter(this->beta_m, "beta_m") << ",\n";
            ss << this->JSONParameter(alpha, "alpha") << "\n";
            ss << "}\n";
            ss << "}";
            return ss.str();
        }

        virtual const std::string Name() {
            return "Schnute Case II";
        }

        virtual std::string ToString() {
            std::stringstream ss;
            ss << "Scnute Case II Growth:\n";
            ss << "alpha = " << alpha << "\n";
            ss << "lmin = " << lmin << "\n";
            ss << "lmax = " << lmax << "\n";
            return ss.str();
        }
    };

    template<typename REAL_T>
    struct SchnuteCaseIII : GrowthBase<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;
        variable alpha;
        variable beta;
        variable lmin;
        variable lmax;

        /**
         * \f$
         * 
         * length\_at\_age = ((lmin + (lmax - lmin)*\frac{(1.0- ( (age - a\_min)))}{(1.0 - ((a\_max - a\_min)))})^{\frac{1.0}{beta}}
         * 
         * \f$
         * @param age
         * @param sex
         * @return length\_at\_age
         */
        virtual const variable Evaluate(const variable& age, const int& sex) {
            variable ret = mas::pow((lmin + (lmax - lmin))*
                    ((static_cast<REAL_T> (1.0) - (age - this->a_min)) /
                    (static_cast<REAL_T> (1.0) - (this->a_max - this->a_min))),
                    static_cast<REAL_T> (1.0) / beta);
            return ret < variable(0.0) ? variable(0.0) : ret;
        }

        virtual const std::string ToJSONString() {
            std::stringstream ss;
            ss.setf(std::ios::fixed, std::ios::floatfield);
            ss << "\"growth\": {\n";
            ss << "\"id\":" << this->id << ",\n";
            ss << "\"model\":\"" << this->Name() << "\",\n";
            ss << "\"parameters\":{\n";
            ss << this->JSONParameter(lmin, "lmin") << ",\n";
            ss << this->JSONParameter(lmax, "lmax") << ",\n";
            ss << this->JSONParameter(this->a_min, "amin") << ",\n";
            ss << this->JSONParameter(this->a_max, "amax") << ",\n";
            ss << this->JSONParameter(this->alpha_f, "alpha_f") << ",\n";
            ss << this->JSONParameter(this->beta_f, "beta_f") << ",\n";
            ss << this->JSONParameter(this->alpha_m, "alpha_m") << ",\n";
            ss << this->JSONParameter(this->beta_m, "beta_m") << ",\n";
            ss << this->JSONParameter(alpha, "alpha") << ",\n";
            ss << this->JSONParameter(beta, "beta") << "\n";
            ss << "}\n";
            ss << "}";
            return ss.str();
        }

        virtual const std::string Name() {
            return "Schnute Case III";
        }

        virtual std::string ToString() {
            std::stringstream ss;
            ss << "Scnute Case III Growth:\n";
            ss << "alpha = " << alpha << "\n";
            ss << "beta = " << beta << "\n";
            ss << "lmin = " << lmin << "\n";
            ss << "lmax = " << lmax << "\n";
            return ss.str();
        }
    };

    template<typename REAL_T>
    struct SchnuteCaseIV : GrowthBase<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;
        variable alpha;
        variable beta;
        variable lmin;
        variable lmax;

        /**
         * \f$
         *  length\_at\_age =  lmin * e^{ln(\frac{lmax }{lmin})*\frac{(1.0- (age - a\_min))}{(1.0 - (a\_max - a\_min)))}}
         * \f$
         * 
         * @param age
         * @param sex
         * @return length\_at\_age  
         */
        virtual const variable Evaluate(const variable& age, const int& sex) {
            variable ret = lmin * mas::mfexp(mas::log(lmax / lmin)*
                    ((static_cast<REAL_T> (1.0) - (age - this->a_min)) /
                    (static_cast<REAL_T> (1.0) - (this->a_max - this->a_min))));
            return ret < variable(0.0) ? variable(0.0) : ret;
        }

        virtual const std::string ToJSONString() {
            std::stringstream ss;
            ss.setf(std::ios::fixed, std::ios::floatfield);
            ss << "\"growth\": {\n";
            ss << "\"id\":" << this->id << ",\n";
            ss << "\"model\":\"" << this->Name() << "\",\n";
            ss << "\"parameters\":{\n";
            ss << this->JSONParameter(lmin, "lmin") << ",\n";
            ss << this->JSONParameter(lmax, "lmax") << ",\n";
            ss << this->JSONParameter(this->a_min, "amin") << ",\n";
            ss << this->JSONParameter(this->a_max, "amax") << ",\n";
            ss << this->JSONParameter(this->alpha_f, "alpha_f") << ",\n";
            ss << this->JSONParameter(this->beta_f, "beta_f") << ",\n";
            ss << this->JSONParameter(this->alpha_m, "alpha_m") << ",\n";
            ss << this->JSONParameter(this->beta_m, "beta_m") << ",\n";
            ss << this->JSONParameter(alpha, "alpha") << ",\n";
            ss << this->JSONParameter(beta, "beta") << "\n";
            ss << "}\n";
            ss << "}";
            return ss.str();
        }

        virtual const std::string Name() {
            return "Schnute Case IV";
        }

        virtual std::string ToString() {
            std::stringstream ss;
            ss << "Scnute Case IV Growth:\n";
            ss << "alpha = " << alpha << "\n";
            ss << "beta = " << beta << "\n";
            ss << "lmin = " << lmin << "\n";
            ss << "lmax = " << lmax << "\n";
            return ss.str();
        }
    };

    template<typename REAL_T>
    struct EmpiricalGrowth : GrowthBase<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;

        //indexed by empirical weight type and sex
        std::map<mas::DataObjectType, std::map<int, mas::EmpricalDataStructure<REAL_T> > > weight_at_age_data;
        typedef typename std::map<mas::DataObjectType, std::map<int, mas::EmpricalDataStructure<REAL_T> > >::iterator waa_it;
        typedef typename std::map<int, mas::EmpricalDataStructure<REAL_T> >::iterator sex_it;
        //indexed by empirical length type and sex
        std::map<mas::DataObjectType, std::map<int, mas::EmpricalDataStructure<REAL_T> > > length_at_age_data;

        void AddEmpiricalData(mas::EmpricalDataStructure<REAL_T >& data, int sex, mas::EMPIRICAL_DATA_TYPE type) {

        }

        virtual void Initialize() {

            //            waa_it it = this->weight_at_age_data.find(CATCH_MEAN_WEIGHT_AT_AGE);
            //            if (it != this->weight_at_age_data.end()) {
            //
            //                sex_it jt = (*it).second.find(mas::FEMALE);
            //
            //                if (jt != (*it).second.end()) {
            //
            //                    mas::EmpricalDataStructure<REAL_T>& eds = (*jt).second;
            //                    mas::GrowthBase<REAL_T>::Do3DInterpolation(eds.empirical_data_at_age, eds.interpolated_data_at_age);
            //
            //
            //                } else {
            //                    std::cout << "Empirical Growth Model does not supply \"catch_empirical_weight_at_age\" data for females.\n";
            //                    mas::mas_log << "Empirical Growth Model does not supply \"catch_empirical_weight_at_age\" data for females.\n";
            //                }
            //
            //                jt = (*it).second.find(mas::MALE);
            //
            //                if (jt != (*it).second.end()) {
            //
            //                    mas::EmpricalDataStructure<REAL_T>& eds = (*jt).second;
            //                    mas::GrowthBase<REAL_T>::Do3DInterpolation(eds.empirical_data_at_age, eds.interpolated_data_at_age);
            //
            //
            //                } else {
            //                    std::cout << "Empirical Growth Model does not supply \"catch_empirical_weight_at_age\" data for males.\n";
            //                    mas::mas_log << "Empirical Growth Model does not supply \"catch_empirical_weight_at_age\" data for males.\n";
            //                }
            //
            //                //                mas::GrowthBase<REAL_T>::Do3DInterpolation()
            //            } else {
            //                std::cout << "Empirical Growth Model does not supply \"catch_empirical_weight_at_age\" data.\n";
            //                mas::mas_log << "Empirical Growth Model does not supply \"catch_empirical_weight_at_age\" data.\n";
            //            }
            //
            //
            //            it = this->weight_at_age_data.find(SURVEY_MEAN_WEIGHT_AT_AGE);
            //            if (it != this->weight_at_age_data.end()) {
            //
            //                sex_it jt = (*it).second.find(mas::FEMALE);
            //
            //                if (jt != (*it).second.end()) {
            //
            //                    mas::EmpricalDataStructure<REAL_T>& eds = (*jt).second;
            //                    mas::GrowthBase<REAL_T>::Do3DInterpolation(eds.empirical_data_at_age, eds.interpolated_data_at_age);
            //
            //
            //                } else {
            //                    std::cout << "Empirical Growth Model does not supply \"survey_empirical_weight_at_age\" data for females.\n";
            //                    mas::mas_log << "Empirical Growth Model does not supply \"survey_empirical_weight_at_age\" data for females.\n";
            //                }
            //
            //                jt = (*it).second.find(mas::MALE);
            //
            //                if (jt != (*it).second.end()) {
            //
            //                    mas::EmpricalDataStructure<REAL_T>& eds = (*jt).second;
            //                    mas::GrowthBase<REAL_T>::Do3DInterpolation(eds.empirical_data_at_age, eds.interpolated_data_at_age);
            //
            //
            //                } else {
            //                    std::cout << "Empirical Growth Model does not supply \"survey_empirical_weight_at_age\" data for males.\n";
            //                    mas::mas_log << "Empirical Growth Model does not supply \"survey_empirical_weight_at_age\" data for males.\n";
            //                }
            //
            //                //                mas::GrowthBase<REAL_T>::Do3DInterpolation()
            //            } else {
            //                std::cout << "Empirical Growth Model does not supply \"survey_empirical_weight_at_age\" data.\n";
            //                mas::mas_log << "Empirical Growth Model does not supply \"survey_empirical_weight_at_age\" data.\n";
            //            }



            //            for (waa_it = this->weight_at_age_data.begin(); waa_it != this->weight_at_age_data.end(); ++waa_it) {
            //                switch ((*waa_it).first) {
            //
            //                    case mas::CATCH_MEAN_WEIGHT_AT_AGE:
            //
            //                        break;
            //
            //                    case mas::SURVEY_MEAN_WEIGHT_AT_AGE:
            //
            //                        break;
            //
            //                    case mas::MEAN_WEIGHT_AT_AGE_SEASON_START:
            //
            //                        break;
            //
            //                    case mas::MEAN_WEIGHT_AT_AGE_SPAWNING:
            //
            //                        break;
            //                }
            //            }


        }

        virtual const variable Evaluate(const variable& age, const int& sex) {

        }

        inline const variable Weight(int year,
                int season,
                const variable& length,
                const REAL_T& age,
                const int& sex,
                const int& id,
                const EMPIRICAL_DATA_TYPE& ew_type) {

           
        }
    };


}


#endif /* MAS_GROWTH_HPP */

