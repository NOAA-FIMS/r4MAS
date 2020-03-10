.onUnload <- function (libpath) {
  library.dynam.unload("R4MAS", libpath)
}