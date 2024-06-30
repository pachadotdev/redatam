#pragma once

#include <string>
#include <vector>
#include <memory>
#include <variant>
#include "Entity.hpp" // Ensure this is uncommented if dependencies are resolved
#include "CursorReader.hpp"

namespace RedatamLib {
class Variable {
private:
  std::unique_ptr<CursorReader> reader;
  // Other private members...

public:
  Variable();
  // Public member functions...
  inline std::string ResolveDataFilename();
  inline long CalculateCharSize();
  long CalculateBitsSize();
  long GetExpectedFileSize();
  std::string ToString() const;
  std::variant<std::string, uint64_t, int, double> GetData();
  bool FileSizeFails(long &expectedSize, long &actual);
  void OpenData();
  bool DataFileExists();
  void CloseData();
  // Other public members and functions...
};
}