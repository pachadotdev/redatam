#include "dictionary_descriptor.h"
#include "entity_descriptor.h"
#include "read_dic.h"
#include "read_ptr.h"
#include "variable_descriptor.h"
#include <boost/filesystem.hpp>
#include <cpp11.hpp>

using namespace cpp11;

template <typename T> strings stream_to_R(const T &obj) {
  std::stringstream ss;
  ss << obj << '\n';
  std::vector<std::string> result = {ss.str()};
  return strings(result);
}

[[cpp11::register]] list read_redatam_(std::string dic_path_in) {
  boost::filesystem::path dic_path{dic_path_in};

  std::string description_symbol = "description";
  std::string levelkeys_symbol = "levelkeys";

  writable::strings entity_class_rvector = {"redatam.entity", "data.frame"};

  writable::strings variable_int_class_rvector = {"redatam.variable.int"};
  writable::strings variable_lng_class_rvector = {"redatam.variable.lng"};
  writable::strings variable_chr_class_rvector = {"redatam.variable.chr"};
  writable::strings variable_bin_class_rvector = {"redatam.variable.bin"};
  writable::strings variable_dbl_class_rvector = {"redatam.variable.dbl"};
  writable::strings variable_pck_class_rvector = {"redatam.variable.pck"};

  std::string instance_len_symbol = "instance.len";
  std::string instance_num_symbol = "instance.num";

  std::string data_path_symbol = "data.path";

  writable::list ans, current_entity;
  writable::strings current_entity_names;

  Redatam::read_dic(
      dic_path,
      // dic + root entity callback
      [&](Redatam::DictionaryDescriptor &dic,
          Redatam::EntityDescriptor &root_entity) {
        writable::list ans(root_entity.num_entities);
        ans.attr("names") = writable::strings(root_entity.num_entities);
        ans.attr("class") = "redatam.database";
        ans.attr(description_symbol) = stream_to_R(dic);
      },
      // entity callback
      [&](Redatam::EntityDescriptor &entity, size_t i) {
        writable::list current_entity(entity.num_vars + 1);
        writable::strings current_entity_names(entity.num_vars + 1);

        writable::integers column0(entity.num_instances);
        Redatam::read_ptr(entity.real_ptr_path, INTEGER(column0));

        std::string column0_description = "Parent instance row in " +
                                          entity.parent +
                                          "(surrogate attribute)\n";
        column0.attr(description_symbol) = column0_description;
        current_entity[0] = column0;
        current_entity_names[0] = entity.parent + "_id";

        writable::integers current_entity_rownames(2);
        current_entity_rownames[0] = NA_INTEGER;
        current_entity_rownames[1] = -entity.num_instances;
        current_entity.attr("row.names") = current_entity_rownames;
        current_entity.attr("class") = entity_class_rvector;
        current_entity.attr("names") = current_entity_names;
        current_entity.attr(description_symbol) = stream_to_R(entity);
        ans[i - 1] = current_entity;
      },
      // variable callback
      [&](Redatam::VariableDescriptor &variable,
          Redatam::EntityDescriptor &entity, size_t i) {
        SEXP column;
        writable::strings column_class;
        switch (variable.declaration->type) {
        case Redatam::VariableDescriptor::Declaration::Type::DBL:
          column = PROTECT(Rf_allocVector(REALSXP, 0));
          column_class = variable_dbl_class_rvector;
          break;
        case Redatam::VariableDescriptor::Declaration::Type::CHR:
          column = PROTECT(Rf_allocVector(STRSXP, 0));
          column_class = variable_chr_class_rvector;
          break;
        case Redatam::VariableDescriptor::Declaration::Type::INT:
          column = PROTECT(Rf_allocVector(INTSXP, 0));
          column_class = variable_int_class_rvector;
          break;
        case Redatam::VariableDescriptor::Declaration::Type::LNG:
          column = PROTECT(Rf_allocVector(INTSXP, 0));
          column_class = variable_lng_class_rvector;
          break;
        case Redatam::VariableDescriptor::Declaration::Type::BIN:
          column = PROTECT(Rf_allocVector(INTSXP, 0));
          column_class = variable_bin_class_rvector;
          break;
        case Redatam::VariableDescriptor::Declaration::Type::PCK:
          column = PROTECT(Rf_allocVector(INTSXP, 0));
          column_class = variable_pck_class_rvector;
          break;
        }
        Rf_setAttrib(column, Rf_install("class"), column_class);

        Rf_setAttrib(column, Rf_install(instance_len_symbol.c_str()),
                     Rf_ScalarInteger(variable.declaration->size));
        Rf_setAttrib(column, Rf_install(instance_num_symbol.c_str()),
                     Rf_ScalarInteger(entity.num_instances));
        Rf_setAttrib(column, Rf_install(data_path_symbol.c_str()),
                     Rf_mkString(variable.real_rbf_path.string().c_str()));

        current_entity_names[i + 1] = variable.name;
        SET_VECTOR_ELT(current_entity, i + 1, column);
        Rf_setAttrib(column, Rf_install(description_symbol.c_str()),
                     stream_to_R(variable));

        // fill data needed to convert to factor: levels and labels
        if (variable.labels.size() > 0 && TYPEOF(column) == INTSXP) {
          auto levels = variable.labels;

          if (variable.descriptor.missing)
            levels.emplace_back(*variable.descriptor.missing, "<MISS>");

          if (variable.descriptor.not_applicable)
            levels.emplace_back(*variable.descriptor.not_applicable, "<NOAP>");

          std::sort(levels.begin(), levels.end());

          writable::strings labels_rvector(levels.size());
          writable::integers levels_rvector(levels.size());
          for (size_t j = 0; j < levels.size(); ++j) {
            labels_rvector[j] = levels[j].second;
            levels_rvector[j] = levels[j].first;
          }
          Rf_setAttrib(column, Rf_install("levels"), labels_rvector);
          Rf_setAttrib(column, Rf_install(levelkeys_symbol.c_str()),
                       levels_rvector);
        }
        UNPROTECT(1);
      });
  return ans;
}
