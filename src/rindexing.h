#ifndef _REDATAM_RINDEXING_H_
#define _REDATAM_RINDEXING_H_

#include <R.h>
#include <Rinternals.h>

extern "C" {
SEXP _redatam_bin_get_rvector(SEXP bin, SEXP indices);
SEXP _redatam_pck_get_rvector(SEXP pck, SEXP indices);
SEXP _redatam_chr_get_rvector(SEXP chr, SEXP indices);
SEXP _redatam_int_get_rvector(SEXP intg, SEXP indices);
SEXP _redatam_lng_get_rvector(SEXP lng, SEXP indices);
SEXP _redatam_real_get_rvector(SEXP real, SEXP indices);
}

#endif // _REDATAM_RINDEXING_H_
