/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   EffectiveSampleSize.hpp
 * Author: matthewsupernaw
 *
 * Created on February 26, 2018, 2:46 PM
 */

#ifndef EFFECTIVESAMPLESIZE_HPP
#define EFFECTIVESAMPLESIZE_HPP

#include <cmath>
#include "Common.hpp"

#define SQUARE(x) ((x)*(x))

namespace mas {

    /**
     * add normal, log normal, beta?
     *
     */


    template<class T>
    static T lgamma_(const T &z) {
        T gamma_r10 = 10.900511;

        T pi = M_PI;
        T gamma_c = 2.0 * mas::sqrt(M_E / pi);

        T sum = 2.48574089138753565546e-5;
        sum += 1.05142378581721974210 / z;
        sum += -3.45687097222016235469 / (z + 1.0);
        sum += 4.51227709466894823700 / (z + 2.0);
        sum += -2.98285225323576655721 / (z + 3.0);
        sum += 1.05639711577126713077 / (z + 4.0);
        sum += -1.95428773191645869583e-1 / (z + 5.0);
        sum += 1.70970543404441224307e-2 / (z + 6.0);
        sum += -5.71926117404305781283e-4 / (z + 7.0);
        sum += 4.63399473359905636708e-6 / (z + 8.0);
        sum += -2.71994908488607703910e-9 / (z + 9.0);

        return mas::log(gamma_c) + (z - 0.5) * mas::log(z + gamma_r10 - 0.5) - (z - 0.5) + mas::log(sum);
    }

    /**
     *  Returns the natural logarithm of the gamma
     *  function using the lanczos approximation
     *  method.
     * 
     */
    template<class T>
    static T GammaLn(const T &xx) {

        T ret;


        int k;
        T Ag;
        T term1, term2;

        T x = xx - T(1.0);


        Ag = T(0.99999999999980993227684700473478) +
                (T(676.520368121885098567009190444019) / (x + T(1)))+
                (T(-1259.13921672240287047156078755283) / (x + T(2)))+
                (T(771.3234287776530788486528258894 / (x + T(3))))+
                (T(-176.61502916214059906584551354) / (x + T(4)))+
                (T(12.507343278686904814458936853) / (x + T(5)))+
                (T(-0.13857109526572011689554707) / (x + T(6)))+
                (T(9.984369578019570859563e-6) / (x + T(7)))+
                (T(1.50563273514931155834e-7) / (x + T(8)));

        term1 = (x + T(0.5)) * std::log((x + T(7.5)) / T(M_E));
        term2 = T(0.9189385332046727418)/*HALF LOG 2M_PI*/ + std::log(Ag);
        ret = term1 + (term2 - T(7.0));

        return ret;

    }

    template<typename T>
    T gammaln(const T& x) {
        T y, tmp, ser;
        static double cof[6] = {(76.18009172947146),
            (-86.50532032941677),
            (24.01409824083091),
            (-1.231739572450155),
            (0.1208650973866179e-2),
            (-0.5395239384953e-5)};

        y = x - 1.0;
        tmp = y + (5.5);
        tmp -= (y + 0.5) * std::log(tmp);
        ser = 1.000000000190015;
        for (int j = 0; j <= 5; j++) {
            y += 1.0;
            ser += cof[j] / y;
        }
        return -tmp + std::log(2.5066282746310005 * ser);
    }

    template<typename T>
    const T GammaLn2(const T& z) {
        return z * mas::log(z) - z - 0.5 *
                (z / 2.0 * M_PI) + 1.0 / 12.0 * z - 1.0 /
                (360.0 * mas::pow(z, 3.0)) + 1.0 / (1260.0 * mas::pow(z, 5.0));
    }

    template<typename T>
    const T st_lgamma(const T& x) {
        return mas::log(mas::fabs(mas::sqrt(2.0 * M_PI / x) * mas::pow((x / M_E), x)));
    }

