/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Output.hpp
 * Author: oppy
 *
 * Created on February 27, 2019, 9:09 AM
 */

#ifndef OUTPUT_HPP
#define OUTPUT_HPP

#include "Information.hpp"
#include "MAS.hpp"
#include "Population.hpp"
#include "Fleet.hpp"

namespace mas {

    template<typename REAL_T>
    class JSONOutputGenerator {
        typedef typename VariableTrait<REAL_T>::variable variable;
        typedef typename std::unordered_map<int, std::shared_ptr<mas::Population<REAL_T> > >::iterator population_iterator;
        typedef typename std::unordered_map<int, std::shared_ptr<mas::Fleet<REAL_T> > >::iterator fleet_iterator;
        typedef typename std::unordered_map<int, std::shared_ptr<mas::Survey<REAL_T> > >::iterator survey_iterator;
        rapidjson::Document document;

        void GenerateArrayObject(rapidjson::Value& array,
                const std::vector<variable>& darray,
                int dimensions,
                size_t imax,
                size_t jmax,
                size_t kmax) {



            size_t i, j, k;

            rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

            if (dimensions == 1) {

                for (i = 0; i < imax; i++) {
                    array.PushBack(darray[i].GetValue(), allocator);
                }

            } else if (dimensions == 2) {
                for (i = 0; i < imax; i++) {
                    array.PushBack(rapidjson::Value(rapidjson::kArrayType), allocator);
                    for (j = 0; j < jmax; j++) {
                        size_t index = i * jmax + j;
                        array[i].PushBack(darray[index].GetValue(), allocator);
                    }
                }

            } else if (dimensions == 3) {

                for (i = 0; i < imax; i++) {
                    array.PushBack(rapidjson::Value(rapidjson::kArrayType), allocator);
                    for (j = 0; j < jmax; j++) {
                        array[i].PushBack(rapidjson::Value(rapidjson::kArrayType), allocator);
                        for (k = 0; k < kmax; k++) {
                            size_t index = i * jmax * kmax + j * kmax + k;
                            array[i][j].PushBack(darray[index].GetValue(), allocator);
                        }
                    }
                }

            }

        }

        void GenerateArrayObject(rapidjson::Value& array,
                const std::shared_ptr<DataObject<REAL_T> >& darray,
                int dimensions,
                size_t imax,
                size_t jmax,
                size_t kmax) {



            size_t i, j, k;

            rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

            if (dimensions == 1) {

                for (i = 0; i < imax; i++) {
                    array.PushBack(darray->get(i), allocator);
                }

            } else if (dimensions == 2) {
                for (i = 0; i < imax; i++) {
                    array.PushBack(rapidjson::Value(rapidjson::kArrayType), allocator);
                    for (j = 0; j < jmax; j++) {
                        array[i].PushBack(darray->get(i, j), allocator);
                    }
                }

            } else if (dimensions == 3) {

                for (i = 0; i < imax; i++) {
                    array.PushBack(rapidjson::Value(rapidjson::kArrayType), allocator);
                    for (j = 0; j < jmax; j++) {
                        array[i].PushBack(rapidjson::Value(rapidjson::kArrayType), allocator);
                        for (k = 0; k < kmax; k++) {
                            size_t index = i * jmax * kmax + j * kmax + k;
                            array[i][j].PushBack(darray->get(i, j, k), allocator);
                        }
                    }
                }

            }

        }

        void GeneratePopulationInitialNumbers(rapidjson::Value& popobject,
                const mas::AreaPopulationInfo<REAL_T> & popinfo) {
            rapidjson::Value obj(rapidjson::kArrayType);
            this->GenerateArrayObject(obj, popinfo.initial_numbers, 1, popinfo.ages.size(), popinfo.seasons, popinfo.ages.size());
            popobject.AddMember("values", obj, this->document.GetAllocator());

        }

        void GeneratePopulationInitialNumbersEquilibrium(rapidjson::Value& popobject,
                const mas::AreaPopulationInfo<REAL_T> & popinfo) {
            rapidjson::Value obj(rapidjson::kArrayType);
            this->GenerateArrayObject(obj, popinfo.initial_equilibrium_numbers, 1, popinfo.ages.size(), popinfo.seasons, popinfo.ages.size());
            popobject.AddMember("values", obj, this->document.GetAllocator());

        }

        void GenerateLengthAtSeasonStart(rapidjson::Value& popobject,
                const mas::AreaPopulationInfo<REAL_T> & popinfo) {

            rapidjson::Value obj(rapidjson::kArrayType);
            this->GenerateArrayObject(obj, popinfo.length_at_season_start, 3, popinfo.years, popinfo.seasons, popinfo.ages.size());
            popobject.AddMember("values", obj, this->document.GetAllocator());
        }

        void GenerateLengthAtSpawning(rapidjson::Value& popobject,
                const mas::AreaPopulationInfo<REAL_T> & popinfo) {

            rapidjson::Value obj(rapidjson::kArrayType);
            this->GenerateArrayObject(obj, popinfo.length_at_spawning, 3, popinfo.years, popinfo.seasons, popinfo.ages.size());
            popobject.AddMember("values", obj, this->document.GetAllocator());
        }

        void GenerateLengthAtCatch(rapidjson::Value& popobject,
                const mas::AreaPopulationInfo<REAL_T> & popinfo) {

            rapidjson::Value obj(rapidjson::kArrayType);
            this->GenerateArrayObject(obj, popinfo.length_at_catch_time, 3, popinfo.years, popinfo.seasons, popinfo.ages.size());
            popobject.AddMember("values", obj, this->document.GetAllocator());
        }

        void GenerateLengthAtSurvey(rapidjson::Value& popobject,
                const mas::AreaPopulationInfo<REAL_T> & popinfo) {

            rapidjson::Value obj(rapidjson::kArrayType);
            this->GenerateArrayObject(obj, popinfo.length_at_survey_time, 3, popinfo.years, popinfo.seasons, popinfo.ages.size());
            popobject.AddMember("values", obj, this->document.GetAllocator());
        }

        void GenerateWeightAtSeasonStart(rapidjson::Value& popobject,
                const mas::AreaPopulationInfo<REAL_T> & popinfo) {

            rapidjson::Value obj(rapidjson::kArrayType);
            this->GenerateArrayObject(obj, popinfo.weight_at_season_start, 3, popinfo.years, popinfo.seasons, popinfo.ages.size());
            popobject.AddMember("values", obj, this->document.GetAllocator());
        }

