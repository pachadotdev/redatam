#ifndef ENTITY_PARSER_H
#define ENTITY_PARSER_H

class EntityParser {
public:
  virtual void Parse(const std::string &filePath) = 0;
};

#endif // ENTITY_PARSER_H
