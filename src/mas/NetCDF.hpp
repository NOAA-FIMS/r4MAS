/* 
 * File:   NetCDF.hpp
 * Author: matthewsupernaw
 *
 * Created on August 26, 2014, 11:02 AM
 */

#ifndef NETCDF_HPP
#define NETCDF_HPP

#include <netcdf>

using namespace netCDF;

class netcdf_input {
    std::string path;
    NcFile input_file;
public:

    netcdf_input() {
    }

    netcdf_input(const std::string& path) {
        this->path = path;
        input_file.open(path, NcFile::read);
    }

    void open(const std::string& path) {
        this->path = path;
        input_file.open(path, NcFile::read);
    }

    int read_int(const std::string& variable, size_t i, size_t j = 0, size_t k = 0, size_t l = 0, size_t m = 0, size_t n = 0) {
        NcVar var = this->input_file.getVar(variable);
        int dimensions = var.getDimCount();
        std::vector<size_t> index;
        switch (dimensions) {
            case 1:
                index = {i};
                break;
            case 2:
                index = {i, j};
                break;
            case 3:
                index = {i, j, k};
                break;
            case 4:
                index = {i, j, k, l};
                break;
            case 5:
                index = {i, j, k, l, m};
                break;
            case 6:
                index = {i, j, k, l, m, n};
                break;
            default:
                index = {i};
        }

        int ret = 0;
        var.getVar(index, &ret);
        return ret;
    }

    long read_long(const std::string& variable, size_t i, size_t j = 0, size_t k = 0, size_t l = 0, size_t m = 0, size_t n = 0) {
        NcVar var = this->input_file.getVar(variable);
        int dimensions = var.getDimCount();
        std::vector<size_t> index;
        switch (dimensions) {
            case 1:
                index = {i};
                break;
            case 2:
                index = {i, j};
                break;
            case 3:
                index = {i, j, k};
                break;
            case 4:
                index = {i, j, k, l};
                break;
            case 5:
                index = {i, j, k, l, m};
                break;
            case 6:
                index = {i, j, k, l, m, n};
                break;
            default:
                index = {i};
        }

        long ret = 0;
        var.getVar(index, &ret);
        return ret;
    }

    float read_float(const std::string& variable, size_t i, size_t j = 0, size_t k = 0, size_t l = 0, size_t m = 0, size_t n = 0) {
        NcVar var = this->input_file.getVar(variable);
        int dimensions = var.getDimCount();
        std::vector<size_t> index;
        switch (dimensions) {
            case 1:
                index = {i};
                break;
            case 2:
                index = {i, j};
                break;
            case 3:
                index = {i, j, k};
                break;
            case 4:
                index = {i, j, k, l};
                break;
            case 5:
                index = {i, j, k, l, m};
                break;
            case 6:
                index = {i, j, k, l, m, n};
                break;
            default:
                index = {i};
        }

        float ret = 0;
        var.getVar(index, &ret);
        return ret;
    }

    double read_double(const std::string& variable, size_t i, size_t j = 0, size_t k = 0, size_t l = 0, size_t m = 0, size_t n = 0) {
        NcVar var = this->input_file.getVar(variable);
        int dimensions = var.getDimCount();
        std::vector<size_t> index;
        switch (dimensions) {
            case 1:
                index = {i};
                break;
            case 2:
                index = {i, j};
                break;
            case 3:
                index = {i, j, k};
                break;
            case 4:
                index = {i, j, k, l};
                break;
            case 5:
                index = {i, j, k, l, m};
                break;
            case 6:
                index = {i, j, k, l, m, n};
                break;
            default:
                index = {i};
        }

        double ret = 0;
        var.getVar(index, &ret);
        return ret;
    }

    std::multimap<std::string, netCDF::NcVar> get_variables() {
        return this->input_file.getVars();
    }

    NcGroupAtt get_global_attributes(std::string att) {
        return this->input_file.getAtt(att);
    }

    std::map<std::string, NcVarAtt> get_attributes(std::string variable) {
        NcVar var = this->input_file.getVar(variable);
        return var.getAtts();
    }

    size_t dimension_size(const std::string& name) {
        NcDim d = this->input_file.getDim(name);
        return d.getSize();
    }

};

class netcdf_output {
    //    NcFile output_file;
    std::string path;
public:
    NcFile output_file;

    netcdf_output() {

    }