        void GenerateWeightAtSpawning(rapidjson::Value& popobject,
                const mas::AreaPopulationInfo<REAL_T> & popinfo) {

            rapidjson::Value obj(rapidjson::kArrayType);
            this->GenerateArrayObject(obj, popinfo.weight_at_spawning, 3, popinfo.years, popinfo.seasons, popinfo.ages.size());
            popobject.AddMember("values", obj, this->document.GetAllocator());
        }

        void GeneratePopulationRecruitmentDeviations(rapidjson::Value& popobject,
                const mas::AreaPopulationInfo<REAL_T> & popinfo) {

        }

        void GeneratePopulationInitialDeviations(rapidjson::Value& popobject,
                const mas::AreaPopulationInfo<REAL_T> & popinfo) {

        }

        void GeneratePopulationNumbers(rapidjson::Value& popobject,
                const mas::AreaPopulationInfo<REAL_T> & popinfo) {

            rapidjson::Value obj(rapidjson::kArrayType);
            this->GenerateArrayObject(obj, popinfo.N, 3, popinfo.years, popinfo.seasons, popinfo.ages.size());
            popobject.AddMember("values", obj, this->document.GetAllocator());
        }

        void GeneratePopulationFishingMortality(rapidjson::Value& popobject,
                const mas::AreaPopulationInfo<REAL_T> & popinfo) {
            rapidjson::Value obj(rapidjson::kArrayType);
            this->GenerateArrayObject(obj, popinfo.F, 3, popinfo.years, popinfo.seasons, popinfo.ages.size());
            popobject.AddMember("values", obj, this->document.GetAllocator());
        }

        void GeneratePopulationImigrants(rapidjson::Value& popobject,
                const mas::AreaPopulationInfo<REAL_T> & popinfo) {
            rapidjson::Value obj(rapidjson::kArrayType);
            this->GenerateArrayObject(obj, popinfo.imigrants, 3, popinfo.years, popinfo.seasons, popinfo.ages.size());
            popobject.AddMember("values", obj, this->document.GetAllocator());

        }

        void GeneratePopulationEmigrants(rapidjson::Value& popobject,
                const mas::AreaPopulationInfo<REAL_T> & popinfo) {
            rapidjson::Value obj(rapidjson::kArrayType);
            this->GenerateArrayObject(obj, popinfo.emigrants, 3, popinfo.years, popinfo.seasons, popinfo.ages.size());
            popobject.AddMember("values", obj, this->document.GetAllocator());

        }

        void GeneratePopulationRedistributedRecruits(rapidjson::Value& popobject,
                const mas::AreaPopulationInfo<REAL_T> & popinfo) {
            rapidjson::Value obj(rapidjson::kArrayType);
            this->GenerateArrayObject(obj, popinfo.redistributed_recruits, 2, popinfo.years, popinfo.seasons, popinfo.ages.size());
            popobject.AddMember("values", obj, this->document.GetAllocator());
        }

        void GeneratePopulationMortality(rapidjson::Value& popobject,
                const mas::AreaPopulationInfo<REAL_T> & popinfo) {
            rapidjson::Value obj(rapidjson::kArrayType);
            this->GenerateArrayObject(obj, popinfo.Z, 3, popinfo.years, popinfo.seasons, popinfo.ages.size());
            popobject.AddMember("values", obj, this->document.GetAllocator());

        }

        void GeneratePopulationCatch(rapidjson::Value& popobject,
                const mas::AreaPopulationInfo<REAL_T> & popinfo) {
            rapidjson::Value obj(rapidjson::kArrayType);
            this->GenerateArrayObject(obj, popinfo.C, 3, popinfo.years, popinfo.seasons, popinfo.ages.size());
            popobject.AddMember("values", obj, this->document.GetAllocator());
        }

        void GeneratePopulationCatchBiomass(rapidjson::Value& popobject,
                const mas::AreaPopulationInfo<REAL_T> & popinfo) {
            rapidjson::Value obj(rapidjson::kArrayType);
            this->GenerateArrayObject(obj, popinfo.C_Biomass, 3, popinfo.years, popinfo.seasons, popinfo.ages.size());
            popobject.AddMember("values", obj, this->document.GetAllocator());
        }

        void GeneratePopulationSurvey(rapidjson::Value& popobject,
                const mas::AreaPopulationInfo<REAL_T> & popinfo) {
            rapidjson::Value obj(rapidjson::kArrayType);
            this->GenerateArrayObject(obj, popinfo.SN, 3, popinfo.years, popinfo.seasons, popinfo.ages.size());
            popobject.AddMember("values", obj, this->document.GetAllocator());
        }

        void GeneratePopulationSurveyBiomass(rapidjson::Value& popobject,
                const mas::AreaPopulationInfo<REAL_T> & popinfo) {
            rapidjson::Value obj(rapidjson::kArrayType);
            this->GenerateArrayObject(obj, popinfo.S_Index, 3, popinfo.years, popinfo.seasons, popinfo.ages.size());
            popobject.AddMember("values", obj, this->document.GetAllocator());

        }

        void GeneratePopulationRecruitment(rapidjson::Value& popobject,
                const mas::AreaPopulationInfo<REAL_T> & popinfo) {
            rapidjson::Value obj(rapidjson::kArrayType);
            this->GenerateArrayObject(obj, popinfo.recruitment, 2, popinfo.years, popinfo.seasons, popinfo.ages.size());
            popobject.AddMember("values", obj, this->document.GetAllocator());
        }

