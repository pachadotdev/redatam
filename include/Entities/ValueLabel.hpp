#ifndef REDATAMLIB_VALUELABEL_HPP
#define REDATAMLIB_VALUELABEL_HPP

#include <string>

namespace RedatamLib {

class ValueLabel {
public:
  std::string Key;
  std::string Value;

  ValueLabel(const std::string &key, const std::string &value)
      : Key(key), Value(value) {}
};

} // namespace RedatamLib

#endif // REDATAMLIB_VALUELABEL_HPP
