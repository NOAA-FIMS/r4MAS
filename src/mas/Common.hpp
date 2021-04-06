/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Common.hpp
 * Author: matthewsupernaw
 *
 * Created on September 16, 2016, 12:42 PM
 */

#ifndef COMMON_HPP
#define COMMON_HPP

#define USE_ATL_AS_ESTIMATION_ENGINE

#ifdef USE_ATL_AS_ESTIMATION_ENGINE
#include "third_party/ATL/ATL.hpp"
#endif

#define MAS_ALMOST_EQUAL(x,y)(std::abs(x-y)<0.01*std::max(std::abs(x),std::abs(y)))

#include <vector>
#include <map>
#include <utility>
#include <tuple>
#include <chrono>
#include <random>
#include <cmath>

namespace mas {

    std::ofstream mas_log("mas.log");



    /**
     * 
     * We are using The Analytics Template Library as the estimation engine, 
     * this can be swapped out by defining the wrapper functions below.
     * 
     */
#ifdef USE_ATL_AS_ESTIMATION_ENGINE

    template<typename REAL_T>
    struct VariableTrait {
        typedef atl::Variable<REAL_T> variable;

        static void SetName(variable& var, const std::string& value) {
            var.SetName(value);
        }

        static void SetValue(variable& var, const REAL_T& value) {
            var.SetValue(value);
        }

        static REAL_T Value(const variable& var) {
            return var.GetValue();
        }

        static void SetMinBoundary(variable& var, const REAL_T& value) {
            var.SetMinBoundary(value);
        }

        static void SetMaxBoundary(variable& var, const REAL_T& value) {
            var.SetMaxBoundary(value);
        }

        static void SetRecording(bool record) {
            variable::tape.SetRecording(record);
        }

        static bool IsRecording() {
            return variable::tape.recording;
        }

    };

    template<class REAL_T, class EXPR>
    inline const atl::ACos<REAL_T, EXPR> acos(const atl::ExpressionBase<REAL_T, EXPR>& exp) {
        return atl::acos(exp);
    }

    template<class REAL_T, class EXPR>
    inline const atl::ASin<REAL_T, EXPR> asin(const atl::ExpressionBase<REAL_T, EXPR>& exp) {
        return atl::asin(exp);
    }

    template<class REAL_T, class EXPR>
    inline const atl::ATan<REAL_T, EXPR> atan(const atl::ExpressionBase<REAL_T, EXPR>& exp) {
        return atl::atan(exp);
    }

    template<class REAL_T, class EXPR>
    inline const atl::Ceil<REAL_T, EXPR> ceil(const atl::ExpressionBase<REAL_T, EXPR>& exp) {
        return atl::ceil(exp);
    }

    template<class REAL_T, class EXPR>
    inline const atl::Cos<REAL_T, EXPR> cos(const atl::ExpressionBase<REAL_T, EXPR>& exp) {
        return atl::cos(exp);
    }

    template<class REAL_T, class EXPR>
    inline const atl::Cosh<REAL_T, EXPR> cosh(const atl::ExpressionBase<REAL_T, EXPR>& exp) {
        return atl::cosh(exp);
    }

    template<class REAL_T, class EXPR>
    inline const atl::Exp<REAL_T, EXPR> exp(const atl::ExpressionBase<REAL_T, EXPR>& exp) {
        return atl::exp(exp);
    }

    template<class REAL_T, class EXPR>
    inline const atl::Variable<REAL_T> mfexp(const atl::ExpressionBase<REAL_T, EXPR>& expr) {
        return atl::mfexp(expr);
    }

    template<class REAL_T, class EXPR>
    inline const atl::Fabs<REAL_T, EXPR> fabs(const atl::ExpressionBase<REAL_T, EXPR>& expr) {
        return atl::fabs(expr);
    }

    template<class REAL_T, class EXPR>
    inline const atl::Log<REAL_T, EXPR> log(const atl::ExpressionBase<REAL_T, EXPR>& exp) {
        return atl::log(exp);
    }

    template<class REAL_T, class EXPR>
    inline const atl::Log10<REAL_T, EXPR> log10(const atl::ExpressionBase<REAL_T, EXPR>& exp) {
        return atl::log10(exp);
    }

