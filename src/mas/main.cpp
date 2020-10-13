/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   main.cpp
 * Author: matthewsupernaw
 *
 * Created on September 14, 2016, 9:57 AM
 */


#include <cstdlib>

#include <iostream>
#include "MAS.hpp"
#include "Options.hpp"
#include "ObjectiveFunction.hpp"
#include "EnsembleEngine.hpp"
using namespace std;

template<typename REAL_T>
void forward() {
    double v = 0.0;
    for (int i = 0; i < atl::Variable<REAL_T>::tape.stack_current; i++) {
        atl::StackEntry<REAL_T>& entry = atl::Variable<REAL_T>::tape.stack[i];
        entry.w->value = 0.0;
        
    }
    for (int i = 0; i < atl::Variable<REAL_T>::tape.stack_current; i++) {
        atl::StackEntry<REAL_T>& entry = atl::Variable<REAL_T>::tape.stack[i];
        v = entry.exp->GetValue();
        entry.w->value = v;
    }
}

template<typename REAL_T>
void Run(const mas::Options<REAL_T>& options) {

#ifndef MAS_ENSEMBLE
    mas::MASObjectiveFunction<double> objective_function;

    objective_function.config_path = options.config;
    objective_function.data_path = options.data;


    //initialize the objective function
    objective_function.Initialize();


    if (objective_function.mas_instance.info.valid_configuration) {
        //create an instance of a L-BFGS minimizer
        atl::LBFGS<double> fm;
        fm.SetPrintWidth(2);
        fm.SetTolerance(options.tol);
        fm.max_line_searches = options.max_ls;
        fm.max_iterations = options.max_iter;
        fm.print_interval = options.iprint;
        //set the objective function
        fm.SetObjectiveFunction(&objective_function);
        
        //run the minimizer
        fm.Run();

        objective_function.Finalize();
    }
#else

    mas::EnsembleEngine<double> ee;
    ee.SetOptions(options);
    ee.Run();
#endif


}

/*
 *
 */
int main(int argc, char** argv) {
    std::vector<std::string> args;
    for (int i = 0; i < argc; i++) {
        args.push_back(std::string(argv[i]));
    }

    for (int i = 0; i < argc; i++) {
        if (args[i] == "-help" || args[i] == "-h") {
            mas::Help();
            exit(0);
        }
    }

    mas::Options<double> options;

    mas::ParseOptions(args, options);

    Run(options);




    //    std::cout << std::endl << "Optimization completed. Calculating variance-covariance matrix" << std::endl << std::endl;



    std::cout << std::endl << "Done" << std::endl << std::endl;



    return 0;
}

