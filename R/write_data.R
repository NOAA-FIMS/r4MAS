#'

write_data <- function(data_list, outfile_path) {
  #Write out data .json file
  jsonlite::write_json(data_list, outfile_path)
}