    template<class REAL_T, class EXPR>
    inline const atl::Sin<REAL_T, EXPR> sin(const atl::ExpressionBase<REAL_T, EXPR>& exp) {
        return atl::sin(exp);
    }

    template<class REAL_T, class EXPR>
    inline const atl::Sinh<REAL_T, EXPR> sinh(const atl::ExpressionBase<REAL_T, EXPR>& exp) {
        return atl::sinh(exp);
    }

    template<class REAL_T, class EXPR>
    inline const atl::Sqrt<REAL_T, EXPR> sqrt(const atl::ExpressionBase<REAL_T, EXPR>& exp) {
        return atl::sqrt(exp);
    }

    template<class REAL_T, class EXPR>
    inline const atl::Tan<REAL_T, EXPR> tan(const atl::ExpressionBase<REAL_T, EXPR>& exp) {
        return atl::tan(exp);
    }

    template<class REAL_T, class EXPR>
    inline const atl::Tanh<REAL_T, EXPR> tanh(const atl::ExpressionBase<REAL_T, EXPR>& exp) {
        return atl::tanh(exp);
    }

    template <class REAL_T, class LHS, class RHS>
    inline const atl::Pow<REAL_T, LHS, RHS> pow(const atl::ExpressionBase<REAL_T, LHS>& a,
            const atl::ExpressionBase<REAL_T, RHS>& b) {
        return atl::pow(a.Cast(), b.Cast());
    }

    template <class REAL_T, class LHS>
    inline const atl::Pow<REAL_T, LHS, atl::Real<REAL_T> > pow(const atl::ExpressionBase<REAL_T, LHS>& a,
            REAL_T b) {
        return atl::pow(a.Cast(), b);
    }

    template <class REAL_T, class RHS>
    inline const atl::Pow<REAL_T, atl::Real<REAL_T>, RHS> pow(const REAL_T& a,
            const atl::ExpressionBase<REAL_T, RHS>& b) {
        return atl::pow(a, b.Cast());
    }
#endif

    template<typename REAL_T>
    struct ModelObject {
        typedef typename VariableTrait<REAL_T>::variable variable;
        int id;
        std::map<variable*, int> estimated_parameters_map;
        std::vector<variable*> random_variables;
        typedef typename std::map<variable*, int>::iterator estimable_parameter_iterator;
        std::vector<variable*> estimated_parameters;
        std::vector<REAL_T> initial_parameter_values;
        std::vector<int> estimated_phase;
        bool used = false;

        void Register(variable& var, int phase = 1) {
            estimated_parameters_map[&var] = phase;
            this->estimated_parameters.push_back(&var);
            initial_parameter_values.push_back(var.GetValue());
            this->estimated_phase.push_back(phase);
        }

        void RegisterRandom(variable& var) {
            this->random_variables.push_back(&var);
        }

        virtual const std::string JSONParameter(variable& var, const std::string& name) {
            std::stringstream ss;
            ss.setf(std::ios::fixed, std::ios::floatfield);
            ss << "\"" << name << "\": {\n";
            ss << "\"value\": " << var.GetValue() << ",\n";
            if (var.bounded_m) {
                ss << "\"min\": " << var.GetMinBoundary() << ",\n";
                ss << "\"max\": " << var.GetMaxBoundary() << ",\n";
            }

            estimable_parameter_iterator it = this->estimated_parameters_map.find(&var);
            if (it != this->estimated_parameters_map.end()) {
                int phase = (*it).second;
                ss << "\"estimated\": \"true\",\n";
                ss << "\"phase\": " << phase << "\n";
            } else {
                ss << "\"estimated\": \"false\"\n";
            }
            ss << "}";
            return ss.str();
        }

        virtual const std::string ToJSONString() {
            return "ModelBase{}";
        }

        virtual std::string ToString() {
            return "ModelBase";
        }
    };

    template<typename T>
    std::ostream& operator<<(std::ostream& out, const ModelObject<T>& model) {
        out << model.ToString();
        return out;
    }

