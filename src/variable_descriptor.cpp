#include "variable_descriptor.h"

#include <fstream>
#include <optional>
#include <regex>
#include <string>

#include "primitives.h"
#include "utils.h"

namespace Redatam {

std::optional<VariableDescriptor::Declaration>
VariableDescriptor::Declaration::fromDeclarationString(
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

VariableDescriptor VariableDescriptor::fread(std::istream &stream) {
  VariableDescriptor d;
  d.name = fread_string(stream);
  d.declaration = Declaration::fromDeclarationString(fread_string(stream));

  // DEBUG
  // if (d.declaration) {
  //   std::cout << "VariableDescriptor rbf_path: " << d.declaration->rbf_path
  //             << std::endl;
  // }
  
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

std::ostream &operator<<(std::ostream &stream,
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

std::ostream &operator<<(std::ostream &stream,
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

std::ostream &operator<<(std::ostream &stream, const VariableDescriptor &d) {
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

int32_t fread_int32_t(std::istream &stream) {
  int32_t value;
  stream.read(reinterpret_cast<char *>(&value), sizeof(value));
  return value;
}

std::optional<int32_t> fread_optional_int32_t(std::istream &stream) {
  int32_t value;
  stream.read(reinterpret_cast<char *>(&value), sizeof(value));
  if (stream)
    return value;
  else
    return std::nullopt;
}

} // namespace Redatam
