#ifndef ENTITY_DESCRIPTOR_H
#define ENTITY_DESCRIPTOR_H

#include <string>
#include <vector>

#include "variable_descriptor.h"

namespace Redatam {

class EntityDescriptor {
 public:
  std::string name1;
  std::string name2;
  std::string parent;
  std::string description;
  std::string ptr_path;
  uint16_t unknown1;
  std::string id_varname;
  std::string name_varname;
  std::string documentation;
  uint16_t num_entities;
  std::array<char, 2> unknown2;
  uint16_t num_vars;
  uint16_t unknown3;
  uint16_t is_selectable;
  std::array<char, 6> unknown6;
  std::string weight_varname;
  std::string label;
  uint16_t is_sensible;
  uint16_t sensible_threshold;
  std::vector<VariableDescriptor> variables;

  static EntityDescriptor fread(std::istream& stream, bool is_root);
};

}  // namespace Redatam

#endif  // ENTITY_DESCRIPTOR_H
