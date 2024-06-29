#ifndef XMLENTITYPARSER_HPP
#define XMLENTITYPARSER_HPP

#include <vector>
#include <string>
#include <stdexcept>
#include <memory>
#include <tinyxml2.h>
#include "RedatamDatabase.hpp"
#include "Entity.hpp"
#include "XmlVariableParser.hpp"

class XmlEntityParser
{
private:
    RedatamDatabase* db;
    std::string rootPath;

    static const std::vector<std::string> validTypes;

    static tinyxml2::XMLElement* getChildByName(tinyxml2::XMLElement* node, const std::string& name)
    {
        for (tinyxml2::XMLElement* n = node->FirstChildElement(); n != nullptr; n = n->NextSiblingElement())
        {
            if (name == n->Name())
                return n;
        }
        throw std::runtime_error("Node not found: " + name);
    }

    static bool hasChildByName(tinyxml2::XMLElement* node, const std::string& name)
    {
        for (tinyxml2::XMLElement* n = node->FirstChildElement(); n != nullptr; n = n->NextSiblingElement())
        {
            if (name == n->Name())
                return true;
        }
        return false;
    }

    static std::vector<tinyxml2::XMLElement*> getChildrenByName(tinyxml2::XMLElement* node, const std::string& name)
    {
        std::vector<tinyxml2::XMLElement*> ret;
        for (tinyxml2::XMLElement* n = node->FirstChildElement(); n != nullptr; n = n->NextSiblingElement())
        {
            if (name == n->Name())
                ret.push_back(n);
        }
        return ret;
    }

    std::shared_ptr<Entity> ReadEntity(tinyxml2::XMLElement* node)
    {
        auto e = std::make_shared<Entity>();
        e->rootPath = rootPath;
        e->Name = getChildByName(node, "name")->GetText();
        if (hasChildByName(node, "label"))
            e->Description = getChildByName(node, "label")->GetText();
        e->IndexFilename = getChildByName(node, "filename")->GetText();
        if (hasChildByName(node, "refCode"))
            e->CodesVariable = getChildByName(node, "refCode")->GetText();
        if (hasChildByName(node, "refLabel"))
            e->LabelVariable = getChildByName(node, "refLabel")->GetText();
        ReadVariables(node, e);
        e->VariableCount = e->Variables.size();

        for (auto* childXml : getChildrenByName(node, "entity"))
        {
            auto child = ReadEntity(childXml);
            e->Children.push_back(child);
        }
        return e;
    }

    void ReadVariables(tinyxml2::XMLElement* node, std::shared_ptr<Entity> e)
    {
        for (auto* variable : getChildrenByName(node, "variable"))
        {
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

            XmlVariableParser parser(v);
            parser.ParseDeclaration(variable);
            parser.ParseValueLabels(variable);

            e->Variables.push_back(v);
        }
    }

    std::string readRange(tinyxml2::XMLElement* variable)
    {
        if (hasChildByName(variable, "range"))
        {
            auto range = getChildByName(variable, "range");
            auto min = getChildByName(range, "rangeMin");
            auto max = getChildByName(range, "rangeMax");
            return std::string(min->GetText()) + "-" + max->GetText();
        }
        else
            return "";
    }

public:
    XmlEntityParser(RedatamDatabase* db) : db(db) {}

    void Parse(const std::string& path)
    {
        rootPath = std::filesystem::path(path).parent_path().string();
        tinyxml2::XMLDocument xmldoc;
        if (xmldoc.LoadFile(path.c_str()) != tinyxml2::XML_SUCCESS)
            throw std::runtime_error("Could not parse XML file.");

        tinyxml2::XMLElement* root_element = xmldoc.RootElement();
        auto root = getChildByName(getChildByName(root_element, "redDictionary_XML"), "root");
        db->Entities.push_back(ReadEntity(root));
    }
};

#endif // XMLENTITYPARSER_HPP
