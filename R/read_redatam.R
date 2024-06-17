
#' Read REDATAM file
#'
#' This function reads a REDATAM file using the C++ code.
#'
#' @param file_path The path to the REDATAM file.
#' @return A character vector with the contents of the file.
#' @export
read_redatam <- function(file_path) {
  .Call(`read_redatam_`, file_path)
}