    enum NLL_Functor_Type {
        LOGNORMAL = 0,
        MULTINOMIAL,
        MULTINOMIAL_ROBUST,
        DIRICHLET_MULTINOMIAL,
        DIRICHLET_MULTINOMIAL_ROBUST,
        BASE
    };

    template<typename REAL_T>
    struct NLLFunctor : mas::ModelObject<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;
        NLL_Functor_Type functor_type = BASE;
        size_t years;
        size_t seasons;
        size_t ages;
        variable neff;
        REAL_T CV = .2;
        REAL_T epsilon = static_cast<REAL_T> (1e-8);

#warning NLLFunctor can only be used once

        atl::intrusive_ptr<mas::DataObject<REAL_T> > lambda;

        NLLFunctor() {
        }

        NLLFunctor(size_t years, size_t seasons, size_t ages) :
        years(years), seasons(seasons), ages(ages), neff(10.0) {
        }



        virtual variable Evaluate(const atl::intrusive_ptr<DataObject<REAL_T> >& observed,
                const std::vector<variable>& predicted,
                size_t N) = 0;

        virtual const REAL_T Neff() {
            return this->neff;
        }

        virtual std::string ToString() {
            return "NLLBase";
        }

    };

    template<typename REAL_T>
    struct Lognormal : mas::NLLFunctor<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;
        NLL_Functor_Type functor_type = LOGNORMAL;
        variable sigma = 0.2;
        bool use_bias_correction = true;

        Lognormal() {
        }

        Lognormal(size_t years, size_t seasons, size_t ages) :
        NLLFunctor<REAL_T>(years, seasons, ages) {
        }

        virtual variable Evaluate(const atl::intrusive_ptr<DataObject<REAL_T> >& observed,
                const std::vector<variable>& predicted,
                size_t N) {
            //
            //
            size_t i, j, k;
            REAL_T obs, se, se2, cv;
            this->years = observed->imax;
            this->seasons = observed->jmax;
            this->ages = observed->kmax;
            this->neff = static_cast<REAL_T> (0.0);
            variable nll = static_cast<REAL_T> (0.0);
            variable expected;

            variable nll1;
            variable nll2;
            switch (observed->dimensions) {
                case 2:

                    if (this->use_bias_correction) {
                        for (i = 0; i < this->years; i++) {
                            for (j = 0; j < this->seasons; j++) {
                                size_t index = i * this->seasons + j;
                                expected = predicted[index];

                                REAL_T obs = observed->get(i, j);
                                if (obs != observed->missing_value) {
                                    // likely_ind(ind) += log(index_sigma(ind, i));
                                    // likely_ind(ind) += 0.5 * square(log(index_obs(ind, i)) - log(index_pred(ind, i))) / index_sigma2(ind, i);
                                    cv = observed->get_error(i, j);
                                    se2 = std::log(cv * cv + 1.0);
                                    se = std::sqrt(se2) / std::sqrt(std::log(M_E));
                                    nll1 += this->lambda->get(i, j) * std::log(se);
//                                     nll2 += this->lambda->get(i, j) * SQUARE((mas::log(obs) - mas::log(expected)))/se;
                                    nll2 += this->lambda->get(i, j) * SQUARE((mas::log((obs / expected)) / se) + 0.5 * se);
                                }
                            }

                        }
                    } else {
                        for (i = 0; i < this->years; i++) {
                            for (j = 0; j < this->seasons; j++) {
                                size_t index = i * this->seasons + j;
                                expected = predicted[index];

                                REAL_T obs = observed->get(i, j);
                                if (obs != observed->missing_value) {
                                    // likely_ind(ind) += log(index_sigma(ind, i));
                                    // likely_ind(ind) += 0.5 * square(log(index_obs(ind, i)) - log(index_pred(ind, i))) / index_sigma2(ind, i);
                                    cv = observed->get_error(i, j);
                                    se2 = std::log(cv * cv + 1.0);
                                    se = std::sqrt(se2) / std::sqrt(std::log(M_E));
                                    nll1 += this->lambda->get(i, j) * std::log(se);
                                    nll2 += this->lambda->get(i, j) * SQUARE(mas::log((obs / expected))) / se2;
                                }
                            }

                        }
                    }
                    nll = nll1 + 0.5 * nll2;

                    break;
                case 3:

                    throw std::invalid_argument("Expected 2 dimensional observation data for log-normal likelihood function.");

                    break;
            }


            //            variable nll = static_cast<REAL_T> (0.0);
            //
            //            //lognormal distribution, ASAP
            //            for (size_t i = 0; i < this->years; i++) {
            //                for (size_t j = 0; j < this->seasons; j++) {
            //                    for (size_t a = 0; a < this->ages; a++) {
            //                        REAL_T cv = observed->get_error(i, j, a);
            //                        REAL_T sigma = std::log(cv);
            //                        size_t index = i * this->seasons * this->ages + j * this->ages + a;
            //                        REAL_T obs = observed->get(i, j, a);
            //
            //                        if (obs != observed->missing_value) {
            //                            nll += this->lambda->get(i, j, a) *(0.5 * SQUARE((std::log(obs) - mas::log(predicted[index]))) / sigma); 
            //                        }
            //                    }
            //                }
            //            }

            //nll *= static_cast<REAL_T> (-1.0);

            return nll;
        }

        virtual std::string ToString() {

            return "lognormal";
        }
    };

    template<typename REAL_T>
    struct DirichletMultinomial : mas::NLLFunctor<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;
        NLL_Functor_Type functor_type = DIRICHLET_MULTINOMIAL;
        variable beta = static_cast<REAL_T> (0.5);

        DirichletMultinomial() {
        }

        DirichletMultinomial(size_t years, size_t seasons, size_t ages) :
        NLLFunctor<REAL_T>(years, seasons, ages) {
        }

        virtual variable Evaluate(const atl::intrusive_ptr<DataObject<REAL_T> >& observed,
                const std::vector<variable>& predicted,
                size_t N) {


            this->years = observed->imax;
            this->seasons = observed->jmax;
            this->ages = observed->kmax;

            this->neff = static_cast<REAL_T> (0.0);
            variable nll = static_cast<REAL_T> (0.0);

            //dirichlet-multinomial distribution
            nll = mas::lgamma_<variable>(this->beta * static_cast<REAL_T> (N)) -
                    mas::lgamma_<variable>(static_cast<REAL_T> (N) + this->beta);
            for (size_t i = 0; i < this->years; i++) {
                for (size_t j = 0; j < this->seasons; j++) {
                    variable temp1 = static_cast<REAL_T> (0.0);
                    variable temp2 = static_cast<REAL_T> (0.0);



                    for (size_t a = 0; a < this->ages; a++) {


                        size_t index = i * this->seasons * this->ages + j * this->ages + a;
                        //                        temp1 += mas::lgamma_<variable>(static_cast<REAL_T> (N) * (observed->get(i, j, a) + o) + this->beta * (predicted[index] + o));
                        //                        temp2 -= mas::lgamma_<variable>(this->beta * (predicted[index] + o));
                        REAL_T obs = observed->get(i, j, a);
                        if (obs != observed->missing_value) {
                            REAL_T o = observed->get_error(i, j, a);
                            nll -= (mas::lgamma_<variable>(static_cast<REAL_T> (N) * (obs + o) + this->beta * (predicted[index] + o)) -
                                    mas::lgamma_<variable>(this->beta * (predicted[index] + o)));
                        }
                    }
                }
            }

            //compute effective sample size
            this->neff = (1.0 + this->beta.GetValue() * static_cast<REAL_T> (N)) / (1.0 + this->beta.GetValue());

            return nll;
        }

        virtual std::string ToString() {

            return "dirichlet_multinomial";
        }
    };

    template<typename REAL_T>
    struct DirichletMultinomialRobust : mas::NLLFunctor<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;
        NLL_Functor_Type functor_type = DIRICHLET_MULTINOMIAL_ROBUST;

        variable beta = static_cast<REAL_T> (0.5);

        DirichletMultinomialRobust() {
        }

        DirichletMultinomialRobust(size_t years, size_t seasons, size_t ages) :
        NLLFunctor<REAL_T>(years, seasons, ages) {
        }

        virtual variable Evaluate(const atl::intrusive_ptr<DataObject<REAL_T> >& observed,
                const std::vector<variable>& predicted,
                size_t N) {

            this->years = observed->imax;
            this->seasons = observed->jmax;
            this->ages = observed->kmax;

            this->neff = static_cast<REAL_T> (0.0);
            variable nll = mas::gammaln<variable>(variable(this->beta * static_cast<REAL_T> (N))) -
                    mas::gammaln<variable>(variable(static_cast<REAL_T> (N) + this->beta * static_cast<REAL_T> (N)));

            //compute the negative log liklihood using the dirichlet-multinomial distribution
            for (size_t i = 0; i < this->years; i++) {
                for (size_t j = 0; j < this->seasons; j++) {
                    variable temp = static_cast<REAL_T> (0.0);

                    for (size_t a = 0; a < this->ages; a++) {
                        size_t index = i * this->seasons * this->ages + j * this->ages + a;
                        temp += (predicted[index] + this->epsilon);
                    }

                    variable temp1 = static_cast<REAL_T> (0.0);
                    variable temp2 = static_cast<REAL_T> (0.0);
                    variable temp3 = static_cast<REAL_T> (0.0);
                    for (size_t a = 0; a < this->ages; a++) {
                        REAL_T o = observed->get_error(i, j, a);
                        size_t index = i * this->seasons * this->ages + j * this->ages + a;
                        variable P_pred = ((predicted[index] + o) + this->epsilon) / temp;
                        //                        nll -= (mas::gammaln<variable>(variable(static_cast<REAL_T> (N) * (observed->get(i, j, a) + o) + this->beta * static_cast<REAL_T> (N) * P_pred)) +
                        //                                mas::gammaln<variable>(variable(this->beta * static_cast<REAL_T> (N) * P_pred)));
                        //                        temp1 += mas::GammaLn<variable>(static_cast<REAL_T> (N) * (observed->get(i, j, a) + o) + this->beta * (P_pred + o));
                        //                        temp2 -= mas::GammaLn<variable>(this->beta * (P_pred + o));
                        //                        temp3 += temp1 + temp2;
                        REAL_T obs = observed->get(i, j, a);
                        if (obs != observed->missing_value) {
                            nll -= (mas::GammaLn<variable>(static_cast<REAL_T> (N) * (obs + o) + this->beta * (P_pred + o)) -
                                    mas::GammaLn<variable>(this->beta * (P_pred + o)));
                        }
                    }

                }
            }

            //compute effective sample size
            this->neff = (1.0 + this->beta.GetValue() * static_cast<REAL_T> (N)) / (1.0 + this->beta.GetValue());

            return nll;
        }

        virtual std::string ToString() {

            return "dirichlet_multinomial_robust";
        }
    };

    template<typename REAL_T>
    struct Multinomial : mas::NLLFunctor<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;
        NLL_Functor_Type functor_type = MULTINOMIAL;

        Multinomial() {
        }

        Multinomial(size_t years, size_t seasons, size_t ages) :
        NLLFunctor<REAL_T>(years, seasons, ages) {
        }

        virtual variable Evaluate(const atl::intrusive_ptr<DataObject<REAL_T> >& observed,
                const std::vector<variable>& predicted,
                size_t N) {
            this->years = observed->imax;
            this->seasons = observed->jmax;
            this->ages = observed->kmax;

            variable nll = static_cast<REAL_T> (0.0);
            this->neff = static_cast<REAL_T> (0.0);
            for (size_t i = 0; i < this->years; i++) {

                for (size_t j = 0; j < this->seasons; j++) {
                    variable temp_sum = 0.0;
                    variable temp1 = static_cast<REAL_T> (0.0);
                    variable temp2 = static_cast<REAL_T> (0.0);

                    variable temp3 = static_cast<REAL_T> (0.0);

                    for (size_t a = 0; a < this->ages; a++) {
                        size_t index = i * this->seasons * this->ages + j * this->ages + a;
                        temp3 += (predicted[index]);
                    }

                    for (size_t a = 0; a < this->ages; a++) {
                        REAL_T obs = observed->get(i, j, a);

                        if (obs != observed->missing_value) {
                            //                            REAL_T o = observed->get_error(i, j, a) * obs;
                            size_t index = i * this->seasons * this->ages + j * this->ages + a;
                            variable P_pred = (predicted[index]) / temp3;
                            temp_sum += this->lambda->get(i, j, a) *((obs) * mas::log(predicted[index]));

                            temp1 += P_pred * (1.0 - P_pred);
                            temp2 += std::pow((observed->get(i, j, a) - P_pred), 2.0);
                        }
                    }
                    if (observed->get_sample_size(i, j) != observed->missing_value) {

                        nll += static_cast<REAL_T> (observed->get_sample_size(i, j)) * temp_sum;
                    }


                    this->neff += temp1 / temp2;

                }
            }
            nll *= static_cast<REAL_T> (-1.0);
            return nll;
        }

        virtual std::string ToString() {

            return "multinomial";
        }
    };

    template<typename REAL_T>
    struct MultinomialRobust : mas::NLLFunctor<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;
        NLL_Functor_Type functor_type = MULTINOMIAL_ROBUST;

        REAL_T epsilon = static_cast<REAL_T> (1e-8);

        MultinomialRobust() {
        }

        MultinomialRobust(size_t years, size_t seasons, size_t ages) :
        NLLFunctor<REAL_T>(years, seasons, ages) {
        }

        virtual variable Evaluate(const atl::intrusive_ptr<DataObject<REAL_T> >& observed,
                const std::vector<variable>& predicted,
                size_t N) {
            this->years = observed->imax;
            this->seasons = observed->jmax;
            this->ages = observed->kmax;

            variable nll = static_cast<REAL_T> (0.0);
            this->neff = static_cast<REAL_T> (0.0);
            for (size_t i = 0; i < this->years; i++) {

                for (size_t j = 0; j < this->seasons; j++) {
                    variable temp_sum = 0.0;
                    REAL_T temp1 = static_cast<REAL_T> (0.0);
                    REAL_T temp2 = static_cast<REAL_T> (0.0);

                    variable temp3 = static_cast<REAL_T> (0.0);

                    for (size_t a = 0; a < this->ages; a++) {
                        size_t index = i * this->seasons * this->ages + j * this->ages + a;
                        temp3 += (predicted[index] + this->epsilon);
                    }

                    for (size_t a = 0; a < this->ages; a++) {
                        REAL_T obs = observed->get(i, j, a);
                        if (obs != observed->missing_value) {
                            //                            REAL_T o = observed->get_error(i, j, a) * obs;
                            size_t index = i * this->seasons * this->ages + j * this->ages + a;
                            variable P_pred = (predicted[index] + this->epsilon) / temp3;
                            temp_sum += this->lambda->get(i, j, a) *((obs) * mas::log(P_pred));

                            temp1 += P_pred.GetValue()*(1.0 - P_pred.GetValue());
                            temp2 += std::pow((observed->get(i, j, a) - P_pred.GetValue()) + epsilon, 2.0);
                        }
                    }
                    if (observed->get_sample_size(i, j) != observed->missing_value) {

                        nll += static_cast<REAL_T> (-1.0) * static_cast<REAL_T> (observed->get_sample_size(i, j)) * temp_sum;
                    }


                    this->neff += temp1 / temp2;

                }
            }
            return nll;
        }

        virtual std::string ToString() {

            return "multinomial_robust";
        }
    };

    template<typename REAL_T>
    struct NLLComponent {
        typedef typename VariableTrait<REAL_T>::variable variable;
        std::vector<variable>* estimated;
        atl::intrusive_ptr<DataObject<REAL_T> > observed;
        atl::intrusive_ptr<mas::NLLFunctor<REAL_T> > nll_functor;
        size_t N;
        static bool record_residuals;
        std::vector<REAL_T> residuals;
        size_t years, seasons, ages;
        REAL_T chi_square = 0.0;
        REAL_T g_test = 0.0;
        REAL_T rmse = 0.0;
        REAL_T rmsle = 0.0;
        REAL_T r_squared = 0.0;
        REAL_T AIC = 0.0; //Akaike’s Information Criterion.
        REAL_T BIC = 0.0; //Bayesian Information Criterion
        static int k; //number of parameters

        NLLComponent(std::vector<variable>* estimated,
                atl::intrusive_ptr<DataObject<REAL_T> > observed,
                atl::intrusive_ptr<mas::NLLFunctor<REAL_T> > nll_functor) :
        estimated(estimated), observed(observed), nll_functor(nll_functor) {
            if (nll_functor->lambda.get() == NULL) {
                nll_functor->lambda = new mas::DataObject<REAL_T>();
                nll_functor->lambda->data.resize(observed->data.size(), static_cast<REAL_T> (1.0));
                nll_functor->lambda->imax = observed->imax;
                nll_functor->lambda->jmax = observed->jmax;
                nll_functor->lambda->jmax = observed->jmax;
                nll_functor->lambda->kmax = observed->kmax;
            }

            if (nll_functor->lambda->get_size() != observed->get_size()) {

                nll_functor->lambda->imax = observed->imax;
                nll_functor->lambda->jmax = observed->jmax;
                nll_functor->lambda->jmax = observed->jmax;
                nll_functor->lambda->kmax = observed->kmax;
                nll_functor->lambda->data.resize(observed->data.size(), static_cast<REAL_T> (1.0));
            }

            if (nll_functor->functor_type == MULTINOMIAL || nll_functor->functor_type == MULTINOMIAL_ROBUST) {
                if (observed->sample_size.size() == 0) {
                    mas_log << "Configuration Warning: No sample size matrix defined for " << observed->id << ". A value of 1 will be used.\n";
                }
            }

        }

        inline void Evaluate(variable& nll_c) {
            nll_c += nll_functor->Evaluate(observed,
                    *estimated,
                    this->observed->get_size());

        }

        void Finalize() {
            this->residuals.resize(estimated->size());
            this->chi_square = 0.0;
            this->years = observed->imax;
            this->seasons = observed->jmax;
            this->ages = observed->kmax;
            REAL_T n = 0.0;
            REAL_T sum_X = 0, sum_Y = 0, sum_XY = 0;
            REAL_T squareSum_X = 0, squareSum_Y = 0;
            REAL_T residual_sum_sqr = 0.0;


            for (size_t i = 0; i < this->years; i++) {
                for (size_t j = 0; j < this->seasons; j++) {
                    for (size_t a = 0; a < this->ages; a++) {

                        size_t index = i * this->seasons * this->ages + j * this->ages + a;
                        REAL_T obs = observed->get(i, j, a);

                        if (obs != observed->missing_value) {
                            n++;
                            residuals[index] = obs - estimated->at(index).GetValue();
                            residual_sum_sqr += std::pow(residuals[index], 2.0);
                            chi_square += (std::pow(residuals[index], 2.0)) / estimated->at(index).GetValue();
                            g_test += obs * std::log(obs / estimated->at(index).GetValue());
                            rmse += std::pow(estimated->at(index).GetValue() - obs, 2.0);
                            rmsle += std::pow(std::log(obs + 1.0) - std::log(estimated->at(index).GetValue() + 1.0), 2.0);

                            // sum of elements of array X. 
                            sum_X = sum_X + estimated->at(index).GetValue();

                            // sum of elements of array Y. 
                            sum_Y = sum_Y + obs;

                            // sum of X[i] * Y[i]. 
                            sum_XY = sum_XY + estimated->at(index).GetValue() * obs;

                            // sum of square of array elements. 
                            squareSum_X = squareSum_X + estimated->at(index).GetValue() * estimated->at(index).GetValue();
                            squareSum_Y = squareSum_Y + obs * obs;

                        } else {
                            residuals[index] = observed->missing_value;
                        }
                    }
                }
            }

            this->AIC = n * std::log(residual_sum_sqr / n) + mas::NLLComponent<REAL_T>::k * 2;
            this->BIC = n * std::log(residual_sum_sqr / n) + mas::NLLComponent<REAL_T>::k * std::log(n);

            g_test *= 2.0;
            rmse = std::sqrt((1.0 / n) * rmse);
            rmsle = std::sqrt((1.0 / n) * rmsle);

            r_squared = (n * sum_XY - sum_X * sum_Y)
                    / std::sqrt((n * squareSum_X - sum_X * sum_X)
                    * (n * squareSum_Y - sum_Y * sum_Y));

        }


    };

    template<typename REAL_T>
    int mas::NLLComponent<REAL_T>::k = 0;

    template<typename REAL_T>
    bool mas::NLLComponent<REAL_T>::record_residuals = false;

    template<typename REAL_T>
    std::ostream& operator<<(std::ostream& out, mas::NLLComponent<REAL_T>& nll) {
        out << "GOF: " << nll.chi_square << "\nResiduals:\n";
        int years = nll.observed->imax;
        int seasons = nll.observed->jmax;
        int ages = nll.observed->kmax;

        if (ages > 1) {
            out << "year\tseason\tage\n";
            for (size_t i = 0; i < years; i++) {
                for (size_t j = 0; j < seasons; j++) {
                    for (size_t a = 0; a < ages; a++) {
                        size_t index = i * seasons * ages + j * ages + a;
                        out << i << "\t" << j << "\t" << a << "\t" << nll.residuals[index] << "\n";
                    }
                }
            }
        } else {
            for (size_t i = 0; i < years; i++) {
                for (size_t j = 0; j < seasons; j++) {
                    size_t index = i * seasons + j;
                    out << i << "\t" << j << "\t" << nll.residuals[index] << "\n";
                }
            }
        }

        return out;
    }

    template<typename REAL_T>
    std::stringstream& operator<<(std::stringstream& out, mas::NLLComponent<REAL_T>& nll) {
        out << "GOF: " << nll.chi_square << "\nResiduals:\n";
        int years = nll.observed->imax;
        int seasons = nll.observed->jmax;
        int ages = nll.observed->kmax;

        if (ages > 1) {
            out << "year\tseason\tage\n";
            for (size_t i = 0; i < years; i++) {
                for (size_t j = 0; j < seasons; j++) {
                    for (size_t a = 0; a < ages; a++) {
                        size_t index = i * seasons * ages + j * ages + a;
                        out << i << "\t" << j << "\t" << a << "\t" << nll.residuals[index] << "\n";
                    }
                }
            }
        } else {
            for (size_t i = 0; i < years; i++) {
                for (size_t j = 0; j < seasons; j++) {
                    size_t index = i * seasons + j;
                    out << i << "\t" << j << "\t" << nll.residuals[index] << "\n";
                }
            }
        }

        return out;
    }




}


#endif /* EFFECTIVESAMPLESIZE_HPP */




