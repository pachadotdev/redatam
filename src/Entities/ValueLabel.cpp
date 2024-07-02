#include "ValueLabel.hpp"

namespace RedatamLib {

ValueLabel::ValueLabel(const std::string &key, const std::string &value)
    : Key(key), Value(value) {}

std::string ValueLabel::GetKey() const { return Key; }

std::string ValueLabel::GetValue() const { return Value; }

void ValueLabel::SetKey(const std::string &key) { Key = key; }

void ValueLabel::SetValue(const std::string &value) { Value = value; }

} // namespace RedatamLib
