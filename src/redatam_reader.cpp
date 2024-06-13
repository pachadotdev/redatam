
#include "redatam_reader.h"

#include <fstream>
#include <sstream>
#include <algorithm>

namespace Redatam {

RedatamReader::RedatamReader(const std::string& file_path)
    : file_path_(file_path) {}

void RedatamReader::read_data() {
  std::ifstream file(file_path_);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open file: " + file_path_);
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  result_ = buffer.str();  // Simplified logic to read file data

  // Utilize utility functions from util.h if needed
  std::string cleaned_result = result_;
  std::transform(cleaned_result.begin(), cleaned_result.end(),
                 cleaned_result.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  result_ = cleaned_result;  // Update result with cleaned data
}

std::string RedatamReader::get_result() const { return result_; }

}  // namespace Redatam
