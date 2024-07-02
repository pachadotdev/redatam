#ifndef REDATAMLIB_NULLCURSORREADER_HPP
#define REDATAMLIB_NULLCURSORREADER_HPP

#include "ICursorReader.hpp"
#include <stdexcept>
#include <string>

namespace RedatamLib {

class NullCursorReader : public ICursorReader {
public:
  void Close() override {}

  bool IsLastPos() const override { return false; }

  void Open() override {}

  double ReadDouble() override { throw std::logic_error("Not implemented"); }

  uint16_t ReadInt16() override { throw std::logic_error("Not implemented"); }

  uint32_t ReadInt32() override { return 0; }

  uint32_t ReadInt32At(int64_t pos) override { return 0; }

  uint32_t ReadLastInt32() override { return 0; }

  uint64_t ReadNumber() override { return 0; }

  std::string ReadString() override { return "0"; }

  uint32_t ReadByte() override { return 0; }

  int64_t Length() const override { return 0; }
};

} // namespace RedatamLib

#endif // REDATAMLIB_NULLCURSORREADER_HPP
