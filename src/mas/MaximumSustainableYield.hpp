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
#include "Common.hpp"

namespace mas {

    template<typename REAL_T>
    struct MaximumSustainableYield {
    typedef typename mas::VariableTrait<REAL_T>::variable variable_t;


        variable_t spr_F0=0;
        variable_t Fmax=0;
        variable_t msy=0;
        variable_t F_msy=0;
        variable_t spr_msy_out=0; // = spr[max_index]=0;
        variable_t spr_msy=0;
        variable_t SR_msy=0; // = spr_msy/spr_F0=0;
        variable_t D_msy=0;
        variable_t R_msy=0;
        variable_t SSB_msy=0;
        variable_t B_msy=0;
        variable_t E_msy=0;

        //F30
        variable_t F30=0;
        variable_t spr_F30_msy=0;
        variable_t SR_F30_msy=0;
        variable_t D_F30_msy=0;
        variable_t R_F30_msy=0;
        variable_t SSB_F30_msy=0;
        variable_t B_F30_msy=0;
        variable_t E_F30_msy=0;

        //F35
        variable_t F35=0;
        variable_t spr_F35_msy=0;
        variable_t SR_F35_msy=0;
        variable_t D_F35_msy=0;
        variable_t R_F35_msy=0;
        variable_t SSB_F35_msy=0;
        variable_t B_F35_msy=0;
        variable_t E_F35_msy=0;

        //F40
        variable_t F40=0;
        variable_t spr_F40_msy=0;
        variable_t SR_F40_msy=0;
        variable_t D_F40_msy=0;
        variable_t R_F40_msy=0;
        variable_t SSB_F40_msy=0;
        variable_t B_F40_msy=0;
        variable_t E_F40_msy=0;




    };

}


#endif /* MAXIMUMSUSTAINABLEYIELD_HPP */




