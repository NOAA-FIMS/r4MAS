#'

read_data <- function(data_file_path) {
  #Read in data .json file
  jsonlite::read_json(data_file_path)
}
