#include <fstream>
#include <string>
#include <regex>
#include <memory>

#include "primitives.h"
#include "util.h"
#include "variable_descriptor.h"

std::unique_ptr<VariableDescriptor::Declaration>
VariableDescriptor::Declaration::fromDeclarationString(
    const std::string &declstr) {
  static const std::regex r(
      "DATASET (BIN|CHR|DBL|INT|LNG|PCK) '([^']+)' SIZE ([0-9]+)",
      std::regex::extended);
  std::smatch match;
  if (std::regex_match(declstr, match, r)) {
    Declaration::Type t = Declaration::Type::CHR;
    if (match[1] == "BIN")
      t = Declaration::Type::BIN;
    else if (match[1] == "CHR")
      t = Declaration::Type::CHR;
    else if (match[1] == "DBL")
      t = Declaration::Type::DBL;
    else if (match[1] == "INT")
      t = Declaration::Type::INT;
    else if (match[1] == "LNG")
      t = Declaration::Type::LNG;
    else if (match[1] == "PCK")
      t = Declaration::Type::PCK;
    std::string p = match[2];
    size_t s = std::stoi(match[3]);
    return std::unique_ptr<Declaration>(new Declaration{t, p, s});
  } else {
    return std::unique_ptr<Declaration>();
  }
}

VariableDescriptor VariableDescriptor::fread(std::istream &stream) {
  VariableDescriptor d;
  d.name = fread_string(stream);
  d.declaration = VariableDescriptor::Declaration::fromDeclarationString(
      fread_string(stream));
  d.filter = fread_string(stream);
  d.range = fread_string(stream);
  d.datatype = fread_string(stream);

  std::string labels_str = fread_string(stream);
  static const std::regex labels_regex("([0-9]+) ([^\t]+)\t",
                                       std::regex::extended);
  std::smatch match;
  while (std::regex_search(labels_str, match, labels_regex)) {
    d.labels.emplace_back(std::stoi(match[1]), match[2]);
    labels_str = match.suffix();
  }

  d.description = fread_string(stream);

  std::string descriptor_str = fread_string(stream);
  static const std::regex missing_regex("MISSING ([0-9]+)");
  static const std::regex notapplicable_regex("NOTAPPLICABLE ([0-9]+)");
  if (std::regex_search(descriptor_str, match, missing_regex)) {
    d.descriptor.missing = std::unique_ptr<int>(new int(std::stoi(match[1])));
  }
  if (std::regex_search(descriptor_str, match, notapplicable_regex)) {
    d.descriptor.not_applicable =
        std::unique_ptr<int>(new int(std::stoi(match[1])));
  }

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
