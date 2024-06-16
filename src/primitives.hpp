#pragma once

#include <algorithm>
#include <boost/filesystem.hpp>
#include <cctype>
#include <fstream>
#include <string>
#include <vector>

namespace Redatam {

constexpr auto BYTE_MASK = 0b00111111;
constexpr auto BYTE_PREFIX = 0b11000000;

inline auto fread_uint8_t(std::istream &stream) { return stream.get(); }

inline auto fread_uint16_t(std::istream &stream) {
  return stream.get() + (stream.get() << 8);
}

inline auto fread_uint32_t(std::istream &stream) {
  return fread_uint16_t(stream) + (fread_uint16_t(stream) << 16);
}

inline void utf8_from_windows1252(std::string &dst, std::byte c) {
  if (static_cast<uint8_t>(c) > 0x7f) {
    dst += static_cast<char>(BYTE_PREFIX |
                             ((static_cast<uint8_t>(c) & BYTE_MASK) >> 6));
    dst +=
        static_cast<char>(0b10000000 | (static_cast<uint8_t>(c) & BYTE_MASK));
  } else {
    dst += static_cast<char>(c);
  }
}

inline std::string fread_string(std::istream &stream) {
  size_t sz = fread_uint16_t(stream);
  std::string result;
  if (sz == 0) return result;
  if (sz == 0xFFFF) sz = fread_uint32_t(stream);
  result.reserve(sz);
  for (size_t i = 0; i < sz; ++i)
    utf8_from_windows1252(result, static_cast<std::byte>(stream.get()));
  return result;
}

inline boost::filesystem::path fread_path(std::istream &stream) {
  auto raw = fread_string(stream);
  std::transform(begin(raw), end(raw), begin(raw), [](char c) {
    return c == '\\' ? boost::filesystem::path::preferred_separator : c;
  });
  return raw;
}

inline std::string fread_fixed_string(std::istream &stream, size_t width) {
  std::string result;
  while (width > 0) {
    auto c = stream.get();
    utf8_from_windows1252(result, static_cast<std::byte>(c));
    --width;
  }
  auto i = result.size() - 1;
  while (i >= 0 && std::isspace(result[i])) --i;
  result.resize(i + 1);
  return result;
}

inline auto fread_decimal(std::istream &stream, size_t digits) {
  size_t result = 0;
  for (size_t i = 0; i < digits; ++i)
    result = result * 10 + (fread_uint8_t(stream) - '0');
  return result;
}

inline auto fread_double(std::istream &stream) {
  double d;
  stream.read(reinterpret_cast<char *>(&d), sizeof(double));
  return d;
}

inline auto fread_PCK(std::istream &stream) { return fread_uint32_t(stream); }

inline auto fread_BIN(std::istream &stream) {
  return (stream.get() << 24) + (stream.get() << 16) + (stream.get() << 8) +
         stream.get();
}

}  // namespace Redatam