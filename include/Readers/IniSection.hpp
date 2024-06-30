#ifndef INISECTION_HPP
#define INISECTION_HPP

#include <string>
#include <unordered_map>

namespace RedatamLib {

class IniSection
{
public:
    std::string Name;
    std::unordered_map<std::string, std::string> Items;

    IniSection(const std::string& key) : Name(key) {}

    std::string SafeGet(const std::string& key, const std::string& defaultValue = "") const
    {
        auto it = Items.find(key);
        if (it == Items.end())
            return defaultValue;
        else
            return it->second;
    }
};

}

#endif // INISECTION_HPP
