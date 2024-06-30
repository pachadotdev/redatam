#ifndef READINI_HPP
#define READINI_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include "IniSection.hpp"

namespace RedatamLib {

class ReadIni {
public:
  static std::vector<IniSection> Read(const std::string &path) {
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

private:
  static std::string cleanBrackets(const std::string &line) {
    return line.substr(1, line.size() - 2);
  }
};

} // namespace RedatamLib

#endif // READINI_HPP
