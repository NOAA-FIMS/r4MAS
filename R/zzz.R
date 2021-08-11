# Load RMAS module -------------------------------------------------------------------------
.onLoad <- function(libname, pkgname) {

  library.dynam("r4MAS", pkgname, libname)

}

.LastLib <- function(libpath)
 {
   library.dynam.unload("r4MAS", libpath)
}
