#include <algorithm>
#include <cpp11.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "dictionary_descriptor.h"
#include "entity_descriptor.h"
#include "primitives.h"
#include "util.h"
#include "variable_descriptor.h"

using namespace cpp11;

template <typename T> sexp serialize_to_R(const T &obj) {
  std::stringstream ss;
  ss << obj << '\n';
  return Rf_mkString(ss.str().c_str());
}

[[cpp11::register]] sexp read_redatam_(strings dic_path_in) {
  r_string r_path_string(dic_path_in);
  UnixPath dic_path(r_path_string);

  std::fstream dic_file(dic_path.as_string(),
                        std::ios_base::in | std::ios_base::binary);

  if (!dic_file)
    error_return("could not open dictionary file for reading");

  auto dic = DictionaryDescriptor::fread(dic_file);
  auto root_entity = EntityDescriptor::fread(dic_file, true);

  sexp description_rsymbol = Rf_mkString("description");
  sexp levelkeys_rsymbol = Rf_mkString("levelkeys");

  sexp entity_class_rvector = Rf_allocVector(STRSXP, 2);
  SET_STRING_ELT(entity_class_rvector, 0, Rf_mkChar("redatam.entity"));
  SET_STRING_ELT(entity_class_rvector, 1, Rf_mkChar("data.frame"));

  sexp variable_int_class_rvector = Rf_mkString("redatam.variable.int");
  sexp variable_lng_class_rvector = Rf_mkString("redatam.variable.lng");
  sexp variable_chr_class_rvector = Rf_mkString("redatam.variable.chr");
  sexp variable_bin_class_rvector = Rf_mkString("redatam.variable.bin");
  sexp variable_dbl_class_rvector = Rf_mkString("redatam.variable.dbl");
  sexp variable_pck_class_rvector = Rf_mkString("redatam.variable.pck");

  sexp instance_len_rsymbol = Rf_mkString("instance.len");
  sexp instance_num_rsymbol = Rf_mkString("instance.num");
  sexp data_path_rsymbol = Rf_mkString("data.path");

  sexp ans = Rf_allocVector(VECSXP, root_entity.num_entities - 1);
  sexp ans_names = Rf_allocVector(STRSXP, root_entity.num_entities - 1);
  Rf_setAttrib(ans, description_rsymbol, serialize_to_R(dic));
  Rf_classgets(ans, Rf_mkString("redatam.database"));

  for (int entity_count = 1; entity_count < root_entity.num_entities;
       ++entity_count) {
    auto entity = EntityDescriptor::fread(dic_file, false);
    SET_STRING_ELT(ans_names, entity_count - 1,
                   Rf_mkChar(entity.name1.c_str()));

    uint32_t num_instances = 0;
    if (!entity.ptr_path.empty()) {
      auto ptr_real_basename =
          locate_icase(dic_path.dir().as_string(), entity.ptr_path.basename());
      if (!ptr_real_basename.empty()) {
        auto ptr_real_path = dic_path.dir() + ptr_real_basename;
        std::fstream ptr_file(ptr_real_path.as_string(),
                              std::ios_base::in | std::ios_base::binary);
        ptr_file.seekg(-sizeof(uint32_t), std::ios_base::end);
        num_instances = fread_uint32_t(ptr_file);
        ptr_file.seekg(0, std::ios_base::beg);

        sexp current_entity = Rf_allocVector(VECSXP, entity.num_vars + 1);
        sexp current_entity_names = Rf_allocVector(STRSXP, entity.num_vars + 1);
        sexp column0 = Rf_allocVector(INTSXP, num_instances);

        uint32_t prev_n;
        uint32_t next_n;
        int parent_id = 0;
        prev_n = fread_uint32_t(ptr_file); // always reads 0 at offset 0
        while (next_n = fread_uint32_t(ptr_file), ptr_file) {
          ++parent_id;
          for (uint32_t k = prev_n; k < next_n; ++k) {
            SET_INTEGER_ELT(column0, k, parent_id);
          }
          prev_n = next_n;
        }
        Rf_setAttrib(column0, description_rsymbol,
                     Rf_mkString(("Parent instance row in " + entity.parent +
                                  "(surrogate attribute)\n")
                                     .c_str()));
        SET_VECTOR_ELT(current_entity, 0, column0);
        SET_STRING_ELT(current_entity_names, 0,
                       Rf_mkChar((entity.parent + "_id").c_str()));

        // compact row names representation (see .set_row_names and
        // .row_names_info)
        sexp rownames = Rf_allocVector(INTSXP, 2);
        SET_INTEGER_ELT(rownames, 0, R_NaInt);
        SET_INTEGER_ELT(rownames, 1, -num_instances);
        Rf_setAttrib(current_entity, R_RowNamesSymbol, rownames);

        Rf_classgets(current_entity, entity_class_rvector);
        Rf_namesgets(current_entity, current_entity_names);

        Rf_setAttrib(current_entity, description_rsymbol,
                     serialize_to_R(entity));
        SET_VECTOR_ELT(ans, entity_count - 1, current_entity);

        for (size_t i = 0; i < entity.num_vars; ++i) {
          sexp column = Rf_allocVector(INTSXP, 0);
          VariableDescriptor var = VariableDescriptor::fread(dic_file);
          if (var.declaration) {
            auto rbf_real_basename =
                locate_icase(dic_path.dir().as_string(),
                             var.declaration->rbf_path.basename());
            if (!rbf_real_basename.empty()) {
              auto rbf_real_path = dic_path.dir() + rbf_real_basename;
              Rf_setAttrib(column, instance_len_rsymbol,
                           Rf_ScalarInteger(var.declaration->size));
              Rf_setAttrib(column, instance_num_rsymbol,
                           Rf_ScalarInteger(num_instances));
              Rf_setAttrib(column, data_path_rsymbol,
                           Rf_mkString(rbf_real_path.as_string().c_str()));

              switch (var.declaration->type) {
              case VariableDescriptor::Declaration::Type::DBL:
                Rf_classgets(column, variable_dbl_class_rvector);
                break;
              case VariableDescriptor::Declaration::Type::CHR:
                Rf_classgets(column, variable_chr_class_rvector);
                break;
              case VariableDescriptor::Declaration::Type::INT:
                Rf_classgets(column, variable_int_class_rvector);
                break;
              case VariableDescriptor::Declaration::Type::LNG:
                Rf_classgets(column, variable_lng_class_rvector);
                break;
              case VariableDescriptor::Declaration::Type::BIN:
                Rf_classgets(column, variable_bin_class_rvector);
                break;
              case VariableDescriptor::Declaration::Type::PCK: {
                Rf_classgets(column, variable_pck_class_rvector);
                break;
              }
              default:
                break;
              }
            }
          }
          SET_STRING_ELT(current_entity_names, i + 1,
                         Rf_mkChar(var.name.c_str()));
          SET_VECTOR_ELT(current_entity, i + 1, column);

          if (var.labels.size() > 0 and
              (var.declaration->type ==
                   VariableDescriptor::Declaration::Type::LNG or
               var.declaration->type ==
                   VariableDescriptor::Declaration::Type::INT or
               var.declaration->type ==
                   VariableDescriptor::Declaration::Type::PCK or
               var.declaration->type ==
                   VariableDescriptor::Declaration::Type::BIN)) {
            std::vector<std::pair<int, std::string>> levels = var.labels;
            if (var.descriptor.missing)
              levels.emplace_back(*var.descriptor.missing, "<MISS>");
            if (var.descriptor.not_applicable)
              levels.emplace_back(*var.descriptor.not_applicable, "<NOAP>");
            std::sort(levels.begin(), levels.end());
            sexp levels_rvector = Rf_allocVector(STRSXP, levels.size());
            sexp levelkeys_rvector = Rf_allocVector(INTSXP, levels.size());
            for (int i = 0; i < static_cast<int>(levels.size()); ++i) {
              SET_STRING_ELT(levels_rvector, i,
                             Rf_mkChar(levels[i].second.c_str()));
              SET_INTEGER_ELT(levelkeys_rvector, i, levels[i].first);
            }
            Rf_setAttrib(column, R_LevelsSymbol, levels_rvector);
            Rf_setAttrib(column, levelkeys_rsymbol, levelkeys_rvector);
          }
          Rf_setAttrib(column, description_rsymbol, serialize_to_R(var));
        }
      }
    }
  }

  Rf_namesgets(ans, ans_names);
  return ans;
}
