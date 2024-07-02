#include "CSVDoc.hpp"
#include <stdexcept>
#include <algorithm>

namespace RedatamLib {

CSVDoc::CSVDoc(const std::string &filename) : Filename(filename) {
  Stream.open(filename);
  if (!Stream.is_open()) {
    throw std::runtime_error("Cannot open file: " + filename);
  }
}

CSVDoc::~CSVDoc() { Close(); }

void CSVDoc::Close() {
  if (Stream.is_open()) {
    Stream.close();
  }
}

void CSVDoc::CommitDictionary() { WriteLine(Columns); }

void CSVDoc::WriteLine(const std::vector<std::string> &values) {
  std::stringstream ss;
  for (size_t i = 0; i < values.size(); ++i) {
    if (i != 0) {
      ss << delimiter;
    }
    std::string output = values[i];
    if (output.find(quote) != std::string::npos) {
      output =
          quote +
          replaceAll(output, std::string(1, quote), std::string(2, quote)) +
          quote;
    } else if (output.find(delimiter) != std::string::npos) {
      output = quote + output + quote;
    }
    ss << output;
  }
  Stream << ss.str() << "\n";
}

void CSVDoc::WriteVariableLabels() {
  // Implementation for writing variable labels
}

std::string CSVDoc::replaceAll(std::string str, const std::string &from,
                               const std::string &to) {
  size_t start_pos = 0;
  while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length();
  }
  return str;
}

} // namespace RedatamLib
