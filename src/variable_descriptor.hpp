#pragma once

#include <array>
#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include "primitives.hpp"
#include "utils.hpp"

namespace Redatam {

class VariableDescriptor {
 public:
  class Declaration {
   public:
    enum class Type { BIN, CHR, DBL, INT, LNG, PCK };
    static std::optional<Declaration> fromDeclarationString(
        const std::string &declstr) {
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
    std::string alias, group;
    size_t decimals = 0;
    std::optional<int> missing, not_applicable;
  };

  std::string name, filter, range, datatype, description;
  std::optional<Declaration> declaration;
  std::vector<std::pair<int, std::string>> labels;
  Descriptor descriptor;

  uint16_t unknown1;
  std::string documentation;
  uint16_t id;
  std::array<unsigned char, 16> unknown;

  // calculated fields
  boost::filesystem::path real_rbf_path;

  template <typename First, typename Last>
  bool resolve_rbf_data(First first, Last last) {
    if (!declaration or declaration->rbf_path.empty()) return false;

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
    for (auto &c : d.unknown) c = stream.get();
    return d;
  }

  friend std::ostream &operator<<(std::ostream &stream,
                                  const VariableDescriptor::Declaration &d) {
    static const std::map<Declaration::Type, std::string> typeToStr = {
        {Declaration::Type::BIN, "BIN"}, {Declaration::Type::CHR, "CHR"},
        {Declaration::Type::DBL, "DBL"}, {Declaration::Type::INT, "INT"},
        {Declaration::Type::LNG, "LNG"}, {Declaration::Type::PCK, "PCK"}};
    return stream << d.rbf_path << " " << typeToStr.at(d.type) << " (" << d.size
                  << ")";
  }

  friend std::ostream &operator<<(std::ostream &stream,
                                  const VariableDescriptor::Descriptor &desc) {
    return stream << "Alias: " << desc.alias << ", Group: " << desc.group
                  << ", Decimals: " << desc.decimals << ", Missing: "
                  << (desc.missing ? std::to_string(*desc.missing) : "N/A")
                  << ", Not Applicable: "
                  << (desc.not_applicable ? std::to_string(*desc.not_applicable)
                                          : "N/A");
  }

  friend std::ostream &operator<<(std::ostream &stream,
                                  const VariableDescriptor &d) {
    std::stringstream ss;
    ss << "Name: " << d.name << "\nDeclaration: ";
    if (d.declaration)
      ss << *d.declaration;
    else
      ss << "NULL";
    ss << "\nFilter: " << d.filter << "\nRange: " << d.range
       << "\nDatatype: " << d.datatype << "\nLabels: ";
    for (const auto &l : d.labels) ss << l.second << ", ";
    ss << "\nDescriptor: " << d.descriptor << "\nDescription: " << d.description
       << "\nUNK1: " << d.unknown1 << "\nDocumentation: " << d.documentation
       << "\nID: " << d.id << "\nUNK: " << d.unknown;
    return stream << ss.str();
  }
};

}  // namespace Redatam