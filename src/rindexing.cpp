#include <algorithm>
#include <cpp11.hpp>

#include "primitives.h"
#include "util.h"

using namespace cpp11;

// translate integer values from `dst` into
// the indices of their corresponding matches in `levelkeys`
// this is necessary because REDATAM can
// store factors as sparse, 0-based integers
// while R requires factors to be contiguous and 1-based
void translate_factor(sexp dst, sexp levelkeys) {
  const int *first = INTEGER(levelkeys);
  const int *last = first + Rf_length(levelkeys);
  for (int i = 0; i < Rf_length(dst); ++i) {
    int v = INTEGER_ELT(dst, i);
    const int *pos = std::lower_bound(first, last, v);
    SET_INTEGER_ELT(dst, i, (pos != last && *pos == v) ? pos - first + 1 : NA_INTEGER);
  }
}

// if `src` has `levels` and `levelkeys` attributes, then
// translate the values in `dst` to the factors in `src`
// using `translate_factor` and set the `levels` and `class`
// attributes of `dst` to turn it into a proper factor object
void configure_factor(sexp dst, sexp src) {
  sexp levels = Rf_getAttrib(src, R_LevelsSymbol);
  sexp levelkeys = Rf_getAttrib(src, Rf_mkString("levelkeys"));
  if (!Rf_isNull(levels) && !Rf_isNull(levelkeys)) {
    translate_factor(dst, levelkeys);
    Rf_setAttrib(dst, R_LevelsSymbol, levels);
    Rf_classgets(dst, Rf_mkString("factor"));
  }
}

// reads a variable-bit object using the provided function in `fread_elt`
// fread_elt should return a word
template <typename F>
sexp variable_bit_get_rvector(sexp robj, sexp indices, F &&fread_elt) {
  std::string path = CHAR(Rf_asChar(STRING_ELT(Rf_getAttrib(robj, Rf_mkString("data.path")), 0)));
  const unsigned int instance_len = INTEGER_ELT(Rf_getAttrib(robj, Rf_mkString("instance.len")), 0);
  const int instance_num = INTEGER_ELT(Rf_getAttrib(robj, Rf_mkString("instance.num")), 0);

  std::fstream f(path, std::ios_base::in | std::ios_base::binary);
  if (!f)
    error_return("could not open data file for reading");

  sexp ans = Rf_allocVector(INTSXP, Rf_length(indices));

  const uint32_t bits_per_word = sizeof(uint32_t) * 8;
  const uint32_t mask = (1 << instance_len) - 1;

  uint32_t element = fread_elt(f);
  size_t current_offset = 0;
  for (int dst_index = 0; dst_index < Rf_length(indices); ++dst_index) {
    int src_index = INTEGER_ELT(indices, dst_index) - 1;

    if (src_index < 0 || src_index >= instance_num) {
      SET_INTEGER_ELT(ans, dst_index, NA_INTEGER);
      continue;
    }

    size_t target_offset = src_index * instance_len / bits_per_word * sizeof(uint32_t);
    if (target_offset != current_offset) {
      f.seekg(target_offset);
      element = fread_elt(f);
      current_offset = target_offset;
    }
    const int shifted_bits = src_index * instance_len % bits_per_word;

    if (bits_per_word - shifted_bits >= instance_len) {
      uint32_t v = element >> (sizeof(uint32_t) * 8 - instance_len - shifted_bits);
      v &= mask;
      SET_INTEGER_ELT(ans, dst_index, v);
    } else {
      uint32_t next_element = fread_elt(f);
      const int bits_in_first = instance_len + shifted_bits - bits_per_word;
      uint32_t v = (element << bits_in_first) + (next_element >> (bits_per_word - bits_in_first));
      SET_INTEGER_ELT(ans, dst_index, v & mask);
      element = next_element;
      ++current_offset;
    }
  }
  configure_factor(ans, robj);
  return ans;
}

template <typename F, typename G, typename NAT>
sexp variable_byte_get_rvector(sexp robj, sexp indices, F &&fread_elt, G &&write_elt, NAT naval, int anstype) {
  std::string path = CHAR(Rf_asChar(STRING_ELT(Rf_getAttrib(robj, Rf_mkString("data.path")), 0)));
  const int instance_len = INTEGER_ELT(Rf_getAttrib(robj, Rf_mkString("instance.len")), 0);
  const int instance_num = INTEGER_ELT(Rf_getAttrib(robj, Rf_mkString("instance.num")), 0);

  std::fstream f(path, std::ios_base::in | std::ios_base::binary);
  if (!f)
    error_return("could not open data file for reading");

  sexp ans = Rf_allocVector(anstype, Rf_length(indices));

  size_t current_offset = 0;
  for (int dst_index = 0; dst_index < Rf_length(indices); ++dst_index) {
    int src_index = INTEGER_ELT(indices, dst_index) - 1;

    if (src_index < 0 || src_index >= instance_num) {
      write_elt(ans, dst_index, naval);
      continue;
    }

    size_t target_offset = src_index * instance_len;
    if (target_offset != current_offset) {
      f.seekg(target_offset);
      current_offset = target_offset;
    }
    auto v = fread_elt(f, instance_len);
    write_elt(ans, dst_index, v);
    current_offset += instance_len;
  }
  return ans;
}

[[cpp11::register]]
sexp bin_get_rvector(sexp bin, sexp indices) {
  return variable_bit_get_rvector(bin, indices, fread_BIN);
}

[[cpp11::register]]
sexp pck_get_rvector(sexp pck, sexp indices) {
  return variable_bit_get_rvector(pck, indices, fread_PCK);
}

[[cpp11::register]]
sexp chr_get_rvector(sexp chr, sexp indices) {
  return variable_byte_get_rvector(chr, indices,
    [](std::fstream &f, size_t instance_len) {
      std::string s = fread_fixed_string(f, instance_len);
      rtrim(s);
      return Rf_mkChar(s.c_str());
    },
    [](sexp dst, int dst_index, sexp s) {
      SET_STRING_ELT(dst, dst_index, s);
    },
    NA_STRING, STRSXP);
}

[[cpp11::register]]
sexp int_get_rvector(sexp intg, sexp indices) {
  sexp ans = variable_byte_get_rvector(intg, indices,
    [](std::fstream &f, int unused) { return fread_uint16_t(f); },
    SET_INTEGER_ELT, NA_INTEGER, INTSXP);
  configure_factor(ans, intg);
  return ans;
}

[[cpp11::register]]
sexp lng_get_rvector(sexp lng, sexp indices) {
  sexp ans = variable_byte_get_rvector(lng, indices,
    [](std::fstream &f, int unused) { return fread_uint32_t(f); },
    SET_INTEGER_ELT, NA_INTEGER, INTSXP);
  configure_factor(ans, lng);
  return ans;
}

[[cpp11::register]]
sexp real_get_rvector(sexp real, sexp indices) {
  return variable_byte_get_rvector(real, indices,
    [](std::fstream &f, int unused) { return fread_double(f); },
    SET_REAL_ELT, NA_REAL, REALSXP);
}
