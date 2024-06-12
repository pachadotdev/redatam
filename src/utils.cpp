#include <algorithm>
#include <boost/filesystem.hpp>
#include <string>

#include "utils.h"

namespace Redatam {
  // Convert a string to lowercase
  std::string tolower(const std::string& s) {
    std::string r;
    r.reserve(s.size());
    std::transform(s.begin(), s.end(), std::back_inserter(r), ::tolower);
    return r;
  }

  // Locate a file in a case-insensitive manner
  boost::filesystem::path locate_icase(const boost::filesystem::path& path) {
    std::string base_lower = tolower(path.filename().string());
    for (auto& p : boost::filesystem::directory_iterator(path.parent_path())) {
      if (base_lower == tolower(p.path().filename().string())) {
        return path.parent_path() / p.path().filename();
      }
    }
    return {};
  }
}  // namespace Redatam
