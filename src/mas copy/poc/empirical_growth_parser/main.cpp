#include <cstdlib>
#include "../../third_party/rapidjson/document.h"
#include "../../Information.hpp"
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

#define INFO_DEBUG

void HandleGrowthModel(rapidjson::Document::MemberIterator & growth_model) {
    typedef double REAL_T;
    mas::GrowthBase<REAL_T>::ages = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    for (int i = 0; i < mas::GrowthBase<REAL_T>::ages.size(); i++) {
        mas::GrowthBase<REAL_T>::ages_to_intrpolate.insert(mas::GrowthBase<REAL_T>::ages[i]);
        mas::GrowthBase<REAL_T>::ages_to_intrpolate.insert(mas::GrowthBase<REAL_T>::ages[i] + .345);
    }


    int years = 0;
    int seasons = 0;
    int ages = 0;

    /*
            int years = this->nyears;
            int seasons = this->nseasons;
            int ages = this->ages.size();
     */
    std::shared_ptr<mas::WeightFunctorBase<REAL_T> > weight_functor(NULL);
    rapidjson::Document::MemberIterator ewaa_it = (*growth_model).value.FindMember("empirical_weight_at_age");

    if (ewaa_it != (*growth_model).value.MemberEnd()) {
        std::vector<int> check = {0, 0, 0, 0};
        
        weight_functor = std::make_shared<mas::EmpiricalWeightFunctor<REAL_T> >();
        mas::EmpiricalWeightFunctor<REAL_T>* eg = (mas::EmpiricalWeightFunctor<REAL_T>*)weight_functor.get();

        for (ewaa_it = (*growth_model).value.MemberBegin(); ewaa_it != (*growth_model).value.MemberEnd(); ++ewaa_it) {

            if ((*ewaa_it).value.IsArray()) {
                rapidjson::Value& v = (*ewaa_it).value;
                for (int ii = 0; ii < v.Size(); ii++) {
                    std::shared_ptr<mas::DataObject<REAL_T> > data_object(new mas::DataObject<REAL_T>());
                    rapidjson::Document::MemberIterator t_it = v[ii].FindMember("data_object_type");
                    mas::DataObjectType type = mas::DataObject<double>::GetType((*t_it).value.GetString());
                    t_it = v[ii].FindMember("sex");
                    mas::FishSexType stype = mas::DataObject<double>::GetSex((*t_it).value.GetString());
                    data_object->type = type;
                    data_object->sex_type = stype;
                    int i, j, k;
                    switch (type) {

                        case mas::CATCH_MEAN_WEIGHT_AT_AGE:
                            INFO_DEBUG
                            data_object->imax = years;
                            data_object->jmax = seasons;
                            data_object->kmax = ages;
                            data_object->dimensions = 3;

                            break;


                        case mas::SURVEY_MEAN_WEIGHT_AT_AGE:
                            INFO_DEBUG
                            data_object->imax = years;
                            data_object->jmax = seasons;
                            data_object->kmax = ages;
                            data_object->dimensions = 3;

                            break;

                        case mas::MEAN_WEIGHT_AT_AGE_SEASON_START:
                            INFO_DEBUG
                            data_object->imax = years;
                            data_object->jmax = seasons;
                            data_object->kmax = ages;
                            data_object->dimensions = 3;

                            break;
                        case mas::MEAN_WEIGHT_AT_AGE_SPAWNING:
                            INFO_DEBUG
                            data_object->imax = years;
                            data_object->jmax = 1;
                            data_object->kmax = ages;
                            data_object->dimensions = 3;

                            break;

                    }



                    t_it = v[ii].FindMember("values");

                    if ((*t_it).value.IsArray()) {
                        rapidjson::Value& v = (*t_it).value;
                        switch (type) {

                            case mas::CATCH_MEAN_WEIGHT_AT_AGE:
                                check[0]++;
                                std::cout << "reading CATCH_MEAN_WEIGHT_AT_AGE\n";
                                INFO_DEBUG
                                data_object->imax = years;
                                data_object->jmax = seasons;
                                data_object->kmax = ages;
                                data_object->dimensions = 3;


                                INFO_DEBUG
                                data_object->imax = v.Size();
                                for (i = 0; i < v.Size(); i++) {
                                    if (!v[i].IsArray()) {
                                        std::cout << "Data Warning: Data Object \"values\" for catch_mean_weight_at_age expect as a 3 dimensional array.\n";
                                        mas::mas_log << "Data Warning: Data Object \"values\" for catch_mean_weight_at_age expect as a 3 dimensional array.\n";
                                    } else {
                                        data_object->jmax = v[i].Size();
                                        for (j = 0; j < v[i].Size(); j++) {
                                            if (!v[i][j].IsArray()) {
                                                std::cout << "Data Warning: Data Object \"values\" for catch_mean_weight_at_age expect as a 3 dimensional array.\n";
                                                mas::mas_log << "Data Warning: Data Object \"values\" for catch_mean_weight_at_age expect as a 3 dimensional array.\n";
                                            } else {
                                                data_object->kmax = v[i][j].Size();
                                                for (k = 0; k < v[i][j].Size(); k++) {
                                                    data_object->data.push_back(static_cast<REAL_T> (v[i][j][k].GetDouble()));
                                                }
                                            }
                                        }
                                    }
                                }




                                break;


                            case mas::SURVEY_MEAN_WEIGHT_AT_AGE:
                                check[1]++;
                                std::cout << "reading SURVEY_MEAN_WEIGHT_AT_AGE\n";
                                INFO_DEBUG
                                data_object->imax = years;
                                data_object->jmax = seasons;
                                data_object->kmax = ages;
                                data_object->dimensions = 3;



                                INFO_DEBUG
                                data_object->imax = v.Size();
                                for (i = 0; i < v.Size(); i++) {
                                    if (!v[i].IsArray()) {
                                        std::cout << "Data Warning: Data Object \"values\" for survey_mean_weight_at_age expect as a 3 dimensional array.\n";
                                        mas::mas_log << "Data Warning: Data Object \"values\" for survey_mean_weight_at_age expect as a 3 dimensional array.\n";
                                    } else {
                                        data_object->jmax = v[i].Size();
                                        for (j = 0; j < v[i].Size(); j++) {
                                            if (!v[i][j].IsArray()) {
                                                std::cout << "Data Warning: Data Object \"values\" for survey_mean_weight_at_age expect as a 3 dimensional array.\n";
                                                mas::mas_log << "Data Warning: Data Object \"values\" for survey_mean_weight_at_age expect as a 3 dimensional array.\n";
                                            } else {
                                                data_object->kmax = v[i][j].Size();
                                                for (k = 0; k < v[i][j].Size(); k++) {
                                                    data_object->data.push_back(static_cast<REAL_T> (v[i][j][k].GetDouble()));
                                                }
                                            }
                                        }
                                    }
                                }
                                break;

                            case mas::MEAN_WEIGHT_AT_AGE_SEASON_START:
                                check[2]++;
                                std::cout << "reading MEAN_WEIGHT_AT_AGE_SEASON_START\n";

                                INFO_DEBUG
                                data_object->imax = years;
                                data_object->jmax = seasons;
                                data_object->kmax = ages;
                                data_object->dimensions = 3;
                                INFO_DEBUG
                                data_object->imax = v.Size();
                                for (i = 0; i < v.Size(); i++) {
                                    if (!v[i].IsArray()) {
                                        std::cout << "Data Warning: Data Object \"values\" for empirical_mean_weight_at_age_season_start expect as a 2 dimensional array.\n";
                                        mas::mas_log << "Data Warning: Data Object \"values\" for empirical_mean_weight_at_age_season_start expect as a 2 dimensional array.\n";
                                    } else {
                                        data_object->jmax = v[i].Size();
                                        for (j = 0; j < v[i].Size(); j++) {
                                            data_object->data.push_back(static_cast<REAL_T> (v[i][j].GetDouble()));
                                        }
                                    }
                                }
                                break;
                            case mas::MEAN_WEIGHT_AT_AGE_SPAWNING:
                                check[3]++;
                                std::cout << "reading MEAN_WEIGHT_AT_AGE_SPAWNING\n";

                                INFO_DEBUG
                                data_object->imax = years;
                                data_object->jmax = 1;
                                data_object->kmax = ages;
                                data_object->dimensions = 3;
                                INFO_DEBUG
                                data_object->imax = v.Size();
                                for (i = 0; i < v.Size(); i++) {
                                    if (!v[i].IsArray()) {
                                        std::cout << "Data Warning: Data Object \"values\" for empirical_mean_weight_at_age_spawning expect as a 2 dimensional array.\n";
                                        mas::mas_log << "Data Warning: Data Object \"values\" for empirical_mean_weight_at_age_spawning expect as a 2 dimensional array.\n";
                                    } else {
                                        data_object->jmax = v[i].Size();
                                        for (j = 0; j < v[i].Size(); j++) {
                                            data_object->data.push_back(static_cast<REAL_T> (v[i][j].GetDouble()));
                                        }
                                    }
                                }
                                break;

                        }

                    }

                    mas::EmpricalDataStructure<REAL_T> eds;
                    eds.empirical_data_at_age = data_object;
                    mas::GrowthBase<REAL_T>::Do3DInterpolation(eds.empirical_data_at_age, eds.interpolated_data_at_age);

                    switch (data_object->sex_type) {
                        case mas::FEMALE:
                            eg->weight_at_age_data[data_object->type][mas::FEMALE] = eds;
                            break;
                        case mas::MALE:
                            eg->weight_at_age_data[data_object->type][mas::MALE] = eds;
                            break;
                        case mas::UNDIFFERENTIATED:
                            eg->weight_at_age_data[data_object->type][mas::FEMALE] = eds;
                            eg->weight_at_age_data[data_object->type][mas::MALE] = eds;
                            break;
                    }
                }
            }
        }

        int check_value = 0;
        for (int i = 0; i < check.size(); i++) {
            check_value += check[i];
        }
        if (check_value != 4) {
            //error here
        }

    } else {

        //instantiate default here
    }
}

void ParseConfig(rapidjson::Document& document) {

    rapidjson::Document::MemberIterator mit;

    for (mit = document.MemberBegin(); mit != document.MemberEnd(); ++mit) {
        std::cout << (*mit).name.GetString() << "\n";
        HandleGrowthModel(mit);
    }

}

void ParseConfig(const std::stringstream& ss) {
    rapidjson::Document document;
    document.Parse(ss.str().c_str());
    ParseConfig(document);
}

void ParseConfig(const std::string & path) {

    std::stringstream ss;
    std::ifstream config;
    config.open(path.c_str());
    if (!config.good()) {
        std::cout << "MAS Configuration file \"" << path << "\" not found.\n";

    }

    while (config.good()) {
        std::string line;
        std::getline(config, line);
        ss << line << "\n";
    }
    ParseConfig(ss);
}

/*
 *
 */
int main(int argc, char** argv) {

    ParseConfig("empirical_growth.json");





}