        void GenerateFleetOutput(rapidjson::Value& popobject,
                const std::shared_ptr<mas::Fleet<REAL_T> >& fleet) {

            rapidjson::Value females(rapidjson::kObjectType);
            rapidjson::Value males(rapidjson::kObjectType);
            rapidjson::Value undiff(rapidjson::kObjectType);

            rapidjson::Value expected_catch_biomassfemales(rapidjson::kObjectType);
            rapidjson::Value expected_catch_biomass_females_t(rapidjson::kObjectType);
            rapidjson::Value expected_catch_biomass_males(rapidjson::kObjectType);
            rapidjson::Value expected_catch_biomass_males_t(rapidjson::kObjectType);
            rapidjson::Value expected_catch_biomas_totals(rapidjson::kObjectType);
            rapidjson::Value expected_catch_biomas_totals_t(rapidjson::kObjectType);

            rapidjson::Value expected_catch_numbers_females(rapidjson::kObjectType);
            rapidjson::Value expected_catch_numbers_males(rapidjson::kObjectType);
            rapidjson::Value expected_catch_numbers_total(rapidjson::kObjectType);

            rapidjson::Value expected_catch_proportions_females(rapidjson::kObjectType);
            rapidjson::Value expected_catch_proportions_males(rapidjson::kObjectType);
            rapidjson::Value expected_catch_proportions_total(rapidjson::kObjectType);

            rapidjson::Value observed(rapidjson::kObjectType);

            rapidjson::Value array(rapidjson::kArrayType);
            this->GenerateArrayObject(array, fleet->numbers_at_age_females, 3, fleet->years, fleet->seasons, fleet->ages);
            expected_catch_numbers_females.AddMember("values", array, this->document.GetAllocator());
            females.AddMember("catch_numbers_at_age", expected_catch_numbers_females, this->document.GetAllocator());

            rapidjson::Value array2(rapidjson::kArrayType);
            this->GenerateArrayObject(array2, fleet->catch_proportion_at_age_females, 3, fleet->years, fleet->seasons, fleet->ages);
            expected_catch_proportions_females.AddMember("values", array2, this->document.GetAllocator());
            females.AddMember("catch_proportion_at_age", expected_catch_proportions_females, this->document.GetAllocator());

            rapidjson::Value array3(rapidjson::kArrayType);
            this->GenerateArrayObject(array3, fleet->catch_biomass_at_age_females, 3, fleet->years, fleet->seasons, fleet->ages);
            expected_catch_biomassfemales.AddMember("values", array3, this->document.GetAllocator());
            females.AddMember("catch_biomass_at_age", expected_catch_biomassfemales, this->document.GetAllocator());

            rapidjson::Value array8(rapidjson::kArrayType);
            this->GenerateArrayObject(array8, fleet->catch_biomass_total_females, 2, fleet->years, fleet->seasons, fleet->ages);
            expected_catch_biomass_females_t.AddMember("values", array8, this->document.GetAllocator());
            females.AddMember("catch_biomass", expected_catch_biomass_females_t, this->document.GetAllocator());

            popobject.AddMember("females", females, this->document.GetAllocator());

            rapidjson::Value array4(rapidjson::kArrayType);
            this->GenerateArrayObject(array4, fleet->numbers_at_age_males, 3, fleet->years, fleet->seasons, fleet->ages);
            expected_catch_numbers_males.AddMember("values", array4, this->document.GetAllocator());
            males.AddMember("catch_numbers_at_age", expected_catch_numbers_males, this->document.GetAllocator());

            rapidjson::Value array5(rapidjson::kArrayType);
            this->GenerateArrayObject(array5, fleet->catch_proportion_at_age_males, 3, fleet->years, fleet->seasons, fleet->ages);
            expected_catch_proportions_males.AddMember("values", array5, this->document.GetAllocator());
            males.AddMember("catch_proportion_at_age", expected_catch_proportions_males, this->document.GetAllocator());

            rapidjson::Value array6(rapidjson::kArrayType);
            this->GenerateArrayObject(array6, fleet->catch_biomass_at_age_males, 3, fleet->years, fleet->seasons, fleet->ages);
            expected_catch_biomass_males.AddMember("values", array6, this->document.GetAllocator());
            males.AddMember("catch_biomass_at_age", expected_catch_biomass_males, this->document.GetAllocator());

            rapidjson::Value array7(rapidjson::kArrayType);
            this->GenerateArrayObject(array7, fleet->catch_biomass_total_males, 2, fleet->years, fleet->seasons, fleet->ages);
            expected_catch_biomass_males_t.AddMember("values", array7, this->document.GetAllocator());
            males.AddMember("catch_biomass", expected_catch_biomass_males_t, this->document.GetAllocator());
            popobject.AddMember("males", males, this->document.GetAllocator());

            rapidjson::Value array9(rapidjson::kArrayType);
            this->GenerateArrayObject(array9, fleet->numbers_at_age, 3, fleet->years, fleet->seasons, fleet->ages);
            expected_catch_numbers_total.AddMember("values", array9, this->document.GetAllocator());
            undiff.AddMember("catch_numbers_at_age", expected_catch_numbers_total, this->document.GetAllocator());

            rapidjson::Value array10(rapidjson::kArrayType);
            this->GenerateArrayObject(array10, fleet->catch_proportion_at_age, 3, fleet->years, fleet->seasons, fleet->ages);
            expected_catch_proportions_total.AddMember("values", array10, this->document.GetAllocator());
            undiff.AddMember("catch_proportion_at_age", expected_catch_proportions_total, this->document.GetAllocator());

            rapidjson::Value array11(rapidjson::kArrayType);
            this->GenerateArrayObject(array11, fleet->catch_biomass_at_age, 3, fleet->years, fleet->seasons, fleet->ages);
            expected_catch_biomas_totals.AddMember("values", array11, this->document.GetAllocator());
            undiff.AddMember("catch_biomass_at_age", expected_catch_biomas_totals, this->document.GetAllocator());

            rapidjson::Value array12(rapidjson::kArrayType);
            this->GenerateArrayObject(array12, fleet->catch_biomass_total, 2, fleet->years, fleet->seasons, fleet->ages);
            expected_catch_biomas_totals_t.AddMember("values", array12, this->document.GetAllocator());
            undiff.AddMember("catch_biomass", expected_catch_biomas_totals_t, this->document.GetAllocator());
            
            popobject.AddMember("undifferentiated", undiff, this->document.GetAllocator());

            for (size_t i = 0; i < fleet->data_objects.size(); i++) {
                rapidjson::Value obj(rapidjson::kObjectType);
                if (fleet->data_objects[i]->type == mas::CATCH_BIOMASS) {


                    if (fleet->data_objects[i]->sex_type == mas::FEMALE) {
                        rapidjson::Value array(rapidjson::kArrayType);
                        this->GenerateArrayObject(array, fleet->data_objects[i], 2, fleet->years, fleet->seasons, fleet->ages);
                        obj.AddMember("sex", "females", this->document.GetAllocator());
                        obj.AddMember("values", array, this->document.GetAllocator());
                        observed.AddMember("fleet_biomass", obj, this->document.GetAllocator());
                    } else if (fleet->data_objects[i]->sex_type == mas::MALE) {
                        rapidjson::Value array(rapidjson::kArrayType);
                        this->GenerateArrayObject(array, fleet->data_objects[i], 2, fleet->years, fleet->seasons, fleet->ages);
                        obj.AddMember("sex", "males", this->document.GetAllocator());
                        obj.AddMember("values", array, this->document.GetAllocator());
                        observed.AddMember("fleet_biomass", obj, this->document.GetAllocator());

                    } else if (fleet->data_objects[i]->sex_type == mas::UNDIFFERENTIATED) {
                        rapidjson::Value array(rapidjson::kArrayType);
                        this->GenerateArrayObject(array, fleet->data_objects[i], 2, fleet->years, fleet->seasons, fleet->ages);
                        obj.AddMember("sex", "undifferentiated", this->document.GetAllocator());
                        obj.AddMember("values", array, this->document.GetAllocator());
                        observed.AddMember("fleet_biomass", obj, this->document.GetAllocator());

                    }


                } else if (fleet->data_objects[i]->type == mas::CATCH_PROPORTION_AT_AGE_N) {
                } else if (fleet->data_objects[i]->type == mas::CATCH_PROPORTION_AT_AGE) {



                    if (fleet->data_objects[i]->sex_type == mas::FEMALE) {
                        rapidjson::Value array(rapidjson::kArrayType);
                        this->GenerateArrayObject(array, fleet->data_objects[i], 3, fleet->years, fleet->seasons, fleet->ages);
                        obj.AddMember("sex", "females", this->document.GetAllocator());
                        obj.AddMember("values", array, this->document.GetAllocator());
                        observed.AddMember("fleet_proportion_at_age", obj, this->document.GetAllocator());
                    } else if (fleet->data_objects[i]->sex_type == mas::MALE) {
                        rapidjson::Value array(rapidjson::kArrayType);
                        this->GenerateArrayObject(array, fleet->data_objects[i], 3, fleet->years, fleet->seasons, fleet->ages);
                        obj.AddMember("sex", "males", this->document.GetAllocator());
                        obj.AddMember("values", array, this->document.GetAllocator());
                        observed.AddMember("fleet_proportion_at_age", obj, this->document.GetAllocator());
                    } else if (fleet->data_objects[i]->sex_type == mas::UNDIFFERENTIATED) {
                        rapidjson::Value array(rapidjson::kArrayType);
                        this->GenerateArrayObject(array, fleet->data_objects[i], 3, fleet->years, fleet->seasons, fleet->ages);
                        obj.AddMember("sex", "undifferentiated", this->document.GetAllocator());
                        obj.AddMember("values", array, this->document.GetAllocator());
                        observed.AddMember("fleet_proportion_at_age", obj, this->document.GetAllocator());
                    }

                } else if (fleet->data_objects[i]->type == mas::CATCH_PROPORTION_AT_LENGTH_N) {
                } else if (fleet->data_objects[i]->type == mas::CATCH_PROPORTION_AT_LENGTH) {
                } else if (fleet->data_objects[i]->type == mas::CATCH_MEAN_SIZE_AT_AGE) {
                } else if (fleet->data_objects[i]->type == mas::CATCH_MEAN_WEIGHT_AT_AGE) {
                }

            }
            popobject.AddMember("observed", observed, this->document.GetAllocator());

            //            rapidjson::Value cbmfobj(rapidjson::kObjectType);
            //            rapidjson::Value array(rapidjson::kArrayType);
            //            this->GenerateArrayObject(array, fleet->catch_biomass_total_females, 2, fleet->years, fleet->seasons, fleet->ages);
            //            cbmfobj.AddMember("sex", "females", this->document.GetAllocator());
            //            cbmfobj.AddMember("values", array, this->document.GetAllocator());
            //
            //            expected_catch_biomass.AddMember("expected_total_catch_biomass_females", cbmfobj, this->document.GetAllocator());
            //
            //            rapidjson::Value cbmmobj(rapidjson::kObjectType);
            //            rapidjson::Value array(rapidjson::kArrayType);
            //            this->GenerateArrayObject(array, fleet->catch_biomass_total_males, 2, fleet->years, fleet->seasons, fleet->ages);
            //            cbmmobj.AddMember("sex", "males", this->document.GetAllocator());
            //            cbmmobj.AddMember("values", array, this->document.GetAllocator());
            //
            //            popobject.AddMember("expected_total_catch_biomass_males", cbmmobj, this->document.GetAllocator());
            //
            //            rapidjson::Value cbmmobj(rapidjson::kObjectType);
            //            rapidjson::Value array(rapidjson::kArrayType);
            //            this->GenerateArrayObject(array, fleet->catch_biomass_total_males, 2, fleet->years, fleet->seasons, fleet->ages);
            //            cbmmobj.AddMember("sex", "males", this->document.GetAllocator());
            //            cbmmobj.AddMember("values", array, this->document.GetAllocator());
            //
            //            popobject.AddMember("catch_biomass_total", cbmmobj, this->document.GetAllocator());

            //            for (size_t i = 0; i < fleet->data_objects.size(); i++) {
            //
            //                if (fleet->data_objects[i]->type == mas::CATCH_BIOMASS) {
            //                    rapidjson::Value obj(rapidjson::kObjectType);
            //
            //                    if (fleet->data_objects[i]->sex_type == mas::FEMALE) {
            //                        rapidjson::Value array(rapidjson::kArrayType);
            //                        this->GenerateArrayObject(array, fleet->catch_biomass_total_females, 2, fleet->years, fleet->seasons, fleet->ages);
            //                        obj.AddMember("sex", "females", this->document.GetAllocator());
            //                        obj.AddMember("values", array, this->document.GetAllocator());
            //
            //                        //catch_biomass_total_females
            //                    } else if (fleet->data_objects[i]->sex_type == mas::MALE) {
            //                        rapidjson::Value array(rapidjson::kArrayType);
            //                        this->GenerateArrayObject(array, fleet->catch_biomass_total_males, 2, fleet->years, fleet->seasons, fleet->ages);
            //                        obj.AddMember("sex", "males", this->document.GetAllocator());
            //                        obj.AddMember("values", array, this->document.GetAllocator());
            //                        //catch_biomass_total_males
            //                    } else if (fleet->data_objects[i]->sex_type == mas::UNDIFFERENTIATED) {
            //                        rapidjson::Value array(rapidjson::kArrayType);
            //                        this->GenerateArrayObject(array, fleet->catch_biomass_total, 2, fleet->years, fleet->seasons, fleet->ages);
            //                        obj.AddMember("sex", "undifferentiated", this->document.GetAllocator());
            //                        obj.AddMember("values", array, this->document.GetAllocator());
            //                    }
            //
            //                    popobject.AddMember("catch_biomass_total", obj, this->document.GetAllocator());
            //                } else if (fleet->data_objects[i]->type == mas::CATCH_PROPORTION_AT_AGE_N) {
            //                } else if (fleet->data_objects[i]->type == mas::CATCH_PROPORTION_AT_AGE) {
            //
            //                    if (fleet->data_objects[i]->sex_type == mas::FEMALE) {
            //                        rapidjson::Value array(rapidjson::kArrayType);
            //                        this->GenerateArrayObject(array, fleet->proportion_at_age_females, 3, fleet->years, fleet->seasons, fleet->ages);
            //                        obj.AddMember("sex", "females", this->document.GetAllocator());
            //                        obj.AddMember("values", array, this->document.GetAllocator());
            //                    } else if (fleet->data_objects[i]->sex_type == mas::MALE) {
            //                        rapidjson::Value array(rapidjson::kArrayType);
            //                        this->GenerateArrayObject(array, fleet->proportion_at_age_males, 3, fleet->years, fleet->seasons, fleet->ages);
            //                        obj.AddMember("sex", "males", this->document.GetAllocator());
            //                        obj.AddMember("values", array, this->document.GetAllocator());
            //                    } else if (fleet->data_objects[i]->sex_type == mas::UNDIFFERENTIATED) {
            //                        rapidjson::Value array(rapidjson::kArrayType);
            //                        this->GenerateArrayObject(array, fleet->proportion_at_age_females, 3, fleet->years, fleet->seasons, fleet->ages);
            //                        obj.AddMember("sex", "females", this->document.GetAllocator());
            //                        obj.AddMember("values", array, this->document.GetAllocator());
            //                    }
            //
            //                } else if (fleet->data_objects[i]->type == mas::CATCH_PROPORTION_AT_LENGTH_N) {
            //                } else if (fleet->data_objects[i]->type == mas::CATCH_PROPORTION_AT_LENGTH) {
            //                } else if (fleet->data_objects[i]->type == mas::CATCH_MEAN_SIZE_AT_AGE) {
            //                } else if (fleet->data_objects[i]->type == mas::CATCH_MEAN_WEIGHT_AT_AGE) {
            //                }
            //
            //            }


        }