    enum SubmodelType {
        RECRUITMENT = 0,
        GROWTH,
        MORTALITY,
        MOVEMENT,
        SELECTIVITY,
        FISHING_MORTALITY
    };

    enum ForecastMethod {
        SS = 0,
        ARIMA,
        ARMA,
        NN
    };

    enum DataUnits {
        MT = 0,
        KG,
        LBS,
        IT,
        NUMBERS,
        NA
    };

    template<typename REAL_T>
    struct UnitsHandler {
        DataUnits input_units;

        REAL_T ConvertTo(const DataUnits& from, const DataUnits& to) {
            //            switch(this->input_units){
            //                
            //                case MT:
            //                    
            //                case KG:
            //                    
            //                case NUMBERS:
            //                
            //                
            //                
            //                
            //            }
        }

    };

    enum DataObjectType {
        CATCH_BIOMASS = 0,
        CATCH_PROPORTION_AT_AGE_N,
        CATCH_PROPORTION_AT_AGE,
        CATCH_PROPORTION_AT_LENGTH_N,
        CATCH_PROPORTION_AT_LENGTH,
        CATCH_MEAN_SIZE_AT_AGE,
        CATCH_MEAN_WEIGHT_AT_AGE,
        CATCH_MEAN_WEIGHT_AT_AGE_SPAWNING,
        CATCH_DISCARD_AT_AGE,
        CATCH_DISCARD_WEIGHT_AT_AGE,
        SURVEY_BIOMASS,
        SURVEY_PROPORTION_AT_AGE_N,
        SURVEY_PROPORTION_AT_AGE,
        SURVEY_PROPORTION_AT_LENGTH_N,
        SURVEY_PROPORTION_AT_LENGTH,
        SURVEY_MEAN_SIZE_AT_AGE,
        SURVEY_MEAN_WEIGHT_AT_AGE,
        SURVEY_MEAN_WEIGHT_AT_AGE_SPAWNING,
        SURVEY_DISCARD_AT_AGE,
        SURVEY_DISCARD_WEIGHT_AT_AGE,
        MEAN_WEIGHT_AT_AGE_SPAWNING,
        MEAN_WEIGHT_AT_AGE_SEASON_START,

        UNKNOWN

    };

    enum FishSexType {
        MALE = 0,
        FEMALE,
        UNDIFFERENTIATED

    };

    template<typename REAL_T>
    struct DataObject {
        std::vector<REAL_T> data;
        std::vector<REAL_T> observation_error;
        DataObjectType type;
        FishSexType sex_type;
        DataUnits units;
        std::string name;
        REAL_T missing_value;
        std::vector<bool> has_row_data;
        std::vector<REAL_T> sample_size;

        uint32_t id;
        uint32_t area_id;
        uint32_t population_id;
        uint32_t dimensions;
        size_t imax = 1;
        size_t jmax = 1;
        size_t kmax = 1;
        size_t lmax = 1;
        size_t mmax = 1;
        size_t nmax = 1;

        size_t get_size() {
            return this->imax * this->jmax * this->kmax * this->lmax * this->mmax * this->nmax;
        }

        inline REAL_T& get(int i) {
            return data.at(i);
        }

        inline REAL_T& get(int i, int j) {
            return data.at(i * jmax + j);
        }

        inline REAL_T& get(int i, int j, int k) {
            return data.at(i * jmax * kmax + j * kmax + k);
        }

        inline REAL_T& get(int i, int j, int k, int l) {
            return data.at(i * jmax * kmax * lmax + j * kmax * lmax + k * lmax + l);
        }

        inline REAL_T& get(int i, int j, int k, int l, int m) {
            return data.at(i * jmax * kmax * lmax * mmax + j * kmax * lmax * mmax + k * lmax * mmax + l * mmax + m);
        }

        inline REAL_T& get(int i, int j, int k, int l, int m, int n) {
            return data.at(i * jmax * kmax * lmax * mmax * nmax + j * kmax * lmax * mmax * nmax + k * lmax * mmax * nmax + l * mmax * nmax + m * nmax + m);
        }

        inline REAL_T& get_error(int i) {
            return observation_error.at(i);
        }

