#include "XmlEntityParser.hpp"

namespace RedatamLib {

XmlEntityParser::XmlEntityParser(RedatamLib::RedatamDatabase *db) : db(db) {}

void XmlEntityParser::Parse(const std::string &path) {
  rootPath = path.substr(0, path.find_last_of('/'));
  tinyxml2::XMLDocument xmldoc;
  xmldoc.LoadFile(path.c_str());
  auto entities = ParseEntities(xmldoc);
  db->Entities.insert(db->Entities.end(), entities.begin(), entities.end());
}

std::vector<std::shared_ptr<Entity>>
XmlEntityParser::ParseEntities(tinyxml2::XMLDocument &doc) {
  std::vector<std::shared_ptr<Entity>> ret;
  auto *root = getChildByName(doc.RootElement(), "redDictionary_XML");
  root = getChildByName(root, "root");
  ret.push_back(ReadEntity(root));
  return ret;
}

tinyxml2::XMLElement *XmlEntityParser::getChildByName(tinyxml2::XMLNode *node,
                                                      const std::string &name) {
  for (auto *child = node->FirstChildElement(); child != nullptr;
       child = child->NextSiblingElement()) {
    if (std::string(child->Name()) == name) {
      return child;
    }
  }
  throw std::runtime_error("Node not found: " + name);
}

bool XmlEntityParser::hasChildByName(tinyxml2::XMLNode *node,
                                     const std::string &name) {
  for (auto *child = node->FirstChildElement(); child != nullptr;
       child = child->NextSiblingElement()) {
    if (std::string(child->Name()) == name) {
      return true;
    }
  }
  return false;
}

std::vector<tinyxml2::XMLElement *>
XmlEntityParser::getChildrenByName(tinyxml2::XMLNode *node,
                                   const std::string &name) {
  std::vector<tinyxml2::XMLElement *> ret;
  for (auto *child = node->FirstChildElement(); child != nullptr;
       child = child->NextSiblingElement()) {
    if (std::string(child->Name()) == name) {
      ret.push_back(child);
    }
  }
  return ret;
}

std::shared_ptr<Entity>
XmlEntityParser::ReadEntity(tinyxml2::XMLElement *node) {
  auto e = std::make_shared<Entity>();
  e->Name = getChildByName(node, "name")->GetText();
  e->Description = getChildByName(node, "description")->GetText();
  e->IndexFilename = getChildByName(node, "index")->GetText();
  e->RelationChild = getChildByName(node, "relationChild")->GetText();
  e->RelationParent = getChildByName(node, "relationParent")->GetText();
  e->CodesVariable = getChildByName(node, "refCode")->GetText();
  if (hasChildByName(node, "refLabel"))
    e->LabelVariable = getChildByName(node, "refLabel")->GetText();

  ReadVariables(node, e);
  e->VariableCount = e->Variables.size();

  for (auto *childXml : getChildrenByName(node, "entity")) {
    auto child = ReadEntity(childXml);
    e->Children.push_back(child);
  }
  return e;
}

void XmlEntityParser::ReadVariables(tinyxml2::XMLElement *node,
                                    std::shared_ptr<Entity> e) {
  for (auto *variable : getChildrenByName(node, "variable")) {
    auto v = std::make_shared<Variable>(e.get());

    v->Name = getChildByName(variable, "name")->GetText();
    if (hasChildByName(variable, "filter"))
      v->Filter = getChildByName(variable, "filter")->GetText();
    v->Range = readRange(variable);
    if (hasChildByName(variable, "varType"))
      v->Type = getChildByName(variable, "varType")->GetText();
    else
      v->Type = "INTEGER";
    v->Label = getChildByName(variable, "label")->GetText();
    if (hasChildByName(variable, "group"))
      v->Group = getChildByName(variable, "group")->GetText();

    XmlVariableParser parser(v);
    parser.ParseDeclaration(variable);
    parser.ParseValueLabels(variable);

    e->Variables.push_back(v);
  }
}

std::string XmlEntityParser::readRange(tinyxml2::XMLElement *variable) {
  if (hasChildByName(variable, "range")) {
    auto *range = getChildByName(variable, "range");
    auto *min = getChildByName(range, "rangeMin");
    auto *max = getChildByName(range, "rangeMax");
    return std::string(min->GetText()) + "-" + std::string(max->GetText());
  } else {
    return "";
  }
}

} // namespace RedatamLib
