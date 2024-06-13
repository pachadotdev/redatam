#include <algorithm>
#include <cpp11.hpp>
#include <fstream>
#include <iostream>

#include "dictionary_descriptor.h"
#include "entity_descriptor.h"
#include "primitives.h"
#include "variable_descriptor.h"

using namespace cpp11;

// Function to clean a string (basic implementation)
std::string clean_string(const std::string& str) {
  std::string cleaned_str = str;
  cleaned_str.erase(std::remove(cleaned_str.begin(), cleaned_str.end(), '\r'),
                    cleaned_str.end());
  cleaned_str.erase(std::remove(cleaned_str.begin(), cleaned_str.end(), '\n'),
                    cleaned_str.end());
  cleaned_str.erase(std::remove(cleaned_str.begin(), cleaned_str.end(), '\t'),
                    cleaned_str.end());
  std::cout << "Cleaned string: " << cleaned_str << std::endl;
  return cleaned_str;
}

[[cpp11::register]] cpp11::list read_redatam_(cpp11::strings file_path) {
  if (file_path.size() != 1) {
    cpp11::stop("file_path must be a single string");
  }

  std::cout << "HERE" << std::endl;

  std::string file_path_str = cpp11::as_cpp<std::string>(file_path);

  std::cout << "HERE 2" << std::endl;

  std::ifstream file(file_path_str, std::ios::binary);
  if (!file) {
    cpp11::stop("Failed to open file");
  }

  std::cout << "HERE 3" << std::endl;

  // Read the entire dictionary and return a list of entities
  Redatam::DictionaryDescriptor dd = Redatam::DictionaryDescriptor::fread(file);

  std::cout << "Total entities read: " << dd.entities.size() << std::endl;

  // Process entities to create an R list
  cpp11::writable::list entities_list;
  cpp11::writable::strings names;

  for (const auto& entity : dd.entities) {
    if (entity.variables.empty()) continue;  // Skip entities with no variables

    cpp11::writable::list entity_list;
    for (const auto& variable : entity.variables) {
      std::string variable_name = variable.name;

      // Clean variable name
      variable_name = clean_string(variable_name);

      // Process variable labels if necessary (assuming labels is a map or
      // similar container)
      std::vector<std::string> variable_labels;
      for (const auto& [value, label] : variable.labels) {
        variable_labels.push_back(clean_string(label));
      }

      // For simplicity, let's just print the first label if it exists
      std::string variable_label =
          variable_labels.empty() ? "" : variable_labels[0];

      entity_list.push_back(
          Rf_mkChar(variable_name.c_str()));  // Convert std::string to CHARSXP
    }
    entities_list.push_back(entity_list);
    names.push_back(
        Rf_mkChar(clean_string(entity.name1)
                      .c_str()));  // Convert and clean std::string to CHARSXP
  }

  std::cout << "HERE 4" << std::endl;
  std::cout << "entities_list size: " << entities_list.size() << std::endl;
  std::cout << "names size: " << names.size() << std::endl;

  entities_list.attr("names") = names;

  std::cout << "HERE 5" << std::endl;

  return entities_list;
}
