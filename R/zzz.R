# Load RMAS module -------------------------------------------------------------------------
.onLoad <- function(libname, pkgname) {

  library.dynam("r4MAS", pkgname, libname)

}
