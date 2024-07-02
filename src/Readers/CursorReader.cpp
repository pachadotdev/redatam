#include "CursorReader.hpp"

namespace RedatamLib {

CursorReader::CursorReader(const std::string &file, bool isString, bool isBin,
                           int size)
    : Filename(file), IsString(isString), IsBinaryDataSet(isBin),
      BlockSize(size), fileSize(0), trailingBits(0), trailingBitsCount(0),
      bytesPos(0) {}

void CursorReader::Open() {
  fileSize = std::filesystem::file_size(Filename);
  stream.open(Filename, std::ios::binary);
  if (!stream)
    throw std::runtime_error("Cannot open file: " + Filename);
  bytesPos = 0;
}

std::string CursorReader::ReadString() {
  std::vector<char> bytes(BlockSize);
  stream.read(bytes.data(), BlockSize);
  std::string str(bytes.begin(), bytes.end());
  std::transform(str.begin(), str.end(), str.begin(), ::tolower);
  return str;
}

double CursorReader::ReadDouble() {
  std::vector<char> bytes(8);
  stream.read(bytes.data(), 8);
  double ret;
  std::memcpy(&ret, bytes.data(), sizeof(ret));
  return ret;
}

uint64_t CursorReader::ReadNumber() {
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

void CursorReader::Close() { stream.close(); }

bool CursorReader::IsLastPos() const {
  return static_cast<uint64_t>(bytesPos) >= fileSize;
}

int64_t CursorReader::Length() const { return stream.tellg(); }

uint32_t CursorReader::ReadInt32At(int64_t pos) {
  auto keepPos = stream.tellg();
  stream.seekg(pos, std::ios::beg);
  uint32_t ret = ReadInt32();
  stream.seekg(keepPos, std::ios::beg);
  return ret;
}

uint32_t CursorReader::ReadInt32() {
  return ReadInt16() + ReadInt16() * 0x10000;
}

uint16_t CursorReader::ReadInt16() { return ReadByte() + ReadByte() * 0x100; }

uint32_t CursorReader::Read4Bytes() {
  return Read2Bytes() * 0x10000 + Read2Bytes();
}

uint32_t CursorReader::Read2Bytes() { return ReadByte() * 0x100 + ReadByte(); }

uint32_t CursorReader::ReadByte() {
  char ret;
  if (!stream.get(ret))
    throw std::runtime_error("Cannot read byte from file: " + Filename);
  return static_cast<unsigned char>(ret);
}

uint64_t CursorReader::feedBits(uint64_t data, int &bitsRead) {
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

} // namespace RedatamLib
