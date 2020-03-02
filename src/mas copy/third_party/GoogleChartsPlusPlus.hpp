/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   GoogleChartsPlusPlus.hpp
 * Author: oppy
 *
 * Created on November 29, 2018, 9:56 AM
 */

#ifndef GOOGLECHARTSPLUSPLUS_HPP
#define GOOGLECHARTSPLUSPLUS_HPP

#include <vector>
#include <sstream>


namespace gcpp {

    template<typename T>
    class DataTable {
        std::vector<std::pair<std::string, std::string> > columns;
        std::vector<std::vector<T> > rows;
    public:

        void AddColumn(const std::string& type, const std::string& value) {
            this->columns.push_back(std::make_pair(type, value));
        }

        void AddRow(const std::vector<T>& row) {
            this->rows.push_back(row);
        }

        std::string Create() {
            std::stringstream ss;
            ss << "var data = new google.visualization.DataTable();\n";
            for (int i = 0; i < this->columns.size(); i++) {
                ss << "data.addColumn('" << this->columns[i].first << "', '" << this->columns[i].second << "');\n";
            }

            ss << "\n";
            ss << "data.addRows([";
            for (int i = 0; i < this->rows.size(); i++) {
                ss<<"[";
                for (int j = 0; j < this->rows[i].size(); j++) {
                    ss << this->rows[i][j];
                    if (j == this->rows[i].size() - 1) {
                        ss << "]";
                    } else {
                        ss << ",";
                    }
                }
                if(i == rows.size()-1){
                    ss<<"]);\n";
                }else{
                    ss<<",\n";
                }
            }
            

            return ss.str();
        }

    };

    template<typename T>
    class ChartObject {
        static size_t uid;
    public:
        size_t id = ChartObject<T>::uid++;
        DataTable<T> data_table;
        std::string title = "";
        std::string subtitle = "";
        std::string htitle = "";
        std::string vtitle = "";
        int width = 900;
        int height = 500;
        virtual std::string Create() = 0;

        std::string GetName() {
            std::stringstream ss;
            ss << "DrawChart" << this->id;
            return ss.str();
        }
    };

    template<typename T>
    size_t ChartObject<T>::uid = 0;

    template<typename T>
    class LineChart : public ChartObject<T> {
    public:

        std::string Create() {
            std::stringstream ss;
            ss << "<script type = \"text/javascript\" src = \"https://www.gstatic.com/charts/loader.js\" > </script> \n<script type = \"text/javascript\" >";
            ss << "\ngoogle.charts.load('current', {'packages':['line']});\n";

            ss << "google.charts.setOnLoadCallback(" << this->GetName() << ");\n";

            ss << "function " << this->GetName() << "(){\n";

            ss << this->data_table.Create();


            ss << "      var options = {\n";
            ss << " chart: {\n";
            ss << "title: '" << this->title << "',\n";
            ss << "subtitle: '" << this->subtitle << "'\n},hAxis:{title : '"<<this->htitle<<"'},vAxis:{title : '"<<this->vtitle<<"'},\n width: " << this->width << ",\nheight: " << this->height << "\n};\n\n";

            ss << " var chart = new google.charts.Line(document.getElementById('line_" << this->id << "'));\n";
            ss << "chart.draw(data, google.charts.Line.convertOptions(options));\n}</script>\n";

            ss << "<div id=\"line_" << this->id << "\"></div>\n\n";

            return ss.str();

        }


    };


}


#endif /* GOOGLECHARTSPLUSPLUS_HPP */

