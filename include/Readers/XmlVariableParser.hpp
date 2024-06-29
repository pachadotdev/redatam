#ifndef XMLVARIABLEPARSER_HPP
#define XMLVARIABLEPARSER_HPP

#include <string>
#include "tinyxml2.h"
#include "Variable.hpp"
#include "ValueLabel.hpp"
#include "XmlEntityParser.hpp"

// forward declaration
class Variable;

class XmlVariableParser
{
private:
    std::shared_ptr<Variable> variable;

    std::string eatStringFromString(std::string& extras)
    {
        if (extras.front() == '\'')
        {
            size_t nEnd = extras.find('\'', 1);
            if (nEnd != std::string::npos)
            {
                std::string retQ = extras.substr(1, nEnd - 1);
                extras = extras.substr(nEnd + 1);
                extras.erase(0, extras.find_first_not_of(" "));
                return retQ;
            }
        }

        size_t n = extras.find(' ');
        if (n == std::string::npos) n = extras.size();
        std::string ret = extras.substr(0, n);
        if (n < extras.size())
            extras = extras.substr(n + 1);
        else
            extras.clear();
        return ret;
    }

    std::string GetMissingLabel(std::string extras, const std::string& tag)
    {
        if (extras.find(tag) == 0)
        {
            std::string label = eatStringFromString(extras);
            std::string value = eatStringFromString(extras);
            this->variable->ValueLabels.emplace_back(value, label);
        }
        return extras;
    }

    void ParseDecimals(std::string extras)
    {
        if (extras.find("DECIMALS ") == 0)
        {
            std::string label = eatStringFromString(extras);
            std::string value = eatStringFromString(extras);
            this->variable->Decimals = std::stoi(value);
        }
    }

public:
    XmlVariableParser(std::shared_ptr<Variable> variable) : variable(variable) {}

    void ParseValueLabels(tinyxml2::XMLElement* variableNode)
    {
        this->variable->ValueLabels.clear();
        if (XmlEntityParser::hasChildByName(variableNode, "valueLabels"))
        {
            auto* values = XmlEntityParser::getChildByName(variableNode, "valueLabels");
            for (tinyxml2::XMLElement* ele = values->FirstChildElement(); ele != nullptr; ele = ele->NextSiblingElement())
            {
                auto* value = XmlEntityParser::getChildByName(ele, "value");
                auto* label = XmlEntityParser::getChildByName(ele, "label");
                this->variable->ValueLabels.emplace_back(value->GetText(), label->GetText());
            }
        }
    }

    void ParseDeclaration(tinyxml2::XMLElement* node)
    {
        auto* choice = XmlEntityParser::getChildByName(node, "varDicChoice");
        auto* typeNode = XmlEntityParser::getChildByName(choice, "datasetType");
        std::string type = typeNode->GetText();
        auto* sizeNode = XmlEntityParser::getChildByName(choice, "datasetSize");
        std::string size = sizeNode->GetText();

        if (this->variable->Type == "STRING" && type != "CHR")
            throw std::runtime_error("Inconsistent type declaration");
        if (this->variable->Type == "REAL" && type != "DBL")
            throw std::runtime_error("Inconsistent type declaration");
        if (type == "DBL")
            this->variable->Size = 64;
        else if (type == "LNG")
            this->variable->Size = 32;
        else if (type == "INT")
        {
            this->variable->Size = 16;
            this->variable->Type = "INT16";
        }
        else if (type == "BIN" || type == "PCK" || type == "CHR")
        {
            this->variable->Size = std::stoi(size);
            if (type == "BIN")
                this->variable->BinaryDataSet = true;
        }
        else
            throw std::runtime_error("Data type '" + type + "' is not supported. Contact idiscontinuos for support.");

        auto* fileNode = XmlEntityParser::getChildByName(node, "filename");
        this->variable->Filename = fileNode->GetText();
    }

    std::string ToString() const
    {
        return this->variable->Name;
    }
};

#endif // XMLVARIABLEPARSER_HPP
