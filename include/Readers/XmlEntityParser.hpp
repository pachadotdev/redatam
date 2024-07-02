#ifndef REDATAMLIB_XMLENTITYPARSER_H
#define REDATAMLIB_XMLENTITYPARSER_H

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <tinyxml2.h>
#include "Entity.hpp"
#include "EntityParser.hpp"
#include "XmlVariableParser.hpp"

namespace RedatamLib {

class RedatamDatabase; // Forward declaration

class XmlEntityParser {
private:
  RedatamDatabase *db;
  std::string rootPath;
  static const std::vector<std::string> validTypes;

public:
  XmlEntityParser(RedatamDatabase *db);

  void Parse(const std::string &path);
  std::vector<std::shared_ptr<Entity>>
  ParseEntities(tinyxml2::XMLDocument &doc);
  static tinyxml2::XMLElement *getChildByName(tinyxml2::XMLNode *node,
                                              const std::string &name);
  static bool hasChildByName(tinyxml2::XMLNode *node, const std::string &name);
  static std::vector<tinyxml2::XMLElement *>
  getChildrenByName(tinyxml2::XMLNode *node, const std::string &name);
  std::shared_ptr<Entity> ReadEntity(tinyxml2::XMLElement *node);
  void ReadVariables(tinyxml2::XMLElement *node, std::shared_ptr<Entity> e);
  std::string readRange(tinyxml2::XMLElement *variable);
};

const std::vector<std::string> XmlEntityParser::validTypes = {
    "DBL", "LNG", "INT", "BIN", "PCK", "CHR"};

} // namespace RedatamLib

#endif // REDATAMLIB_XMLENTITYPARSER_H
