#pragma once

#include <array>
#include <boost/filesystem.hpp>
#include <fstream>
#include <iomanip>
#include <string>

namespace Redatam {

inline std::string tolower(const std::string &s) {
  std::string r;
  r.reserve(s.size());
  std::transform(begin(s), end(s), back_inserter(r),
                 [](char ch) { return std::tolower(ch); });
  return r;
}

inline boost::filesystem::path
locate_icase(const boost::filesystem::path &path) {
  std::string base_lower = tolower(path.filename().string());
  for (auto &p : boost::filesystem::directory_iterator(path.parent_path()))
    if (base_lower == tolower(p.path().filename().string()))
      return path.parent_path() / p.path().filename();
  return "";
}

template <typename T, size_t N>
std::ostream &operator<<(std::ostream &stream, const std::array<T, N> &arr) {
  for (int c : arr)
    stream << "0x" << std::setbase(16) << std::setw(2) << std::setfill('0') << c
           << " ";
  return stream << std::setbase(10) << std::setfill('0');
}

} // namespace Redatam
