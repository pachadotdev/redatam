#include "dictionary_descriptor.h"

#include <fstream>
#include <iostream>
#include <string>

#include "entity_descriptor.h"
#include "primitives.h"

namespace Redatam {

DictionaryDescriptor DictionaryDescriptor::fread(std::istream& stream) {
  DictionaryDescriptor d;

  d.unknown1 = fread_uint32_t(stream);
  std::cout << "Read unknown1: " << d.unknown1 << std::endl;

  stream.seekg(1029, std::ios_base::cur);
  std::cout << "Skipped 1029 bytes" << std::endl;

  d.name = fread_string(stream);
  std::cout << "Read name: " << d.name << std::endl;

  for (auto& c : d.creation_date) c = stream.get();
  std::cout << "Read creation_date: "
            << std::string(d.creation_date.begin(), d.creation_date.end())
            << std::endl;

  for (auto& c : d.modification_date) c = stream.get();
  std::cout << "Read modification_date: "
            << std::string(d.modification_date.begin(),
                           d.modification_date.end())
            << std::endl;

  d.root_dir = fread_string(stream);
  std::cout << "Read root_dir: " << d.root_dir << std::endl;

  d.unknown2 = fread_string(stream);
  std::cout << "Read unknown2: " << d.unknown2 << std::endl;

  std::cout << "Reading entities..." << std::endl;

  // Read entities
  while (stream.peek() != std::istream::traits_type::eof()) {
    std::cout << "Reading an entity..." << std::endl;
    EntityDescriptor entity = EntityDescriptor::fread(stream, false);
    std::cout << "Entity name: " << entity.name1
              << ", Variables: " << entity.variables.size() << std::endl;
    d.entities.push_back(entity);
  }

  std::cout << "Total entities read: " << d.entities.size() << std::endl;
  return d;
}

std::ostream& operator<<(std::ostream& stream, const DictionaryDescriptor& d) {
  return stream << "UNK1: " << d.unknown1 << "\nName: " << d.name
                << "\nCreation date: "
                << std::string(d.creation_date.begin(), d.creation_date.end())
                << "\nModification date: "
                << std::string(d.creation_date.begin(),
                               d.modification_date.end())
                << "\nRoot dir: " << d.root_dir << std::endl;
}

}  // namespace Redatam
