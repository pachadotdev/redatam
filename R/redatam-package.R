#' @title COMPLETE
#'
#' @description
#' COMPLETE
#'
#' @name redatam-package
#' @useDynLib redatam, "_redatam_read_rdtm"
#' @useDynLib redatam, "_redatam_bin_get_rvector"
#' @useDynLib redatam, "_redatam_chr_get_rvector"
#' @useDynLib redatam, "_redatam_int_get_rvector"
#' @useDynLib redatam, "_redatam_lng_get_rvector"
#' @useDynLib redatam, "_redatam_pck_get_rvector"
#' @useDynLib redatam, "_redatam_real_get_rvector"
"_PACKAGE"

#' Read a REDATAM dictionary file with the corresponding entity tables
#' @param dic_path Path to the REDATAM dictionary file
#' @export 
read_redatam <- function(dic_path) read_rdtm(dic_path)

variable_indexing <- function(x, i, funcname = "bin") {
  if (is.logical(i)) {
    i <- which(i)
  }
  if (!is.numeric(i) && !is.integer(i)) {
    stop("invalid indexing mode")
  }
  i <- as.integer(i)
  negindx <- all(i <= 0)
  posindx <- any(i >= 0)
  if (negindx && posindx) {
    stop("only 0's may be mixed with negative subscripts")
  }
  if (negindx) {
    i <- (1:length(x))[i]
  }
  i <- i[i != 0]

  switch(funcname,
    bin = bin_get_rvector(x, i),
    pck = pck_get_rvector(x, i),
    chr = chr_get_rvector(x, i),
    int = int_get_rvector(x, i),
    lng = lng_get_rvector(x, i),
    real = real_get_rvector(x, i)
  )
}

assign_indexing <- function(x, i, val) stop("writing to redatam objects is not allowed")
