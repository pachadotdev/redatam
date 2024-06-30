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
  virtual ~CursorReader() = default;

  CursorReader(const std::string &file, bool isString, bool isBin, int size)
      : Filename(file), IsString(isString), IsBinaryDataSet(isBin),
        BlockSize(size), fileSize(0), trailingBits(0), trailingBitsCount(0),
        bytesPos(0) {}

  void Open() {
    fileSize = std::filesystem::file_size(Filename);
    stream.open(Filename, std::ios::binary);
    if (!stream)
      throw std::runtime_error("Cannot open file: " + Filename);
    bytesPos = 0;
  }

  std::string ReadString() {
    std::vector<char> bytes(BlockSize);
    stream.read(bytes.data(), BlockSize);
    std::string str(bytes.begin(), bytes.end());
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
  }

  double ReadDouble() {
    std::vector<char> bytes(8);
    stream.read(bytes.data(), 8);
    double ret;
    std::memcpy(&ret, bytes.data(), sizeof(ret));
    return ret;
  }

  uint64_t ReadNumber() {
    int bitsRead = 0;
    uint64_t data = 0;
    data = feedBits(data, bitsRead);
    while (bitsRead < BlockSize) {
      if (IsBinaryDataSet)
        trailingBits = this->Read4Bytes();
      else
        trailingBits = this->ReadInt32();
      bytesPos += 4;
      trailingBitsCount = 32;
      data = feedBits(data, bitsRead);
    }
    return data;
  }

  void Close() { stream.close(); }

  bool IsLastPos() const { return static_cast<uint64_t>(bytesPos) >= fileSize; }

  int64_t Length() const { return stream.tellg(); }

  uint32_t ReadInt32At(int64_t pos) {
    auto keepPos = stream.tellg();
    stream.seekg(pos * 4, std::ios::beg);
    uint32_t ret = this->ReadInt32();
    stream.seekg(keepPos);
    return ret;
  }

  uint32_t ReadLastInt32() {
    auto keepPos = stream.tellg();
    stream.seekg(-4, std::ios::end);
    uint32_t ret = this->ReadInt32();
    stream.seekg(keepPos);
    return ret;
  }

  uint32_t ReadInt32() { return ReadInt16() + ReadInt16() * 0x10000; }

  uint16_t ReadInt16() { return ReadByte() + ReadByte() * 0x100; }

  uint32_t Read4Bytes() { return Read2Bytes() * 0x10000 + Read2Bytes(); }

  uint32_t Read2Bytes() { return ReadByte() * 0x100 + ReadByte(); }

  uint32_t ReadByte() {
    char ret;
    if (!stream.get(ret))
      throw std::runtime_error("Cannot read byte from file: " + Filename);
    return static_cast<unsigned char>(ret);
  }

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

  uint64_t feedBits(uint64_t data, int &bitsRead) {
    while (bitsRead < BlockSize && trailingBitsCount > 0) {
      data <<= 1;
      uint32_t bit = (trailingBits >> 31) & 1;
      data += bit;
      trailingBits <<= 1;
      trailingBitsCount--;
      bitsRead++;
    }
    return data;
  }
};

} // namespace RedatamLib

#endif // REDATAMLIB_CURSORREADER_HPP