        void GenerateSurveyOutput(rapidjson::Value& popobject,
                const std::shared_ptr<mas::Survey<REAL_T> >& survey) {

            rapidjson::Value females(rapidjson::kObjectType);
            rapidjson::Value males(rapidjson::kObjectType);
            rapidjson::Value undiff(rapidjson::kObjectType);

            rapidjson::Value expected_survey_biomassfemales(rapidjson::kObjectType);
            rapidjson::Value expected_survey_biomass_females_t(rapidjson::kObjectType);
            rapidjson::Value expected_survey_biomass_males(rapidjson::kObjectType);
            rapidjson::Value expected_survey_biomass_males_t(rapidjson::kObjectType);
            rapidjson::Value expected_survey_biomas_totals(rapidjson::kObjectType);
            rapidjson::Value expected_survey_biomas_totals_t(rapidjson::kObjectType);

            rapidjson::Value expected_survey_numbers_females(rapidjson::kObjectType);
            rapidjson::Value expected_survey_numbers_males(rapidjson::kObjectType);
            rapidjson::Value expected_survey_numbers_total(rapidjson::kObjectType);

            rapidjson::Value expected_survey_proportions_females(rapidjson::kObjectType);
            rapidjson::Value expected_survey_proportions_males(rapidjson::kObjectType);
            rapidjson::Value expected_survey_proportions_total(rapidjson::kObjectType);

            rapidjson::Value observed(rapidjson::kObjectType);

            rapidjson::Value array(rapidjson::kArrayType);
            this->GenerateArrayObject(array, survey->survey_numbers_at_age_females, 3, survey->years, survey->seasons, survey->ages);
            expected_survey_numbers_females.AddMember("values", array, this->document.GetAllocator());
            females.AddMember("survey_numbers_at_age", expected_survey_numbers_females, this->document.GetAllocator());

            rapidjson::Value array2(rapidjson::kArrayType);
            this->GenerateArrayObject(array2, survey->survey_proportion_at_age_females, 3, survey->years, survey->seasons, survey->ages);
            expected_survey_proportions_females.AddMember("values", array2, this->document.GetAllocator());
            females.AddMember("survey_proportion_at_age", expected_survey_proportions_females, this->document.GetAllocator());

            rapidjson::Value array3(rapidjson::kArrayType);
            this->GenerateArrayObject(array3, survey->survey_biomass_at_age_females, 3, survey->years, survey->seasons, survey->ages);
            expected_survey_biomassfemales.AddMember("values", array3, this->document.GetAllocator());
            females.AddMember("survey_biomass_at_age", expected_survey_biomassfemales, this->document.GetAllocator());

            rapidjson::Value array8(rapidjson::kArrayType);
            this->GenerateArrayObject(array8, survey->survey_biomass_total_females, 2, survey->years, survey->seasons, survey->ages);
            expected_survey_biomass_females_t.AddMember("values", array8, this->document.GetAllocator());
            females.AddMember("survey_biomass", expected_survey_biomass_females_t, this->document.GetAllocator());

            popobject.AddMember("females", females, this->document.GetAllocator());

            rapidjson::Value array4(rapidjson::kArrayType);
            this->GenerateArrayObject(array4, survey->survey_numbers_at_age_males, 3, survey->years, survey->seasons, survey->ages);
            expected_survey_numbers_males.AddMember("values", array4, this->document.GetAllocator());
            males.AddMember("survey_numbers_at_age", expected_survey_numbers_males, this->document.GetAllocator());

            rapidjson::Value array5(rapidjson::kArrayType);
            this->GenerateArrayObject(array5, survey->survey_proportion_at_age_males, 3, survey->years, survey->seasons, survey->ages);
            expected_survey_proportions_males.AddMember("values", array5, this->document.GetAllocator());
            males.AddMember("survey_proportion_at_age", expected_survey_proportions_males, this->document.GetAllocator());

            rapidjson::Value array6(rapidjson::kArrayType);
            this->GenerateArrayObject(array6, survey->survey_biomass_at_age_males, 3, survey->years, survey->seasons, survey->ages);
            expected_survey_biomass_males.AddMember("values", array6, this->document.GetAllocator());
            males.AddMember("survey_biomass_at_age", expected_survey_biomass_males, this->document.GetAllocator());

            rapidjson::Value array7(rapidjson::kArrayType);
            this->GenerateArrayObject(array7, survey->survey_biomass_total_males, 2, survey->years, survey->seasons, survey->ages);
            expected_survey_biomass_males_t.AddMember("values", array7, this->document.GetAllocator());
            males.AddMember("survey_biomass", expected_survey_biomass_males_t, this->document.GetAllocator());
            popobject.AddMember("males", males, this->document.GetAllocator());

            rapidjson::Value array9(rapidjson::kArrayType);
            this->GenerateArrayObject(array9, survey->survey_numbers_at_age, 3, survey->years, survey->seasons, survey->ages);
            expected_survey_numbers_total.AddMember("values", array9, this->document.GetAllocator());
            undiff.AddMember("survey_numbers_at_age", expected_survey_numbers_total, this->document.GetAllocator());

            rapidjson::Value array10(rapidjson::kArrayType);
            this->GenerateArrayObject(array10, survey->survey_proportion_at_age, 3, survey->years, survey->seasons, survey->ages);
            expected_survey_proportions_total.AddMember("values", array10, this->document.GetAllocator());
            undiff.AddMember("survey_proportion_at_age", expected_survey_proportions_total, this->document.GetAllocator());

            rapidjson::Value array11(rapidjson::kArrayType);
            this->GenerateArrayObject(array11, survey->survey_biomass_at_age, 3, survey->years, survey->seasons, survey->ages);
            expected_survey_biomas_totals.AddMember("values", array11, this->document.GetAllocator());
            undiff.AddMember("survey_biomass_at_age", expected_survey_biomas_totals, this->document.GetAllocator());

            rapidjson::Value array12(rapidjson::kArrayType);
            this->GenerateArrayObject(array12, survey->survey_biomass_total, 2, survey->years, survey->seasons, survey->ages);
            expected_survey_biomas_totals_t.AddMember("values", array12, this->document.GetAllocator());
            undiff.AddMember("survey_biomass", expected_survey_biomas_totals_t, this->document.GetAllocator());
            popobject.AddMember("undifferentiated", undiff, this->document.GetAllocator());

            for (size_t i = 0; i < survey->data_objects.size(); i++) {
                rapidjson::Value obj(rapidjson::kObjectType);
                if (survey->data_objects[i]->type == mas::SURVEY_BIOMASS) {


                    if (survey->data_objects[i]->sex_type == mas::FEMALE) {
                        rapidjson::Value array(rapidjson::kArrayType);
                        this->GenerateArrayObject(array, survey->data_objects[i], 2, survey->years, survey->seasons, survey->ages);
                        obj.AddMember("sex", "females", this->document.GetAllocator());
                        obj.AddMember("values", array, this->document.GetAllocator());
                        observed.AddMember("survey_biomass", obj, this->document.GetAllocator());
                    } else if (survey->data_objects[i]->sex_type == mas::MALE) {
                        rapidjson::Value array(rapidjson::kArrayType);
                        this->GenerateArrayObject(array, survey->data_objects[i], 2, survey->years, survey->seasons, survey->ages);
                        obj.AddMember("sex", "males", this->document.GetAllocator());
                        obj.AddMember("values", array, this->document.GetAllocator());
                        observed.AddMember("survey_biomass", obj, this->document.GetAllocator());

                    } else if (survey->data_objects[i]->sex_type == mas::UNDIFFERENTIATED) {
                        rapidjson::Value array(rapidjson::kArrayType);
                        this->GenerateArrayObject(array, survey->data_objects[i], 2, survey->years, survey->seasons, survey->ages);
                        obj.AddMember("sex", "undifferentiated", this->document.GetAllocator());
                        obj.AddMember("values", array, this->document.GetAllocator());
                        observed.AddMember("survey_biomass", obj, this->document.GetAllocator());

                    }


                } else if (survey->data_objects[i]->type == mas::SURVEY_PROPORTION_AT_AGE_N) {
                } else if (survey->data_objects[i]->type == mas::SURVEY_PROPORTION_AT_AGE) {



                    if (survey->data_objects[i]->sex_type == mas::FEMALE) {
                        rapidjson::Value array(rapidjson::kArrayType);
                        this->GenerateArrayObject(array, survey->data_objects[i], 3, survey->years, survey->seasons, survey->ages);
                        obj.AddMember("sex", "females", this->document.GetAllocator());
                        obj.AddMember("values", array, this->document.GetAllocator());
                        observed.AddMember("survey_proportion_at_age", obj, this->document.GetAllocator());
                    } else if (survey->data_objects[i]->sex_type == mas::MALE) {
                        rapidjson::Value array(rapidjson::kArrayType);
                        this->GenerateArrayObject(array, survey->data_objects[i], 3, survey->years, survey->seasons, survey->ages);
                        obj.AddMember("sex", "males", this->document.GetAllocator());
                        obj.AddMember("values", array, this->document.GetAllocator());
                        observed.AddMember("survey_proportion_at_age", obj, this->document.GetAllocator());
                    } else if (survey->data_objects[i]->sex_type == mas::UNDIFFERENTIATED) {
                        rapidjson::Value array(rapidjson::kArrayType);
                        this->GenerateArrayObject(array, survey->data_objects[i], 3, survey->years, survey->seasons, survey->ages);
                        obj.AddMember("sex", "undifferentiated", this->document.GetAllocator());
                        obj.AddMember("values", array, this->document.GetAllocator());
                        observed.AddMember("survey_proportion_at_age", obj, this->document.GetAllocator());
                    }

                } else if (survey->data_objects[i]->type == mas::SURVEY_PROPORTION_AT_LENGTH_N) {
                } else if (survey->data_objects[i]->type == mas::SURVEY_PROPORTION_AT_LENGTH) {
                } else if (survey->data_objects[i]->type == mas::SURVEY_MEAN_SIZE_AT_AGE) {
                } else if (survey->data_objects[i]->type == mas::SURVEY_MEAN_WEIGHT_AT_AGE) {
                }

            }

            popobject.AddMember("observed", observed, this->document.GetAllocator());

        }

