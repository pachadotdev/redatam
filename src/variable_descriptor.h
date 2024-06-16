#pragma once

#include <array>
#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>
#include <optional>
#include <regex>
#include <string>
#include <vector>

#include "primitives.h"
#include "utils.h"

namespace Redatam {

class VariableDescriptor {
public:
  class Declaration {
  public:
    enum class Type { BIN, CHR, DBL, INT, LNG, PCK };
    static std::optional<Declaration>
    fromDeclarationString(const std::string &declstr) {
      std::regex re("^(\\S+)\\s+(\\d+)$");
      std::smatch match;
      if (std::regex_search(declstr, match, re)) {
        Declaration d;
        d.rbf_path = match[1].str();
        d.size = std::stoul(match[2].str());
        return d;
      }
      return std::nullopt;
    }
    Type type;
    boost::filesystem::path rbf_path;
    size_t size;
  };

  class Descriptor {
  public:
    std::string alias;
    size_t decimals = 0;
    std::string group;
    std::optional<int> missing;
    std::optional<int> not_applicable;
  };

  std::string name;
  std::optional<Declaration> declaration;
  std::string filter;
  std::string range;
  std::string datatype;
  std::vector<std::pair<int, std::string>> labels;
  std::string description;
  Descriptor descriptor;

  uint16_t unknown1;
  std::string documentation;
  uint16_t id;
  std::array<unsigned char, 16> unknown;

  // calculated fields
  boost::filesystem::path real_rbf_path;
  template <typename First, typename Last>
  bool resolve_rbf_data(First first, Last last) {
    if (!declaration or declaration->rbf_path.empty())
      return false;

    real_rbf_path = declaration->rbf_path;
    while (!exists(real_rbf_path) && first != last) {
      real_rbf_path = locate_icase(*first / declaration->rbf_path.filename());
      ++first;
    }

    return exists(real_rbf_path);
  }

  static int32_t fread_int32_t(std::istream &stream) {
    int32_t value;
    stream.read(reinterpret_cast<char *>(&value), sizeof(value));
    return value;
  }

  static std::optional<int32_t> fread_optional_int32_t(std::istream &stream) {
    int32_t value;
    stream.read(reinterpret_cast<char *>(&value), sizeof(value));
    if (stream)
      return value;
    else
      return std::nullopt;
  }

  static VariableDescriptor fread(std::istream &stream) {
    VariableDescriptor d;
    d.name = fread_string(stream);
    d.declaration = Declaration::fromDeclarationString(fread_string(stream));

    d.filter = fread_string(stream);
    d.range = fread_string(stream);
    d.datatype = fread_string(stream);

    uint32_t num_labels = fread_uint32_t(stream);
    for (uint32_t i = 0; i < num_labels; ++i) {
      int value = fread_int32_t(stream);
      std::string label = fread_string(stream);
      d.labels.emplace_back(value, label);
    }

    d.description = fread_string(stream);
    d.descriptor.alias = fread_string(stream);
    d.descriptor.decimals = fread_uint32_t(stream);
    d.descriptor.group = fread_string(stream);
    d.descriptor.missing = fread_optional_int32_t(stream);
    d.descriptor.not_applicable = fread_optional_int32_t(stream);
    d.unknown1 = fread_uint16_t(stream);
    d.documentation = fread_string(stream);
    d.id = fread_uint16_t(stream);
    for (auto &c : d.unknown)
      c = stream.get();
    return d;
  }

  friend std::ostream &operator<<(std::ostream &stream,
                                  const VariableDescriptor::Declaration &d) {
    stream << d.rbf_path << " ";
    switch (d.type) {
    case VariableDescriptor::Declaration::Type::BIN:
      stream << "BIN";
      break;
    case VariableDescriptor::Declaration::Type::CHR:
      stream << "CHR";
      break;
    case VariableDescriptor::Declaration::Type::DBL:
      stream << "DBL";
      break;
    case VariableDescriptor::Declaration::Type::INT:
      stream << "INT";
      break;
    case VariableDescriptor::Declaration::Type::LNG:
      stream << "LNG";
      break;
    case VariableDescriptor::Declaration::Type::PCK:
      stream << "PCK";
      break;
    default:
      stream << "???";
    }
    return stream << " (" << d.size << ")";
  }

  friend std::ostream &operator<<(std::ostream &stream,
                                  const VariableDescriptor::Descriptor &d) {
    if (!d.alias.empty())
      stream << "ALIAS " << d.alias << " ";
    stream << "DECIMALS " << d.decimals << " ";
    if (!d.group.empty())
      stream << "GROUP " << d.group << " ";
    if (d.missing)
      stream << " MISSING " << (*d.missing) << " ";
    if (d.not_applicable)
      stream << " NOTAPPLICABLE " << (*d.not_applicable);
    return stream;
  }

  friend std::ostream &operator<<(std::ostream &stream,
                                  const VariableDescriptor &d) {
    stream << "Name: " << d.name << "\nDeclaration: ";
    if (d.declaration)
      stream << *d.declaration;
    else
      stream << "NULL";
    stream << "\nFilter: " << d.filter << "\nRange: " << d.range
           << "\nDatatype: " << d.datatype << "\nLabels: ";
    for (const auto &l : d.labels)
      stream << l.second << ", ";
    return stream << "\nDescriptor: " << d.descriptor
                  << "\nDescription: " << d.description
                  << "\nUNK1: " << d.unknown1
                  << "\nDocumentation: " << d.documentation << "\nID: " << d.id
                  << "\nUNK: " << d.unknown;
  }
};

} // namespace Redatam
