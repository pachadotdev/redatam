#ifndef REDATAMLIB_CURSORREADER_HPP
#define REDATAMLIB_CURSORREADER_HPP

#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <stdexcept>
#include <cstring>
#include <algorithm>
#include "ICursorReader.hpp"

namespace RedatamLib {

class CursorReader : public ICursorReader {
public:
  CursorReader(const std::string &file, bool isString, bool isBin, int size);

  void Open() override;
  std::string ReadString() override;
  double ReadDouble() override;
  uint64_t ReadNumber() override;
  void Close() override;
  bool IsLastPos() const override;
  int64_t Length() const override;
  uint32_t ReadInt32At(int64_t pos) override;
  uint32_t ReadInt32() override;
  uint16_t ReadInt16() override;
  uint32_t ReadByte() override;
  uint32_t ReadLastInt32() override;

private:
  mutable std::ifstream stream;
  std::string Filename;
  bool IsString;
  bool IsBinaryDataSet;
  int BlockSize;
  uint64_t fileSize;
  uint32_t trailingBits = 0;
  int trailingBitsCount = 0;
  int bytesPos = 0;

  uint64_t feedBits(uint64_t data, int &bitsRead);
  uint32_t Read4Bytes();
  uint32_t Read2Bytes();
};

} // namespace RedatamLib

#endif // REDATAMLIB_CURSORREADER_HPP
