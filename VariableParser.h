#ifndef VARIABLE_PARSER_H
#define VARIABLE_PARSER_H

class VariableParser {
public:
  virtual void Parse(const std::string &filePath) = 0;
};

#endif // VARIABLE_PARSER_H
