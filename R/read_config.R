#'

read_config <- function(config_file_path) {
  #Read in config.json file
  jsonlite::read_json(config_file_path)
}
