#include <unicode/unistr.h>

#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <iostream>
#include <vector>

#include "Variable.h"

std::vector<std::string> readData(const std::string &filename, int fieldSize) {
  std::ifstream file(filename, std::ios::binary);
  if (!file.is_open()) {
    std::cerr << "Could not open data file: " << filename << "\n";
    return {};
  }

  std::vector<std::string> data;
  char *buffer = new char[fieldSize];

  while (file.read(buffer, fieldSize)) {
    icu::UnicodeString unicodeStr(
        buffer, fieldSize,
        "ISO-8859-1"); // Adjusted to handle Latin-1 encoding
    std::string value;
    unicodeStr.toUTF8String(value);
    boost::algorithm::trim(value);
    // Remove non-printable characters
    value.erase(std::remove_if(value.begin(), value.end(),
                               [](char c) { return !isprint(c); }),
                value.end());
    data.push_back(value);
  }

  delete[] buffer;
  file.close();
  return data;
}