        inline REAL_T& get_error(int i, int j) {
            return observation_error.at(i * jmax + j);
        }

        inline REAL_T& get_error(int i, int j, int k) {
            return observation_error.at(i * jmax * kmax + j * kmax + k);
        }

        inline REAL_T& get_error(int i, int j, int k, int l) {
            return observation_error.at(i * jmax * kmax * lmax + j * kmax * lmax + k * lmax + l);
        }

        inline REAL_T& get_error(int i, int j, int k, int l, int m) {
            return observation_error.at(i * jmax * kmax * lmax * mmax + j * kmax * lmax * mmax + k * lmax * mmax + l * mmax + m);
        }

        inline REAL_T& get_error(int i, int j, int k, int l, int m, int n) {
            return observation_error.at(i * jmax * kmax * lmax * mmax * nmax + j * kmax * lmax * mmax * nmax + k * lmax * mmax * nmax + l * mmax * nmax + m * nmax + m);
        }

        inline REAL_T get_sample_size(int i, int j) {
            if (sample_size.size() == 0) {
                return 1.0;
            } else {
                return sample_size.at(i * jmax + j);
            }
        }

        static DataUnits GetUnits(const std::string& str) {

            if (str == "MT") {
                return mas::MT;
            } else if (str == "KG") {
                return mas::KG;
            } else if (str == "LBS") {
                return mas::LBS;
            } else if (str == "NUMBERS") {
                return mas::NUMBERS;
            } else {
                return mas::MT;
            }
        }

        static FishSexType GetSex(const std::string& str) {

            if (str == "male") {
                return mas::MALE;
            } else if (str == "female") {
                return mas::FEMALE;
            } else if (str == "undifferentiated") {
                return mas::UNDIFFERENTIATED;
            } else {
                return mas::UNDIFFERENTIATED;
            }
        }

        static DataObjectType GetType(const std::string& str) {
            if (str == "catch_biomass") {
                return CATCH_BIOMASS;
            } else if (str == "catch_proportion_at_age_N") {
                return CATCH_PROPORTION_AT_AGE_N;
            } else if (str == "catch_proportion_at_age") {
                return CATCH_PROPORTION_AT_AGE;
            } else if (str == "catch_proportion_at_length_N") {
                return CATCH_PROPORTION_AT_LENGTH_N;
            } else if (str == "catch_proportion_at_length") {
                return CATCH_PROPORTION_AT_LENGTH;
            } else if (str == "catch_mean_size_at_age") {
                return CATCH_MEAN_SIZE_AT_AGE;
            } else if (str == "catch_empirical_weight_at_age") {
                return CATCH_MEAN_WEIGHT_AT_AGE;
            } else if (str == "catch_empirical_weight_at_age_spawning") {
                return CATCH_MEAN_WEIGHT_AT_AGE_SPAWNING;
            } else if (str == "empirical_weight_at_age_spawning") {
                return MEAN_WEIGHT_AT_AGE_SPAWNING;
            } else if (str == "survey_biomass") {
                return SURVEY_BIOMASS;
            } else if (str == "survey_proportion_at_age_N") {
                return SURVEY_PROPORTION_AT_AGE_N;
            } else if (str == "survey_proportion_at_age") {
                return SURVEY_PROPORTION_AT_AGE;
            } else if (str == "survey_proportion_at_length_N") {
                return SURVEY_PROPORTION_AT_LENGTH_N;
            } else if (str == "survey_proportion_at_length") {
                return SURVEY_PROPORTION_AT_LENGTH;
            } else if (str == "survey_mean_size_at_age") {
                return SURVEY_MEAN_SIZE_AT_AGE;
            } else if (str == "survey_empirical_weight_at_age") {
                return SURVEY_MEAN_WEIGHT_AT_AGE;
            } else if (str == "survey_empirical_weight_at_age_spawning") {
                return SURVEY_MEAN_WEIGHT_AT_AGE_SPAWNING;
            } else if (str == "empirical_weight_at_age_season_start") {
                return MEAN_WEIGHT_AT_AGE_SEASON_START;
            } else {
                std::cout << "Data Error: unknown data_object_type \"" << str << "\"";
            }

            return UNKNOWN;

        }

