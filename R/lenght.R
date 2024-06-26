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
