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

description <- function(x, ...) UseMethod("description")

#' @export
description.redatam.database <- function(x, ...) cat(attr(x, "description"))

#' @export
description.redatam.entity <- function(x, ...) cat(attr(x, "description"))

#' @export
description.redatam.variable.int <- function(x, ...) cat(attr(x, "description"))

#' @export
description.redatam.variable.lng <- function(x, ...) cat(attr(x, "description"))

#' @export
description.redatam.variable.chr <- function(x, ...) cat(attr(x, "description"))

#' @export
description.redatam.variable.bin <- function(x, ...) cat(attr(x, "description"))

#' @export
description.redatam.variable.dbl <- function(x, ...) cat(attr(x, "description"))

#' @export
description.redatam.variable.pck <- function(x, ...) cat(attr(x, "description"))

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

#' @export
"[.redatam.variable.bin" <- function(x, i, value = "bin") variable_indexing(x, i, value)

#' @export
"[.redatam.variable.pck" <- function(x, i, value = "pck") variable_indexing(x, i, value)

#' @export
"[.redatam.variable.chr" <- function(x, i, value = "chr") variable_indexing(x, i, value)

#' @export
"[.redatam.variable.int" <- function(x, i, value = "int") variable_indexing(x, i, value)

#' @export
"[.redatam.variable.lng" <- function(x, i, value = "lng") variable_indexing(x, i, value)

#' @export
"[.redatam.variable.real" <- function(x, i, value = "real") variable_indexing(x, i, value)

assign_indexing <- function(x, i, val) stop("writing to redatam objects is not allowed")

#' @export
"[<-.redatam.database" <- function(x, i, value) assign_indexing(x, i, value)

#' @export
"[<-.redatam.entity" <- function(x, i, value) assign_indexing(x, i, value)

#' @export
"[<-.redatam.variable.bin" <- function(x, i, value) assign_indexing(x, i, value)

#' @export
"[<-.redatam.variable.pck" <- function(x, i, value) assign_indexing(x, i, value)

#' @export
"[<-.redatam.variable.chr" <- function(x, i, value) assign_indexing(x, i, value)

#' @export
"[<-.redatam.variable.int" <- function(x, i, value) assign_indexing(x, i, value)

#' @export
"[<-.redatam.variable.lng" <- function(x, i, value) assign_indexing(x, i, value)

#' @export
"[<-.redatam.variable.real" <- function(x, i, value) assign_indexing(x, i, value)

#' @export
length.redatam.variable.bin <- function(x) attr(x, "instance.num")

#' @export
length.redatam.variable.pck <- function(x) attr(x, "instance.num")

#' @export
length.redatam.variable.chr <- function(x) attr(x, "instance.num")

#' @export
length.redatam.variable.int <- function(x) attr(x, "instance.num")

#' @export
length.redatam.variable.lng <- function(x) attr(x, "instance.num")

#' @export
length.redatam.variable.real <- function(x) attr(x, "instance.num")

#' @export
print.redatam.variable.bin <- function(x, ...) print(x[1:length(x)], ...)

#' @export
print.redatam.variable.pck <- function(x, ...) print(x[1:length(x)], ...)

#' @export
print.redatam.variable.chr <- function(x, ...) print(x[1:length(x)], ...)

#' @export
print.redatam.variable.int <- function(x, ...) print(x[1:length(x)], ...)

#' @export
print.redatam.variable.lng <- function(x, ...) print(x[1:length(x)], ...)

#' @export
print.redatam.variable.real <- function(x, ...) print(x[1:length(x)], ...)

#' @export
format.redatam.variable.bin <- function(x, ...) format(x[1:length(x)], ...)

#' @export
format.redatam.variable.pck <- function(x, ...) format(x[1:length(x)], ...)

#' @export
format.redatam.variable.chr <- function(x, ...) format(x[1:length(x)], ...)

#' @export
format.redatam.variable.int <- function(x, ...) format(x[1:length(x)], ...)

#' @export
format.redatam.variable.lng <- function(x, ...) format(x[1:length(x)], ...)

#' @export
format.redatam.variable.real <- function(x, ...) format(x[1:length(x)], ...)
