#ifndef REDATAMLIB_DATABLOCK_HPP
#define REDATAMLIB_DATABLOCK_HPP

#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <cstring>
#include <cctype> // For std::tolower

namespace RedatamLib {

class DataBlock {
public:
  std::vector<uint8_t> data;
  size_t n = 0;

  // Constructor from file path
  DataBlock(const std::string &path) {
    std::ifstream file(path, std::ios::binary);
    data = std::vector<uint8_t>(std::istreambuf_iterator<char>(file), {});
  }

  // Constructor from byte vector
  DataBlock(const std::vector<uint8_t> &bytes) : data(bytes) {}

  // Get a part of the data block
  DataBlock getPart(size_t prevStart, size_t iStart) {
    std::vector<uint8_t> part(data.begin() + prevStart, data.begin() + iStart);
    return DataBlock(part);
  }

  // Eat a plausible string
  bool eatPlausibleString(std::string &cad, bool filterByContent = true) {
    if (!PlausibleString(cad, filterByContent))
      return false;
    cad = eatShortString();
    return true;
  }

  // Check if a string is plausible
  bool PlausibleString(std::string &cad, bool filterByContent = true) {
    size_t keepN = n;
    cad = "";
    if (n + 2 >= data.size())
      return false;
    int length = eat16int();
    if (length < 0 || length > 128 || n + length > data.size()) {
      n = keepN;
      return false;
    }
    move(-2);
    cad = eatShortString();
    n = keepN;

    if (filterByContent && !IsText(cad))
      return false;
    return true;
  }

  // Check if a string is text
  bool IsText(const std::string &cad) {
    for (char c : cad) {
      c = std::tolower(c);
      if ((c < 'a' || c > 'z') && c != ' ' && c != '-' && c != '_' &&
          (c < '0' || c > '9'))
        return false;
    }
    return true;
  }

  // Move back in the string
  size_t moveBackString(size_t maxLength = 65536) {
    move(-2);
    size_t offset = 0;
    while (offset < n) {
      auto bytes = calcSize16(offset);
      if (matches(data.data(), bytes.data(), bytes.size(), n - offset)) {
        n -= offset;
        return offset;
      }
      offset++;
      if (offset > maxLength)
        break;
    }
    return -1;
  }

  // Make a string block
  std::vector<uint8_t> makeStringBlock(const std::string &entity) {
    auto intSize = calcSize16(entity.size());
    auto text = makeString(entity);
    return addArrays(intSize, text);
  }

  // Make a string
  static std::vector<uint8_t> makeString(const std::string &entity) {
    return std::vector<uint8_t>(entity.begin(), entity.end());
  }

  // Calculate size
  std::vector<uint8_t> calcSize(const std::string &cad) {
    size_t i = cad.size();
    auto part1 = calcSize16(i / 65536);
    auto part2 = calcSize16(i % 65536);
    return addArrays(part1, part2);
  }

  // Calculate size 16
  std::vector<uint8_t> calcSize16(size_t n) {
    return {static_cast<uint8_t>(n % 256), static_cast<uint8_t>(n / 256)};
  }

  // Eat a string
  std::string eatString() {
    int length = eat32intInv();
    return eatChars(length);
  }

  // Eat a short string
  std::string eatShortString() {
    int length = eat16int();
    if (length == 0xFFFF) {
      return eatString();
    } else {
      return eatChars(length);
    }
  }

  // Eat a 32-bit integer in inverse order
  int eat32intInv() { return eat16int() + 0x10000 * eat16int(); }

  // Eat a 32-bit integer
  int eat32int() { return 0x10000 * eat16int() + eat16int(); }

  // Eat a 16-bit integer
  int eat16int() { return 0x1 * eatByte() + 0x100 * eatByte(); }

  // Eat a byte
  uint8_t eatByte() { return data[n++]; }

  // Eat characters
  std::string eatChars(size_t length) {
    std::string cad(data.begin() + n, data.begin() + n + length);
    n += length;
    return cad;
  }

  // Add two arrays
  std::vector<uint8_t> addArrays(const std::vector<uint8_t> &a,
                                 const std::vector<uint8_t> &b) {
    std::vector<uint8_t> ret(a);
    ret.insert(ret.end(), b.begin(), b.end());
    return ret;
  }

  // Move to a specific item
  bool moveTo(const std::vector<uint8_t> &item) {
    int ret = SearchBytes(data, item, n);
    if (ret == -1)
      return false;
    n = ret;
    return true;
  }

  // Move by an offset
  void move(int i) { n += i; }

  std::vector<int> GetAllMatches(const std::vector<uint8_t> &block) {
    std::vector<int> matches;
    // Implementation here
    return matches;
  }

private:
  // Search bytes
  int SearchBytes(const std::vector<uint8_t> &haystack,
                  const std::vector<uint8_t> &needle, size_t start = 0) {
    size_t len = needle.size();
    size_t limit = haystack.size() - len;
    for (size_t i = start; i <= limit; i++) {
      if (matches(haystack.data(), needle.data(), needle.size(), i))
        return i;
    }
    return -1;
  }

  // Check if matches
  bool matches(const uint8_t *haystack, const uint8_t *needle, size_t needleLen,
               size_t offset) {
    return std::memcmp(haystack + offset, needle, needleLen) == 0;
  }
};

} // namespace RedatamLib

#endif // REDATAMLIB_DATABLOCK_HPP
