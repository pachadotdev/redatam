#ifndef REDATAMLIB_VARIABLEPARSER_HPP
#define REDATAMLIB_VARIABLEPARSER_HPP

#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <stdexcept>
#include "ValueLabel.hpp"
#include "Variable.hpp"

namespace RedatamLib {

class VariableParser {
private:
  std::shared_ptr<RedatamLib::Variable> variable;

public:
  VariableParser(const std::shared_ptr<RedatamLib::Variable> &var)
      : variable(var) {}

  void ParseValueLabels();
  void ParseDecimals(std::string &extras);
  std::string GetMissingLabel(std::string &extras, const std::string &tag);
  void ParseDeclaration();
  void ParseMissingAndPrecision();

private:
  std::vector<std::string> split(const std::string &s, char delimiter);
  bool tryParse(const std::string &str, int &result);
  std::string eatStringFromString(std::string &extras);
};

} // namespace RedatamLib

#endif // REDATAMLIB_VARIABLEPARSER_HPP
