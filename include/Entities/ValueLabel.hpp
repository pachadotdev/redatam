#ifndef REDATAMLIB_VALUELABEL_HPP
#define REDATAMLIB_VALUELABEL_HPP

#include <string>

namespace RedatamLib {

class ValueLabel {
public:
  ValueLabel() = default;
  ValueLabel(const std::string &key, const std::string &value);

  std::string GetKey() const;
  std::string GetValue() const;
  void SetKey(const std::string &key);
  void SetValue(const std::string &value);

private:
  std::string Key;
  std::string Value;
};

} // namespace RedatamLib

#endif // REDATAMLIB_VALUELABEL_HPP
