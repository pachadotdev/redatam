#include "read.h"
#include "rindexing.h"
#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>

extern "C" {
	static const R_CMethodDef CEntries[] = {
    {"_redatam_read_rdtm", (DL_FUNC)&_redatam_read_rdtm, 1},
    {"_redatam_bin_get_rvector", (DL_FUNC)&_redatam_bin_get_rvector, 2},
    {"_redatam_pck_get_rvector", (DL_FUNC)&_redatam_pck_get_rvector, 2},
    {"_redatam_chr_get_rvector", (DL_FUNC)&_redatam_chr_get_rvector, 2},
    {"_redatam_int_get_rvector", (DL_FUNC)&_redatam_int_get_rvector, 2},
    {"_redatam_lng_get_rvector", (DL_FUNC)&_redatam_lng_get_rvector, 2},
    {"_redatam_real_get_rvector", (DL_FUNC)&_redatam_real_get_rvector, 2},
    {NULL, NULL, 0}
	};

	void R_init_redatam(DllInfo* info) {
    R_RegisterCCallable("redatam", "_redatam_read_rdtm", (DL_FUNC)&_redatam_read_rdtm);
    R_RegisterCCallable("redatam", "_redatam_bin_get_rvector", (DL_FUNC)&_redatam_bin_get_rvector);
    R_RegisterCCallable("redatam", "_redatam_pck_get_rvector", (DL_FUNC)&_redatam_pck_get_rvector);
    R_RegisterCCallable("redatam", "_redatam_chr_get_rvector", (DL_FUNC)&_redatam_chr_get_rvector);
    R_RegisterCCallable("redatam", "_redatam_int_get_rvector", (DL_FUNC)&_redatam_int_get_rvector);
    R_RegisterCCallable("redatam", "_redatam_lng_get_rvector", (DL_FUNC)&_redatam_lng_get_rvector);
    R_RegisterCCallable("redatam", "_redatam_real_get_rvector", (DL_FUNC)&_redatam_real_get_rvector);
    R_registerRoutines(info, NULL, NULL, NULL, NULL);
    R_useDynamicSymbols(info, TRUE);
  }
}
