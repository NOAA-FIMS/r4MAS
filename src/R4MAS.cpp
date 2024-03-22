/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   R4MAS.cpp
 * Author: mattadmin
 * 
 * Created on January 23, 2020, 9:03 AM
 */


//#include <TMB.hpp>
#include "R4MAS.hpp"
#include "init.hpp"

template<class Type>
Type objective_function<Type>::operator()() {
    std::shared_ptr<mas::MAS<Type> > mas = mas::MAS<Type>::GetInstance();

    
    PARAMETER_VECTOR(p);

    for (int i = 0; i < p.size(); i++) {
        *mas->info.estimated_parameters[i] = p[i];
        std::cout << p[i] << std::endl;
    }

    Type ret;

    mas->Run(ret);
    std::cout << "ret = " << mas::VariableTrait<Type>::GetValue(ret) << std::endl;
    return ret;
}