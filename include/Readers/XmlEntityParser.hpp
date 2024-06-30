#ifndef XMLENTITYPARSER_HPP
#define XMLENTITYPARSER_HPP

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include "tinyxml2.h"
#include "Entity.hpp"
#include "Variable.hpp"
#include "RedatamDatabase.hpp"
#include "XmlVariableParser.hpp"

namespace RedatamLib {

class XmlEntityParser {
private:
    std::shared_ptr<RedatamDatabase> db;
    std::string rootPath;
    std::vector<std::string> validTypes = { "DBL", "LNG", "INT", "BIN", "PCK", "CHR" };

public:
    XmlEntityParser(const std::shared_ptr<RedatamDatabase>& db) : db(db) {}

    void Parse(const std::string& path) {
        rootPath = path.substr(0, path.find_last_of('/'));
        tinyxml2::XMLDocument xmldoc;
        xmldoc.LoadFile(path.c_str());
        db->Entities = ParseEntities(xmldoc);
    }

    std::vector<std::shared_ptr<Entity>> ParseEntities(tinyxml2::XMLDocument& doc) {
        std::vector<std::shared_ptr<Entity>> ret;
        auto* root = getChildByName(doc.RootElement(), "redDictionary_XML");
        root = getChildByName(root, "root");
        ret.push_back(ReadEntity(root));
        return ret;
    }

    static tinyxml2::XMLElement* getChildByName(tinyxml2::XMLNode* node, const std::string& name) {
        for (auto* child = node->FirstChildElement(); child != nullptr; child = child->NextSiblingElement()) {
            if (std::string(child->Name()) == name) {
                return child;
            }
        }
        throw std::runtime_error("Node not found: " + name);
    }

    static bool hasChildByName(tinyxml2::XMLNode* node, const std::string& name) {
        for (auto* child = node->FirstChildElement(); child != nullptr; child = child->NextSiblingElement()) {
            if (std::string(child->Name()) == name) {
                return true;
            }
        }
        return false;
    }

    static std::vector<tinyxml2::XMLElement*> getChildrenByName(tinyxml2::XMLNode* node, const std::string& name) {
        std::vector<tinyxml2::XMLElement*> ret;
        for (auto* child = node->FirstChildElement(); child != nullptr; child = child->NextSiblingElement()) {
            if (std::string(child->Name()) == name) {
                ret.push_back(child);
            }
        }
        return ret;
    }

    std::shared_ptr<Entity> ReadEntity(tinyxml2::XMLElement* node) {
        auto e = std::make_shared<Entity>();
        e->rootPath = rootPath;
        e->Name = getChildByName(node, "name")->GetText();
        if (hasChildByName(node, "label"))
            e->Description = getChildByName(node, "label")->GetText();
        e->IndexFilename = getChildByName(node, "indexFile")->GetText();
        e->RelationChild = getChildByName(node, "relationChild")->GetText();
        e->RelationParent = getChildByName(node, "relationParent")->GetText();
        e->CodesVariable = getChildByName(node, "refCode")->GetText();
        if (hasChildByName(node, "refLabel"))
            e->LabelVariable = getChildByName(node, "refLabel")->GetText();

        ReadVariables(node, e);
        e->VariableCount = e->getVariables().size();

        for (auto* childXml : getChildrenByName(node, "entity")) {
            auto child = ReadEntity(childXml);
            e->getChildren().push_back(child);
        }
        return e;
    }

    void ReadVariables(tinyxml2::XMLElement* node, std::shared_ptr<Entity> e) {
        for (auto* variable : getChildrenByName(node, "variable")) {
            auto v = std::make_shared<Variable>(e);

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

            XmlVariableParser parser(v); // Ensure XmlVariableParser is used correctly
            parser.ParseDeclaration(variable);
            parser.ParseValueLabels(variable);

            e->getVariables().push_back(v);
        }
    }

    std::string readRange(tinyxml2::XMLElement* variable) {
        if (hasChildByName(variable, "range")) {
            auto* range = getChildByName(variable, "range");
            auto* min = getChildByName(range, "rangeMin");
            auto* max = getChildByName(range, "rangeMax");
            return std::string(min->GetText()) + "-" + std::string(max->GetText());
        } else {
            return "";
        }
    }
};

} // namespace RedatamLib

#endif // XMLENTITYPARSER_HPP
