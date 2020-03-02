/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   main.cpp
 * Author: z
 *
 * Created on May 10, 2017
 */

#include <string>

#include "CatchAtAge.hpp"

// using namespace std;

/*
 *
 */
int
main( int argc, char** argv )
{
    CatchAtAge<double> caa_obj_func;

    if (argc > 1)
    {
        caa_obj_func.Initialize(std::stoi(argv[1]));
    }
    else
    {
        caa_obj_func.Initialize(0);
    }

    atl::LBFGS<double> func_min;

    func_min.SetObjectiveFunction(&caa_obj_func);
    func_min.Run();

    caa_obj_func.Report();
    caa_obj_func.GetVarCovar();

    return 0;
}
