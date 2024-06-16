#pragma once

#include <array>
#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

namespace Redatam {

class VariableDescriptor {
public:
  class Declaration {
  public:
    enum class Type { BIN, CHR, DBL, INT, LNG, PCK };
    static std::optional<Declaration>
    fromDeclarationString(const std::string &declstr);
    Type type;
    boost::filesystem::path rbf_path;
    size_t size;
  };
  class Descriptor {
  public:
    std::string alias;
    size_t decimals = 0;
    std::string group;
    std::optional<int> missing;
    std::optional<int> not_applicable;
  };

  std::string name;
  std::optional<Declaration> declaration;
  std::string filter;
  std::string range;
  std::string datatype;
  std::vector<std::pair<int, std::string>> labels;
  std::string description;
  Descriptor descriptor;

  uint16_t unknown1;
  std::string documentation;
  uint16_t id;
  std::array<unsigned char, 16> unknown;

  // calculated fields
  boost::filesystem::path real_rbf_path;
  template <typename First, typename Last>
  bool resolve_rbf_data(First first, Last last) {
    if (!declaration or declaration->rbf_path.empty())
      return false;

    real_rbf_path = declaration->rbf_path;
    while (!exists(real_rbf_path) && first != last) {
      real_rbf_path = locate_icase(*first / declaration->rbf_path.filename());
      ++first;
    }

    // DEBUG
    // std::cout << "VariableDescriptor real_rbf_path: " << real_rbf_path
    //           << std::endl;

    return exists(real_rbf_path);
  }

  static VariableDescriptor fread(std::istream &stream);
};
std::ostream &operator<<(std::ostream &stream, const VariableDescriptor &d);
std::ostream &operator<<(std::ostream &stream,
                         const VariableDescriptor::Declaration &d);

int32_t fread_int32_t(std::istream &stream);
std::optional<int32_t> fread_optional_int32_t(std::istream &stream);

} // namespace Redatam
