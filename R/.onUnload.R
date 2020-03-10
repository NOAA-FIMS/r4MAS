onUnload <- function (libpath) {
  library.dynam.unload("RMAS", libpath)
}