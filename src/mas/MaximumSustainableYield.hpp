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
    struct MaximumSustainableYield {
        REAL_T spr_F0=0;
        REAL_T Fmax=0;
        REAL_T msy=0;
        REAL_T F_msy=0;
        REAL_T spr_msy_out=0; // = spr[max_index]=0;
        REAL_T spr_msy=0;
        REAL_T SR_msy=0; // = spr_msy/spr_F0=0;
        REAL_T D_msy=0;
        REAL_T R_msy=0;
        REAL_T SSB_msy=0;
        REAL_T B_msy=0;
        REAL_T E_msy=0;

        //F30
        REAL_T F30=0;
        REAL_T spr_F30_msy=0;
        REAL_T SR_F30_msy=0;
        REAL_T D_F30_msy=0;
        REAL_T R_F30_msy=0;
        REAL_T SSB_F30_msy=0;
        REAL_T B_F30_msy=0;
        REAL_T E_F30_msy=0;

        //F35
        REAL_T F35=0;
        REAL_T spr_F35_msy=0;
        REAL_T SR_F35_msy=0;
        REAL_T D_F35_msy=0;
        REAL_T R_F35_msy=0;
        REAL_T SSB_F35_msy=0;
        REAL_T B_F35_msy=0;
        REAL_T E_F35_msy=0;

        //F40
        REAL_T F40=0;
        REAL_T spr_F40_msy=0;
        REAL_T SR_F40_msy=0;
        REAL_T D_F40_msy=0;
        REAL_T R_F40_msy=0;
        REAL_T SSB_F40_msy=0;
        REAL_T B_F40_msy=0;
        REAL_T E_F40_msy=0;
    };

}


#endif /* MAXIMUMSUSTAINABLEYIELD_HPP */




