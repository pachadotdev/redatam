#pragma once

#include <algorithm>
#include <array>
#include <boost/filesystem.hpp>
#include <fstream>
#include <iomanip>
#include <string>

namespace Redatam {

inline std::string tolower(const std::string &s) {
  std::string r(s);
  std::transform(r.begin(), r.end(), r.begin(), ::tolower);
  return r;
}

inline boost::filesystem::path
locate_icase(const boost::filesystem::path &path) {
  std::string base_lower = tolower(path.filename().string());
  auto it =
      std::find_if(boost::filesystem::directory_iterator(path.parent_path()),
                   {}, [&](const auto &p) {
                     return base_lower == tolower(p.path().filename().string());
                   });
  return it != boost::filesystem::directory_iterator()
             ? it->path()
             : boost::filesystem::path();
}

template <typename T, size_t N>
std::ostream &operator<<(std::ostream &stream, const std::array<T, N> &arr) {
  for (const auto &c : arr)
    stream << "0x" << std::hex << std::setw(2) << std::setfill('0') << c << " ";
  return stream << std::dec << std::setfill('0');
}

} // namespace Redatam