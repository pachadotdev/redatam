
#include "primitives.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>

namespace Redatam {

uint8_t fread_uint8_t(std::istream& stream) {
    return stream.get();
}

uint16_t fread_uint16_t(std::istream& stream) {
    return stream.get() + (stream.get() << 8);
}

uint32_t fread_uint32_t(std::istream& stream) {
    return fread_uint16_t(stream) + (fread_uint16_t(stream) << 16);
}

void utf8_from_windows1252(std::string& dst, unsigned char c) {
    if (c > 0x7f) {
        dst += static_cast<char>(0b11000000 | ((c & 0b11000000) >> 6));
        dst += static_cast<char>(0b10000000 | (c & 0b00111111));
    } else {
        dst += static_cast<char>(c);
    }
}

std::string fread_string(std::istream& stream) {
    size_t sz = fread_uint16_t(stream);
    std::string result;
    if (sz == 0)
        return result;
    if (sz == 0xFFFF)
        sz = fread_uint32_t(stream);
    result.reserve(sz);
    for (size_t i = 0; i < sz; ++i)
        utf8_from_windows1252(result, stream.get());

    return result;
}

std::filesystem::path fread_path(std::istream& stream) {
    std::string path_str = fread_string(stream);
    return std::filesystem::path(path_str);
}

// Additional functions from redatam4r
std::string fread_fixed_string(std::istream& stream, size_t width) {
  std::string result(width, ' ');
  stream.read(&result[0], width);
  return result;
}

size_t fread_decimal(std::istream& stream, size_t digits) {
    size_t result = 0;
    for (size_t i = 0; i < digits; ++i) {
        result = result * 10 + (stream.get() - '0');
    }
    return result;
}

double fread_double(std::istream& stream) {
    double result;
    stream.read(reinterpret_cast<char*>(&result), sizeof(result));
    return result;
}

uint32_t fread_PCK(std::istream& stream) {
    uint32_t result;
    stream.read(reinterpret_cast<char*>(&result), sizeof(result));
    return result;
}

uint32_t fread_BIN(std::istream& stream) {
    uint32_t result;
    stream.read(reinterpret_cast<char*>(&result), sizeof(result));
    return result;
}

} // namespace Redatam
