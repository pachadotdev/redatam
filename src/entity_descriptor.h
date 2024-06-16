#pragma once

#include <array>
#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>
#include <string>

#include "primitives.h"
#include "utils.h"

namespace Redatam {

class EntityDescriptor {
public:
  std::string name1;
  std::string name2;
  std::string parent;
  std::string description;
  boost::filesystem::path ptr_path;

  uint16_t unknown1;

  std::string id_varname;
  std::string name_varname;
  std::string documentation;

  uint16_t num_entities;
  std::array<unsigned char, 3> unknown2;

  uint32_t num_vars;

  uint16_t unknown3;
  uint16_t is_selectable;
  std::array<unsigned char, 16> unknown6;

  std::string label;

  uint16_t is_sensible;
  uint16_t sensible_threshold;

  std::string weight_varname;

  // calculated fields
  boost::filesystem::path real_ptr_path;
  size_t num_instances;

  // looks for ptr file in the provided range of directories
  template <typename First, typename Last>
  bool resolve_ptr_data(First first, Last last) {
    if (ptr_path.empty())
      return false;

    real_ptr_path = ptr_path;
    while (!exists(real_ptr_path) && first != last) {
      real_ptr_path = locate_icase(*first / ptr_path.filename());
      ++first;
    }
    if (!exists(real_ptr_path))
      return false;

    std::fstream ptr_file(real_ptr_path.c_str(),
                          std::ios_base::in | std::ios_base::binary);
    if (!ptr_file)
      return false;

    ptr_file.seekg(-sizeof(uint32_t), std::ios_base::end);
    num_instances = fread_uint32_t(ptr_file);
    return true;
  }

  static EntityDescriptor fread(std::istream &stream, bool is_root) {
    EntityDescriptor d;
    for (; d.name1.empty(); d.name1 = fread_string(stream)) {
    }

    if (!is_root)
      d.name2 = fread_string(stream);

    d.parent = fread_string(stream);
    d.description = fread_string(stream);
    d.ptr_path = fread_path(stream);

    d.unknown1 = fread_uint16_t(stream);
    d.id_varname = fread_string(stream);
    d.name_varname = fread_string(stream);
    d.documentation = fread_string(stream);
    d.num_entities = fread_uint16_t(stream);
    for (auto &c : d.unknown2)
      c = stream.get();
    d.num_vars = fread_uint16_t(stream);
    d.unknown3 = fread_uint16_t(stream);
    d.is_selectable = fread_uint16_t(stream);
    for (auto &c : d.unknown6)
      c = stream.get();
    d.weight_varname = fread_string(stream);
    d.label = fread_string(stream);
    d.is_sensible = fread_uint16_t(stream);
    d.sensible_threshold = fread_uint16_t(stream);

    return d;
  }

  friend std::ostream &operator<<(std::ostream &stream,
                                  const EntityDescriptor &d) {
    return stream << "Name1: " << d.name1 << "\nName2: " << d.name2
                  << "\nParent: " << d.parent
                  << "\nDescription: " << d.description
                  << "\nPTR file: " << d.ptr_path
                  << "\nVar ID: " << d.id_varname
                  << "\nVar Name: " << d.name_varname
                  << "\nDocumentation: " << d.documentation
                  << "\nNum Vars: " << d.num_vars
                  << "\nWeighting variable: " << d.weight_varname
                  << "\nLabel: " << d.label
                  << "\nSelectable: " << (d.is_selectable ? "Yes" : "No")
                  << "\nUNK2: " << d.unknown2 << "\nUNK6: " << d.unknown6;
  }
};

} // namespace Redatam
