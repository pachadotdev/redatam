#ifndef VALUELABEL_HPP
#define VALUELABEL_HPP

#include <string>

class ValueLabel
{
public:
    std::string Key;
    std::string Value;

    ValueLabel(const std::string& key, const std::string& value)
        : Key(key), Value(value)
    {
    }
    
    ~ValueLabel() = default;
};

#endif // VALUELABEL_HPP