        REAL_T Convert(const mas::DataUnits& to) {
            switch (this->units) {
                case mas::MT:
                    switch (to) {
                        case mas::KG:
                            return 1000.0;
                        case mas::MT:
                            return 1.0;
                        case mas::LBS:
                            return 2204.62;
                        default:
                            return 1.0;
                    }
                case mas::KG:
                    switch (to) {
                        case mas::KG:
                            return 1.0;
                        case mas::MT:
                            return 0.001;
                        case mas::LBS:
                            return 2.20462;
                        default:
                            return 1.0;
                    }
                case mas::LBS:
                    switch (to) {
                        case mas::KG:
                            return 0.453592;
                        case mas::MT:
                            return 0.000453592;
                        case mas::LBS:
                            return 1.0;
                        default:
                            return 1.0;
                    }
                default:
                    return 1.0;
            }
        }

        void Validate() {
            this->has_row_data.resize(this->imax, false);
            for (int i = 0; i < this->imax; i++) {
                for (int j = 0; j < this->jmax; j++) {
                    for (int k = 0; k < this->kmax; k++) {
                        size_t index = (i * this->jmax * this->kmax)+(j * this->kmax) + k;
                        if (this->data[index] != this->missing_value) {
                            this->has_row_data[i] = true;

                            break;
                        }
                    }
                }
            }
        }




    };

    template<typename REAL_T>
    struct InitialNumbers {
        mas::FishSexType type;
        int area_id;
        std::vector<REAL_T> values;
    };

    template<typename T>
    std::ostream& operator<<(std::ostream& out, mas::DataObject<T>& data_object) {

        out.precision(10);
        switch (data_object.type) {
            case CATCH_BIOMASS:
                out << "Catch Biomass:";
                break;
            case CATCH_PROPORTION_AT_AGE_N:
                out << "Catch Proportion at Age Sample Size:";
                break;
            case CATCH_PROPORTION_AT_AGE:
                out << "Catch Proportion at Age:";
                break;
            case CATCH_PROPORTION_AT_LENGTH_N:
                out << "Catch Proportion at Length Sample Size:";
                break;
            case CATCH_PROPORTION_AT_LENGTH:
                out << "Catch Proportion at Length:";
                break;
            case CATCH_MEAN_SIZE_AT_AGE:
                out << "Catch Mean Size at Age:";
                break;
            case SURVEY_BIOMASS:
                out << "Survey Biomass";
                break;
            case SURVEY_PROPORTION_AT_AGE_N:
                out << "Survey Proportion at Age Sample Size:";
                break;
            case SURVEY_PROPORTION_AT_AGE:
                out << "Survey Proportion at Age:";
                break;
            case SURVEY_PROPORTION_AT_LENGTH_N:
                out << "Survey Proportion at Length Sample Size:";
                break;
            case SURVEY_PROPORTION_AT_LENGTH:
                out << "Survey Proportion at Length:";
                break;
            case SURVEY_MEAN_SIZE_AT_AGE:
                out << "Survey Mean Size at Age:";
                break;
        }

        out << "Area: " << data_object.area_id << "\n";
        out << "Population: " << data_object.population_id << "\n";
        switch (data_object.sex_type) {
            case MALE:
                out << "Male\n";
                break;
            case FEMALE:
                out << "Female\n";
                break;
            case UNDIFFERENTIATED:
                out << "Undifferentiated\n";
                break;

        }
        out << "Dimensions: " << data_object.dimensions << "\n";

        switch (data_object.type) {
            case CATCH_BIOMASS:
                for (int i = 0; i < data_object.imax; i++) {
                    out << data_object.get(i) << "\t";
                }
                out << "\n";
                break;
            case CATCH_PROPORTION_AT_AGE_N:
                out << "Catch Proportion at Age Sample Size:";
                break;
            case CATCH_PROPORTION_AT_AGE:
                for (int i = 0; i < data_object.imax; i++) {
                    for (int j = 0; j < data_object.jmax; j++) {
                        for (int k = 0; k < data_object.kmax; k++) {
                            out << data_object.get(i, j, k) << "\t";
                        }
                        out << "\n";
                    }
                    out << "\n";
                }
                break;
            case CATCH_PROPORTION_AT_LENGTH_N:
                out << "Catch Proportion at Length Sample Size:";
                break;
            case CATCH_PROPORTION_AT_LENGTH:
                out << "Catch Proportion at Length:";
                break;
            case CATCH_MEAN_SIZE_AT_AGE:
                out << "Catch Mean Size at Age:";
                break;
            case SURVEY_BIOMASS:
                for (int i = 0; i < data_object.imax; i++) {
                    out << data_object.get(i) << "\t";
                }
                out << "\n";

                break;
            case SURVEY_PROPORTION_AT_AGE_N:
                out << "Survey Proportion at Age Sample Size:";
                break;
            case SURVEY_PROPORTION_AT_AGE:
                for (int i = 0; i < data_object.imax; i++) {
                    for (int j = 0; j < data_object.jmax; j++) {
                        for (int k = 0; k < data_object.kmax; k++) {
                            out << data_object.get(i, j, k) << "\t";
                        }
                        out << "\n";
                    }
                    out << "\n";
                }
                break;
            case SURVEY_PROPORTION_AT_LENGTH_N:
                out << "Survey Proportion at Length Sample Size:";
                break;
            case SURVEY_PROPORTION_AT_LENGTH:
                out << "Survey Proportion at Length:";
                break;
            case SURVEY_MEAN_SIZE_AT_AGE:
                out << "Survey Mean Size at Age:";
                break;
        }


        //        for (int i = 0; i < data_object.imax; i++) {
        //            for (int j = 0; j < data_object.jmax; j++) {
        //                for (int k = 0; k < data_object.kmax; k++) {
        //                    for (int l = 0; l < data_object.lmax; l++) {
        //                        for (int m = 0; m < data_object.mmax; m++) {
        //                            for (int n = 0; n < data_object.nmax; n++) {
        //                                out <<
        //                                        data_object.get(i, j, k, l, m, n) << " ";
        //                            }
        //                        }
        //                    }
        //
        //                }
        //
        //            }
        //            out << "\n";
        //        }
        //

        out << std::flush;
        return out;
    }

