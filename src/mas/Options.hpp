/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Options.hpp
 * Author: mattadmin
 *
 * Created on March 28, 2019, 9:35 AM
 */

#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include "third_party/ATL/lib/Utilities/StringUtil.hpp"



namespace mas {

    enum Minimizer {
        LBFGS,
        LBFGS2,
        BFGS,
        GD,
        CGD,
        NM
    };

    enum MCMC {
        NUTS,
        GIBBS,
        MH
    };

    template<typename T>
    struct Options {
        //minimizer options
        Minimizer minimizer = LBFGS;
        T tol = static_cast<T> (1e-4);
        int max_iter = 5000;
        int max_ls = 50;
        int iprint = 10;
#ifdef MAS_ENSEMBLE
        bool convergence_required = false;
#endif

        //mcmc options
        MCMC mcmc = MH;
        int boot_strap = 5000;
        int mcmc_iter = 5000;
        int esemble_size = 10;
        std::string config = "NA";
        std::string data = "NA";
        std::string output = "mas_report.json";
        std::string output_directory = "NA";
        bool operating_model = false;


    };

    void Help() {
        std::cout << std::endl;
        std::cout << util::center("                ███╗ ███╗  █████╗  ███████╗", 60) << std::endl;
        std::cout << util::center("                ████╗ ████║██╔══██╗██╔════╝", 60) << std::endl;
        std::cout << util::center("                ██╔████╔██║███████║███████╗", 60) << std::endl;
        std::cout << util::center("                ██║╚██╔╝██║██╔══██║╚════██║", 60) << std::endl;
        std::cout << util::center("                ██║ ╚═╝ ██║██║  ██║███████║", 60) << std::endl;
        std::cout << util::center("Meta-Population Assessment System", 60) << std::endl;
        std::cout << util::center("National Oceanic and Atmospheric Administration", 60) << std::endl;
        std::cout << util::center("National Marine Fisheries Service", 60) << std::endl;
        std::cout << std::endl;
        std::cout << "-min    [argument]\n";
        std::cout << std::setw(35) << " " << std::left << "l-bfgs (default)\n";
        std::cout << std::setw(35) << " " << std::left << "l-bfgs2 \n";
        std::cout << std::setw(35) << " " << std::left << "bfgs \n";
        std::cout << std::setw(35) << " " << std::left << "conjugate_gradient \n";
        std::cout << std::setw(35) << " " << std::left << "gradient_descent \n";
        std::cout << std::setw(35) << " " << std::left << "nelder_mead \n\n";
        std::cout << "-tol          [minimizer tolerance]\n";
        std::cout << "-max_iter     [max iterations]\n";
        std::cout << "-max_ls     [max line searches]\n";
        std::cout << "-iprint        [print interval]\n";
        std::cout << "-mcmc         [argument]\n";
        std::cout << std::setw(35) << " " << std::left << "nuts - No u turn sampler\n";
        std::cout << std::setw(35) << " " << std::left << "gibbs - Gibbs sample \n";
        std::cout << std::setw(35) << " " << std::left << "mh - Metropolis-Hastings\n\n";
        std::cout << "-mcmc_iter    [max iterations]\n";
        std::cout << "-boot_strap   [max iterations]\n";
        std::cout << "-config       [path_to_config_file]\n";
        std::cout << "-data         [path_to_data_file]\n";
        std::cout << "-o            [output_directory]\n";
        std::cout << "-help  \n";
    }

    template<typename T>
    void ParseOptions(const std::vector<std::string>& args, mas::Options<T>& options) {

        for (int i = 0; i < args.size(); i++) {

            if (args[i] == "-min" && args.size()> (i + 1)) {
                if (args[i + 1] == "l-bfgs") {
                    options.minimizer = mas::LBFGS;
                } else if (args[i + 1] == "l-bfgs2") {
                    options.minimizer = mas::LBFGS2;
                } else if (args[i + 1] == "bfgs") {
                    options.minimizer = mas::BFGS;
                } else if (args[i + 1] == "gradient_descent") {
                    options.minimizer = mas::GD;
                } else if (args[i + 1] == "conjugate_gradient") {
                    options.minimizer = mas::CGD;
                } else if (args[i + 1] == "nelder_mead") {
                    options.minimizer = mas::NM;
                } else {
                    std::cout << "Warning: unknown minimizer type \"" << args[i + 1] << "\"\n";
                }
            }

#ifdef MAS_ENSEMBLE
            if (args[i] == "-convergence_required") {
                options.convergence_required = true;
            }
#endif



            if (args[i] == "-mcmc" && args.size()> (i + 1)) {
                if (args[i + 1] == "nuts") {
                    options.mcmc = mas::NUTS;
                } else if (args[i + 1] == "gibbs") {
                    options.mcmc = mas::GIBBS;
                } else if (args[i + 1] == "mh") {
                    options.mcmc = mas::MH;
                } else {
                    std::cout << "Warning: unknown mcmc routine type \"" << args[i + 1] << "\"\n";
                }
            }

            if (args[i] == "-operating_model") {
                options.operating_model = true;
            }

            if (args[i] == "-config" && args.size() > (i + 1)) {
                options.config = args[i + 1];
            }

            if (args[i] == "-data" && args.size() > (i + 1)) {
                options.data = args[i + 1];
            }

            if (args[i] == "-output" && args.size() > (i + 1)) {
                options.output = args[i + 1];
            }

            if (args[i] == "-ensemble_size" && args.size() > (i + 1)) {
                options.esemble_size = util::StringToNumber<int>(args[i + 1]);
            }

            if (args[i] == "-max_iter" && args.size() > (i + 1)) {
                options.max_iter = util::StringToNumber<int>(args[i + 1]);
            }

            if (args[i] == "-max_ls" && args.size() > (i + 1)) {
                options.max_ls = util::StringToNumber<int>(args[i + 1]);
            }

            if (args[i] == "-iprint" && args.size() > (i + 1)) {
                options.iprint = util::StringToNumber<int>(args[i + 1]);
            }
        }

    }

}


#endif /* OPTIONS_HPP */

