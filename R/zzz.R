# Load RMAS module -------------------------------------------------------------------------
.onLoad <- function(libname, pkgname) {

  library.dynam("RMAS", pkgname, libname)

}

# Unload RMAS ---------------------------------------------------------------------------------

# .onUnload <- function(libpath) {
#
#   library.dynam.unload("RMAS", libpath)
#
# }
