library(testthat)
library(RMAS)
library(Rcpp)
library(jsonlite)

# Use examples from 'extdata' in 'inst' because it is loaded with RMAS package
test_example_path <- system.file("extdata", package="RMAS")

# Find path of RMAS dll
libs_path <- system.file("libs", package="RMAS")
dll_name <- paste("RMAS", .Platform$dynlib.ext, sep="")

if(.Platform$OS.type == "windows") {
  dll_path <- file.path(libs_path, .Platform$r_arch, dll_name)
} else {
  dll_path <- file.path(libs_path, dll_name)
}

test_check("RMAS")
