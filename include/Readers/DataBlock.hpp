#ifndef DATABLOCK_HPP
#define DATABLOCK_HPP

#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <cstring>
#include <cctype>

namespace RedatamLib {

class DataBlock {
public:
  std::vector<uint8_t> data;
  int n = 0;

  DataBlock(const std::string &path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
      throw std::runtime_error("Cannot open file: " + path);
    }

    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    data.resize(size);
    if (!file.read(reinterpret_cast<char *>(data.data()), size)) {
      throw std::runtime_error("Error reading file: " + path);
    }
  }

  DataBlock(const std::vector<uint8_t> &bytes) : data(bytes) {}

  DataBlock getPart(int prevStart, int iStart) const {
    if (prevStart < 0 || static_cast<size_t>(iStart) > data.size() ||
        prevStart >= iStart) {
      throw std::out_of_range("Invalid range for getPart");
    }
    return DataBlock(
        std::vector<uint8_t>(data.begin() + prevStart, data.begin() + iStart));
  }

  std::string toString() const { return std::string(data.begin(), data.end()); }

  std::string toLower() const {
    std::string result = toString();
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
  }

  std::string toUpper() const {
    std::string result = toString();
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
  }

  bool startsWith(const std::string &prefix) const {
    if (prefix.size() > data.size()) {
      return false;
    }
    return std::equal(prefix.begin(), prefix.end(), data.begin());
  }

  bool endsWith(const std::string &suffix) const {
    if (suffix.size() > data.size()) {
      return false;
    }
    return std::equal(suffix.rbegin(), suffix.rend(), data.rbegin());
  }

  bool contains(const std::string &str) const {
    return std::search(data.begin(), data.end(), str.begin(), str.end()) !=
           data.end();
  }

  size_t indexOf(const std::string &str) const {
    auto it = std::search(data.begin(), data.end(), str.begin(), str.end());
    if (it != data.end()) {
      return std::distance(data.begin(), it);
    }
    return std::string::npos;
  }

  std::string substring(size_t start, size_t length) const {
    if (start >= data.size()) {
      throw std::out_of_range("Invalid start position for substring");
    }
    return std::string(data.begin() + start, data.begin() + start + length);
  }

  std::vector<std::string> split(char delimiter) const {
    std::vector<std::string> tokens;
    std::string token;
    for (auto ch : data) {
      if (ch == delimiter) {
        tokens.push_back(token);
        token.clear();
      } else {
        token.push_back(ch);
      }
    }
    if (!token.empty()) {
      tokens.push_back(token);
    }
    return tokens;
  }

  std::string trim() const {
    auto start = std::find_if_not(data.begin(), data.end(), ::isspace);
    auto end = std::find_if_not(data.rbegin(), data.rend(), ::isspace).base();
    return (start < end) ? std::string(start, end) : std::string();
  }

  std::string replace(char oldChar, char newChar) const {
    std::string result = toString();
    std::replace(result.begin(), result.end(), oldChar, newChar);
    return result;
  }

  bool isEmpty() const { return data.empty(); }

  size_t size() const { return data.size(); }

  std::vector<std::string> plausibleStrings() const {
    std::vector<std::string> strings;
    std::string currentString;
    for (auto ch : data) {
      if (std::isprint(ch) || std::isspace(ch)) {
        currentString.push_back(ch);
      } else if (!currentString.empty()) {
        strings.push_back(currentString);
        currentString.clear();
      }
    }
    if (!currentString.empty()) {
      strings.push_back(currentString);
    }
    return strings;
  }

  void toFile(const std::string &path) const {
    std::ofstream file(path, std::ios::binary);
    if (!file) {
      throw std::runtime_error("Cannot open file: " + path);
    }
    file.write(reinterpret_cast<const char *>(data.data()), data.size());
  }

  static DataBlock fromFile(const std::string &path) { return DataBlock(path); }

  static DataBlock fromString(const std::string &str) {
    return DataBlock(std::vector<uint8_t>(str.begin(), str.end()));
  }

  static DataBlock fromHexString(const std::string &hexStr) {
    std::vector<uint8_t> bytes;
    for (size_t i = 0; i < hexStr.size(); i += 2) {
      std::string byteString = hexStr.substr(i, 2);
      uint8_t byte =
          static_cast<uint8_t>(strtol(byteString.c_str(), nullptr, 16));
      bytes.push_back(byte);
    }
    return DataBlock(bytes);
  }
};

} // namespace RedatamLib

#endif // DATABLOCK_HPP
