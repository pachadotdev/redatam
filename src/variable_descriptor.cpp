#include "variable_descriptor.h"

#include <boost/filesystem.hpp>
#include <fstream>
#include <optional>
#include <regex>
#include <string>

#include "primitives.h"
#include "utils.h"

namespace Redatam {

std::optional<VariableDescriptor::Declaration>
VariableDescriptor::Declaration::fromDeclarationString(
    const std::string& declstr) {
  static const std::regex r(
      "DATASET (BIN|CHR|DBL|INT|LNG|PCK) '([^']+)' SIZE ([0-9]+) RBF_PATH "
      "'([^']+)'",
      std::regex::extended);
  if (std::smatch match; std::regex_match(declstr, match, r)) {
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
    std::replace(begin(p), end(p), '\\', boost::filesystem::path::preferred_separator);
    size_t s = std::stoi(match[3]);
    std::string rbf_path = match[4];  // Extract rbf_path
    return Declaration{t, p, s, rbf_path};
  } else {
    return std::nullopt;
  }
}

VariableDescriptor VariableDescriptor::fread(std::istream& stream) {
  VariableDescriptor d;
  d.name = fread_string(stream);
  d.declaration = VariableDescriptor::Declaration::fromDeclarationString(
      fread_string(stream));
  d.filter = fread_string(stream);
  d.range = fread_string(stream);
  d.datatype = fread_string(stream);

  std::string labels_str = fread_string(stream);
  static const std::regex labels_regex("([0-9]+) ([^\\t]+)\\t",
                                       std::regex::extended);
  std::smatch match;
  while (std::regex_search(labels_str, match, labels_regex)) {
    d.labels.emplace_back(std::stoi(match[1]), match[2]);
    labels_str = match.suffix().str();
  }

  d.description = fread_string(stream);

  std::string descriptor_str = fread_string(stream);
  static const std::regex missing_regex("MISSING ([0-9]+)");
  static const std::regex notapplicable_regex("NOTAPPLICABLE ([0-9]+)");
  if (std::regex_search(descriptor_str, match, missing_regex)) {
    d.missing_value = std::stoi(match[1]);
  }
  if (std::regex_search(descriptor_str, match, notapplicable_regex)) {
    d.not_applicable_value = std::stoi(match[1]);
  }

  return d;
}

bool VariableDescriptor::resolve_rbf_data(
    std::vector<boost::filesystem::path>::const_iterator begin,
    std::vector<boost::filesystem::path>::const_iterator end) {
  if (!declaration) return false;
  for (auto it = begin; it != end; ++it) {
    boost::filesystem::path potential_path = *it / declaration->rbf_path;
    if (boost::filesystem::exists(potential_path)) {
      declaration->path = potential_path.string();
      return true;
    }
  }
  return false;
}

std::ostream& operator<<(std::ostream& stream, const VariableDescriptor& d) {
  stream << "Name: " << d.name
         << "\nDeclaration: " << (d.declaration ? "Yes" : "No")
         << "\nFilter: " << d.filter << "\nRange: " << d.range
         << "\nDatatype: " << d.datatype << "\nLabels: ";
  for (const auto& label : d.labels) {
    stream << label.first << " " << label.second << "\t";
  }
  stream << "\nDescription: " << d.description << "\nMissing value: "
         << (d.missing_value ? std::to_string(d.missing_value.value()) : "None")
         << "\nNot applicable value: "
         << (d.not_applicable_value
                 ? std::to_string(d.not_applicable_value.value())
                 : "None");
  return stream;
}

}  // namespace Redatam
