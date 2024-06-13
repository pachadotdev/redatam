#ifndef DICTIONARY_DESCRIPTOR_H
#define DICTIONARY_DESCRIPTOR_H

#include <string>
#include <vector>

#include "entity_descriptor.h"

namespace Redatam {

class DictionaryDescriptor {
 public:
  uint32_t unknown1;
  std::string name;
  std::array<char, 10> creation_date;
  std::array<char, 10> modification_date;
  std::string root_dir;
  std::string unknown2;
  std::vector<EntityDescriptor> entities;

  static DictionaryDescriptor fread(std::istream& stream);
};

}  // namespace Redatam

#endif  // DICTIONARY_DESCRIPTOR_H
