# README

# (RMAS) R interface to the Metapopulation Assessment System

**************

https://nmfs-fish-tools.github.io/RMAS/

**************


[![CII Best Practices](https://bestpractices.coreinfrastructure.org/projects/2787/badge)](https://bestpractices.coreinfrastructure.org/projects/2787)

**************

## This is a repository for the Stock Assesment Tool: RMAS
- Supported by the NOAA Fisheries Integrated Toolbox

## About RMAS

## Installation instructions
`RMAS` runs the Metapopulation Assessment System model solver, which is written in C++. Since this package contains C++ files, you will need to ensure you have a C++ compiler installed. You can install a C++ compiler here, please make sure it is C++14 or more recent.

Please install `RMAS` from Github using the `devtools` package as follows:
```
devtools::install_github("nmfs-fish-tools/RMAS")
```

**Windows users**: If you receive errors related to C++ when installing RMAS, it could be because your R installation uses the default Makevars.win file. To update your Makevars.win file, please run the following code in your R terminal:

```
dotR <- file.path(Sys.getenv("HOME"), ".R")
if (!file.exists(dotR)) 
  dir.create(dotR)
M <- file.path(dotR, "Makevars.win")
if (!file.exists(M)) 
  file.create(M)
cat("\nCXX14FLAGS=-O3 -Wno-unused-variable -Wno-unused-function",
    "CXX14 = $(BINPREF)g++ -m$(WIN) -std=c++1y",
    "CXX11FLAGS=-O3 -Wno-unused-variable -Wno-unused-function",
    file = M, sep = "\n", append = TRUE)
```

## Disclaimer

“The United States Department of Commerce (DOC) GitHub project code is provided on an ‘as is’ basis and the user assumes responsibility for its use. DOC has relinquished control of the information and no longer has responsibility to protect the integrity, confidentiality, or availability of the information. Any claims against the Department of Commerce stemming from the use of its GitHub project will be governed by all applicable Federal law. Any reference to specific commercial products, processes, or services by service mark, trademark, manufacturer, or otherwise, does not constitute or imply their endorsement, recommendation or favoring by the Department of Commerce. The Department of Commerce seal and logo, or the seal and logo of a DOC bureau, shall not be used in any manner to imply endorsement of any commercial product or activity by DOC or the United States Government.”

- This project code is made available through GitHub but is managed by NOAA at
 https://vlab.ncep.noaa.gov/redmine/projects/integrated-fisheries-toolbox/files

***** *******
