#include <fstream>
#include <string>

#include "primitives.h"
#include "dictionary_descriptor.h"

DictionaryDescriptor DictionaryDescriptor::fread(std::istream &stream) {
  DictionaryDescriptor d;
  // always 02 05 00 00?
  d.unknown1 = fread_uint32_t(stream);
  // most of this is probably uninitialized data
  stream.seekg(1029, std::ios_base::cur);
  d.name = fread_string(stream);

  stream.read(reinterpret_cast<char*>(d.creation_date.data()), d.creation_date.size());
  stream.read(reinterpret_cast<char*>(d.modification_date.data()), d.modification_date.size());

  d.root_dir = fread_string(stream);
  d.unknown2 = fread_string(stream);
  return d;
}

std::ostream &operator<<(std::ostream &stream, const DictionaryDescriptor &d) {
  return stream << "UNK1: " << d.unknown1 << "\nName: " << d.name
                << "\nCreation date: " << d.creation_date.data()
                << "\nModification date: " << d.modification_date.data()
                << "\nRoot dir: " << d.root_dir << std::endl;
}
