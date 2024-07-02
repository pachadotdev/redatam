#include "XmlVariableParser.hpp"

namespace RedatamLib {

void XmlVariableParser::ParseValueLabels(tinyxml2::XMLElement *variableNode) {
  variable->ValueLabels.clear();
  if (XmlVariableParser::hasChildByName(variableNode, "valueLabels")) {
    auto *values =
        XmlVariableParser::getChildByName(variableNode, "valueLabels");
    for (auto *ele = values->FirstChildElement(); ele != nullptr;
         ele = ele->NextSiblingElement()) {
      auto *value = XmlVariableParser::getChildByName(ele, "value");
      auto *label = XmlVariableParser::getChildByName(ele, "label");
      variable->ValueLabels.emplace_back(value->GetText(), label->GetText());
    }
  }
}

void XmlVariableParser::ParseDeclaration(tinyxml2::XMLElement *node) {
  auto *choice = XmlVariableParser::getChildByName(node, "varDicChoice");
  auto *typeNode = XmlVariableParser::getChildByName(choice, "datasetType");
  std::string type = typeNode->GetText();
  auto *sizeNode = XmlVariableParser::getChildByName(choice, "datasetSize");
  std::string size = sizeNode->GetText();

  if (variable->Type == "STRING" && type != "CHR")
    throw std::runtime_error("Inconsistent type declaration");
  if (variable->Type == "REAL" && type != "DBL")
    throw std::runtime_error("Inconsistent type declaration");

  if (type == "DBL")
    variable->Size = 64;
  else if (type == "LNG")
    variable->Size = 32;
  else if (type == "INT") {
    variable->Size = 16;
    variable->Type = "INT16";
  } else if (type == "BIN") {
    variable->Size = std::stoi(size);
    variable->BinaryDataSet = true;
  } else if (type == "PCK" || type == "CHR") {
    variable->Size = std::stoi(size);
  } else {
    throw std::runtime_error("Data type '" + type + "' is not supported.");
  }

  auto *fileNode = XmlVariableParser::getChildByName(node, "filename");
  variable->Filename = fileNode->GetText();
}

tinyxml2::XMLElement *
XmlVariableParser::getChildByName(tinyxml2::XMLNode *node,
                                  const std::string &name) {
  for (auto *child = node->FirstChildElement(); child != nullptr;
       child = child->NextSiblingElement()) {
    if (std::string(child->Name()) == name) {
      return child;
    }
  }
  throw std::runtime_error("Node not found: " + name);
}

bool XmlVariableParser::hasChildByName(tinyxml2::XMLNode *node,
                                       const std::string &name) {
  for (auto *child = node->FirstChildElement(); child != nullptr;
       child = child->NextSiblingElement()) {
    if (std::string(child->Name()) == name) {
      return true;
    }
  }
  return false;
}

} // namespace RedatamLib
