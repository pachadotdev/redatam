#ifndef _REDATAM_REDATAM_H_
#define _REDATAM_REDATAM_H_

#include <R.h>
#include <Rinternals.h>

extern "C" {
SEXP _redatam_read_rdtm(SEXP dic_path);
}

#endif
