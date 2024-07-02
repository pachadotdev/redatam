#ifndef REDATAMLIB_DATABLOCK_HPP
#define REDATAMLIB_DATABLOCK_HPP

#include <string>
#include <vector>

namespace RedatamLib {

class DataBlock {
public:
  DataBlock(const std::string &path);
  DataBlock(const std::vector<unsigned char> &bytes);

  DataBlock getPart(int prevStart, int iStart);
  bool eatPlausibleString(std::string &cad, bool filterByContent = true);
  bool PlausibleString(std::string &cad, bool filterByContent = true);
  std::vector<int> GetAllMatches(const std::vector<unsigned char> &block);
  bool IsText(const std::string &cad);
  int moveBackString(int maxLength = 65536);
  std::string eatShortString();
  std::string eatString();
  int eat32intInv();
  int eat32int();
  int eat16int();
  unsigned char eatByte();
  std::string eatChars(int length);
  std::vector<unsigned char> addArrays(const std::vector<unsigned char> &a,
                                       const std::vector<unsigned char> &b,
                                       const std::vector<unsigned char> &c);
  std::vector<unsigned char> addArrays(const std::vector<unsigned char> &a,
                                       const std::vector<unsigned char> &b);
  std::vector<unsigned char> calcSize16(int n);
  bool moveTo(const std::string &item);
  bool moveTo(const std::vector<unsigned char> &item);
  void move(int i);

private:
  std::vector<unsigned char> data;
  int n = 0;

  int SearchBytes(const std::vector<unsigned char> &haystack,
                  const std::vector<unsigned char> &needle, int start = 0);
  bool matches(const std::vector<unsigned char> &haystack,
               const std::vector<unsigned char> &needle, int offset);
  std::vector<unsigned char> makeString(const std::string &str);
};

} // namespace RedatamLib

#endif // REDATAMLIB_DATABLOCK_HPP
