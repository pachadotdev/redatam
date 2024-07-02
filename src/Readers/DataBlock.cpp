#include "DataBlock.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <iterator>
#include <codecvt>

namespace RedatamLib {

DataBlock::DataBlock(const std::string &path) {
  std::ifstream file(path, std::ios::binary);
  if (!file)
    throw std::runtime_error("Unable to open file");
  data = std::vector<unsigned char>((std::istreambuf_iterator<char>(file)),
                                    std::istreambuf_iterator<char>());
}

DataBlock::DataBlock(const std::vector<unsigned char> &bytes) : data(bytes) {}

int DataBlock::getN() const { return n; }

const std::vector<unsigned char> &DataBlock::getData() const { return data; }

std::vector<unsigned char> DataBlock::makeString(const std::string &str) {
  std::vector<unsigned char> result(str.begin(), str.end());
  result.push_back('\0');
  return result;
}

DataBlock DataBlock::getPart(int prevStart, int iStart) {
  std::vector<unsigned char> part(data.begin() + prevStart,
                                  data.begin() + iStart);
  return DataBlock(part);
}


bool DataBlock::eatPlausibleString(std::string &cad, bool filterByContent) {
  if (!PlausibleString(cad, filterByContent))
    return false;
  cad = eatShortString();
  return true;
}

bool DataBlock::PlausibleString(std::string &cad, bool filterByContent) {
  int keepN = n;
  cad = "";
  if (n + 2 >= static_cast<int>(data.size()))
    return false; // Cast to int
  int length = eat16int();
  if (length < 0 || length > 128 ||
      n + length > static_cast<int>(data.size())) { // Cast to int
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

std::vector<int>
DataBlock::GetAllMatches(const std::vector<unsigned char> &block) {
  std::vector<int> ret;
  int keepN = n;
  while (moveTo(block)) {
    ret.push_back(n);
    n++;
  }
  n = keepN;
  return ret;
}

bool DataBlock::IsText(const std::string &cad) {
  for (char c : cad) {
    if (!isalnum(c) && c != ' ' && c != '-' && c != '_')
      return false;
  }
  return true;
}

int DataBlock::moveBackString(int maxLength) {
  move(-2);
  int offset = 0;
  while (offset < n) {
    auto bytes = std::vector<unsigned char>(data.begin() + n - offset - 1,
                                            data.begin() + n - offset);
    if (IsText(eatChars(bytes.size())))
      return n - offset;
    offset++;
  }
  return -1;
}

std::string DataBlock::eatShortString() {
  int length = eat16int();
  if (length == 0xFFFF) {
    return eatString();
  } else {
    return eatChars(length);
  }
}

std::string DataBlock::eatString() {
  std::string result;
  while (n < static_cast<int>(data.size()) && data[n] != '\0') {
    result.push_back(static_cast<char>(data[n++]));
  }
  // Move past the null character, if any
  if (n < static_cast<int>(data.size()) && data[n] == '\0') {
    n++;
  }
  return result;
}

int DataBlock::eat32intInv() { return eat16int() + 0x10000 * eat16int(); }

int DataBlock::eat32int() { return 0x10000 * eat16int() + eat16int(); }

int DataBlock::eat16int() { return 0x1 * eatByte() + 0x100 * eatByte(); }

unsigned char DataBlock::eatByte() { return data[n++]; }

std::string DataBlock::eatChars(int length) {
  std::string cad(data.begin() + n, data.begin() + n + length);
  n += length;
  return cad;
}

std::vector<unsigned char>
DataBlock::addArrays(const std::vector<unsigned char> &a,
                     const std::vector<unsigned char> &b,
                     const std::vector<unsigned char> &c) {
  return addArrays(addArrays(a, b), c);
}

std::vector<unsigned char>
DataBlock::addArrays(const std::vector<unsigned char> &a,
                     const std::vector<unsigned char> &b) {
  std::vector<unsigned char> ret(a.size() + b.size());
  std::copy(a.begin(), a.end(), ret.begin());
  std::copy(b.begin(), b.end(), ret.begin() + a.size());
  return ret;
}

std::vector<unsigned char> DataBlock::calcSize16(int n) {
  unsigned char n1 = static_cast<unsigned char>(n % 256);
  unsigned char n2 = static_cast<unsigned char>(n / 256);
  return std::vector<unsigned char>{n1, n2};
}

bool DataBlock::moveTo(const std::string &item) {
  return moveTo(makeString(item));
}

bool DataBlock::moveTo(const std::vector<unsigned char> &item) {
  int ret = SearchBytes(data, item, n);
  if (ret == -1)
    return false;
  n = ret;
  return true;
}

void DataBlock::move(int i) { n += i; }

int DataBlock::SearchBytes(const std::vector<unsigned char> &haystack,
                           const std::vector<unsigned char> &needle,
                           int start) {
  auto it = std::search(haystack.begin() + start, haystack.end(),
                        needle.begin(), needle.end());
  if (it == haystack.end())
    return -1;
  return std::distance(haystack.begin(), it);
}

bool DataBlock::matches(const std::vector<unsigned char> &haystack,
                        const std::vector<unsigned char> &needle, int offset) {
  return std::equal(needle.begin(), needle.end(), haystack.begin() + offset);
}

std::vector<unsigned char> DataBlock::makeString(const std::string &str) {
  return std::vector<unsigned char>(str.begin(), str.end());
}

} // namespace RedatamLib
