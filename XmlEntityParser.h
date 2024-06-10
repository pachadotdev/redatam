#ifndef XML_ENTITY_PARSER_H
#define XML_ENTITY_PARSER_H

#include <string>

#include "EntityParser.h"
#include "RedatamDatabase.h"
#include "pugixml.hpp"

class XmlEntityParser : public EntityParser {
public:
  void ParseEntity(pugi::xml_node entityNode, const std::string &filePath);
  void Parse(const std::string &filePath) override;
  RedatamDatabase getDatabase() const;

private:
  RedatamDatabase database;
};

#endif // XML_ENTITY_PARSER_H
