#ifndef XML_VARIABLE_PARSER_H
#define XML_VARIABLE_PARSER_H

#include <string>

#include "VariableParser.h"

class XmlVariableParser : public VariableParser {
public:
  void Parse(const std::string &filePath) override;
};

#endif // XML_VARIABLE_PARSER_H
