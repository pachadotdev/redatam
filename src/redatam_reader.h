
#pragma once

#include <string>
#include <filesystem>

namespace Redatam {

class RedatamReader {
 public:
  explicit RedatamReader(const std::string& file_path);
  void read_data();
  std::string get_result() const;

 private:
  std::string file_path_;
  std::string result_;  // Store the result of the read operation
};

}  // namespace Redatam
