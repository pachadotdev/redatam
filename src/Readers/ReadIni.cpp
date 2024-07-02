#include "ReadIni.hpp"

namespace RedatamLib {

std::vector<IniSection> ReadIni::Read(const std::string &path) {
  std::vector<IniSection> ret;
  std::ifstream file(path);
  std::string line;
  std::unique_ptr<IniSection> currentSection = nullptr;

  while (std::getline(file, line)) {
    if (line.front() == '[' && line.back() == ']') {
      if (currentSection != nullptr) {
        ret.push_back(*currentSection);
      }
      currentSection = std::make_unique<IniSection>(cleanBrackets(line));
    } else if (line.find('=') != std::string::npos) {
      size_t n = line.find('=');
      std::string key = line.substr(0, n);
      std::string value = line.substr(n + 1);
      if (currentSection != nullptr) {
        currentSection->Items[key] = value;
      }
    }
  }
  if (currentSection != nullptr) {
    ret.push_back(*currentSection);
  }

  return ret;
}

std::string ReadIni::cleanBrackets(const std::string &line) {
  return line.substr(1, line.size() - 2);
}

} // namespace RedatamLib
