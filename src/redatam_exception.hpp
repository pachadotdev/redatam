#pragma once

#include <string>

namespace Redatam {
class Exception : public std::exception {
public:
  Exception(const std::string &msg) : msg(msg) {}
  Exception(std::string &&msg) : msg(std::move(msg)) {}

  const char *what() const noexcept override { return msg.c_str(); }

private:
  std::string msg;
};
} // namespace Redatam
