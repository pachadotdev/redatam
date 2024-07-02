#ifndef REDATAMLIB_XMLVARIABLEPARSER_HPP
#define REDATAMLIB_XMLVARIABLEPARSER_HPP

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include "tinyxml2.h"
#include "Variable.hpp"

namespace RedatamLib {

class XmlVariableParser {
private:
  std::shared_ptr<Variable> variable;

public:
  XmlVariableParser(const std::shared_ptr<Variable> &variable)
      : variable(variable) {}

  void ParseValueLabels(tinyxml2::XMLElement *variableNode);
  void ParseDeclaration(tinyxml2::XMLElement *node);
  static tinyxml2::XMLElement *getChildByName(tinyxml2::XMLNode *node,
                                              const std::string &name);
  static bool hasChildByName(tinyxml2::XMLNode *node, const std::string &name);
};

} // namespace RedatamLib

#endif // REDATAMLIB_XMLVARIABLEPARSER_HPP
