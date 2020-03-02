/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MaximumSustainableYield.hpp
 * Author: mattadmin
 *
 * Created on September 4, 2019, 12:07 PM
 */

#ifndef MAXIMUMSUSTAINABLEYIELD_HPP
#define MAXIMUMSUSTAINABLEYIELD_HPP


namespace mas {

    template<typename REAL_T>
    struct MSY {
        typedef typename VariableTrait<REAL_T>::variable variable;
        std::vector<variable> biomass;
        std::vector<variable> yield;
        std::vector<variable> fishing_mortality;
        variable msy;
        variable b_msy;
        variable f_msy;
        variable mey;
        variable f_mey;
    };

}


#endif /* MAXIMUMSUSTAINABLEYIELD_HPP */

