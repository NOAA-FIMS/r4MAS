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
	typedef typename std::unordered_map<int,
			atl::intrusive_ptr<mas::Population<REAL_T> > >::iterator population_iterator;
	typedef typename std::unordered_map<int,
			atl::intrusive_ptr<mas::Fleet<REAL_T> > >::iterator fleet_iterator;
	typedef typename std::unordered_map<int,
			atl::intrusive_ptr<mas::Survey<REAL_T> > >::iterator survey_iterator;
	typedef typename std::unordered_map<int,
			atl::intrusive_ptr<mas::Area<REAL_T> > >::iterator area_iterator;
	rapidjson::Document document;

	void GenerateArrayObject(rapidjson::Value &array,
			const std::vector<variable> &darray, int dimensions, size_t imax,
			size_t jmax, size_t kmax) {

		size_t i, j, k;

		rapidjson::Document::AllocatorType &allocator = document.GetAllocator();

		if (dimensions == 1) {

			for (i = 0; i < imax; i++) {
				array.PushBack(darray[i].GetValue(), allocator);
			}

		} else if (dimensions == 2) {
			for (i = 0; i < imax; i++) {
				array.PushBack(rapidjson::Value(rapidjson::kArrayType),
						allocator);
				for (j = 0; j < jmax; j++) {
					size_t index = i * jmax + j;
					array[i].PushBack(darray[index].GetValue(), allocator);
				}
			}

		} else if (dimensions == 3) {

			for (i = 0; i < imax; i++) {
				array.PushBack(rapidjson::Value(rapidjson::kArrayType),
						allocator);
				for (j = 0; j < jmax; j++) {
					array[i].PushBack(rapidjson::Value(rapidjson::kArrayType),
							allocator);
					for (k = 0; k < kmax; k++) {
						size_t index = i * jmax * kmax + j * kmax + k;
						array[i][j].PushBack(darray[index].GetValue(),
								allocator);
					}
				}
			}

		}

	}

	void GenerateArrayObject(rapidjson::Value &array,
			const std::vector<REAL_T> &darray, int dimensions, size_t imax,
			size_t jmax, size_t kmax) {

		size_t i, j, k;

		rapidjson::Document::AllocatorType &allocator = document.GetAllocator();

		if (dimensions == 1) {

			for (i = 0; i < imax; i++) {
				array.PushBack(darray[i], allocator);
			}

		} else if (dimensions == 2) {
			for (i = 0; i < imax; i++) {
				array.PushBack(rapidjson::Value(rapidjson::kArrayType),
						allocator);
				for (j = 0; j < jmax; j++) {
					size_t index = i * jmax + j;
					array[i].PushBack(darray[index], allocator);
				}
			}

		} else if (dimensions == 3) {

			for (i = 0; i < imax; i++) {
				array.PushBack(rapidjson::Value(rapidjson::kArrayType),
						allocator);
				for (j = 0; j < jmax; j++) {
					array[i].PushBack(rapidjson::Value(rapidjson::kArrayType),
							allocator);
					for (k = 0; k < kmax; k++) {
						size_t index = i * jmax * kmax + j * kmax + k;
						array[i][j].PushBack(darray[index], allocator);
					}
				}
			}

		}

	}

	void GenerateArrayObject(rapidjson::Value &array,
			const atl::intrusive_ptr<DataObject<REAL_T> > &darray,
			int dimensions, size_t imax, size_t jmax, size_t kmax) {

		size_t i, j, k;

		rapidjson::Document::AllocatorType &allocator = document.GetAllocator();

		if (dimensions == 1) {

			for (i = 0; i < imax; i++) {
				array.PushBack(darray->get(i), allocator);
			}

		} else if (dimensions == 2) {
			for (i = 0; i < imax; i++) {
				array.PushBack(rapidjson::Value(rapidjson::kArrayType),
						allocator);
				for (j = 0; j < jmax; j++) {
					array[i].PushBack(darray->get(i, j), allocator);
				}
			}

		} else if (dimensions == 3) {

			for (i = 0; i < imax; i++) {
				array.PushBack(rapidjson::Value(rapidjson::kArrayType),
						allocator);
				for (j = 0; j < jmax; j++) {
					array[i].PushBack(rapidjson::Value(rapidjson::kArrayType),
							allocator);
					for (k = 0; k < kmax; k++) {
						size_t index = i * jmax * kmax + j * kmax + k;
						array[i][j].PushBack(darray->get(i, j, k), allocator);
					}
				}
			}

		}

	}

	void GeneratePopulationInitialNumbers(rapidjson::Value &popobject,
			const mas::Subpopulation<REAL_T> &popinfo) {
		rapidjson::Value obj(rapidjson::kArrayType);
		this->GenerateArrayObject(obj, popinfo.initial_numbers, 1,
				popinfo.ages.size(), popinfo.seasons, popinfo.ages.size());
		popobject.AddMember("values", obj, this->document.GetAllocator());

	}

	void GeneratePopulationInitialNumbersEquilibrium(
			rapidjson::Value &popobject,
			const mas::Subpopulation<REAL_T> &popinfo) {
		rapidjson::Value obj(rapidjson::kArrayType);
		this->GenerateArrayObject(obj, popinfo.initial_equilibrium_numbers, 1,
				popinfo.ages.size(), popinfo.seasons, popinfo.ages.size());
		popobject.AddMember("values", obj, this->document.GetAllocator());

	}

	void GenerateLengthAtSeasonStart(rapidjson::Value &popobject,
			const mas::Subpopulation<REAL_T> &popinfo) {

		rapidjson::Value obj(rapidjson::kArrayType);
		this->GenerateArrayObject(obj, popinfo.length_at_season_start, 3,
				popinfo.years, popinfo.seasons, popinfo.ages.size());
		popobject.AddMember("values", obj, this->document.GetAllocator());
	}

	void GenerateLengthAtSpawning(rapidjson::Value &popobject,
			const mas::Subpopulation<REAL_T> &popinfo) {

		rapidjson::Value obj(rapidjson::kArrayType);
		this->GenerateArrayObject(obj, popinfo.length_at_spawning, 3,
				popinfo.years, popinfo.seasons, popinfo.ages.size());
		popobject.AddMember("values", obj, this->document.GetAllocator());
	}

	void GenerateLengthAtCatch(rapidjson::Value &popobject,
			const mas::Subpopulation<REAL_T> &popinfo) {

		rapidjson::Value obj(rapidjson::kArrayType);
		this->GenerateArrayObject(obj, popinfo.length_at_catch_time, 3,
				popinfo.years, popinfo.seasons, popinfo.ages.size());
		popobject.AddMember("values", obj, this->document.GetAllocator());
	}

	void GenerateLengthAtSurvey(rapidjson::Value &popobject,
			const mas::Subpopulation<REAL_T> &popinfo) {

		rapidjson::Value obj(rapidjson::kArrayType);
		this->GenerateArrayObject(obj, popinfo.length_at_survey_time, 3,
				popinfo.years, popinfo.seasons, popinfo.ages.size());
		popobject.AddMember("values", obj, this->document.GetAllocator());
	}

	void GenerateWeightAtSeasonStart(rapidjson::Value &popobject,
			const mas::Subpopulation<REAL_T> &popinfo) {

		rapidjson::Value obj(rapidjson::kArrayType);
		this->GenerateArrayObject(obj, popinfo.weight_at_season_start, 3,
				popinfo.years, popinfo.seasons, popinfo.ages.size());
		popobject.AddMember("values", obj, this->document.GetAllocator());
	}

	void GenerateWeightAtSpawning(rapidjson::Value &popobject,
			const mas::Subpopulation<REAL_T> &popinfo) {

		rapidjson::Value obj(rapidjson::kArrayType);
		this->GenerateArrayObject(obj, popinfo.weight_at_spawning, 3,
				popinfo.years, popinfo.seasons, popinfo.ages.size());
		popobject.AddMember("values", obj, this->document.GetAllocator());
	}

	void GeneratePopulationRecruitmentDeviations(rapidjson::Value &popobject,
			const mas::Subpopulation<REAL_T> &popinfo) {

	}

	void GeneratePopulationInitialDeviations(rapidjson::Value &popobject,
			const mas::Subpopulation<REAL_T> &popinfo) {

	}

	void GeneratePopulationNumbers(rapidjson::Value &popobject,
			const mas::Subpopulation<REAL_T> &popinfo) {

		rapidjson::Value obj(rapidjson::kArrayType);
		this->GenerateArrayObject(obj, popinfo.numbers_at_age, 3, popinfo.years,
				popinfo.seasons, popinfo.ages.size());
		popobject.AddMember("values", obj, this->document.GetAllocator());
	}

	void GeneratePopulationNumbers(rapidjson::Value &popobject,
			const mas::Population<REAL_T> &popinfo,
			const mas::FishSexType &sex) {

		rapidjson::Value obj(rapidjson::kArrayType);
		switch (sex) {

		case mas::UNDIFFERENTIATED:
			this->GenerateArrayObject(obj, popinfo.numbers_at_age, 3,
					popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		case mas::MALE:
			this->GenerateArrayObject(obj, popinfo.numbers_at_age_males, 3,
					popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		case mas::FEMALE:
			this->GenerateArrayObject(obj, popinfo.numbers_at_age_females, 3,
					popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		}
		popobject.AddMember("values", obj, this->document.GetAllocator());
	}

	void GenerateAreaNumbers(rapidjson::Value &popobject,
			const mas::Area<REAL_T> &popinfo, const mas::FishSexType &sex) {
		rapidjson::Value obj(rapidjson::kArrayType);
		switch (sex) {

		case mas::UNDIFFERENTIATED:
			this->GenerateArrayObject(obj, popinfo.numbers_at_age, 3,
					popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		case mas::MALE:
			this->GenerateArrayObject(obj, popinfo.numbers_at_age_males, 3,
					popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		case mas::FEMALE:
			this->GenerateArrayObject(obj, popinfo.numbers_at_age_females, 3,
					popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		}
		popobject.AddMember("values", obj, this->document.GetAllocator());
	}

	void GeneratePopulationFishingMortality(rapidjson::Value &popobject,
			const mas::Subpopulation<REAL_T> &popinfo) {
		rapidjson::Value obj(rapidjson::kArrayType);

		this->GenerateArrayObject(obj, popinfo.F, 2, popinfo.years,
				popinfo.seasons, popinfo.ages.size());
		popobject.AddMember("values", obj, this->document.GetAllocator());
	}

	void GeneratePopulationFishingMortality(rapidjson::Value &popobject,
			const mas::Population<REAL_T> &popinfo,
			const mas::FishSexType &sex) {
		rapidjson::Value obj(rapidjson::kArrayType);
		switch (sex) {

		case mas::UNDIFFERENTIATED:
			this->GenerateArrayObject(obj, popinfo.fishing_mortality, 2,
					popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		case mas::MALE:
			this->GenerateArrayObject(obj, popinfo.fishing_mortality_males, 2,
					popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		case mas::FEMALE:
			this->GenerateArrayObject(obj, popinfo.fishing_mortality_females, 2,
					popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		}
		popobject.AddMember("values", obj, this->document.GetAllocator());
	}

	void GeneratePopulationImigrants(rapidjson::Value &popobject,
			const mas::Subpopulation<REAL_T> &popinfo) {
		rapidjson::Value obj(rapidjson::kArrayType);
		this->GenerateArrayObject(obj, popinfo.imigrants, 3, popinfo.years,
				popinfo.seasons, popinfo.ages.size());
		popobject.AddMember("values", obj, this->document.GetAllocator());

	}

	void GeneratePopulationEmigrants(rapidjson::Value &popobject,
			const mas::Subpopulation<REAL_T> &popinfo) {
		rapidjson::Value obj(rapidjson::kArrayType);
		this->GenerateArrayObject(obj, popinfo.emigrants, 3, popinfo.years,
				popinfo.seasons, popinfo.ages.size());
		popobject.AddMember("values", obj, this->document.GetAllocator());

	}

	void GeneratePopulationRecruits(rapidjson::Value &popobject,
			const mas::Subpopulation<REAL_T> &popinfo) {
		rapidjson::Value obj(rapidjson::kArrayType);
		this->GenerateArrayObject(obj, popinfo.recruitment, 2, popinfo.years,
				popinfo.seasons, popinfo.ages.size());
		popobject.AddMember("values", obj, this->document.GetAllocator());
	}

	void GeneratePopulationRecruits(rapidjson::Value &popobject,
			const mas::Population<REAL_T> &popinfo,
			const mas::FishSexType &sex) {
		rapidjson::Value obj(rapidjson::kArrayType);
		switch (sex) {

		case mas::UNDIFFERENTIATED:
			this->GenerateArrayObject(obj, popinfo.recruits, 2, popinfo.years,
					popinfo.seasons, popinfo.ages);
			break;
		case mas::MALE:
			this->GenerateArrayObject(obj, popinfo.recruits_males, 2,
					popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		case mas::FEMALE:
			this->GenerateArrayObject(obj, popinfo.recruits_females, 2,
					popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		}
		popobject.AddMember("values", obj, this->document.GetAllocator());
	}

	void GenerateAreaRecruits(rapidjson::Value &popobject,
			const mas::Area<REAL_T> &popinfo, const mas::FishSexType &sex) {
		rapidjson::Value obj(rapidjson::kArrayType);
		switch (sex) {

		case mas::UNDIFFERENTIATED:
			this->GenerateArrayObject(obj, popinfo.recruits, 2, popinfo.years,
					popinfo.seasons, popinfo.ages);
			break;
		case mas::MALE:
			this->GenerateArrayObject(obj, popinfo.recruits_males, 2,
					popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		case mas::FEMALE:
			this->GenerateArrayObject(obj, popinfo.recruits_females, 2,
					popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		}
		popobject.AddMember("values", obj, this->document.GetAllocator());
	}

	void GeneratePopulationRedistributedRecruits(rapidjson::Value &popobject,
			const mas::Subpopulation<REAL_T> &popinfo) {
		rapidjson::Value obj(rapidjson::kArrayType);
		this->GenerateArrayObject(obj, popinfo.redistributed_recruits, 2,
				popinfo.years, popinfo.seasons, popinfo.ages.size());
		popobject.AddMember("values", obj, this->document.GetAllocator());
	}

	void GeneratePopulationMortality(rapidjson::Value &popobject,
			const mas::Subpopulation<REAL_T> &popinfo) {
		rapidjson::Value obj(rapidjson::kArrayType);
		this->GenerateArrayObject(obj, popinfo.Z, 3, popinfo.years,
				popinfo.seasons, popinfo.ages.size());
		popobject.AddMember("values", obj, this->document.GetAllocator());

	}

	void GeneratePopulationCatch(rapidjson::Value &popobject,
			const mas::Subpopulation<REAL_T> &popinfo) {
		rapidjson::Value obj(rapidjson::kArrayType);
		this->GenerateArrayObject(obj, popinfo.catch_at_age, 3, popinfo.years,
				popinfo.seasons, popinfo.ages.size());
		popobject.AddMember("values", obj, this->document.GetAllocator());
	}

	void GeneratePopulationCatch(rapidjson::Value &popobject,
			const mas::Population<REAL_T> &popinfo,
			const mas::FishSexType &sex) {
		rapidjson::Value obj(rapidjson::kArrayType);
		switch (sex) {

		case mas::UNDIFFERENTIATED:
			this->GenerateArrayObject(obj, popinfo.catch_numbers_at_age, 3,
					popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		case mas::MALE:
			this->GenerateArrayObject(obj, popinfo.catch_numbers_at_age_males,
					3, popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		case mas::FEMALE:
			this->GenerateArrayObject(obj, popinfo.catch_numbers_at_age_females,
					3, popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		}

		popobject.AddMember("values", obj, this->document.GetAllocator());
	}

	void GenerateAreaCatch(rapidjson::Value &popobject,
			const mas::Area<REAL_T> &popinfo, const mas::FishSexType &sex) {
		rapidjson::Value obj(rapidjson::kArrayType);
		switch (sex) {

		case mas::UNDIFFERENTIATED:
			this->GenerateArrayObject(obj, popinfo.catch_at_age, 3,
					popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		case mas::MALE:
			this->GenerateArrayObject(obj, popinfo.catch_at_age_males, 3,
					popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		case mas::FEMALE:
			this->GenerateArrayObject(obj, popinfo.catch_at_age_females, 3,
					popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		}

		popobject.AddMember("values", obj, this->document.GetAllocator());
	}

	void GeneratePopulationCatchBiomass(rapidjson::Value &popobject,
			const mas::Subpopulation<REAL_T> &popinfo) {
		rapidjson::Value obj(rapidjson::kArrayType);
		this->GenerateArrayObject(obj, popinfo.catch_biomass_at_age, 3,
				popinfo.years, popinfo.seasons, popinfo.ages.size());
		popobject.AddMember("values", obj, this->document.GetAllocator());
	}

	void GeneratePopulationCatchBiomass(rapidjson::Value &popobject,
			const mas::Population<REAL_T> &popinfo,
			const mas::FishSexType &sex) {
		rapidjson::Value obj(rapidjson::kArrayType);
		switch (sex) {

		case mas::UNDIFFERENTIATED:
			this->GenerateArrayObject(obj, popinfo.catch_biomass_at_age, 3,
					popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		case mas::MALE:
			this->GenerateArrayObject(obj, popinfo.catch_biomass_at_age_males,
					3, popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		case mas::FEMALE:
			this->GenerateArrayObject(obj, popinfo.catch_biomass_at_age_females,
					3, popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		}
		popobject.AddMember("values", obj, this->document.GetAllocator());
	}

	void GenerateAreaCatchBiomass(rapidjson::Value &popobject,
			const mas::Area<REAL_T> &popinfo, const mas::FishSexType &sex) {
		rapidjson::Value obj(rapidjson::kArrayType);
		switch (sex) {

		case mas::UNDIFFERENTIATED:
			this->GenerateArrayObject(obj, popinfo.catch_biomass_at_age, 3,
					popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		case mas::MALE:
			this->GenerateArrayObject(obj, popinfo.catch_biomass_at_age_males,
					3, popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		case mas::FEMALE:
			this->GenerateArrayObject(obj, popinfo.catch_biomass_at_age_females,
					3, popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		}
		popobject.AddMember("values", obj, this->document.GetAllocator());
	}

	void GeneratePopulationSurvey(rapidjson::Value &popobject,
			const mas::Subpopulation<REAL_T> &popinfo) {
		rapidjson::Value obj(rapidjson::kArrayType);
		this->GenerateArrayObject(obj, popinfo.survey_numbers_at_age, 3,
				popinfo.years, popinfo.seasons, popinfo.ages.size());
		popobject.AddMember("values", obj, this->document.GetAllocator());
	}

	void GeneratePopulationSurvey(rapidjson::Value &popobject,
			const mas::Population<REAL_T> &popinfo,
			const mas::FishSexType &sex) {
		rapidjson::Value obj(rapidjson::kArrayType);
		switch (sex) {

		case mas::UNDIFFERENTIATED:
			this->GenerateArrayObject(obj, popinfo.survey_numbers_at_age, 3,
					popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		case mas::MALE:
			this->GenerateArrayObject(obj, popinfo.survey_numbers_at_age_males,
					3, popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		case mas::FEMALE:
			this->GenerateArrayObject(obj,
					popinfo.survey_numbers_at_age_females, 3, popinfo.years,
					popinfo.seasons, popinfo.ages);
			break;
		}
		popobject.AddMember("values", obj, this->document.GetAllocator());
	}

	void GenerateAreaSurvey(rapidjson::Value &popobject,
			const mas::Area<REAL_T> &popinfo, const mas::FishSexType &sex) {
		rapidjson::Value obj(rapidjson::kArrayType);
		switch (sex) {

		case mas::UNDIFFERENTIATED:
			this->GenerateArrayObject(obj, popinfo.survey_numbers_at_age, 3,
					popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		case mas::MALE:
			this->GenerateArrayObject(obj, popinfo.survey_numbers_at_age_males,
					3, popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		case mas::FEMALE:
			this->GenerateArrayObject(obj,
					popinfo.survey_numbers_at_age_females, 3, popinfo.years,
					popinfo.seasons, popinfo.ages);
			break;
		}
		popobject.AddMember("values", obj, this->document.GetAllocator());
	}

	void GeneratePopulationSurveyBiomass(rapidjson::Value &popobject,
			const mas::Subpopulation<REAL_T> &popinfo) {
		rapidjson::Value obj(rapidjson::kArrayType);

		this->GenerateArrayObject(obj, popinfo.survey_index_at_age, 3,
				popinfo.years, popinfo.seasons, popinfo.ages.size());
		popobject.AddMember("values", obj, this->document.GetAllocator());

	}

	void GeneratePopulationSurveyBiomass(rapidjson::Value &popobject,
			const mas::Population<REAL_T> &popinfo,
			const mas::FishSexType &sex) {
		rapidjson::Value obj(rapidjson::kArrayType);
		switch (sex) {

		case mas::UNDIFFERENTIATED:
			this->GenerateArrayObject(obj, popinfo.survey_biomass_at_age, 3,
					popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		case mas::MALE:
			this->GenerateArrayObject(obj, popinfo.survey_biomass_at_age_males,
					3, popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		case mas::FEMALE:
			this->GenerateArrayObject(obj,
					popinfo.survey_biomass_at_age_females, 3, popinfo.years,
					popinfo.seasons, popinfo.ages);
			break;
		}
		popobject.AddMember("values", obj, this->document.GetAllocator());

	}

	void GenerateReferencePoints(rapidjson::Value &popobject,
			const mas::Population<REAL_T> &popinfo,
			const mas::FishSexType &sex) {

		switch (sex) {

		case mas::UNDIFFERENTIATED:
			popobject.AddMember("note",
					"these values represent the sum across all subpopulations for this population",
					this->document.GetAllocator());
			popobject.AddMember("msy", popinfo.msy.msy,
					this->document.GetAllocator());
			popobject.AddMember("spr_F0", popinfo.msy.spr_F0,
					this->document.GetAllocator());
			popobject.AddMember("F_msy", popinfo.msy.F_msy,
					this->document.GetAllocator());
			popobject.AddMember("spr_msy", popinfo.msy.spr_msy,
					this->document.GetAllocator());
			popobject.AddMember("SR_msy", popinfo.msy.SR_msy,
					this->document.GetAllocator());
			popobject.AddMember("R_msy", popinfo.msy.R_msy,
					this->document.GetAllocator());
			popobject.AddMember("SSB_msy", popinfo.msy.SSB_msy,
					this->document.GetAllocator());
			popobject.AddMember("B_msy", popinfo.msy.B_msy,
					this->document.GetAllocator());
			popobject.AddMember("E_msy", popinfo.msy.E_msy,
					this->document.GetAllocator());
			popobject.AddMember("F30", popinfo.msy.F30,
					this->document.GetAllocator());
			popobject.AddMember("spr_F30_msy", popinfo.msy.spr_F30_msy,
					this->document.GetAllocator());
			popobject.AddMember("SR_F30_msy", popinfo.msy.SR_F30_msy,
					this->document.GetAllocator());
			popobject.AddMember("R_F30_msy", popinfo.msy.R_F30_msy,
					this->document.GetAllocator());
			popobject.AddMember("SSB_F30_msy", popinfo.msy.SSB_F30_msy,
					this->document.GetAllocator());
			popobject.AddMember("B_F30_msy", popinfo.msy.B_F30_msy,
					this->document.GetAllocator());
			popobject.AddMember("E_F30_msy", popinfo.msy.E_F30_msy,
					this->document.GetAllocator());
			popobject.AddMember("F35", popinfo.msy.F35,
					this->document.GetAllocator());
			popobject.AddMember("spr_F35_msy", popinfo.msy.spr_F35_msy,
					this->document.GetAllocator());
			popobject.AddMember("SR_F35_msy", popinfo.msy.SR_F35_msy,
					this->document.GetAllocator());
			popobject.AddMember("R_F35_msy", popinfo.msy.R_F35_msy,
					this->document.GetAllocator());
			popobject.AddMember("SSB_F35_msy", popinfo.msy.SSB_F35_msy,
					this->document.GetAllocator());
			popobject.AddMember("B_F35_msy", popinfo.msy.B_F35_msy,
					this->document.GetAllocator());
			popobject.AddMember("E_F35_msy", popinfo.msy.E_F35_msy,
					this->document.GetAllocator());
			popobject.AddMember("F40", popinfo.msy.F40,
					this->document.GetAllocator());
			popobject.AddMember("spr_F40_msy", popinfo.msy.spr_F40_msy,
					this->document.GetAllocator());
			popobject.AddMember("SR_F40_msy", popinfo.msy.SR_F40_msy,
					this->document.GetAllocator());
			popobject.AddMember("R_F40_msy", popinfo.msy.R_F40_msy,
					this->document.GetAllocator());
			popobject.AddMember("SSB_F40_msy", popinfo.msy.SSB_F40_msy,
					this->document.GetAllocator());
			popobject.AddMember("B_F40_msy", popinfo.msy.B_F40_msy,
					this->document.GetAllocator());
			popobject.AddMember("E_F40_msy", popinfo.msy.E_F40_msy,
					this->document.GetAllocator());
			break;
		case mas::FEMALE:
			popobject.AddMember("note",
					"these values represent the mean across all female subpopulations",
					this->document.GetAllocator());
			popobject.AddMember("msy", popinfo.msy_females.msy,
					this->document.GetAllocator());
			popobject.AddMember("spr_F0", popinfo.msy_females.spr_F0,
					this->document.GetAllocator());
			popobject.AddMember("F_msy", popinfo.msy_females.F_msy,
					this->document.GetAllocator());
			popobject.AddMember("spr_msy", popinfo.msy_females.spr_msy,
					this->document.GetAllocator());
			popobject.AddMember("SR_msy", popinfo.msy_females.SR_msy,
					this->document.GetAllocator());
			popobject.AddMember("R_msy", popinfo.msy_females.R_msy,
					this->document.GetAllocator());
			popobject.AddMember("SSB_msy", popinfo.msy_females.SSB_msy,
					this->document.GetAllocator());
			popobject.AddMember("B_msy", popinfo.msy_females.B_msy,
					this->document.GetAllocator());
			popobject.AddMember("E_msy", popinfo.msy_females.E_msy,
					this->document.GetAllocator());
			popobject.AddMember("F30", popinfo.msy_females.F30,
					this->document.GetAllocator());
			popobject.AddMember("spr_F30_msy", popinfo.msy_females.spr_F30_msy,
					this->document.GetAllocator());
			popobject.AddMember("SR_F30_msy", popinfo.msy_females.SR_F30_msy,
					this->document.GetAllocator());
			popobject.AddMember("R_F30_msy", popinfo.msy_females.R_F30_msy,
					this->document.GetAllocator());
			popobject.AddMember("SSB_F30_msy", popinfo.msy_females.SSB_F30_msy,
					this->document.GetAllocator());
			popobject.AddMember("B_F30_msy", popinfo.msy_females.B_F30_msy,
					this->document.GetAllocator());
			popobject.AddMember("E_F30_msy", popinfo.msy_females.E_F30_msy,
					this->document.GetAllocator());
			popobject.AddMember("F35", popinfo.msy_females.F35,
					this->document.GetAllocator());
			popobject.AddMember("spr_F35_msy", popinfo.msy_females.spr_F35_msy,
					this->document.GetAllocator());
			popobject.AddMember("SR_F35_msy", popinfo.msy_females.SR_F35_msy,
					this->document.GetAllocator());
			popobject.AddMember("R_F35_msy", popinfo.msy_females.R_F35_msy,
					this->document.GetAllocator());
			popobject.AddMember("SSB_F35_msy", popinfo.msy_females.SSB_F35_msy,
					this->document.GetAllocator());
			popobject.AddMember("B_F35_msy", popinfo.msy_females.B_F35_msy,
					this->document.GetAllocator());
			popobject.AddMember("E_F35_msy", popinfo.msy_females.E_F35_msy,
					this->document.GetAllocator());
			popobject.AddMember("F40", popinfo.msy_females.F40,
					this->document.GetAllocator());
			popobject.AddMember("spr_F40_msy", popinfo.msy_females.spr_F40_msy,
					this->document.GetAllocator());
			popobject.AddMember("SR_F40_msy", popinfo.msy_females.SR_F40_msy,
					this->document.GetAllocator());
			popobject.AddMember("R_F40_msy", popinfo.msy_females.R_F40_msy,
					this->document.GetAllocator());
			popobject.AddMember("SSB_F40_msy", popinfo.msy_females.SSB_F40_msy,
					this->document.GetAllocator());
			popobject.AddMember("B_F40_msy", popinfo.msy_females.B_F40_msy,
					this->document.GetAllocator());
			popobject.AddMember("E_F40_msy", popinfo.msy_females.E_F40_msy,
					this->document.GetAllocator());
			break;
		case mas::MALE:
			popobject.AddMember("note",
					"these values represent the mean across all male subpopulations",
					this->document.GetAllocator());
			popobject.AddMember("msy", popinfo.msy_males.msy,
					this->document.GetAllocator());
			popobject.AddMember("spr_F0", popinfo.msy_males.spr_F0,
					this->document.GetAllocator());
			popobject.AddMember("F_msy", popinfo.msy_males.F_msy,
					this->document.GetAllocator());
			popobject.AddMember("spr_msy", popinfo.msy_males.spr_msy,
					this->document.GetAllocator());
			popobject.AddMember("SR_msy", popinfo.msy_males.SR_msy,
					this->document.GetAllocator());
			popobject.AddMember("R_msy", popinfo.msy_males.R_msy,
					this->document.GetAllocator());
			popobject.AddMember("SSB_msy", popinfo.msy_males.SSB_msy,
					this->document.GetAllocator());
			popobject.AddMember("B_msy", popinfo.msy_males.B_msy,
					this->document.GetAllocator());
			popobject.AddMember("E_msy", popinfo.msy_males.E_msy,
					this->document.GetAllocator());
			popobject.AddMember("F30", popinfo.msy_males.F30,
					this->document.GetAllocator());
			popobject.AddMember("spr_F30_msy", popinfo.msy_males.spr_F30_msy,
					this->document.GetAllocator());
			popobject.AddMember("SR_F30_msy", popinfo.msy_males.SR_F30_msy,
					this->document.GetAllocator());
			popobject.AddMember("R_F30_msy", popinfo.msy_males.R_F30_msy,
					this->document.GetAllocator());
			popobject.AddMember("SSB_F30_msy", popinfo.msy_males.SSB_F30_msy,
					this->document.GetAllocator());
			popobject.AddMember("B_F30_msy", popinfo.msy_males.B_F30_msy,
					this->document.GetAllocator());
			popobject.AddMember("E_F30_msy", popinfo.msy_males.E_F30_msy,
					this->document.GetAllocator());
			popobject.AddMember("F35", popinfo.msy_males.F35,
					this->document.GetAllocator());
			popobject.AddMember("spr_F35_msy", popinfo.msy_males.spr_F35_msy,
					this->document.GetAllocator());
			popobject.AddMember("SR_F35_msy", popinfo.msy_males.SR_F35_msy,
					this->document.GetAllocator());
			popobject.AddMember("R_F35_msy", popinfo.msy_males.R_F35_msy,
					this->document.GetAllocator());
			popobject.AddMember("SSB_F35_msy", popinfo.msy_males.SSB_F35_msy,
					this->document.GetAllocator());
			popobject.AddMember("B_F35_msy", popinfo.msy_males.B_F35_msy,
					this->document.GetAllocator());
			popobject.AddMember("E_F35_msy", popinfo.msy_males.E_F35_msy,
					this->document.GetAllocator());
			popobject.AddMember("F40", popinfo.msy_males.F40,
					this->document.GetAllocator());
			popobject.AddMember("spr_F40_msy", popinfo.msy_males.spr_F40_msy,
					this->document.GetAllocator());
			popobject.AddMember("SR_F40_msy", popinfo.msy_males.SR_F40_msy,
					this->document.GetAllocator());
			popobject.AddMember("R_F40_msy", popinfo.msy_males.R_F40_msy,
					this->document.GetAllocator());
			popobject.AddMember("SSB_F40_msy", popinfo.msy_males.SSB_F40_msy,
					this->document.GetAllocator());
			popobject.AddMember("B_F40_msy", popinfo.msy_males.B_F40_msy,
					this->document.GetAllocator());
			popobject.AddMember("E_F40_msy", popinfo.msy_males.E_F40_msy,
					this->document.GetAllocator());
			break;

		}
	}

	void GenerateReferencePoints(rapidjson::Value &popobject,
			const mas::Subpopulation<REAL_T> &popinfo) {
		popobject.AddMember("msy", popinfo.msy.msy,
					this->document.GetAllocator());
		popobject.AddMember("spr_F0", popinfo.msy.spr_F0,
				this->document.GetAllocator());
		popobject.AddMember("F_msy", popinfo.msy.F_msy,
				this->document.GetAllocator());
		popobject.AddMember("spr_msy", popinfo.msy.spr_msy,
				this->document.GetAllocator());
		popobject.AddMember("SR_msy", popinfo.msy.SR_msy,
				this->document.GetAllocator());
		popobject.AddMember("R_msy", popinfo.msy.R_msy,
				this->document.GetAllocator());
		popobject.AddMember("SSB_msy", popinfo.msy.SSB_msy,
				this->document.GetAllocator());
		popobject.AddMember("B_msy", popinfo.msy.B_msy,
				this->document.GetAllocator());
		popobject.AddMember("E_msy", popinfo.msy.E_msy,
				this->document.GetAllocator());
		popobject.AddMember("F30", popinfo.msy.F30,
				this->document.GetAllocator());
		popobject.AddMember("spr_F30_msy", popinfo.msy.spr_F30_msy,
				this->document.GetAllocator());
		popobject.AddMember("SR_F30_msy", popinfo.msy.SR_F30_msy,
				this->document.GetAllocator());
		popobject.AddMember("R_F30_msy", popinfo.msy.R_F30_msy,
				this->document.GetAllocator());
		popobject.AddMember("SSB_F30_msy", popinfo.msy.SSB_F30_msy,
				this->document.GetAllocator());
		popobject.AddMember("B_F30_msy", popinfo.msy.B_F30_msy,
				this->document.GetAllocator());
		popobject.AddMember("E_F30_msy", popinfo.msy.E_F30_msy,
				this->document.GetAllocator());
		popobject.AddMember("F35", popinfo.msy.F35,
				this->document.GetAllocator());
		popobject.AddMember("spr_F35_msy", popinfo.msy.spr_F35_msy,
				this->document.GetAllocator());
		popobject.AddMember("SR_F35_msy", popinfo.msy.SR_F35_msy,
				this->document.GetAllocator());
		popobject.AddMember("R_F35_msy", popinfo.msy.R_F35_msy,
				this->document.GetAllocator());
		popobject.AddMember("SSB_F35_msy", popinfo.msy.SSB_F35_msy,
				this->document.GetAllocator());
		popobject.AddMember("B_F35_msy", popinfo.msy.B_F35_msy,
				this->document.GetAllocator());
		popobject.AddMember("E_F35_msy", popinfo.msy.E_F35_msy,
				this->document.GetAllocator());
		popobject.AddMember("F40", popinfo.msy.F40,
				this->document.GetAllocator());
		popobject.AddMember("spr_F40_msy", popinfo.msy.spr_F40_msy,
				this->document.GetAllocator());
		popobject.AddMember("SR_F40_msy", popinfo.msy.SR_F40_msy,
				this->document.GetAllocator());
		popobject.AddMember("R_F40_msy", popinfo.msy.R_F40_msy,
				this->document.GetAllocator());
		popobject.AddMember("SSB_F40_msy", popinfo.msy.SSB_F40_msy,
				this->document.GetAllocator());
		popobject.AddMember("B_F40_msy", popinfo.msy.B_F40_msy,
				this->document.GetAllocator());
		popobject.AddMember("E_F40_msy", popinfo.msy.E_F40_msy,
				this->document.GetAllocator());

	}

	void GenerateReferencePoints(rapidjson::Value &popobject,
			const mas::Area<REAL_T> &popinfo) {
		popobject.AddMember("msy", popinfo.msy.msy,
					this->document.GetAllocator());
		popobject.AddMember("spr_F0", popinfo.msy.spr_F0,
				this->document.GetAllocator());
		popobject.AddMember("F_msy", popinfo.msy.F_msy,
				this->document.GetAllocator());
		popobject.AddMember("spr_msy", popinfo.msy.spr_msy,
				this->document.GetAllocator());
		popobject.AddMember("SR_msy", popinfo.msy.SR_msy,
				this->document.GetAllocator());
		popobject.AddMember("R_msy", popinfo.msy.R_msy,
				this->document.GetAllocator());
		popobject.AddMember("SSB_msy", popinfo.msy.SSB_msy,
				this->document.GetAllocator());
		popobject.AddMember("B_msy", popinfo.msy.B_msy,
				this->document.GetAllocator());
		popobject.AddMember("E_msy", popinfo.msy.E_msy,
				this->document.GetAllocator());
		popobject.AddMember("F30", popinfo.msy.F30,
				this->document.GetAllocator());
		popobject.AddMember("spr_F30_msy", popinfo.msy.spr_F30_msy,
				this->document.GetAllocator());
		popobject.AddMember("SR_F30_msy", popinfo.msy.SR_F30_msy,
				this->document.GetAllocator());
		popobject.AddMember("R_F30_msy", popinfo.msy.R_F30_msy,
				this->document.GetAllocator());
		popobject.AddMember("SSB_F30_msy", popinfo.msy.SSB_F30_msy,
				this->document.GetAllocator());
		popobject.AddMember("B_F30_msy", popinfo.msy.B_F30_msy,
				this->document.GetAllocator());
		popobject.AddMember("E_F30_msy", popinfo.msy.E_F30_msy,
				this->document.GetAllocator());
		popobject.AddMember("F35", popinfo.msy.F35,
				this->document.GetAllocator());
		popobject.AddMember("spr_F35_msy", popinfo.msy.spr_F35_msy,
				this->document.GetAllocator());
		popobject.AddMember("SR_F35_msy", popinfo.msy.SR_F35_msy,
				this->document.GetAllocator());
		popobject.AddMember("R_F35_msy", popinfo.msy.R_F35_msy,
				this->document.GetAllocator());
		popobject.AddMember("SSB_F35_msy", popinfo.msy.SSB_F35_msy,
				this->document.GetAllocator());
		popobject.AddMember("B_F35_msy", popinfo.msy.B_F35_msy,
				this->document.GetAllocator());
		popobject.AddMember("E_F35_msy", popinfo.msy.E_F35_msy,
				this->document.GetAllocator());
		popobject.AddMember("F40", popinfo.msy.F40,
				this->document.GetAllocator());
		popobject.AddMember("spr_F40_msy", popinfo.msy.spr_F40_msy,
				this->document.GetAllocator());
		popobject.AddMember("SR_F40_msy", popinfo.msy.SR_F40_msy,
				this->document.GetAllocator());
		popobject.AddMember("R_F40_msy", popinfo.msy.R_F40_msy,
				this->document.GetAllocator());
		popobject.AddMember("SSB_F40_msy", popinfo.msy.SSB_F40_msy,
				this->document.GetAllocator());
		popobject.AddMember("B_F40_msy", popinfo.msy.B_F40_msy,
				this->document.GetAllocator());
		popobject.AddMember("E_F40_msy", popinfo.msy.E_F40_msy,
				this->document.GetAllocator());

	}

	void GenerateAreaSurveyBiomass(rapidjson::Value &popobject,
			const mas::Area<REAL_T> &popinfo, const mas::FishSexType &sex) {
		rapidjson::Value obj(rapidjson::kArrayType);
		switch (sex) {

		case mas::UNDIFFERENTIATED:
			this->GenerateArrayObject(obj, popinfo.survey_biomass_at_age, 3,
					popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		case mas::MALE:
			this->GenerateArrayObject(obj, popinfo.survey_biomass_at_age_males,
					3, popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		case mas::FEMALE:
			this->GenerateArrayObject(obj,
					popinfo.survey_biomass_at_age_females, 3, popinfo.years,
					popinfo.seasons, popinfo.ages);
			break;
		}
		popobject.AddMember("values", obj, this->document.GetAllocator());

	}

	void GeneratePopulationAbundance(rapidjson::Value &popobject,
			const mas::Subpopulation<REAL_T> &popinfo) {
		rapidjson::Value obj(rapidjson::kArrayType);
		this->GenerateArrayObject(obj, popinfo.abundance, 2, popinfo.years,
				popinfo.seasons, popinfo.ages.size());
		popobject.AddMember("values", obj, this->document.GetAllocator());
	}

	void GeneratePopulationAbundance(rapidjson::Value &popobject,
			const mas::Population<REAL_T> &popinfo,
			const mas::FishSexType &sex) {
		rapidjson::Value obj(rapidjson::kArrayType);
		switch (sex) {

		case mas::UNDIFFERENTIATED:
			this->GenerateArrayObject(obj, popinfo.abundance, 2, popinfo.years,
					popinfo.seasons, popinfo.ages);
			break;
		case mas::MALE:
			this->GenerateArrayObject(obj, popinfo.abundance_males, 2,
					popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		case mas::FEMALE:
			this->GenerateArrayObject(obj, popinfo.abundance_females, 2,
					popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		}
		popobject.AddMember("values", obj, this->document.GetAllocator());
	}

	void GenerateAreaAbundance(rapidjson::Value &popobject,
			const mas::Area<REAL_T> &popinfo, const mas::FishSexType &sex) {
		rapidjson::Value obj(rapidjson::kArrayType);
		switch (sex) {

		case mas::UNDIFFERENTIATED:
			this->GenerateArrayObject(obj, popinfo.abundance, 2, popinfo.years,
					popinfo.seasons, popinfo.ages);
			break;
		case mas::MALE:
			this->GenerateArrayObject(obj, popinfo.abundance_males, 2,
					popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		case mas::FEMALE:
			this->GenerateArrayObject(obj, popinfo.abundance_females, 2,
					popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		}
		popobject.AddMember("values", obj, this->document.GetAllocator());
	}

	void GeneratePopulationRecruitment(rapidjson::Value &popobject,
			const mas::Subpopulation<REAL_T> &popinfo) {
		rapidjson::Value obj(rapidjson::kArrayType);
		this->GenerateArrayObject(obj, popinfo.recruitment, 2, popinfo.years,
				popinfo.seasons, popinfo.ages.size());
		popobject.AddMember("values", obj, this->document.GetAllocator());
	}

	void GeneratePopulationRecruitment(rapidjson::Value &popobject,
			const mas::Population<REAL_T> &popinfo,
			const mas::FishSexType &sex) {
		rapidjson::Value obj(rapidjson::kArrayType);
		switch (sex) {

		case mas::UNDIFFERENTIATED:
			this->GenerateArrayObject(obj, popinfo.recruits, 2, popinfo.years,
					popinfo.seasons, popinfo.ages);
			break;
		case mas::MALE:
			this->GenerateArrayObject(obj, popinfo.recruits_males, 2,
					popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		case mas::FEMALE:
			this->GenerateArrayObject(obj, popinfo.recruits_females, 2,
					popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		}
		popobject.AddMember("values", obj, this->document.GetAllocator());
	}

	void GenerateAreaRecruitment(rapidjson::Value &popobject,
			const mas::Area<REAL_T> &popinfo, const mas::FishSexType &sex) {
		rapidjson::Value obj(rapidjson::kArrayType);
		switch (sex) {

		case mas::UNDIFFERENTIATED:
			this->GenerateArrayObject(obj, popinfo.recruits, 2, popinfo.years,
					popinfo.seasons, popinfo.ages);
			break;
		case mas::MALE:
			this->GenerateArrayObject(obj, popinfo.recruits_males, 2,
					popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		case mas::FEMALE:
			this->GenerateArrayObject(obj, popinfo.recruits_females, 2,
					popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		}
		popobject.AddMember("values", obj, this->document.GetAllocator());
	}

	void GeneratePopulationBiomassTotal(rapidjson::Value &popobject,
			const mas::Subpopulation<REAL_T> &popinfo) {
		rapidjson::Value obj(rapidjson::kArrayType);
		this->GenerateArrayObject(obj, popinfo.biomass_total, 2, popinfo.years,
				popinfo.seasons, popinfo.ages.size());
		popobject.AddMember("values", obj, this->document.GetAllocator());
	}

	void GeneratePopulationBiomassTotal(rapidjson::Value &popobject,
			const mas::Population<REAL_T> &popinfo,
			const mas::FishSexType &sex) {
		rapidjson::Value obj(rapidjson::kArrayType);
		switch (sex) {

		case mas::UNDIFFERENTIATED:
			this->GenerateArrayObject(obj, popinfo.biomass_total, 2,
					popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		case mas::MALE:
			this->GenerateArrayObject(obj, popinfo.biomass_total_males, 2,
					popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		case mas::FEMALE:
			this->GenerateArrayObject(obj, popinfo.biomass_total_females, 2,
					popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		}
		popobject.AddMember("values", obj, this->document.GetAllocator());
	}

	void GenerateAreaBiomassTotal(rapidjson::Value &popobject,
			const mas::Area<REAL_T> &popinfo, const mas::FishSexType &sex) {
		rapidjson::Value obj(rapidjson::kArrayType);
		switch (sex) {

		case mas::UNDIFFERENTIATED:
			this->GenerateArrayObject(obj, popinfo.biomass, 2, popinfo.years,
					popinfo.seasons, popinfo.ages);
			break;
		case mas::MALE:
			this->GenerateArrayObject(obj, popinfo.biomass_males, 2,
					popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		case mas::FEMALE:
			this->GenerateArrayObject(obj, popinfo.biomass_females, 2,
					popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		}
		popobject.AddMember("values", obj, this->document.GetAllocator());
	}

	void GeneratePopulationSpawningStockBiomass(rapidjson::Value &popobject,
			const mas::Subpopulation<REAL_T> &popinfo) {
		rapidjson::Value obj(rapidjson::kArrayType);
                rapidjson::Value obj2(rapidjson::kArrayType);

		this->GenerateArrayObject(obj, popinfo.spawning_stock_biomass, 2,
				popinfo.years, popinfo.seasons, popinfo.ages.size());
                this->GenerateArrayObject(obj2, popinfo.spawning_stock_biomass_variance, 2,
				popinfo.years, popinfo.seasons, popinfo.ages.size());
		popobject.AddMember("values", obj, this->document.GetAllocator());
                popobject.AddMember("variance", obj2, this->document.GetAllocator());
	}

	void GeneratePopulationSpawningStockBiomass(rapidjson::Value &popobject,
			const mas::Population<REAL_T> &popinfo,
			const mas::FishSexType &sex) {
		rapidjson::Value obj(rapidjson::kArrayType);
		switch (sex) {

		case mas::UNDIFFERENTIATED:
			this->GenerateArrayObject(obj, popinfo.spawning_stock_biomass, 2,
					popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		case mas::MALE:
			this->GenerateArrayObject(obj, popinfo.spawning_stock_biomass_males,
					2, popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		case mas::FEMALE:
			this->GenerateArrayObject(obj,
					popinfo.spawning_stock_biomass_females, 2, popinfo.years,
					popinfo.seasons, popinfo.ages);
			break;
		}
		popobject.AddMember("values", obj, this->document.GetAllocator());
	}

	void GenerateAreaSpawningStockBiomass(rapidjson::Value &popobject,
			const mas::Area<REAL_T> &popinfo, const mas::FishSexType &sex) {
		rapidjson::Value obj(rapidjson::kArrayType);
		switch (sex) {

		case mas::UNDIFFERENTIATED:
			this->GenerateArrayObject(obj, popinfo.spawning_stock_biomass, 2,
					popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		case mas::MALE:
			this->GenerateArrayObject(obj, popinfo.spawning_stock_biomass_males,
					2, popinfo.years, popinfo.seasons, popinfo.ages);
			break;
		case mas::FEMALE:
			this->GenerateArrayObject(obj,
					popinfo.spawning_stock_biomass_females, 2, popinfo.years,
					popinfo.seasons, popinfo.ages);
			break;
		}
		popobject.AddMember("values", obj, this->document.GetAllocator());
	}

	void GenerateFleetOutput(rapidjson::Value &popobject,
			const atl::intrusive_ptr<mas::Fleet<REAL_T> > &fleet) {

		rapidjson::Value females(rapidjson::kObjectType);
		rapidjson::Value males(rapidjson::kObjectType);
		rapidjson::Value undiff(rapidjson::kObjectType);

		rapidjson::Value expected_catch_biomassfemales(rapidjson::kObjectType);
		rapidjson::Value expected_catch_biomass_females_t(
				rapidjson::kObjectType);
		rapidjson::Value expected_catch_biomass_males(rapidjson::kObjectType);
		rapidjson::Value expected_catch_biomass_males_t(rapidjson::kObjectType);
		rapidjson::Value expected_catch_biomas_totals(rapidjson::kObjectType);
		rapidjson::Value expected_catch_biomas_totals_t(rapidjson::kObjectType);

		rapidjson::Value expected_catch_abundance_males(rapidjson::kObjectType);
		rapidjson::Value expected_catch_abundance(rapidjson::kObjectType);
		rapidjson::Value expected_catch_abundance_females(
				rapidjson::kObjectType);

		rapidjson::Value expected_catch_numbers_females(rapidjson::kObjectType);
		rapidjson::Value expected_catch_numbers_males(rapidjson::kObjectType);
		rapidjson::Value expected_catch_numbers_total(rapidjson::kObjectType);

		rapidjson::Value expected_catch_proportions_females(
				rapidjson::kObjectType);
		rapidjson::Value expected_catch_proportions_males(
				rapidjson::kObjectType);
		rapidjson::Value expected_catch_proportions_total(
				rapidjson::kObjectType);

		rapidjson::Value observed(rapidjson::kObjectType);
		rapidjson::Value array(rapidjson::kArrayType);
		this->GenerateArrayObject(array, fleet->catch_at_age_females, 3,
				fleet->years, fleet->seasons, fleet->ages);
		expected_catch_numbers_females.AddMember("units", "1000 fish",
				this->document.GetAllocator());
		expected_catch_numbers_females.AddMember("values", array,
				this->document.GetAllocator());
		females.AddMember("catch_at_age", expected_catch_numbers_females,
				this->document.GetAllocator());

		rapidjson::Value array2(rapidjson::kArrayType);
		this->GenerateArrayObject(array2,
				fleet->catch_proportion_at_age_females, 3, fleet->years,
				fleet->seasons, fleet->ages);
		expected_catch_proportions_females.AddMember("values", array2,
				this->document.GetAllocator());
		females.AddMember("catch_proportion_at_age",
				expected_catch_proportions_females,
				this->document.GetAllocator());

		rapidjson::Value array3(rapidjson::kArrayType);
		this->GenerateArrayObject(array3, fleet->catch_biomass_at_age_females,
				3, fleet->years, fleet->seasons, fleet->ages);
		expected_catch_biomassfemales.AddMember("units", "mt",
				this->document.GetAllocator());
		expected_catch_biomassfemales.AddMember("values", array3,
				this->document.GetAllocator());
		females.AddMember("catch_biomass_at_age", expected_catch_biomassfemales,
				this->document.GetAllocator());

		rapidjson::Value array8(rapidjson::kArrayType);
		this->GenerateArrayObject(array8, fleet->catch_biomass_total_females, 2,
				fleet->years, fleet->seasons, fleet->ages);
		expected_catch_biomass_females_t.AddMember("units", "mt",
				this->document.GetAllocator());
		expected_catch_biomass_females_t.AddMember("values", array8,
				this->document.GetAllocator());
		females.AddMember("catch_biomass", expected_catch_biomass_females_t,
				this->document.GetAllocator());

		rapidjson::Value array9(rapidjson::kArrayType);
		this->GenerateArrayObject(array9, fleet->catch_abundance_females, 2,
				fleet->years, fleet->seasons, fleet->ages);
		expected_catch_abundance_females.AddMember("units", "1000 fish",
				this->document.GetAllocator());
		expected_catch_abundance_females.AddMember("values", array9,
				this->document.GetAllocator());
		females.AddMember("catch_abundance", expected_catch_abundance_females,
				this->document.GetAllocator());

		popobject.AddMember("females", females, this->document.GetAllocator());

		rapidjson::Value array4(rapidjson::kArrayType);
		this->GenerateArrayObject(array4, fleet->catch_at_age_males, 3,
				fleet->years, fleet->seasons, fleet->ages);
		expected_catch_numbers_males.AddMember("units", "1000 fish",
				this->document.GetAllocator());
		expected_catch_numbers_males.AddMember("values", array4,
				this->document.GetAllocator());
		males.AddMember("catch_at_age", expected_catch_numbers_males,
				this->document.GetAllocator());

		rapidjson::Value array5(rapidjson::kArrayType);
		this->GenerateArrayObject(array5, fleet->catch_proportion_at_age_males,
				3, fleet->years, fleet->seasons, fleet->ages);
		expected_catch_proportions_males.AddMember("values", array5,
				this->document.GetAllocator());
		males.AddMember("catch_proportion_at_age",
				expected_catch_proportions_males,
				this->document.GetAllocator());

		rapidjson::Value array6(rapidjson::kArrayType);
		this->GenerateArrayObject(array6, fleet->catch_biomass_at_age_males, 3,
				fleet->years, fleet->seasons, fleet->ages);
		expected_catch_biomass_males.AddMember("units", "mt",
				this->document.GetAllocator());
		expected_catch_biomass_males.AddMember("values", array6,
				this->document.GetAllocator());
		males.AddMember("catch_biomass_at_age", expected_catch_biomass_males,
				this->document.GetAllocator());

		rapidjson::Value array7(rapidjson::kArrayType);
		this->GenerateArrayObject(array7, fleet->catch_biomass_total_males, 2,
				fleet->years, fleet->seasons, fleet->ages);
		expected_catch_biomass_males_t.AddMember("units", "mt",
				this->document.GetAllocator());
		expected_catch_biomass_males_t.AddMember("values", array7,
				this->document.GetAllocator());
		males.AddMember("catch_biomass", expected_catch_biomass_males_t,
				this->document.GetAllocator());
		popobject.AddMember("males", males, this->document.GetAllocator());

		rapidjson::Value array88(rapidjson::kArrayType);
		this->GenerateArrayObject(array88, fleet->catch_abundance_males, 2,
				fleet->years, fleet->seasons, fleet->ages);
		expected_catch_abundance_males.AddMember("units", "1000 fish",
				this->document.GetAllocator());
		expected_catch_abundance_males.AddMember("values", array88,
				this->document.GetAllocator());
		males.AddMember("catch_abundance", expected_catch_abundance_males,
				this->document.GetAllocator());

		rapidjson::Value array99(rapidjson::kArrayType);
		this->GenerateArrayObject(array99, fleet->catch_at_age, 3, fleet->years,
				fleet->seasons, fleet->ages);
		expected_catch_numbers_total.AddMember("units", "1000 fish",
				this->document.GetAllocator());
		expected_catch_numbers_total.AddMember("values", array99,
				this->document.GetAllocator());
		undiff.AddMember("catch_at_age", expected_catch_numbers_total,
				this->document.GetAllocator());

		rapidjson::Value array10(rapidjson::kArrayType);
		this->GenerateArrayObject(array10, fleet->catch_proportion_at_age, 3,
				fleet->years, fleet->seasons, fleet->ages);
		expected_catch_proportions_total.AddMember("values", array10,
				this->document.GetAllocator());
		undiff.AddMember("catch_proportion_at_age",
				expected_catch_proportions_total,
				this->document.GetAllocator());

		rapidjson::Value array11(rapidjson::kArrayType);
		this->GenerateArrayObject(array11, fleet->catch_biomass_at_age, 3,
				fleet->years, fleet->seasons, fleet->ages);
		expected_catch_biomas_totals.AddMember("units", "mt",
				this->document.GetAllocator());
		expected_catch_biomas_totals.AddMember("values", array11,
				this->document.GetAllocator());
		undiff.AddMember("catch_biomass_at_age", expected_catch_biomas_totals,
				this->document.GetAllocator());

		rapidjson::Value array12(rapidjson::kArrayType);
		this->GenerateArrayObject(array12, fleet->catch_biomass_total, 2,
				fleet->years, fleet->seasons, fleet->ages);
		expected_catch_biomas_totals_t.AddMember("units", "mt",
				this->document.GetAllocator());
		expected_catch_biomas_totals_t.AddMember("values", array12,
				this->document.GetAllocator());
		undiff.AddMember("catch_biomass", expected_catch_biomas_totals_t,
				this->document.GetAllocator());

		rapidjson::Value array13(rapidjson::kArrayType);
		this->GenerateArrayObject(array13, fleet->catch_abundance, 2,
				fleet->years, fleet->seasons, fleet->ages);
		expected_catch_abundance.AddMember("units", "1000 fish",
				this->document.GetAllocator());
		expected_catch_abundance.AddMember("values", array13,
				this->document.GetAllocator());
		undiff.AddMember("catch_abundance", expected_catch_abundance,
				this->document.GetAllocator());

		popobject.AddMember("undifferentiated", undiff,
				this->document.GetAllocator());

		for (size_t i = 0; i < fleet->data_objects.size(); i++) {
			rapidjson::Value obj(rapidjson::kObjectType);
			if (fleet->data_objects[i]->type == mas::CATCH_BIOMASS) {

				if (fleet->data_objects[i]->sex_type == mas::FEMALE) {
					rapidjson::Value array(rapidjson::kArrayType);
					this->GenerateArrayObject(array, fleet->data_objects[i], 2,
							fleet->years, fleet->seasons, fleet->ages);
					obj.AddMember("sex", "females",
							this->document.GetAllocator());
					obj.AddMember("values", array,
							this->document.GetAllocator());
					observed.AddMember("fleet_biomass", obj,
							this->document.GetAllocator());
				} else if (fleet->data_objects[i]->sex_type == mas::MALE) {
					rapidjson::Value array(rapidjson::kArrayType);
					this->GenerateArrayObject(array, fleet->data_objects[i], 2,
							fleet->years, fleet->seasons, fleet->ages);
					obj.AddMember("sex", "males",
							this->document.GetAllocator());
					obj.AddMember("values", array,
							this->document.GetAllocator());
					observed.AddMember("fleet_biomass", obj,
							this->document.GetAllocator());

				} else if (fleet->data_objects[i]->sex_type
						== mas::UNDIFFERENTIATED) {
					rapidjson::Value array(rapidjson::kArrayType);
					this->GenerateArrayObject(array, fleet->data_objects[i], 2,
							fleet->years, fleet->seasons, fleet->ages);
					obj.AddMember("sex", "undifferentiated",
							this->document.GetAllocator());
					obj.AddMember("values", array,
							this->document.GetAllocator());
					observed.AddMember("fleet_biomass", obj,
							this->document.GetAllocator());

				}

			} else if (fleet->data_objects[i]->type == mas::CATCH_ABUNDANCE) {
			} else if (fleet->data_objects[i]->type
					== mas::CATCH_PROPORTION_AT_AGE) {

				if (fleet->data_objects[i]->sex_type == mas::FEMALE) {
					rapidjson::Value array(rapidjson::kArrayType);
					this->GenerateArrayObject(array, fleet->data_objects[i], 3,
							fleet->years, fleet->seasons, fleet->ages);
					obj.AddMember("sex", "females",
							this->document.GetAllocator());
					obj.AddMember("values", array,
							this->document.GetAllocator());
					observed.AddMember("fleet_proportion_at_age", obj,
							this->document.GetAllocator());
				} else if (fleet->data_objects[i]->sex_type == mas::MALE) {
					rapidjson::Value array(rapidjson::kArrayType);
					this->GenerateArrayObject(array, fleet->data_objects[i], 3,
							fleet->years, fleet->seasons, fleet->ages);
					obj.AddMember("sex", "males",
							this->document.GetAllocator());
					obj.AddMember("values", array,
							this->document.GetAllocator());
					observed.AddMember("fleet_proportion_at_age", obj,
							this->document.GetAllocator());
				} else if (fleet->data_objects[i]->sex_type
						== mas::UNDIFFERENTIATED) {
					rapidjson::Value array(rapidjson::kArrayType);
					this->GenerateArrayObject(array, fleet->data_objects[i], 3,
							fleet->years, fleet->seasons, fleet->ages);
					obj.AddMember("sex", "undifferentiated",
							this->document.GetAllocator());
					obj.AddMember("values", array,
							this->document.GetAllocator());
					observed.AddMember("fleet_proportion_at_age", obj,
							this->document.GetAllocator());
				}

			} else if (fleet->data_objects[i]->type
					== mas::CATCH_PROPORTION_AT_LENGTH_N) {
			} else if (fleet->data_objects[i]->type
					== mas::CATCH_PROPORTION_AT_LENGTH) {
			} else if (fleet->data_objects[i]->type
					== mas::CATCH_MEAN_SIZE_AT_AGE) {
			} else if (fleet->data_objects[i]->type
					== mas::CATCH_MEAN_WEIGHT_AT_AGE) {
			}

		}
		popobject.AddMember("observed", observed,
				this->document.GetAllocator());

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
		//                } else if (fleet->data_objects[i]->type == mas::CATCH_ABUNDANCE) {
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

	void GenerateSurveyOutput(rapidjson::Value &popobject,
			const atl::intrusive_ptr<mas::Survey<REAL_T> > &survey) {

		rapidjson::Value females(rapidjson::kObjectType);
		rapidjson::Value males(rapidjson::kObjectType);
		rapidjson::Value undiff(rapidjson::kObjectType);

		rapidjson::Value expected_survey_biomassfemales(rapidjson::kObjectType);
		rapidjson::Value expected_survey_biomass_females_t(
				rapidjson::kObjectType);
		rapidjson::Value expected_survey_biomass_males(rapidjson::kObjectType);
		rapidjson::Value expected_survey_biomass_males_t(
				rapidjson::kObjectType);
		rapidjson::Value expected_survey_biomas_totals(rapidjson::kObjectType);
		rapidjson::Value expected_survey_biomas_totals_t(
				rapidjson::kObjectType);

		rapidjson::Value expected_survey_numbers_females(
				rapidjson::kObjectType);
		rapidjson::Value expected_survey_numbers_males(rapidjson::kObjectType);
		rapidjson::Value expected_survey_numbers_total(rapidjson::kObjectType);

		rapidjson::Value expected_survey_proportions_females(
				rapidjson::kObjectType);
		rapidjson::Value expected_survey_proportions_males(
				rapidjson::kObjectType);
		rapidjson::Value expected_survey_proportions_total(
				rapidjson::kObjectType);

		rapidjson::Value observed(rapidjson::kObjectType);

		rapidjson::Value array(rapidjson::kArrayType);
		this->GenerateArrayObject(array, survey->survey_numbers_at_age_females,
				3, survey->years, survey->seasons, survey->ages);
		expected_survey_numbers_females.AddMember("units", "1000 fish",
				this->document.GetAllocator());
		expected_survey_numbers_females.AddMember("values", array,
				this->document.GetAllocator());
		females.AddMember("survey_numbers_at_age",
				expected_survey_numbers_females, this->document.GetAllocator());

		rapidjson::Value array2(rapidjson::kArrayType);
		this->GenerateArrayObject(array2,
				survey->survey_proportion_at_age_females, 3, survey->years,
				survey->seasons, survey->ages);
		expected_survey_proportions_females.AddMember("values", array2,
				this->document.GetAllocator());
		females.AddMember("survey_proportion_at_age",
				expected_survey_proportions_females,
				this->document.GetAllocator());

		rapidjson::Value array3(rapidjson::kArrayType);
		this->GenerateArrayObject(array3, survey->survey_biomass_at_age_females,
				3, survey->years, survey->seasons, survey->ages);
		expected_survey_biomassfemales.AddMember("units", "mt",
				this->document.GetAllocator());
		expected_survey_biomassfemales.AddMember("values", array3,
				this->document.GetAllocator());
		females.AddMember("survey_biomass_at_age",
				expected_survey_biomassfemales, this->document.GetAllocator());

		rapidjson::Value array8(rapidjson::kArrayType);
		this->GenerateArrayObject(array8, survey->survey_biomass_total_females,
				2, survey->years, survey->seasons, survey->ages);
		expected_survey_biomass_females_t.AddMember("units", "mt",
				this->document.GetAllocator());
		expected_survey_biomass_females_t.AddMember("values", array8,
				this->document.GetAllocator());
		females.AddMember("survey_biomass", expected_survey_biomass_females_t,
				this->document.GetAllocator());

		popobject.AddMember("females", females, this->document.GetAllocator());

		rapidjson::Value array4(rapidjson::kArrayType);
		this->GenerateArrayObject(array4, survey->survey_numbers_at_age_males,
				3, survey->years, survey->seasons, survey->ages);
		expected_survey_numbers_males.AddMember("units", "1000 fish",
				this->document.GetAllocator());
		expected_survey_numbers_males.AddMember("values", array4,
				this->document.GetAllocator());
		males.AddMember("survey_numbers_at_age", expected_survey_numbers_males,
				this->document.GetAllocator());

		rapidjson::Value array5(rapidjson::kArrayType);
		this->GenerateArrayObject(array5,
				survey->survey_proportion_at_age_males, 3, survey->years,
				survey->seasons, survey->ages);
		expected_survey_proportions_males.AddMember("values", array5,
				this->document.GetAllocator());
		males.AddMember("survey_proportion_at_age",
				expected_survey_proportions_males,
				this->document.GetAllocator());

		rapidjson::Value array6(rapidjson::kArrayType);
		this->GenerateArrayObject(array6, survey->survey_biomass_at_age_males,
				3, survey->years, survey->seasons, survey->ages);
		expected_survey_biomass_males.AddMember("units", "mt",
				this->document.GetAllocator());
		expected_survey_biomass_males.AddMember("values", array6,
				this->document.GetAllocator());
		males.AddMember("survey_biomass_at_age", expected_survey_biomass_males,
				this->document.GetAllocator());

		rapidjson::Value array7(rapidjson::kArrayType);
		this->GenerateArrayObject(array7, survey->survey_biomass_total_males, 2,
				survey->years, survey->seasons, survey->ages);
		expected_survey_biomass_males_t.AddMember("units", "mt",
				this->document.GetAllocator());
		expected_survey_biomass_males_t.AddMember("values", array7,
				this->document.GetAllocator());
		males.AddMember("survey_biomass", expected_survey_biomass_males_t,
				this->document.GetAllocator());
		popobject.AddMember("males", males, this->document.GetAllocator());

		rapidjson::Value array9(rapidjson::kArrayType);
		this->GenerateArrayObject(array9, survey->survey_numbers_at_age, 3,
				survey->years, survey->seasons, survey->ages);
		expected_survey_numbers_total.AddMember("units", "1000 fish",
				this->document.GetAllocator());
		expected_survey_numbers_total.AddMember("values", array9,
				this->document.GetAllocator());
		undiff.AddMember("survey_numbers_at_age", expected_survey_numbers_total,
				this->document.GetAllocator());

		rapidjson::Value array10(rapidjson::kArrayType);
		this->GenerateArrayObject(array10, survey->survey_proportion_at_age, 3,
				survey->years, survey->seasons, survey->ages);
		expected_survey_proportions_total.AddMember("values", array10,
				this->document.GetAllocator());
		undiff.AddMember("survey_proportion_at_age",
				expected_survey_proportions_total,
				this->document.GetAllocator());

		rapidjson::Value array11(rapidjson::kArrayType);
		this->GenerateArrayObject(array11, survey->survey_biomass_at_age, 3,
				survey->years, survey->seasons, survey->ages);
		expected_survey_biomas_totals.AddMember("units", "mt",
				this->document.GetAllocator());
		expected_survey_biomas_totals.AddMember("values", array11,
				this->document.GetAllocator());
		undiff.AddMember("survey_biomass_at_age", expected_survey_biomas_totals,
				this->document.GetAllocator());

		rapidjson::Value array12(rapidjson::kArrayType);
		this->GenerateArrayObject(array12, survey->survey_biomass_total, 2,
				survey->years, survey->seasons, survey->ages);
		expected_survey_biomas_totals_t.AddMember("units", "mt",
				this->document.GetAllocator());
		expected_survey_biomas_totals_t.AddMember("values", array12,
				this->document.GetAllocator());
		undiff.AddMember("survey_biomass", expected_survey_biomas_totals_t,
				this->document.GetAllocator());
		popobject.AddMember("undifferentiated", undiff,
				this->document.GetAllocator());

		for (size_t i = 0; i < survey->data_objects.size(); i++) {
			rapidjson::Value obj(rapidjson::kObjectType);
			if (survey->data_objects[i]->type == mas::SURVEY_BIOMASS) {

				if (survey->data_objects[i]->sex_type == mas::FEMALE) {
					rapidjson::Value array(rapidjson::kArrayType);
					this->GenerateArrayObject(array, survey->data_objects[i], 2,
							survey->years, survey->seasons, survey->ages);
					obj.AddMember("sex", "females",
							this->document.GetAllocator());
					obj.AddMember("values", array,
							this->document.GetAllocator());
					observed.AddMember("survey_biomass", obj,
							this->document.GetAllocator());
				} else if (survey->data_objects[i]->sex_type == mas::MALE) {
					rapidjson::Value array(rapidjson::kArrayType);
					this->GenerateArrayObject(array, survey->data_objects[i], 2,
							survey->years, survey->seasons, survey->ages);
					obj.AddMember("sex", "males",
							this->document.GetAllocator());
					obj.AddMember("values", array,
							this->document.GetAllocator());
					observed.AddMember("survey_biomass", obj,
							this->document.GetAllocator());

				} else if (survey->data_objects[i]->sex_type
						== mas::UNDIFFERENTIATED) {
					rapidjson::Value array(rapidjson::kArrayType);
					this->GenerateArrayObject(array, survey->data_objects[i], 2,
							survey->years, survey->seasons, survey->ages);
					obj.AddMember("sex", "undifferentiated",
							this->document.GetAllocator());
					obj.AddMember("values", array,
							this->document.GetAllocator());
					observed.AddMember("survey_biomass", obj,
							this->document.GetAllocator());

				}

			} else if (survey->data_objects[i]->type == mas::SURVEY_ABUNDANCE) {
			} else if (survey->data_objects[i]->type
					== mas::SURVEY_PROPORTION_AT_AGE) {

				if (survey->data_objects[i]->sex_type == mas::FEMALE) {
					rapidjson::Value array(rapidjson::kArrayType);
					this->GenerateArrayObject(array, survey->data_objects[i], 3,
							survey->years, survey->seasons, survey->ages);
					obj.AddMember("sex", "females",
							this->document.GetAllocator());
					obj.AddMember("values", array,
							this->document.GetAllocator());
					observed.AddMember("survey_proportion_at_age", obj,
							this->document.GetAllocator());
				} else if (survey->data_objects[i]->sex_type == mas::MALE) {
					rapidjson::Value array(rapidjson::kArrayType);
					this->GenerateArrayObject(array, survey->data_objects[i], 3,
							survey->years, survey->seasons, survey->ages);
					obj.AddMember("sex", "males",
							this->document.GetAllocator());
					obj.AddMember("values", array,
							this->document.GetAllocator());
					observed.AddMember("survey_proportion_at_age", obj,
							this->document.GetAllocator());
				} else if (survey->data_objects[i]->sex_type
						== mas::UNDIFFERENTIATED) {
					rapidjson::Value array(rapidjson::kArrayType);
					this->GenerateArrayObject(array, survey->data_objects[i], 3,
							survey->years, survey->seasons, survey->ages);
					obj.AddMember("sex", "undifferentiated",
							this->document.GetAllocator());
					obj.AddMember("values", array,
							this->document.GetAllocator());
					observed.AddMember("survey_proportion_at_age", obj,
							this->document.GetAllocator());
				}

			} else if (survey->data_objects[i]->type
					== mas::SURVEY_PROPORTION_AT_LENGTH_N) {
			} else if (survey->data_objects[i]->type
					== mas::SURVEY_PROPORTION_AT_LENGTH) {
			} else if (survey->data_objects[i]->type
					== mas::SURVEY_MEAN_SIZE_AT_AGE) {
			} else if (survey->data_objects[i]->type
					== mas::SURVEY_MEAN_WEIGHT_AT_AGE) {
			}

		}

		popobject.AddMember("observed", observed,
				this->document.GetAllocator());

	}

	void GenerateAreaOutput(rapidjson::Value &popobject,
			const atl::intrusive_ptr<mas::Area<REAL_T> > &area) {
	}

	void GenerateConfig(rapidjson::Value &popobject,
			const mas::Information<REAL_T> &info) {

	}

	void GenerateData(rapidjson::Value &popobject,
			const mas::Information<REAL_T> &info) {

	}

public:

	const std::string GenerateOutput(mas::MAS<REAL_T> &mas) {
		mas::Information<REAL_T> *info = &mas.info;
		document.SetObject();
		rapidjson::Document::AllocatorType &allocator = document.GetAllocator();
		//            document.AddMember("nyears", info->nyears, allocator);
		//            document.AddMember("nseasons", info->nseasons, allocator);
		//            document.AddMember("nages", static_cast<int> (info->ages.size()), allocator);

		rapidjson::Value likelihood_components(rapidjson::kArrayType);

		rapidjson::Value metrics(rapidjson::kObjectType);

		rapidjson::Value estimated_parameters(rapidjson::kObjectType);
		rapidjson::Value estimated_parameters_array(rapidjson::kArrayType);
		mas.ComputeGoodnessOfFit();
		metrics.AddMember("chi-squared", mas.chi_squared, allocator);
		metrics.AddMember("g-test", mas.g_test, allocator);
		metrics.AddMember("RMSE", mas.rmse, allocator);
		metrics.AddMember("RMSLE", mas.rmsle, allocator);
		metrics.AddMember("AIC", mas.AIC, allocator);
		metrics.AddMember("BIC", mas.BIC, allocator);

		rapidjson::Value rcomponent(rapidjson::kObjectType);
		rcomponent.AddMember("name", "recruitment_likelihood", allocator);
		rcomponent.AddMember("value", mas.recruitment_likelihood, allocator);
		likelihood_components.PushBack(rcomponent, allocator);

		rapidjson::Value scomponent(rapidjson::kObjectType);
		scomponent.AddMember("name", "selectivity_likelihood", allocator);
		scomponent.AddMember("value", mas.selectivity_likelihood, allocator);
		likelihood_components.PushBack(scomponent, allocator);

		fleet_iterator fit;
		for (fit = mas.info.fleets.begin(); fit != mas.info.fleets.end();
				++fit) {

			mas::Fleet<REAL_T> *f = (mas::Fleet<REAL_T>*) (*fit).second.get();

			for (int i = 0; i < f->nll_component_values.size(); i++) {
				rapidjson::Value component(rapidjson::kObjectType);
				rapidjson::Value name;
				name.SetString(f->nll_component_values[i].GetName().c_str(),
						f->nll_component_values[i].GetName().size(), allocator);

				rapidjson::Value fname;
				fname.SetString(
						f->nll_components[i].nll_functor->ToString().c_str(),
						f->nll_components[i].nll_functor->ToString().size(),
						allocator);

				component.AddMember("name", name, allocator);
				component.AddMember("function", fname, allocator);
				component.AddMember("value",
						f->nll_component_values[i].GetValue(), allocator);
				component.AddMember("chi-squared",
						f->nll_components[i].chi_square, allocator);
				component.AddMember("g-test", f->nll_components[i].g_test,
						allocator);
				component.AddMember("RMSE", f->nll_components[i].rmse,
						allocator);
				component.AddMember("RMSLE", f->nll_components[i].rmsle,
						allocator);
				component.AddMember("r-squared", f->nll_components[i].r_squared,
						allocator);
				component.AddMember("AIC", f->nll_components[i].AIC, allocator);
				component.AddMember("BIC", f->nll_components[i].BIC, allocator);

				rapidjson::Value residuals(rapidjson::kArrayType);
				for (int j = 0; j < f->nll_components[i].residuals.size();
						j++) {
					residuals.PushBack(f->nll_components[i].residuals[j],
							allocator);
				}
				component.AddMember("residuals", residuals, allocator);
				likelihood_components.PushBack(component, allocator);
			}
		}

		survey_iterator sit;
		for (sit = mas.info.survey_models.begin();
				sit != mas.info.survey_models.end(); ++sit) {

			mas::Survey<REAL_T> *f = (mas::Survey<REAL_T>*) (*sit).second.get();

			for (int i = 0; i < f->nll_component_values.size(); i++) {
				rapidjson::Value component(rapidjson::kObjectType);
				rapidjson::Value name;
				name.SetString(f->nll_component_values[i].GetName().c_str(),
						f->nll_component_values[i].GetName().size(), allocator);

				rapidjson::Value fname;
				fname.SetString(
						f->nll_components[i].nll_functor->ToString().c_str(),
						f->nll_components[i].nll_functor->ToString().size(),
						allocator);

				component.AddMember("name", name, allocator);
				component.AddMember("function", fname, allocator);
				component.AddMember("value",
						f->nll_component_values[i].GetValue(), allocator);
				component.AddMember("chi-squared",
						f->nll_components[i].chi_square, allocator);
				component.AddMember("g-test", f->nll_components[i].g_test,
						allocator);
				component.AddMember("RMSE", f->nll_components[i].rmse,
						allocator);
				component.AddMember("RMSLE", f->nll_components[i].rmsle,
						allocator);
				component.AddMember("r-squared", f->nll_components[i].r_squared,
						allocator);
				component.AddMember("AIC", f->nll_components[i].AIC, allocator);
				component.AddMember("BIC", f->nll_components[i].BIC, allocator);

				rapidjson::Value residuals(rapidjson::kArrayType);
				for (int j = 0; j < f->nll_components[i].residuals.size();
						j++) {
					residuals.PushBack(f->nll_components[i].residuals[j],
							allocator);
				}
				component.AddMember("residuals", residuals, allocator);
				likelihood_components.PushBack(component, allocator);
			}
		}

		metrics.AddMember("likelihood_components", likelihood_components,
				allocator);
		document.AddMember("metrics", metrics, allocator);

		for (int i = 0; i < mas.info.estimated_parameters.size(); i++) {
			rapidjson::Value parameter(rapidjson::kObjectType);
			std::string n = mas.info.estimated_parameters[i]->GetName();
			//                rapidjson::GenericStringRef<char> name(n.c_str(), n.size());
			rapidjson::Value name;
			name.SetString(n.c_str(), n.size(), allocator);
			parameter.AddMember("name", name, allocator);
			parameter.AddMember("value",
					mas.info.estimated_parameters[i]->GetValue(), allocator);
			parameter.AddMember("gradient_value",
					atl::Variable<REAL_T>::tape.Value(
							mas.info.estimated_parameters[i]->info->id),
					allocator);
			estimated_parameters_array.PushBack(parameter, allocator);

		}
		estimated_parameters.AddMember("parameters", estimated_parameters_array,
				allocator);

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
			varcovar_values.PushBack(rapidjson::Value(rapidjson::kArrayType),
					allocator);
			for (int j = 0; j < mas.variance_covaiance.GetColumns(); j++) {
				varcovar_values[i].PushBack(mas.variance_covaiance(i, j),
						allocator);
			}
		}
		varcovar.AddMember("values", varcovar_values, allocator);
		estimated_parameters.AddMember("variance_covariance", varcovar,
				allocator);

		document.AddMember("estimated_parameters", estimated_parameters,
				allocator);
		population_iterator pit;

		rapidjson::Value popdyn(rapidjson::kObjectType);

		rapidjson::Value ages_array(rapidjson::kArrayType);
		for (int i = 0; i < mas.info.ages.size(); i++) {
			ages_array.PushBack(mas.info.ages[i], allocator);
		}
		popdyn.AddMember("nyears", mas.info.nyears, allocator);
		popdyn.AddMember("nseasons", mas.info.nseasons, allocator);
		popdyn.AddMember("nareas", (int) mas.info.areas.size(), allocator);
		popdyn.AddMember("nages", (int) mas.info.ages.size(), allocator);
		popdyn.AddMember("nfleets", (int) mas.info.fleets.size(), allocator);
		popdyn.AddMember("nsurveys", (int) mas.info.survey_models.size(),
				allocator);
		popdyn.AddMember("ages", ages_array, allocator);
		rapidjson::Value pops_array(rapidjson::kArrayType);

		for (pit = info->populations.begin(); pit != info->populations.end();
				++pit) {
			rapidjson::Value Popobject(rapidjson::kObjectType);
			Popobject.AddMember("id", (*pit).second->id, allocator);
			rapidjson::Value females(rapidjson::kObjectType);
			rapidjson::Value males(rapidjson::kObjectType);
			rapidjson::Value undiff(rapidjson::kObjectType);

			rapidjson::Value fmsy(rapidjson::kObjectType);
			this->GenerateReferencePoints(fmsy, *(*pit).second, mas::FEMALE);
			females.AddMember("MSY", fmsy, allocator);

			rapidjson::Value frecruits(rapidjson::kObjectType);
			frecruits.AddMember("units", "1000 fish", allocator);
			this->GeneratePopulationRecruitment(frecruits, *(*pit).second,
					mas::FEMALE);
			females.AddMember("recruits", frecruits, allocator);

			rapidjson::Value fssb(rapidjson::kObjectType);
			fssb.AddMember("units", "mt", allocator);
			this->GeneratePopulationSpawningStockBiomass(fssb, *(*pit).second,
					mas::FEMALE);
			females.AddMember("spawning_stock_biomass", fssb, allocator);

			rapidjson::Value fnumbers(rapidjson::kObjectType);
			fnumbers.AddMember("units", "1000 fish", allocator);
			this->GeneratePopulationNumbers(fnumbers, *(*pit).second,
					mas::FEMALE);
			females.AddMember("numbers_at_age", fnumbers, allocator);

			rapidjson::Value fabundance(rapidjson::kObjectType);
			fabundance.AddMember("units", "1000 fish", allocator);
			this->GeneratePopulationAbundance(fabundance, *(*pit).second,
					mas::FEMALE);
			females.AddMember("abundance", fabundance, allocator);

			rapidjson::Value fbiomasst(rapidjson::kObjectType);
			fbiomasst.AddMember("units", "mt", allocator);
			this->GeneratePopulationBiomassTotal(fbiomasst, *(*pit).second,
					mas::FEMALE);
			females.AddMember("biomass", fbiomasst, allocator);

			rapidjson::Value fcnumbers(rapidjson::kObjectType);
			fcnumbers.AddMember("units", "1000 fish", allocator);
			this->GeneratePopulationCatch(fcnumbers, *(*pit).second,
					mas::FEMALE);
			females.AddMember("catch_at_age", fcnumbers, allocator);

			rapidjson::Value fcnumbersb(rapidjson::kObjectType);
			fcnumbersb.AddMember("units", "mt", allocator);
			this->GeneratePopulationCatchBiomass(fcnumbersb, *(*pit).second,
					mas::FEMALE);
			females.AddMember("catch_biomass_at_age", fcnumbersb, allocator);

			rapidjson::Value fsnumbers(rapidjson::kObjectType);
			fsnumbers.AddMember("units", "1000 fish", allocator);
			this->GeneratePopulationSurvey(fsnumbers, *(*pit).second,
					mas::FEMALE);
			females.AddMember("survey_numbers_at_age", fsnumbers, allocator);

			rapidjson::Value fsnumbersb(rapidjson::kObjectType);
			fsnumbersb.AddMember("units", "mt", allocator);
			this->GeneratePopulationSurveyBiomass(fsnumbersb, *(*pit).second,
					mas::FEMALE);
			females.AddMember("survey_biomass_at_age", fsnumbersb, allocator);

			rapidjson::Value ffmort(rapidjson::kObjectType);
			this->GeneratePopulationFishingMortality(ffmort, *(*pit).second,
					mas::FEMALE);
			females.AddMember("fishing_mortality", ffmort, allocator);

			Popobject.AddMember("females", females, allocator);

			rapidjson::Value mmsy(rapidjson::kObjectType);
			this->GenerateReferencePoints(mmsy, *(*pit).second, mas::MALE);
			males.AddMember("MSY", mmsy, allocator);

			rapidjson::Value mrecruits(rapidjson::kObjectType);
			mrecruits.AddMember("units", "1000 fish", allocator);
			this->GeneratePopulationRecruitment(mrecruits, *(*pit).second,
					mas::MALE);
			males.AddMember("recruits", mrecruits, allocator);

			rapidjson::Value mssb(rapidjson::kObjectType);
			mssb.AddMember("units", "mt", allocator);
			this->GeneratePopulationSpawningStockBiomass(mssb, *(*pit).second,
					mas::MALE);
			males.AddMember("spawning_stock_biomass", mssb, allocator);

			rapidjson::Value mnumbers(rapidjson::kObjectType);
			mnumbers.AddMember("units", "1000 fish", allocator);
			this->GeneratePopulationNumbers(mnumbers, *(*pit).second,
					mas::MALE);
			males.AddMember("numbers_at_age", mnumbers, allocator);

			rapidjson::Value mabundance(rapidjson::kObjectType);
			mabundance.AddMember("units", "1000 fish", allocator);
			this->GeneratePopulationAbundance(mabundance, *(*pit).second,
					mas::MALE);
			males.AddMember("abundance", mabundance, allocator);

			rapidjson::Value mbiomasst(rapidjson::kObjectType);
			mbiomasst.AddMember("units", "mt", allocator);
			this->GeneratePopulationBiomassTotal(mbiomasst, *(*pit).second,
					mas::MALE);
			males.AddMember("biomass", mbiomasst, allocator);

			rapidjson::Value mcnumbers(rapidjson::kObjectType);
			mcnumbers.AddMember("units", "1000 fish", allocator);
			this->GeneratePopulationCatch(mcnumbers, *(*pit).second, mas::MALE);
			males.AddMember("catch_at_age", mcnumbers, allocator);

			rapidjson::Value mcnumbersb(rapidjson::kObjectType);
			mcnumbersb.AddMember("units", "mt", allocator);
			this->GeneratePopulationCatchBiomass(mcnumbersb, *(*pit).second,
					mas::MALE);
			males.AddMember("catch_biomass_at_age", mcnumbersb, allocator);

			rapidjson::Value msnumbers(rapidjson::kObjectType);
			msnumbers.AddMember("units", "1000 fish", allocator);
			this->GeneratePopulationSurvey(msnumbers, *(*pit).second,
					mas::MALE);
			males.AddMember("survey_numbers_at_age", msnumbers, allocator);

			rapidjson::Value msnumbersb(rapidjson::kObjectType);
			msnumbersb.AddMember("units", "mt", allocator);
			this->GeneratePopulationSurveyBiomass(msnumbersb, *(*pit).second,
					mas::MALE);
			males.AddMember("survey_biomass_at_age", msnumbersb, allocator);

			rapidjson::Value mfmort(rapidjson::kObjectType);
			this->GeneratePopulationFishingMortality(mfmort, *(*pit).second,
					mas::MALE);
			males.AddMember("fishing_mortality", mfmort, allocator);

			Popobject.AddMember("males", males, allocator);

			rapidjson::Value urecruits(rapidjson::kObjectType);
			urecruits.AddMember("units", "1000 fish", allocator);
			this->GeneratePopulationRecruitment(urecruits, *(*pit).second,
					mas::UNDIFFERENTIATED);
			undiff.AddMember("recruits", urecruits, allocator);

			rapidjson::Value ussb(rapidjson::kObjectType);
			ussb.AddMember("units", "mt", allocator);
			this->GeneratePopulationSpawningStockBiomass(ussb, *(*pit).second,
					mas::UNDIFFERENTIATED);
			undiff.AddMember("spawning_stock_biomass", ussb, allocator);

			rapidjson::Value unumbers(rapidjson::kObjectType);
			unumbers.AddMember("units", "1000 fish", allocator);
			this->GeneratePopulationNumbers(unumbers, *(*pit).second,
					mas::UNDIFFERENTIATED);
			undiff.AddMember("numbers_at_age", unumbers, allocator);

			rapidjson::Value uabundance(rapidjson::kObjectType);
			uabundance.AddMember("units", "1000 fish", allocator);
			this->GeneratePopulationAbundance(uabundance, *(*pit).second,
					mas::UNDIFFERENTIATED);
			undiff.AddMember("abundance", uabundance, allocator);

			rapidjson::Value ubiomasst(rapidjson::kObjectType);
			ubiomasst.AddMember("units", "mt", allocator);
			this->GeneratePopulationBiomassTotal(ubiomasst, *(*pit).second,
					mas::UNDIFFERENTIATED);
			undiff.AddMember("biomass", ubiomasst, allocator);

			rapidjson::Value ucnumbers(rapidjson::kObjectType);
			ucnumbers.AddMember("units", "1000 fish", allocator);
			this->GeneratePopulationCatch(ucnumbers, *(*pit).second,
					mas::UNDIFFERENTIATED);
			undiff.AddMember("catch_at_age", ucnumbers, allocator);

			rapidjson::Value ucnumbersb(rapidjson::kObjectType);
			ucnumbersb.AddMember("units", "mt", allocator);
			this->GeneratePopulationCatchBiomass(ucnumbersb, *(*pit).second,
					mas::UNDIFFERENTIATED);
			undiff.AddMember("catch_biomass_at_age", ucnumbersb, allocator);

			rapidjson::Value usnumbers(rapidjson::kObjectType);
			usnumbers.AddMember("units", "1000 fish", allocator);
			this->GeneratePopulationSurvey(usnumbers, *(*pit).second,
					mas::UNDIFFERENTIATED);
			undiff.AddMember("survey_numbers_at_age", usnumbers, allocator);

			rapidjson::Value usnumbersb(rapidjson::kObjectType);
			usnumbersb.AddMember("units", "mt", allocator);
			this->GeneratePopulationSurveyBiomass(usnumbersb, *(*pit).second,
					mas::UNDIFFERENTIATED);
			undiff.AddMember("survey_biomass_at_age", usnumbersb, allocator);

			rapidjson::Value ufmort(rapidjson::kObjectType);
			this->GeneratePopulationFishingMortality(ufmort, *(*pit).second,
					mas::UNDIFFERENTIATED);
			undiff.AddMember("fishing_mortality", ufmort, allocator);

			Popobject.AddMember("undifferentiated", undiff, allocator);

			rapidjson::Value msy(rapidjson::kObjectType);
			this->GenerateReferencePoints(msy, *(*pit).second,
					mas::UNDIFFERENTIATED);
			Popobject.AddMember("MSY", msy, allocator);

			rapidjson::Value pops_area_array(rapidjson::kArrayType);
			for (int i = 0; i < (*pit).second->areas_list.size(); i++) {

				rapidjson::Value area(rapidjson::kObjectType);
				area.AddMember("id", (*pit).second->areas_list[i]->id,
						allocator);
				area.AddMember("description",
						"contributions to this area by the population",
						allocator);

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

				females.AddMember("initial_f",
						(*pit).second->females[(*pit).second->areas_list[i]->id].initialF,
						allocator);

				rapidjson::Value fnvalues(rapidjson::kArrayType);
				this->GenerateLengthAtSeasonStart(flengthseasonstart,
						(*pit).second->females[(*pit).second->areas_list[i]->id]);
				females.AddMember("length_at_season_start", flengthseasonstart,
						allocator);

				this->GenerateLengthAtSpawning(flengthspawning,
						(*pit).second->females[(*pit).second->areas_list[i]->id]);
				females.AddMember("length_at_spawning", flengthspawning,
						allocator);

				this->GenerateLengthAtCatch(flengthcatch,
						(*pit).second->females[(*pit).second->areas_list[i]->id]);
				females.AddMember("length_at_catch", flengthcatch, allocator);

				this->GenerateLengthAtSurvey(flengthsurvey,
						(*pit).second->females[(*pit).second->areas_list[i]->id]);
				females.AddMember("length_at_survey", flengthsurvey, allocator);

				this->GenerateWeightAtSeasonStart(fweightseasonstart,
						(*pit).second->females[(*pit).second->areas_list[i]->id]);
				females.AddMember("length_at_season_start", fweightseasonstart,
						allocator);

				this->GenerateWeightAtSpawning(fweightspawning,
						(*pit).second->females[(*pit).second->areas_list[i]->id]);
				females.AddMember("length_at_spawning", fweightspawning,
						allocator);

				females.AddMember("unfished_spawning_biomass_equilibrium",
						(*pit).second->females[(*pit).second->areas_list[i]->id].SB0,
						allocator);

				init_numbers_eq.AddMember("units", "1000 fish", allocator);
				this->GeneratePopulationInitialNumbersEquilibrium(
						init_numbers_eq,
						(*pit).second->females[(*pit).second->areas_list[i]->id]);
				females.AddMember("initial_numbers_equilibrium",
						init_numbers_eq, allocator);

				init_numbers.AddMember("units", "1000 fish", allocator);
				this->GeneratePopulationInitialNumbers(init_numbers,
						(*pit).second->females[(*pit).second->areas_list[i]->id]);
				females.AddMember("initial_numbers", init_numbers, allocator);

				rapidjson::Value frecruits(rapidjson::kObjectType);
				frecruits.AddMember("units", "1000 fish", allocator);
				this->GeneratePopulationRecruitment(frecruits,
						(*pit).second->females[(*pit).second->areas_list[i]->id]);
				females.AddMember("recruits", frecruits, allocator);

				rapidjson::Value fabundance(rapidjson::kObjectType);
				fabundance.AddMember("units", "1000 fish", allocator);
				this->GeneratePopulationAbundance(fabundance,
						(*pit).second->females[(*pit).second->areas_list[i]->id]);
				females.AddMember("abundance", fabundance, allocator);

				rapidjson::Value fbiomasst(rapidjson::kObjectType);
				fbiomasst.AddMember("units", "mt", allocator);
				this->GeneratePopulationBiomassTotal(fbiomasst,
						(*pit).second->females[(*pit).second->areas_list[i]->id]);
				females.AddMember("biomass", fbiomasst, allocator);

				rapidjson::Value fssb(rapidjson::kObjectType);
				fssb.AddMember("units", "mt", allocator);
				this->GeneratePopulationSpawningStockBiomass(fssb,
						(*pit).second->females[(*pit).second->areas_list[i]->id]);
				females.AddMember("spawning_stock_biomass", fssb, allocator);

				rapidjson::Value fredistrecruits(rapidjson::kObjectType);
				fredistrecruits.AddMember("units", "1000 fish", allocator);
				this->GeneratePopulationRedistributedRecruits(fredistrecruits,
						(*pit).second->females[(*pit).second->areas_list[i]->id]);
				females.AddMember("redistributed_recruits", fredistrecruits,
						allocator);

				fnumbers.AddMember("units", "1000 fish", allocator);
				this->GeneratePopulationNumbers(fnumbers,
						(*pit).second->females[(*pit).second->areas_list[i]->id]);
				females.AddMember("numbers_at_age", fnumbers, allocator);

				rapidjson::Value fcnumbers(rapidjson::kObjectType);
				fcnumbers.AddMember("units", "1000 fish", allocator);
				this->GeneratePopulationCatch(fcnumbers,
						(*pit).second->females[(*pit).second->areas_list[i]->id]);
				females.AddMember("catch_at_age", fcnumbers, allocator);

				rapidjson::Value fcnumbersb(rapidjson::kObjectType);
				fcnumbersb.AddMember("units", "mt", allocator);
				this->GeneratePopulationCatchBiomass(fcnumbersb,
						(*pit).second->females[(*pit).second->areas_list[i]->id]);
				females.AddMember("catch_biomass_at_age", fcnumbersb,
						allocator);

				rapidjson::Value fsnumbers(rapidjson::kObjectType);
				fsnumbers.AddMember("units", "1000 fish", allocator);
				this->GeneratePopulationSurvey(fsnumbers,
						(*pit).second->females[(*pit).second->areas_list[i]->id]);
				females.AddMember("survey_numbers_at_age", fsnumbers,
						allocator);

				rapidjson::Value fsnumbersb(rapidjson::kObjectType);
				fsnumbersb.AddMember("units", "mt", allocator);
				this->GeneratePopulationSurveyBiomass(fsnumbersb,
						(*pit).second->females[(*pit).second->areas_list[i]->id]);
				females.AddMember("survey_biomass", fsnumbersb, allocator);

				rapidjson::Value ffmort(rapidjson::kObjectType);
				this->GeneratePopulationFishingMortality(ffmort,
						(*pit).second->females[(*pit).second->areas_list[i]->id]);
				females.AddMember("fishing_mortality", ffmort, allocator);

				rapidjson::Value fmsy(rapidjson::kObjectType);
				this->GenerateReferencePoints(fmsy,
						(*pit).second->females[(*pit).second->areas_list[i]->id]);
				females.AddMember("MSY", fmsy, allocator);

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

				males.AddMember("initial_f",
						(*pit).second->males[(*pit).second->areas_list[i]->id].initialF,
						allocator);

				rapidjson::Value mnvalues(rapidjson::kArrayType);
				this->GenerateLengthAtSeasonStart(mlengthseasonstart,
						(*pit).second->males[(*pit).second->areas_list[i]->id]);
				males.AddMember("length_at_season_start", mlengthseasonstart,
						allocator);

				this->GenerateLengthAtSpawning(mlengthspawning,
						(*pit).second->males[(*pit).second->areas_list[i]->id]);
				males.AddMember("length_at_spawning", mlengthspawning,
						allocator);

				this->GenerateLengthAtCatch(mlengthcatch,
						(*pit).second->males[(*pit).second->areas_list[i]->id]);
				males.AddMember("length_at_catch", mlengthcatch, allocator);

				this->GenerateLengthAtSurvey(mlengthsurvey,
						(*pit).second->males[(*pit).second->areas_list[i]->id]);
				males.AddMember("length_at_survey", mlengthsurvey, allocator);

				this->GenerateWeightAtSeasonStart(mweightseasonstart,
						(*pit).second->males[(*pit).second->areas_list[i]->id]);
				males.AddMember("length_at_season_start", mweightseasonstart,
						allocator);

				this->GenerateWeightAtSpawning(mweightspawning,
						(*pit).second->males[(*pit).second->areas_list[i]->id]);
				males.AddMember("length_at_spawning", mweightspawning,
						allocator);

				males.AddMember("unfished_spawning_biomass_equilibrium",
						(*pit).second->males[(*pit).second->areas_list[i]->id].SB0,
						allocator);

				mminit_numbers_eq.AddMember("units", "1000 fish", allocator);
				this->GeneratePopulationInitialNumbersEquilibrium(
						mminit_numbers_eq,
						(*pit).second->males[(*pit).second->areas_list[i]->id]);
				males.AddMember("initial_numbers_equilibrium",
						mminit_numbers_eq, allocator);

				minit_numbers.AddMember("units", "1000 fish", allocator);
				this->GeneratePopulationInitialNumbers(minit_numbers,
						(*pit).second->males[(*pit).second->areas_list[i]->id]);
				males.AddMember("initial_numbers", minit_numbers, allocator);

				rapidjson::Value mrecruits(rapidjson::kObjectType);
				mrecruits.AddMember("units", "1000 fish", allocator);
				this->GeneratePopulationRecruitment(mrecruits,
						(*pit).second->males[(*pit).second->areas_list[i]->id]);
				males.AddMember("recruits", mrecruits, allocator);

				rapidjson::Value mabundance(rapidjson::kObjectType);
				mabundance.AddMember("units", "1000 fish", allocator);
				this->GeneratePopulationAbundance(mabundance,
						(*pit).second->males[(*pit).second->areas_list[i]->id]);
				males.AddMember("abundance", mabundance, allocator);

				rapidjson::Value mbiomasst(rapidjson::kObjectType);
				mbiomasst.AddMember("units", "mt", allocator);
				this->GeneratePopulationBiomassTotal(mbiomasst,
						(*pit).second->males[(*pit).second->areas_list[i]->id]);
				males.AddMember("biomass", mbiomasst, allocator);

				rapidjson::Value mssb(rapidjson::kObjectType);
				mssb.AddMember("units", "mt", allocator);
				this->GeneratePopulationSpawningStockBiomass(mssb,
						(*pit).second->males[(*pit).second->areas_list[i]->id]);
				males.AddMember("spawning_stock_biomass", mssb, allocator);

				rapidjson::Value mredistrecruits(rapidjson::kObjectType);
				mredistrecruits.AddMember("units", "1000 fish", allocator);
				this->GeneratePopulationRedistributedRecruits(mredistrecruits,
						(*pit).second->males[(*pit).second->areas_list[i]->id]);
				males.AddMember("redistributed_recruits", mredistrecruits,
						allocator);

				mnumbers.AddMember("units", "1000 fish", allocator);
				this->GeneratePopulationNumbers(mnumbers,
						(*pit).second->males[(*pit).second->areas_list[i]->id]);
				males.AddMember("numbers_at_age", mnumbers, allocator);

				rapidjson::Value mcnumbers(rapidjson::kObjectType);
				mcnumbers.AddMember("units", "1000 fish", allocator);
				this->GeneratePopulationCatch(mcnumbers,
						(*pit).second->males[(*pit).second->areas_list[i]->id]);
				males.AddMember("catch_at_age", mcnumbers, allocator);

				rapidjson::Value mcnumbersb(rapidjson::kObjectType);
				mcnumbersb.AddMember("units", "mt", allocator);
				this->GeneratePopulationCatchBiomass(mcnumbersb,
						(*pit).second->males[(*pit).second->areas_list[i]->id]);
				males.AddMember("catch_biomass", mcnumbersb, allocator);

				rapidjson::Value msnumbers(rapidjson::kObjectType);
				msnumbers.AddMember("units", "1000 fish", allocator);
				this->GeneratePopulationSurvey(msnumbers,
						(*pit).second->males[(*pit).second->areas_list[i]->id]);
				males.AddMember("survey_numbers_at_age", msnumbers, allocator);

				rapidjson::Value msnumbersb(rapidjson::kObjectType);
				msnumbersb.AddMember("units", "mt", allocator);
				this->GeneratePopulationSurveyBiomass(msnumbersb,
						(*pit).second->males[(*pit).second->areas_list[i]->id]);
				males.AddMember("survey_biomass", msnumbersb, allocator);

				rapidjson::Value mfmort(rapidjson::kObjectType);
				this->GeneratePopulationFishingMortality(mfmort,
						(*pit).second->males[(*pit).second->areas_list[i]->id]);
				males.AddMember("fishing_mortality", mfmort, allocator);

				rapidjson::Value mmsy(rapidjson::kObjectType);
				this->GenerateReferencePoints(mmsy,
						(*pit).second->males[(*pit).second->areas_list[i]->id]);
				males.AddMember("MSY", mmsy, allocator);

				area.AddMember("males", males, allocator);

				pops_area_array.PushBack(area, allocator);

			}
			Popobject.AddMember("areas", pops_area_array, allocator);

			pops_array.PushBack(Popobject, allocator);
			//                popdyn.AddMember("population", Popobject, allocator);

		}
		popdyn.AddMember("populations", pops_array, allocator);

		rapidjson::Value fleets_array(rapidjson::kArrayType);
		//            fleet_iterator fit;
		for (fit = mas.info.fleets.begin(); fit != mas.info.fleets.end();
				++fit) {
			rapidjson::Value fleet(rapidjson::kObjectType);
			fleet.AddMember("id", (*fit).second->id, allocator);
			this->GenerateFleetOutput(fleet, (*fit).second);
			//                popdyn.AddMember("fleet", fleet, allocator);
			fleets_array.PushBack(fleet, allocator);
		}
		popdyn.AddMember("fleets", fleets_array, allocator);

		rapidjson::Value surveys_array(rapidjson::kArrayType);
		//            survey_iterator sit;
		for (sit = mas.info.survey_models.begin();
				sit != mas.info.survey_models.end(); ++sit) {
			rapidjson::Value survey(rapidjson::kObjectType);
			survey.AddMember("id", (*sit).second->id, allocator);
			this->GenerateSurveyOutput(survey, (*sit).second);
			surveys_array.PushBack(survey, allocator);
			//                popdyn.AddMember("survey", survey, allocator);
		}
		popdyn.AddMember("surveys", surveys_array, allocator);

		rapidjson::Value areas_array(rapidjson::kArrayType);
		area_iterator ait;
		for (ait = mas.info.areas.begin(); ait != mas.info.areas.end(); ++ait) {
			rapidjson::Value Popobject(rapidjson::kObjectType);
			Popobject.AddMember("id", (*ait).second->id, allocator);

			rapidjson::Value msy(rapidjson::kObjectType);
			this->GenerateReferencePoints(msy, *(*ait).second);
			Popobject.AddMember("MSY", msy, allocator);

			rapidjson::Value females(rapidjson::kObjectType);
			rapidjson::Value males(rapidjson::kObjectType);
			rapidjson::Value undiff(rapidjson::kObjectType);

			rapidjson::Value frecruits(rapidjson::kObjectType);
			frecruits.AddMember("units", "1000 fish", allocator);
			this->GenerateAreaRecruitment(frecruits, *(*ait).second,
					mas::FEMALE);
			females.AddMember("recruits", frecruits, allocator);

			rapidjson::Value fssb(rapidjson::kObjectType);
			fssb.AddMember("units", "mt", allocator);
			this->GenerateAreaSpawningStockBiomass(fssb, *(*ait).second,
					mas::FEMALE);
			females.AddMember("spawning_stock_biomass", fssb, allocator);

			rapidjson::Value fnumbers(rapidjson::kObjectType);
			fnumbers.AddMember("units", "1000 fish", allocator);
			this->GenerateAreaNumbers(fnumbers, *(*ait).second, mas::FEMALE);
			females.AddMember("numbers_at_age", fnumbers, allocator);

			rapidjson::Value fabundance(rapidjson::kObjectType);
			fabundance.AddMember("units", "1000 fish", allocator);
			this->GenerateAreaAbundance(fabundance, *(*ait).second,
					mas::FEMALE);
			females.AddMember("abundance", fabundance, allocator);

			rapidjson::Value fbiomasst(rapidjson::kObjectType);
			fbiomasst.AddMember("units", "mt", allocator);
			this->GenerateAreaBiomassTotal(fbiomasst, *(*ait).second,
					mas::FEMALE);
			females.AddMember("biomass", fbiomasst, allocator);

			rapidjson::Value fcnumbers(rapidjson::kObjectType);
			fcnumbers.AddMember("units", "1000 fish", allocator);
			this->GenerateAreaCatch(fcnumbers, *(*ait).second, mas::FEMALE);
			females.AddMember("catch_at_age", fcnumbers, allocator);

			rapidjson::Value fcnumbersb(rapidjson::kObjectType);
			fcnumbersb.AddMember("units", "mt", allocator);
			this->GenerateAreaCatchBiomass(fcnumbersb, *(*ait).second,
					mas::FEMALE);
			females.AddMember("catch_biomass_at_age", fcnumbersb, allocator);

			rapidjson::Value fsnumbers(rapidjson::kObjectType);
			fsnumbers.AddMember("units", "1000 fish", allocator);
			this->GenerateAreaSurvey(fsnumbers, *(*ait).second, mas::FEMALE);
			females.AddMember("survey_numbers_at_age", fsnumbers, allocator);

			rapidjson::Value fsnumbersb(rapidjson::kObjectType);
			fsnumbersb.AddMember("units", "mt", allocator);
			this->GenerateAreaSurveyBiomass(fsnumbersb, *(*ait).second,
					mas::FEMALE);
			females.AddMember("survey_biomass_at_age", fsnumbersb, allocator);

			Popobject.AddMember("females", females, allocator);

			rapidjson::Value mrecruits(rapidjson::kObjectType);
			mrecruits.AddMember("units", "1000 fish", allocator);
			this->GenerateAreaRecruitment(mrecruits, *(*ait).second, mas::MALE);
			males.AddMember("recruits", mrecruits, allocator);

			rapidjson::Value mssb(rapidjson::kObjectType);
			mssb.AddMember("units", "mt", allocator);
			this->GenerateAreaSpawningStockBiomass(mssb, *(*ait).second,
					mas::MALE);
			males.AddMember("spawning_stock_biomass", mssb, allocator);

			rapidjson::Value mnumbers(rapidjson::kObjectType);
			mnumbers.AddMember("units", "1000 fish", allocator);
			this->GenerateAreaNumbers(mnumbers, *(*ait).second, mas::MALE);
			males.AddMember("numbers_at_age", mnumbers, allocator);

			rapidjson::Value mabundance(rapidjson::kObjectType);
			mabundance.AddMember("units", "1000 fish", allocator);
			this->GenerateAreaAbundance(mabundance, *(*ait).second, mas::MALE);
			males.AddMember("abundance", mabundance, allocator);

			rapidjson::Value mbiomasst(rapidjson::kObjectType);
			mbiomasst.AddMember("units", "mt", allocator);
			this->GenerateAreaBiomassTotal(mbiomasst, *(*ait).second,
					mas::MALE);
			males.AddMember("biomass", mbiomasst, allocator);

			rapidjson::Value mcnumbers(rapidjson::kObjectType);
			mcnumbers.AddMember("units", "1000 fish", allocator);
			this->GenerateAreaCatch(mcnumbers, *(*ait).second, mas::MALE);
			males.AddMember("catch_at_age", mcnumbers, allocator);

			rapidjson::Value mcnumbersb(rapidjson::kObjectType);
			mcnumbersb.AddMember("units", "mt", allocator);
			this->GenerateAreaCatchBiomass(mcnumbersb, *(*ait).second,
					mas::MALE);
			males.AddMember("catch_biomass_at_age", mcnumbersb, allocator);

			rapidjson::Value msnumbers(rapidjson::kObjectType);
			msnumbers.AddMember("units", "1000 fish", allocator);
			this->GenerateAreaSurvey(msnumbers, *(*ait).second, mas::MALE);
			males.AddMember("survey_numbers_at_age", msnumbers, allocator);

			rapidjson::Value msnumbersb(rapidjson::kObjectType);
			msnumbersb.AddMember("units", "mt", allocator);
			this->GenerateAreaSurveyBiomass(msnumbersb, *(*ait).second,
					mas::MALE);
			males.AddMember("survey_biomass_at_age", msnumbersb, allocator);

			Popobject.AddMember("males", males, allocator);

			rapidjson::Value urecruits(rapidjson::kObjectType);
			urecruits.AddMember("units", "1000 fish", allocator);
			this->GenerateAreaRecruitment(urecruits, *(*ait).second,
					mas::UNDIFFERENTIATED);
			undiff.AddMember("recruits", urecruits, allocator);

			rapidjson::Value ussb(rapidjson::kObjectType);
			ussb.AddMember("units", "mt", allocator);
			this->GenerateAreaSpawningStockBiomass(ussb, *(*ait).second,
					mas::UNDIFFERENTIATED);
			undiff.AddMember("spawning_stock_biomass", ussb, allocator);

			rapidjson::Value unumbers(rapidjson::kObjectType);
			unumbers.AddMember("units", "1000 fish", allocator);
			this->GenerateAreaNumbers(unumbers, *(*ait).second,
					mas::UNDIFFERENTIATED);
			undiff.AddMember("numbers_at_age", unumbers, allocator);

			rapidjson::Value uabundance(rapidjson::kObjectType);
			uabundance.AddMember("units", "1000 fish", allocator);
			this->GenerateAreaAbundance(uabundance, *(*ait).second,
					mas::UNDIFFERENTIATED);
			undiff.AddMember("abundance", uabundance, allocator);

			rapidjson::Value ubiomasst(rapidjson::kObjectType);
			ubiomasst.AddMember("units", "mt", allocator);
			this->GenerateAreaBiomassTotal(ubiomasst, *(*ait).second,
					mas::UNDIFFERENTIATED);
			undiff.AddMember("biomass", ubiomasst, allocator);

			rapidjson::Value ucnumbers(rapidjson::kObjectType);
			ucnumbers.AddMember("units", "1000 fish", allocator);
			this->GenerateAreaCatch(ucnumbers, *(*ait).second,
					mas::UNDIFFERENTIATED);
			undiff.AddMember("catch_at_age", ucnumbers, allocator);

			rapidjson::Value ucnumbersb(rapidjson::kObjectType);
			ucnumbersb.AddMember("units", "mt", allocator);
			this->GenerateAreaCatchBiomass(ucnumbersb, *(*ait).second,
					mas::UNDIFFERENTIATED);
			undiff.AddMember("catch_biomass_at_age", ucnumbersb, allocator);

			rapidjson::Value usnumbers(rapidjson::kObjectType);
			usnumbers.AddMember("units", "1000 fish", allocator);
			this->GenerateAreaSurvey(usnumbers, *(*ait).second,
					mas::UNDIFFERENTIATED);
			undiff.AddMember("survey_numbers_at_age", usnumbers, allocator);

			rapidjson::Value usnumbersb(rapidjson::kObjectType);
			usnumbersb.AddMember("units", "mt", allocator);
			this->GenerateAreaSurveyBiomass(usnumbersb, *(*ait).second,
					mas::UNDIFFERENTIATED);
			undiff.AddMember("survey_biomass_at_age", usnumbersb, allocator);

			Popobject.AddMember("undifferentiated", undiff, allocator);
			areas_array.PushBack(Popobject, allocator);
		}
		popdyn.AddMember("areas", areas_array, allocator);

		document.AddMember("population_dynamics", popdyn, allocator);
		rapidjson::StringBuffer buffer;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);

		return buffer.GetString();

	}

};

}

#endif /* OUTPUT_HPP */