    template <typename T>
    T StringToNumber(const std::string &Text) {
        std::istringstream ss(Text);
        T result;
        return (ss >> result) ? result : 0;
    }


    unsigned seed_g = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator_g(seed_g);

    int rbinomial(double p, unsigned int n) {


        std::binomial_distribution<int> distribution(n, p);

        int ret = distribution(generator_g);
        return ret;

    }

    std::vector<int> rmultinom(int size, const std::vector<double>& prob) {
        // meaning of n, size, prob as in ?rmultinom
        // opposite of sample() - n=number of draws
        double pp;
        int ii;
        int probsize = prob.size();
        // Return object
        std::vector<int> draws(probsize);
        if (size < 0) throw std::range_error("Invalid size");
        long double p_tot = 0.;
        p_tot = std::accumulate(prob.begin(), prob.end(), p_tot);
        if (std::fabs((double) (p_tot - 1.)) > 1e-7) {
            throw std::range_error("Probabilities don't sum to 1.");
        }

        // do as rbinom
        if (size == 0) {
            return draws;
        }
        //rmultinom(size, REAL(prob), k, &INTEGER(ans)[ik]);
        // for each slot
        for (ii = 0; ii < probsize - 1; ii++) { /* (p_tot, n) are for "remaining binomial" */
            if (prob[ii]) {
                pp = prob[ii] / p_tot;
                // >= 1; > 1 happens because of rounding 
                draws[ii] = ((pp < 1.) ? (int) mas::rbinomial(pp, size) : size);
                size -= draws[ii];
            } // else { ret[ii] = 0; }
            // all done
            if (size <= 0) return draws;
            // i.e. p_tot = sum(prob[(k+1):K]) 
            p_tot -= prob[ii];
        }
        // the rest go here
        draws[probsize - 1] = size;
        return draws;
    }


}


#endif /* COMMON_HPP */

