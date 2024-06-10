#include "CSVExporter.h"

#include <unicode/translit.h>
#include <unicode/unistr.h>

#include <boost/algorithm/string.hpp>
#include <fstream>
#include <iostream>
#include <vector>

#include "Variable.h"

extern std::vector<std::string> readData(const std::string &filename,
                                         int fieldSize);

std::string removeAccents(const std::string &input) {
  UErrorCode status = U_ZERO_ERROR;
  icu::UnicodeString unicodeStr = icu::UnicodeString::fromUTF8(input);
  std::unique_ptr<icu::Transliterator> trans(
      icu::Transliterator::createInstance(
          "NFD; [:Nonspacing Mark:] Remove; NFC", UTRANS_FORWARD, status));
  if (U_FAILURE(status)) {
    std::cerr << "Error creating transliterator: " << u_errorName(status)
              << std::endl;
    return input;
  }
  trans->transliterate(unicodeStr);

  std::string result;
  unicodeStr.toUTF8String(result);
  return result;
}

std::string transliterate(const std::string &input) {
  std::string result = removeAccents(input);
  boost::algorithm::trim(result);
  return result;
}

void CSVExporter::Export(const Entity &entity, const std::string &outputPath) {
  std::string filename = outputPath + "/" + entity.name + ".csv";
  std::ofstream file(filename);

  if (!file.is_open()) {
    std::cerr << "Could not open file for writing: " << filename << "\n";
    return;
  }

  // Write headers
  std::cout << "Writing CSV headers for entity: " << entity.name << std::endl;
  for (const auto &variable : entity.variables) {
    file << transliterate(variable.name) << "\t";
  }
  file << "\n";

  // Read data for each variable
  std::vector<std::vector<std::string>> allData;
  for (const auto &variable : entity.variables) {
    try {
      std::vector<std::string> data =
          readData(variable.filename, variable.fieldSize);
      for (auto &value : data) {
        value = transliterate(value);
      }
      allData.push_back(data);
    } catch (const std::exception &e) {
      std::cerr << "Error reading data for variable: " << variable.name
                << ". Exception: " << e.what() << std::endl;
    } catch (...) {
      std::cerr << "Unknown error reading data for variable: " << variable.name
                << std::endl;
    }
  }

  // Write data to CSV
  if (!allData.empty()) {
    size_t numRows = allData[0].size();
    for (size_t i = 0; i < numRows; ++i) {
      for (size_t j = 0; j < allData.size(); ++j) {
        if (i < allData[j].size()) {
          file << allData[j][i];
        }
        if (j < allData.size() - 1) {
          file << "\t";
        }
      }
      file << "\n";
    }
  }

  file.close();
  std::cout << "Finished writing CSV for entity: " << entity.name << std::endl;
}