        void GenerateAreaOutput(rapidjson::Value& popobject,
                const std::shared_ptr<mas::Area<REAL_T> >& area) {
        }

        void GenerateConfig(rapidjson::Value& popobject, const mas::Information<REAL_T>& info) {

        }

        void GenerateData(rapidjson::Value& popobject, const mas::Information<REAL_T>& info) {

        }


    public:

        const std::string GenerateOutput(mas::MAS<REAL_T>& mas) {
            mas::Information<REAL_T>* info = &mas.info;
            document.SetObject();
            rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
            document.AddMember("nyears", info->nyears, allocator);
            document.AddMember("nseasons", info->nseasons, allocator);
            document.AddMember("nages", static_cast<int> (info->ages.size()), allocator);

            rapidjson::Value estimated_parameters(rapidjson::kObjectType);
            REAL_T gof = mas.ComputeGoodnessOfFit();
            document.AddMember("chi_square", gof, allocator);
            for (int i = 0; i < mas.info.estimated_parameters.size(); i++) {
                rapidjson::Value parameter(rapidjson::kObjectType);
                std::string n = mas.info.estimated_parameters[i]->GetName();
                //                rapidjson::GenericStringRef<char> name(n.c_str(), n.size());
                rapidjson::Value name;
                name.SetString(n.c_str(), n.size(), allocator);
                parameter.AddMember("name", name, allocator);
                parameter.AddMember("value", mas.info.estimated_parameters[i]->GetValue(), allocator);
                parameter.AddMember("gradient_value", atl::Variable<REAL_T>::tape.Value(mas.info.estimated_parameters[i]->info->id), allocator);

                estimated_parameters.AddMember("parameter",
                        parameter, allocator);
            }

            rapidjson::Value stddev(rapidjson::kObjectType);
            rapidjson::Value stddev_values(rapidjson::kArrayType);
            for (int i = 0; i < mas.std_dev.GetSize(); i++) {
                stddev_values.PushBack(mas.std_dev(i), allocator);
            }

            stddev.AddMember("values", stddev_values, allocator);
            estimated_parameters.AddMember("standard_deviation", stddev, allocator);

            rapidjson::Value varcovar(rapidjson::kObjectType);
            rapidjson::Value varcovar_values(rapidjson::kArrayType);
           
            for (int i = 0; i < mas.variance_covaiance.GetRows(); i++) {
                varcovar_values.PushBack(rapidjson::Value(rapidjson::kArrayType), allocator);
                for (int j = 0; j < mas.variance_covaiance.GetColumns(); j++) {
                    varcovar_values[i].PushBack(mas.variance_covaiance(i, j), allocator);
                }
            }
            varcovar.AddMember("values", varcovar_values, allocator);
            estimated_parameters.AddMember("variance_covariance", varcovar, allocator);

            document.AddMember("estimated_parameters", estimated_parameters, allocator);
            population_iterator pit;

            rapidjson::Value popdyn(rapidjson::kObjectType);
            for (pit = info->populations.begin(); pit != info->populations.end(); ++pit) {
                rapidjson::Value Popobject(rapidjson::kObjectType);
                Popobject.AddMember("id", (*pit).second->id, allocator);


                for (int i = 0; i < (*pit).second->areas_list.size(); i++) {
                    
                    rapidjson::Value area(rapidjson::kObjectType);
                    area.AddMember("id", (*pit).second->areas_list[i]->id, allocator);
                    rapidjson::Value females(rapidjson::kObjectType);
                    rapidjson::Value fnumbers(rapidjson::kObjectType);
                    rapidjson::Value flengthseasonstart(rapidjson::kObjectType);
                    rapidjson::Value flengthspawning(rapidjson::kObjectType);
                    rapidjson::Value flengthcatch(rapidjson::kObjectType);
                    rapidjson::Value flengthsurvey(rapidjson::kObjectType);
                    rapidjson::Value fweightseasonstart(rapidjson::kObjectType);
                    rapidjson::Value fweightspawning(rapidjson::kObjectType);
                    rapidjson::Value init_numbers_eq(rapidjson::kObjectType);
                    rapidjson::Value init_numbers(rapidjson::kObjectType);

                    rapidjson::Value fnvalues(rapidjson::kArrayType);
                    this->GenerateLengthAtSeasonStart(flengthseasonstart,
                            (*pit).second->females[(*pit).second->areas_list[i]->id]);
                    females.AddMember("length_at_season_start", flengthseasonstart, allocator);

                    this->GenerateLengthAtSpawning(flengthspawning,
                            (*pit).second->females[(*pit).second->areas_list[i]->id]);
                    females.AddMember("length_at_spawning", flengthspawning, allocator);

                    this->GenerateLengthAtCatch(flengthcatch,
                            (*pit).second->females[(*pit).second->areas_list[i]->id]);
                    females.AddMember("length_at_catch", flengthcatch, allocator);

                    this->GenerateLengthAtSurvey(flengthsurvey,
                            (*pit).second->females[(*pit).second->areas_list[i]->id]);
                    females.AddMember("length_at_survey", flengthsurvey, allocator);

                    this->GenerateWeightAtSeasonStart(fweightseasonstart,
                            (*pit).second->females[(*pit).second->areas_list[i]->id]);
                    females.AddMember("length_at_season_start", fweightseasonstart, allocator);

                    this->GenerateWeightAtSpawning(fweightspawning,
                            (*pit).second->females[(*pit).second->areas_list[i]->id]);
                    females.AddMember("length_at_spawning", fweightspawning, allocator);

                    females.AddMember("unfished_spawning_biomass_equilibrium",
                            (*pit).second->females[(*pit).second->areas_list[i]->id].SB0, allocator);

                    females.AddMember("initial_f",
                            (*pit).second->females[(*pit).second->areas_list[i]->id].initialF, allocator);


                    this->GeneratePopulationInitialNumbersEquilibrium(init_numbers_eq,
                            (*pit).second->females[(*pit).second->areas_list[i]->id]);
                    females.AddMember("initial_numbers_equilibrium", init_numbers_eq, allocator);

                    this->GeneratePopulationInitialNumbers(init_numbers,
                            (*pit).second->females[(*pit).second->areas_list[i]->id]);
                    females.AddMember("initial_numbers", init_numbers, allocator);

                    rapidjson::Value frecruits(rapidjson::kObjectType);
                    this->GeneratePopulationRecruitment(frecruits,
                            (*pit).second->females[(*pit).second->areas_list[i]->id]);
                    females.AddMember("recruits", frecruits, allocator);

                    rapidjson::Value fredistrecruits(rapidjson::kObjectType);
                    this->GeneratePopulationRedistributedRecruits(fredistrecruits,
                            (*pit).second->females[(*pit).second->areas_list[i]->id]);
                    females.AddMember("redistributed_recruits", fredistrecruits, allocator);

                    this->GeneratePopulationNumbers(fnumbers, (*pit).second->females[(*pit).second->areas_list[i]->id]);
                    females.AddMember("N", fnumbers, allocator);

                    rapidjson::Value fcnumbers(rapidjson::kObjectType);
                    this->GeneratePopulationCatch(fcnumbers, (*pit).second->females[(*pit).second->areas_list[i]->id]);
                    females.AddMember("C", fcnumbers, allocator);

                    rapidjson::Value fcnumbersb(rapidjson::kObjectType);
                    this->GeneratePopulationCatchBiomass(fcnumbersb, (*pit).second->females[(*pit).second->areas_list[i]->id]);
                    females.AddMember("CB", fcnumbersb, allocator);

                    rapidjson::Value fsnumbers(rapidjson::kObjectType);
                    this->GeneratePopulationSurvey(fsnumbers, (*pit).second->females[(*pit).second->areas_list[i]->id]);
                    females.AddMember("SN", fsnumbers, allocator);

                    rapidjson::Value fsnumbersb(rapidjson::kObjectType);
                    this->GeneratePopulationSurveyBiomass(fsnumbersb, (*pit).second->females[(*pit).second->areas_list[i]->id]);
                    females.AddMember("SNB", fsnumbersb, allocator);


                    area.AddMember("females", females, allocator);


                    /**
                     * Males
                     */

                    rapidjson::Value males(rapidjson::kObjectType);
                    rapidjson::Value mnumbers(rapidjson::kObjectType);
                    rapidjson::Value mlengthseasonstart(rapidjson::kObjectType);
                    rapidjson::Value mlengthspawning(rapidjson::kObjectType);
                    rapidjson::Value mlengthcatch(rapidjson::kObjectType);
                    rapidjson::Value mlengthsurvey(rapidjson::kObjectType);
                    rapidjson::Value mweightseasonstart(rapidjson::kObjectType);
                    rapidjson::Value mweightspawning(rapidjson::kObjectType);
                    rapidjson::Value mminit_numbers_eq(rapidjson::kObjectType);
                    rapidjson::Value minit_numbers(rapidjson::kObjectType);

                    rapidjson::Value mnvalues(rapidjson::kArrayType);
                    this->GenerateLengthAtSeasonStart(mlengthseasonstart,
                            (*pit).second->males[(*pit).second->areas_list[i]->id]);
                    males.AddMember("length_at_season_start", mlengthseasonstart, allocator);

                    this->GenerateLengthAtSpawning(mlengthspawning,
                            (*pit).second->males[(*pit).second->areas_list[i]->id]);
                    males.AddMember("length_at_spawning", mlengthspawning, allocator);

                    this->GenerateLengthAtCatch(mlengthcatch,
                            (*pit).second->males[(*pit).second->areas_list[i]->id]);
                    males.AddMember("length_at_catch", mlengthcatch, allocator);

                    this->GenerateLengthAtSurvey(mlengthsurvey,
                            (*pit).second->males[(*pit).second->areas_list[i]->id]);
                    males.AddMember("length_at_survey", mlengthsurvey, allocator);

                    this->GenerateWeightAtSeasonStart(mweightseasonstart,
                            (*pit).second->males[(*pit).second->areas_list[i]->id]);
                    males.AddMember("length_at_season_start", mweightseasonstart, allocator);

                    this->GenerateWeightAtSpawning(mweightspawning,
                            (*pit).second->males[(*pit).second->areas_list[i]->id]);
                    males.AddMember("length_at_spawning", mweightspawning, allocator);

                    males.AddMember("unfished_spawning_biomass_equilibrium",
                            (*pit).second->males[(*pit).second->areas_list[i]->id].SB0, allocator);

                    males.AddMember("initial_f",
                            (*pit).second->males[(*pit).second->areas_list[i]->id].initialF, allocator);


                    this->GeneratePopulationInitialNumbersEquilibrium(mminit_numbers_eq,
                            (*pit).second->males[(*pit).second->areas_list[i]->id]);
                    males.AddMember("initial_numbers_equilibrium", mminit_numbers_eq, allocator);

                    this->GeneratePopulationInitialNumbers(minit_numbers,
                            (*pit).second->males[(*pit).second->areas_list[i]->id]);
                    males.AddMember("initial_numbers", minit_numbers, allocator);

                    rapidjson::Value mrecruits(rapidjson::kObjectType);
                    this->GeneratePopulationRecruitment(mrecruits,
                            (*pit).second->males[(*pit).second->areas_list[i]->id]);
                    males.AddMember("recruits", mrecruits, allocator);

                    rapidjson::Value mredistrecruits(rapidjson::kObjectType);
                    this->GeneratePopulationRedistributedRecruits(mredistrecruits,
                            (*pit).second->males[(*pit).second->areas_list[i]->id]);
                    males.AddMember("redistributed_recruits", mredistrecruits, allocator);

                    this->GeneratePopulationNumbers(mnumbers, (*pit).second->males[(*pit).second->areas_list[i]->id]);
                    males.AddMember("N", mnumbers, allocator);

                    rapidjson::Value mcnumbers(rapidjson::kObjectType);
                    this->GeneratePopulationCatch(mcnumbers, (*pit).second->males[(*pit).second->areas_list[i]->id]);
                    males.AddMember("C", mcnumbers, allocator);

                    rapidjson::Value mcnumbersb(rapidjson::kObjectType);
                    this->GeneratePopulationCatchBiomass(mcnumbersb, (*pit).second->males[(*pit).second->areas_list[i]->id]);
                    males.AddMember("CB", mcnumbersb, allocator);

                    rapidjson::Value msnumbers(rapidjson::kObjectType);
                    this->GeneratePopulationSurvey(msnumbers, (*pit).second->males[(*pit).second->areas_list[i]->id]);
                    males.AddMember("SN", msnumbers, allocator);

                    rapidjson::Value msnumbersb(rapidjson::kObjectType);
                    this->GeneratePopulationSurveyBiomass(msnumbersb, (*pit).second->males[(*pit).second->areas_list[i]->id]);
                    males.AddMember("SNB", msnumbersb, allocator);


                    area.AddMember("males", males, allocator);

                    Popobject.AddMember("area", area, allocator);


                }

                popdyn.AddMember("population", Popobject, allocator);

            }

            fleet_iterator fit;
            for (fit = mas.info.fleets.begin(); fit != mas.info.fleets.end(); ++fit) {
                rapidjson::Value fleet(rapidjson::kObjectType);
                fleet.AddMember("id", (*fit).second->id, allocator);
                this->GenerateFleetOutput(fleet, (*fit).second);
                popdyn.AddMember("fleet", fleet, allocator);
            }

            survey_iterator sit;
            for (sit = mas.info.survey_models.begin(); sit != mas.info.survey_models.end(); ++sit) {
                rapidjson::Value survey(rapidjson::kObjectType);
                survey.AddMember("id", (*sit).second->id, allocator);
                this->GenerateSurveyOutput(survey, (*sit).second);
                popdyn.AddMember("survey", survey, allocator);
            }

            document.AddMember("population_dynamics", popdyn, allocator);
            rapidjson::StringBuffer buffer;
            rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
            document.Accept(writer);

            return buffer.GetString();

        }

    };

}



#endif /* OUTPUT_HPP */