    netcdf_output(std::string path) {
        this->path = path;
        output_file.open(path, NcFile::replace);
    }

    void add_variable(const std::string& name, const std::string& data_type, const std::vector<std::string>& dimensions) {
        NcVar var = this->output_file.addVar(name, data_type, dimensions);
        //        NcType t = var.getType();
        //        switch (t.getId()) {
        //            case NcType::nc_INT:
        //                var.setFill(true, static_cast<int> (0));
        //                break;
        //            case NcType::nc_INT64:
        //                var.setFill(true, static_cast<long> (0));
        //                break;
        //            case NcType::nc_FLOAT:
        //                var.setFill(true, static_cast<float> (0));
        //                break;
        //            case NcType::nc_DOUBLE:
        //                var.setFill(true, static_cast<double> (0));
        //                break;
        //
        //        }
    }

    void add_dimension(const std::string& name, size_t size) {
        this->output_file.addDim(name, size);
    }

    size_t dimension_size(const std::string& name) {
        NcDim d = this->output_file.getDim(name);
        return d.getSize();
    }

    void add_variable_attribute(const std::string& variable_name, const std::string& attribute_name, const std::string& attribute) {
        NcVar var = this->output_file.getVar(variable_name);
        var.putAtt(attribute_name, attribute);
    }

    void add_global_attribute(const std::string& name, const std::string& attribute) {
        this->output_file.putAtt(name, attribute);
    }

    void create() {
        this->output_file.enddef();
    }

    void close() {
        this->output_file.close();
    }

    void write_int(const std::string& variable, const int& value, size_t i, size_t j = 0, size_t k = 0, size_t l = 0, size_t m = 0, size_t n = 0) {
        NcVar var = this->output_file.getVar(variable);
        int dimensions = var.getDimCount();
        std::vector<size_t> index;
        switch (dimensions) {
            case 1:
                index = {i};
                break;
            case 2:
                index = {i, j};
                break;
            case 3:
                index = {i, j, k};
                break;
            case 4:
                index = {i, j, k, l};
                break;
            case 5:
                index = {i, j, k, l, m};
                break;
            case 6:
                index = {i, j, k, l, m, n};
                break;
            default:
                index = {i};
        }

        var.putVar(index, value);

    }

    void write_long(const std::string& variable, const long& value, size_t i, size_t j = 0, size_t k = 0, size_t l = 0, size_t m = 0, size_t n = 0) {
        NcVar var = this->output_file.getVar(variable);
        int dimensions = var.getDimCount();
        std::vector<size_t> index;
        switch (dimensions) {
            case 1:
                index = {i};
                break;
            case 2:
                index = {i, j};
                break;
            case 3:
                index = {i, j, k};
                break;
            case 4:
                index = {i, j, k, l};
                break;
            case 5:
                index = {i, j, k, l, m};
                break;
            case 6:
                index = {i, j, k, l, m, n};
                break;
            default:
                index = {i};
        }

        var.putVar(index, value);

    }

    void write_float(const std::string& variable, const float& value, size_t i, size_t j = 0, size_t k = 0, size_t l = 0, size_t m = 0, size_t n = 0) {
        NcVar var = this->output_file.getVar(variable);
        int dimensions = var.getDimCount();
        std::vector<size_t> index;
        switch (dimensions) {
            case 1:
                index = {i};
                break;
            case 2:
                index = {i, j};
                break;
            case 3:
                index = {i, j, k};
                break;
            case 4:
                index = {i, j, k, l};
                break;
            case 5:
                index = {i, j, k, l, m};
                break;
            case 6:
                index = {i, j, k, l, m, n};
                break;
            default:
                index = {i};
        }

        var.putVar(index, value);

    }

    void write_double(const std::string& variable, const double& value, size_t i, size_t j = 0, size_t k = 0, size_t l = 0, size_t m = 0, size_t n = 0) {
        NcVar var = this->output_file.getVar(variable);
        int dimensions = var.getDimCount();
        std::vector<size_t> index;
        switch (dimensions) {
            case 1:
                index = {i};
                break;
            case 2:
                index = {i, j};
                break;
            case 3:
                index = {i, j, k};
                break;
            case 4:
                index = {i, j, k, l};
                break;
            case 5:
                index = {i, j, k, l, m};
                break;
            case 6:
                index = {i, j, k, l, m, n};
                break;
            default:
                index = {i};
        }

        var.putVar(index, value);

    }

};


#endif /* NETCDF_HPP */




