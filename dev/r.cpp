#include <Rcpp.h>
#include <redatam/dic/dictionary_descriptor.h>
#include <redatam/dic/entity_descriptor.h>
#include <redatam/dic/variable_descriptor.h>
#include <redatam/fs.h>
#include <redatam/read_dic.h>
#include <redatam/read_ptr.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace Rcpp;

template <typename T> RObject stream_to_R(const T &obj) {
  std::stringstream ss;
  ss << obj << '\n';
  return wrap(ss.str());
}

// redatam4r_read_redatam_ function
RObject redatam4r_read_redatam_(RObject dic_path_in) {
  Redatam::fs::path dic_path{as<std::string>(dic_path_in)};

  RObject description_rsymbol = wrap("description");
  RObject levelkeys_rsymbol = wrap("levelkeys");

  RObject entity_class_rvector =
      CharacterVector::create("redatam.entity", "data.frame");

  RObject variable_int_class_rvector = wrap("redatam.variable.int");
  RObject variable_lng_class_rvector = wrap("redatam.variable.lng");
  RObject variable_chr_class_rvector = wrap("redatam.variable.chr");
  RObject variable_bin_class_rvector = wrap("redatam.variable.bin");
  RObject variable_dbl_class_rvector = wrap("redatam.variable.dbl");
  RObject variable_pck_class_rvector = wrap("redatam.variable.pck");

  RObject instance_len_rsymbol = wrap("instance.len");
  RObject instance_num_rsymbol = wrap("instance.num");
  RObject data_path_rsymbol = wrap("data.path");

  RObject ans, ans_names;
  RObject current_entity, current_entity_names;

  Redatam::read_dic(
      dic_path,
      // dic + root entity callback
      [&](Redatam::DictionaryDescriptor &dic,
          Redatam::EntityDescriptor &root_entity) {
        ans = RObject::create(Named("description") = stream_to_R(dic));
        ans.attr("class") = "redatam.database";
        ans_names = CharacterVector(root_entity.num_entities - 1);
        for (size_t i = 0; i < root_entity.num_entities - 1; ++i) {
          ans_names[i] = root_entity.entities[i + 1].name1;
        }
        ans.attr("names") = ans_names;
      },
      // entity callback
      [&](Redatam::EntityDescriptor &entity, size_t i) {
        ans_names[i - 1] = entity.name1;

        current_entity = RObject::create(
            Named(entity.parent + "_id") = IntegerVector(entity.num_instances));
        current_entity_names = CharacterVector(entity.num_vars + 1);

        IntegerVector column0 = current_entity[entity.parent + "_id"];
        Redatam::read_ptr(entity.real_ptr_path, column0.begin());

        std::string column0_description = "Parent instance row in " +
                                          entity.parent +
                                          "(surrogate attribute)\n";
        column0.attr("description") = stream_to_R(column0_description);

        // compact row names representation (see .set_row_names and
        // .row_names_info)
        IntegerVector current_entity_rownames =
            IntegerVector::create(R_NaInt, -entity.num_instances);
        current_entity.attr("row.names") = current_entity_rownames;

        current_entity.attr("class") = entity_class_rvector;
        current_entity.attr("names") = current_entity_names;
        current_entity.attr("description") = stream_to_R(entity);

        ans[i - 1] = current_entity;
      },
      // variable callback
      [&](Redatam::VariableDescriptor &variable,
          Redatam::EntityDescriptor &entity, size_t i) {
        // create a column vector for this variable of the appropiate type and
        // class
        RObject column;
        switch (variable.declaration->type) {
        case Redatam::VariableDescriptor::Declaration::Type::DBL:
          column = NumericVector(0);
          column.attr("class") = variable_dbl_class_rvector;
          break;
        case Redatam::VariableDescriptor::Declaration::Type::CHR:
          column = CharacterVector(0);
          column.attr("class") = variable_chr_class_rvector;
          break;
        case Redatam::VariableDescriptor::Declaration::Type::INT:
          column = IntegerVector(0);
          column.attr("class") = variable_int_class_rvector;
          break;
        case Redatam::VariableDescriptor::Declaration::Type::LNG:
          column = IntegerVector(0);
          column.attr("class") = variable_lng_class_rvector;
          break;
        case Redatam::VariableDescriptor::Declaration::Type::BIN:
          column = IntegerVector(0);
          column.attr("class") = variable_bin_class_rvector;
          break;
        case Redatam::VariableDescriptor::Declaration::Type::PCK:
          column = IntegerVector(0);
          column.attr("class") = variable_pck_class_rvector;
          break;
        }

        // fill data needed for indexing: datatype size, number of instances and
        // rbf file path
        column.attr("instance.len") = variable.declaration->size;
        column.attr("instance.num") = entity.num_instances;
        column.attr("data.path") = variable.real_rbf_path;

        current_entity_names[i + 1] = variable.name;
        current_entity[i + 1] = column;
        column.attr("description") = stream_to_R(variable);

        // fill data needed to convert to factor: levels and labels
        if (variable.labels.size() > 0 && column.sexp_type() == INTSXP) {
          auto levels = variable.labels;

          if (variable.descriptor.missing)
            levels.emplace_back(*variable.descriptor.missing, "<MISS>");

          if (variable.descriptor.not_applicable)
            levels.emplace_back(*variable.descriptor.not_applicable, "<NOAP>");

          std::sort(levels.begin(), levels.end());

          CharacterVector labels_rvector(levels.size());
          IntegerVector levels_rvector(levels.size());
          for (size_t i = 0; i < levels.size(); ++i) {
            labels_rvector[i] = levels[i].second;
            levels_rvector[i] = levels[i].first;
          }
          column.attr("levels") = labels_rvector;
          column.attr("levelkeys") = levels_rvector;
        }
      });

  return ans;
}

// Expose the function to R
RCPP_MODULE(redatam4r) { function("read_redatam", &redatam4r_read_redatam_); }
