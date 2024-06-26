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
