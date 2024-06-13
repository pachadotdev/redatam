
#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <cstdint>
#include <filesystem>
#include <istream>
#include <string>

namespace Redatam {

uint8_t fread_uint8_t(std::istream& stream);
uint16_t fread_uint16_t(std::istream& stream);
uint32_t fread_uint32_t(std::istream& stream);
std::string fread_string(std::istream& stream);
std::filesystem::path fread_path(std::istream& stream);
std::string fread_fixed_string(std::istream& stream, size_t width);
size_t fread_decimal(std::istream& stream, size_t digits);
double fread_double(std::istream& stream);
uint32_t fread_PCK(std::istream& stream);
uint32_t fread_BIN(std::istream& stream);

}  // namespace Redatam

#endif  // PRIMITIVES_H
