#pragma once

#include <array>
#include <fstream>
#include <iostream>
#include <string>

#include "primitives.hpp"

namespace Redatam {

class DictionaryDescriptor {
public:
  uint32_t unknown1;
  std::string name;
  std::array<char, 8> creation_date;
  std::array<char, 8> modification_date;
  std::string root_dir;
  std::string unknown2;

  static DictionaryDescriptor fread(std::istream &stream) {
    DictionaryDescriptor d;
    d.unknown1 = fread_uint32_t(stream); // always 02 05 00 00?
    stream.seekg(
        1029,
        std::ios_base::cur); // most of this is probably uninitialized data
    d.name = fread_string(stream);

    for (auto &c : d.creation_date)
      c = stream.get();
    for (auto &c : d.modification_date)
      c = stream.get();

    d.root_dir = fread_string(stream);

    d.unknown2 = fread_string(stream);
    return d;
  }

  friend std::ostream &operator<<(std::ostream &stream,
                                  const DictionaryDescriptor &d) {
    return stream << "UNK1: " << d.unknown1 << "\nName: " << d.name
                  << "\nCreation date: "
                  << std::string(d.creation_date.begin(), d.creation_date.end())
                  << "\nModification date: "
                  << std::string(d.modification_date.begin(),
                                 d.modification_date.end())
                  << "\nRoot dir: " << d.root_dir << std::endl;
  }
};

} // namespace Redatam
