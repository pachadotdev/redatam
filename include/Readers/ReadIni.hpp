#ifndef READINI_HPP
#define READINI_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include "IniSection.hpp"

class ReadIni
{
public:
    static std::vector<IniSection> Read(const std::string& path)
    {
        std::vector<IniSection> ret;
        std::ifstream file(path);
        std::string line;
        IniSection* currentSection = nullptr;

        while (std::getline(file, line))
        {
            if (line.front() == '[' && line.back() == ']')
            {
                if (currentSection != nullptr)
                    ret.push_back(*currentSection);
                currentSection = new IniSection(cleanBrackets(line));
            }
            else if (line.find('=') != std::string::npos)
            {
                size_t n = line.find('=');
                std::string key = line.substr(0, n);
                if (currentSection != nullptr)
                    currentSection->Items[key] = line.substr(n + 1);
            }
        }
        if (currentSection != nullptr)
            ret.push_back(*currentSection);

        return ret;
    }

private:
    static std::string cleanBrackets(const std::string& line)
    {
        return line.substr(1, line.size() - 2);
    }
};

#endif // READINI_HPP
