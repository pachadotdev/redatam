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
