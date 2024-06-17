#pragma once

#include <algorithm>
#include <fstream>
// #include <iostream>
#include <string>
#include <vector>

#include "dictionary_descriptor.h"
#include "entity_descriptor.h"
#include "primitives.h"
#include "redatam_exception.h"
#include "variable_descriptor.h"

namespace Redatam {

template <typename F, typename G, typename H>
void read_dic(const boost::filesystem::path &dic_path, F &&dictionary_cb,
              G &&entity_cb, H &&variable_cb) {
  std::fstream dic_file(dic_path, std::ios_base::in | std::ios_base::binary);

  if (!dic_file) {
    // std::cerr << "Could not read the dic file: " << dic_path.string() << std::endl;
    throw Exception(std::string("Could not read the dic file: ") +
                    dic_path.string());
  }

  auto dic = DictionaryDescriptor::fread(dic_file);
  auto root_entity = EntityDescriptor::fread(dic_file, true);
  dictionary_cb(dic, root_entity);

  std::vector<boost::filesystem::path> search_paths;
  search_paths.push_back(dic_path.parent_path());

  for (size_t entity_count = 1; entity_count < root_entity.num_entities;
       ++entity_count) {
    EntityDescriptor entity = EntityDescriptor::fread(dic_file, false);
    // std::cerr << "Reading entity: " << entity_count
    //           << " with ptr path: " << entity.ptr_path.string() << std::endl;
    if (!entity.resolve_ptr_data(begin(search_paths), end(search_paths))) {
      // std::cerr << "Could not read ptr data file: "
      //   << entity.ptr_path.string()
      //   << std::endl;
      throw Exception(std::string("Could not read ptr data file: ") +
                      entity.ptr_path.string());
    }

    entity_cb(entity, entity_count);

    for (size_t var_id = 0; var_id < entity.num_vars; ++var_id) {
      VariableDescriptor variable = VariableDescriptor::fread(dic_file);
      // std::cerr << "Reading variable: " << var_id
      //           << " with rbf path: " << variable.real_rbf_path.string()
      //           << std::endl;
      if (!variable.resolve_rbf_data(begin(search_paths), end(search_paths))) {
        // std::cerr << "Could not read rbf data file: "
        //           << variable.real_rbf_path.string() << std::endl;
        throw Exception(std::string("Could not read rbf data file: ") +
                        variable.real_rbf_path.string());
      }

      variable_cb(variable, entity, var_id);
    }
  }
}

}  // namespace Redatam
