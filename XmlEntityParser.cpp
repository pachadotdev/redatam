#include "XmlEntityParser.h"

#include <algorithm>
#include <boost/filesystem.hpp>
#include <iostream>
#include <pugixml.hpp>

namespace fs = boost::filesystem;

void replaceBackslashes(std::string &path) {
  std::replace(path.begin(), path.end(), '\\', '/');
}

std::string getAbsolutePath(const std::string &basePath,
                            const std::string &relativePath) {
  fs::path base(basePath);
  base = base.parent_path();
  fs::path absolutePath = base / relativePath;
  return absolutePath.string();
}

void XmlEntityParser::ParseEntity(pugi::xml_node entityNode,
                                  const std::string &filePath) {
  Entity entity;
  entity.name = entityNode.child("name").child_value();
  std::cout << "Parsed entity: " << entity.name << std::endl;

  // Parse variables
  for (pugi::xml_node variableNode : entityNode.children("variable")) {
    Variable variable;
    variable.name = variableNode.child("name").child_value();
    std::string relativePath = variableNode.child("filename").child_value();
    replaceBackslashes(relativePath);
    variable.filename = getAbsolutePath(filePath, relativePath);
    variable.varType = variableNode.child("varType").child_value();
    variable.fieldSize =
        std::stoi(variableNode.child("fieldSize").child_value());
    std::cout << "Parsed variable: " << variable.name
              << " with filename: " << variable.filename << std::endl;
    entity.variables.push_back(variable);
  }

  database.entities.push_back(entity);

  // Parse nested entities
  for (pugi::xml_node nestedEntityNode : entityNode.children("entity")) {
    ParseEntity(nestedEntityNode, filePath);
  }
}

void XmlEntityParser::Parse(const std::string &filePath) {
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(filePath.c_str());
  if (!result) {
    std::cerr << "XML [" << filePath << "] parsed with errors, attr value: ["
              << doc.child("node").attribute("attr").value() << "]\n";
    std::cerr << "Error description: " << result.description() << "\n";
    std::cerr << "Error offset: " << result.offset << " (error at [..."
              << (filePath.c_str() + result.offset) << "]\n";
    return;
  }

  std::cout << "Parsing entities..." << std::endl;
  pugi::xml_node root = doc.child("redDictionary_XML").child("root");
  for (pugi::xml_node entityNode : root.children("entity")) {
    ParseEntity(entityNode, filePath);
  }
  std::cout << "Finished parsing entities." << std::endl;
}

RedatamDatabase XmlEntityParser::getDatabase() const { return database; }
