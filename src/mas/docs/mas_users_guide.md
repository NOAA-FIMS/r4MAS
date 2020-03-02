Metapopulation Assessment System (MAS) User Guide
-------------------------------------------

The Metapopulation Assessment System (MAS) allows for optimization of population models including multiple populations and areas. The software uses a C++-based auto-differentiation engine called the Analytics Template Library (ATL), takes JSON input files (config and data), and estimates population time series and key life history parameters.  

### Installation
To run MAS, you'll need a C++ compiler and ATL software. If you are using a Windows PC, download and install a C++ compiler (a common one is the MinGW gcc compiler available here: https://sourceforge.net/projects/mingw-w64/) and ensure the `bin` directory of your compiler is on the system environment PATH variable.

To install MAS, you can clone the github repository from NOAA's Vlab (https://vlab.ncep.noaa.gov/redmine/projects/mas). First, you will need to install ATL. This can be done running the shell script entitled `get_atl.sh` within the Third_Party directory of the MAS repository. This script clones the ATL Github repository into the Third_Party MAS directory, but if you'd like to clone it yourself elsewhere, you can clone directly from the Github repository (https://github.com/nmfs-general-modeling-tools/ATL). If you manually clone ATL, you will need to run the `get_numericalsolvers.sh` shell script within the third_party directory of the ATL repository. The developer documentation for ATL can be found at https://nmfs-general-modeling-tools.github.io/ATL/html/annotated.html. Once MAS is downloaded/cloned, you can compile the model using the `make.bat` file in the main repository.

Running MAS
---------------------

After MAS is compiled, you should see a single executable (MAS.exe) in the bin directory. You can run MAS from the command line or terminal by calling the location of the mas executable, followed by `-config` with the name of the configuration .json file and `-data` followed by the name of the data .json. Input files for a simple test case (Case 1) are provided in the Tests/ directory.

```
../../bin/mas -config mas_case_1_config.json  -data mas_case_1_data.json 
```

## Config file setup

## Data file setup

Model options and structure
---------------------

## Recruitment function options

MAS supports a number of different recruitment functions. Which parameters are needed in the config file depends on which of the following recruitment functions are used. In all equations below, the spawning stock biomass (i.e. biomass of reproductively mature females) at time $t$ is represented as $S(t)$, the recruited fish at time $t$ are given as $R(t)$, unfished recruitment is $R_0$ and the spawning biomass in an unfished state is $SB_0$. 
$$SB_0 = R_0 * \left( \sum_{a=2}^{A-1} \left(N_{a-1}exp(-M) \mathrm{mat}_a \mathrm{f}_a \right) + \frac{N_{A-1}exp(-M)\mathrm{mat}_A\mathrm{f}_A}{1-exp(-M)}\right)$$

where $N_1 = 1.0$, $M$ is the natural mortality rate, and $\mathrm{mat}_a$ and $\mathrm{f}_a$ are the maturity and fecundity at age $a$ respectively.

| Recruitment function name | Parameters | Equations | Tested? |
| ----------- | ----------- | ----------- | ----------- |
| Ricker      | $\alpha, \beta$ | $R(t) = \alpha S(t-1) exp(-\beta S(t-1))  exp(-0.5*\sigma_R^2)$ | No |
| Ricker Alternative   | $R_0, h, \sigma_R$  | $R(t) = \frac{R_0SB_0}{SB_0} exp(h(1-SB(t-1)/SB_0) exp(-0.5*\sigma_R^2)$ | No | 
| Beverton-Holt| $\alpha, \beta, \sigma_R$  | $R(t) = \frac{\alpha S(t-1)}{\beta + S(t-1)}  exp(-0.5*\sigma_R^2)$ | No |
| Beverton-Holt Alternative | $R_0, h, \sigma_R$ | $R(t) = \frac{4hR_0 S(t-1)}{SB_0 (1-h) + S(t-1)*(5h-1)}  exp(-0.5*\sigma_R^2)$ | In progress |
|Shepherd | $\alpha, \beta, c$ | $R(t) = \frac{\alpha S(t-1)}{1+\left(\frac{S(t-1)}{\beta}\right)^c}$ | No |
|Deriso | $\alpha, \beta, c$ | $R(t) = \alpha S(t-1)\left(1-\beta c S(t-1)\right)^{1/c}$| No |

## Growth options

Growth can be specified through empirical weight-at-age matrices only in MAS currently, although we plan to implement a growth curve option in the future.

## Maturity and fecundity

## Natural mortality

## Population trajectory

Output 
---------------------
