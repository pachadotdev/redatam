#ifndef _REDATAM_ENTITY_DESCRIPTOR_H_
#define _REDATAM_ENTITY_DESCRIPTOR_H_

#include <array>
#include <string>

#include "util.h"

class EntityDescriptor {
public:
  std::string name1;
  std::string name2;
  std::string parent;
  std::string description;
  std::string id_varname;
  std::string name_varname;
  std::string documentation;
  std::string label;
  std::string weight_varname;

  uint16_t unknown1;
  uint16_t num_entities;
  uint32_t num_vars;
  uint16_t unknown3;
  uint16_t is_selectable;
  uint16_t is_sensible;
  uint16_t sensible_threshold;

  std::array<unsigned char, 3> unknown2;
  std::array<unsigned char, 16> unknown6;

  DOSPath ptr_path;

  static EntityDescriptor fread(std::istream &stream, bool is_root);
};

std::ostream &operator<<(std::ostream &stream, const EntityDescriptor &d);

#endif
