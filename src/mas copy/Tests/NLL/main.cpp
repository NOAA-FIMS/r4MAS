/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: matthewsupernaw
 *
 * Created on March 7, 2018, 9:53 AM
 */

#include <cstdlib>
#include "../../NLLComponents.hpp"

using namespace std;

void TestMultinomial1() {

    typedef atl::Variable<double> variable;
    size_t N = 100;
    std::shared_ptr<mas::DataObject<double> > observed(new mas::DataObject<double>());
    observed->imax = 1;
    observed->jmax = 1;
    observed->kmax = 10;
    observed->data = {0.02, 0.04, 0.08, 0.16, 0.32, 0.16, 0.08, 0.04, 0.02, 0.08};
    std::vector<variable> predicted = {0.03, 0.02, 0.09, 0.14, 0.34, 0.13, 0.10, 0.06, 0.04, 0.05};

    mas::Multinomial<double> nll(1, 1, 10);

    std::cout << "\nTest 1 Multinomial:\nNLL = " << nll.Evaluate(observed, predicted, N) << "\nNeff = " << nll.Neff() << "\n";

}


void TestMultinomial2() {

    typedef atl::Variable<double> variable;
    size_t N = 100;
    std::shared_ptr<mas::DataObject<double> > observed(new mas::DataObject<double>());
    observed->imax = 1;
    observed->jmax = 1;
    observed->kmax = 10;
    observed->data = {0.00 ,0.04, 0.09, 0.15, 0.34, 0.14, 0.08, 0.04, 0.02, 0.10};
    std::vector<variable> predicted = {0.0,0.02,0.11,0.12,0.33,0.14,0.11,0.06,0.04,0.07};

    mas::Multinomial<double> nll(1, 1, 10);

    std::cout << "\nTest 2 Multinomial:\nNLL = " << nll.Evaluate(observed, predicted, N) << "\nNeff = " << nll.Neff() << "\n";

}

void TestMultinomialRobust() {

    typedef atl::Variable<double> variable;
    size_t N = 100;
    std::shared_ptr<mas::DataObject<double> > observed(new mas::DataObject<double>());
    observed->imax = 1;
    observed->jmax = 1;
    observed->kmax = 10;
    observed->data = {0.02, 0.04, 0.08, 0.16, 0.32, 0.16, 0.08, 0.04, 0.02, 0.08};
    std::vector<variable> predicted = {0.03, 0.02, 0.09, 0.14, 0.34, 0.13, 0.10, 0.06, 0.04, 0.05};

    mas::MultinomialRobust<double> nll(1, 1, 10);

    std::cout << "\nTest Multinomial Robust:\nNLL = " << nll.Evaluate(observed, predicted, N) << "\nNeff = " << nll.Neff() << "\n";

}

void TestMultinomialRobust2() {

    typedef atl::Variable<double> variable;
    size_t N = 100;
    std::shared_ptr<mas::DataObject<double> > observed(new mas::DataObject<double>());
    observed->imax = 1;
    observed->jmax = 1;
    observed->kmax = 10;
    observed->data = {0.00 ,0.04, 0.09, 0.15, 0.34, 0.14, 0.08, 0.04, 0.02, 0.10};
    std::vector<variable> predicted = {0.0,0.02,0.11,0.12,0.33,0.14,0.11,0.06,0.04,0.07};

    mas::MultinomialRobust<double> nll(1, 1, 10);

    std::cout << "\nTest 2 Multinomial Robust:\nNLL = " << nll.Evaluate(observed, predicted, N) << "\nNeff = " << nll.Neff() << "\n";

}



void TestDirichletMultinomial() {

    typedef atl::Variable<double> variable;
    size_t N = 100;
    std::shared_ptr<mas::DataObject<double> > observed(new mas::DataObject<double>());
    observed->imax = 1;
    observed->jmax = 1;
    observed->kmax = 10;
    observed->data = {0.02, 0.04, 0.08, 0.16, 0.32, 0.16, 0.08, 0.04, 0.02, 0.08};
    std::vector<variable> predicted = {0.03, 0.02, 0.09, 0.14, 0.34, 0.13, 0.10, 0.06, 0.04, 0.05};

    mas::DirichletMultinomial<double> nll(1, 1, 10);
    nll.beta = 0.4;
    
    std::cout << "\nTest Dirichlet Multinomial:\nNLL = " << nll.Evaluate(observed, predicted, N) << "\nNeff = " << nll.Neff() << "\n";

}


void TestDirichletMultinomial2() {

    typedef atl::Variable<double> variable;
    size_t N = 100;
    std::shared_ptr<mas::DataObject<double> > observed(new mas::DataObject<double>());
    observed->imax = 1;
    observed->jmax = 1;
    observed->kmax = 10;
    observed->data = {0.01, 0.04, 0.09, 0.15, 0.34 ,0.14 ,0.08, 0.04, 0.02, 0.10};
    std::vector<variable> predicted = {0.1,0.02,0.11,0.12,0.33,0.14,0.11,0.06,0.04,0.07};

    mas::DirichletMultinomial<double> nll(1, 1, 10);
   
    
    std::cout << "\nTest 2 Dirichlet Multinomial:\nNLL = " << nll.Evaluate(observed, predicted, N) << "\nNeff = " << nll.Neff() << "\n";

}

void TestDirichletMultinomialRobust() {

    typedef atl::Variable<double> variable;
    size_t N = 100;
    std::shared_ptr<mas::DataObject<double> > observed(new mas::DataObject<double>());
    observed->imax = 1;
    observed->jmax = 1;
    observed->kmax = 10;
    observed->data = {0.02, 0.04, 0.08, 0.16, 0.32, 0.16, 0.08, 0.04, 0.02, 0.08};
    std::vector<variable> predicted = {0.03, 0.02, 0.09, 0.14, 0.34, 0.13, 0.10, 0.06, 0.04, 0.05};

    mas::DirichletMultinomialRobust<double> nll(1, 1, 10);


    std::cout << "\nTest Dirichlet Multinomial Robust:\nNLL = " << nll.Evaluate(observed, predicted, N) << "\nNeff = " << nll.Neff() << "\n";

}

void TestDirichletMultinomialRobust2() {

    typedef atl::Variable<double> variable;
    size_t N = 100;
    std::shared_ptr<mas::DataObject<double> > observed(new mas::DataObject<double>());
    observed->imax = 1;
    observed->jmax = 1;
    observed->kmax = 10;
    observed->data = {0.00, 0.04, 0.09 ,0.15, 0.34 ,0.14 ,0.08 ,0.04 ,0.02, 0.10};
    std::vector<variable> predicted = {0.0,0.02,0.11,0.12,0.33,0.14,0.11,0.06,0.04,0.07};

    mas::DirichletMultinomialRobust<double> nll(1, 1, 10);


    std::cout << "\nTest 2  Dirichlet Multinomial Robust:\nNLL = " << nll.Evaluate(observed, predicted, N) << "\nNeff = " << nll.Neff() << "\n";

}


/*
 * 
 */
int main(int argc, char** argv) {
    TestMultinomial1();
    TestMultinomial2();
    TestMultinomialRobust();
    TestMultinomialRobust2();
    TestDirichletMultinomial();
    TestDirichletMultinomial2();
    TestDirichletMultinomialRobust();
    TestDirichletMultinomialRobust2();
    return 0;
}

