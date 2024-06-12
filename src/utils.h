#pragma once

#include <string>
#include <fstream>
#include <array>
#include <iomanip>

namespace Redatam {
  // Convert a string to lowercase
  std::string tolower(const std::string& s);

  // Locate a file in a case-insensitive manner
  boost::filesystem::path locate_icase(const boost::filesystem::path& path);

  // Overload the << operator to print an array in hexadecimal format
  template<typename T, size_t N>
  std::ostream& operator<<(std::ostream& stream, const std::array<T, N>& arr) {
      for (int c : arr)
          stream << "0x" << std::setbase(16) << std::setw(2) << std::setfill('0') << c << " ";
      return stream << std::setbase(10) << std::setfill('0');
  }
} // namespace Redatam
