#ifndef VARIABLE_DESCRIPTOR_H
#define VARIABLE_DESCRIPTOR_H

#include <boost/filesystem.hpp>
#include <optional>
#include <string>
#include <vector>

namespace Redatam {

class VariableDescriptor {
 public:
  struct Declaration {
    enum class Type { BIN, CHR, DBL, INT, LNG, PCK };
    Type type;
    std::string path;
    size_t size;
    std::string rbf_path;

    static std::optional<Declaration> fromDeclarationString(
        const std::string& declstr);
  };

  std::string name;
  std::optional<Declaration> declaration;
  std::string filter;
  std::string range;
  std::string datatype;
  std::vector<std::pair<int, std::string>> labels;
  std::string description;
  std::optional<int> missing_value;
  std::optional<int> not_applicable_value;

  static VariableDescriptor fread(std::istream& stream);
  bool resolve_rbf_data(
      std::vector<boost::filesystem::path>::const_iterator begin,
      std::vector<boost::filesystem::path>::const_iterator end);
};

}  // namespace Redatam

#endif  // VARIABLE_DESCRIPTOR_H
