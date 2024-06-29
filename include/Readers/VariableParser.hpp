#ifndef VARIABLEPARSER_HPP
#define VARIABLEPARSER_HPP

#include <string>
#include <sstream>
#include "Variable.hpp"
#include "ValueLabel.hpp"

class VariableParser
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
                extras = extras.erase(0, extras.find_first_not_of(" "));
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
    VariableParser(std::shared_ptr<Variable> variable) : variable(variable) {}

    void ParseValueLabels()
    {
        std::istringstream stream(this->variable->ValuesLabelsRaw);
        std::string item;
        this->variable->ValueLabels.clear();
        if (this->variable->ValuesLabelsRaw.empty()) return;
        while (std::getline(stream, item, '\t'))
        {
            size_t i = item.find(' ');
            if (i == std::string::npos) break;
            std::string part1 = item.substr(0, i);
            std::string part2 = item.substr(i + 1);
            try
            {
                int x = std::stoi(part1);
                this->variable->ValueLabels.emplace_back(part1, part2);
            }
            catch (const std::invalid_argument&)
            {
                this->variable->ValueLabels.emplace_back("0", item);
            }
        }
    }

    void ParseDeclaration()
    {
        std::string info = this->variable->Declaration;
        std::string dataset = eatStringFromString(info);
        std::string type = eatStringFromString(info);
        std::string fileRaw = eatStringFromString(info);
        std::string sizeLabel = eatStringFromString(info);
        std::string size = eatStringFromString(info);

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
        this->variable->Filename = fileRaw;
    }

    void ParseMissingAndPrecision()
    {
        std::string extras = this->variable->Group;
        extras = GetMissingLabel(extras, "MISSING");
        extras = GetMissingLabel(extras, "NOTAPPLICABLE");
        ParseDecimals(extras);
    }

    std::string ToString() const
    {
        return this->variable->Name;
    }
};

#endif // VARIABLEPARSER_HPP
