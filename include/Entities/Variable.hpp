#ifndef REDATAMLIB_VARIABLE_HPP
#define REDATAMLIB_VARIABLE_HPP

#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <filesystem>
#include <memory>
#include "CursorReader.hpp"
#include "NullCursorReader.hpp"
#include "ValueLabel.hpp"

namespace RedatamLib {

class Entity; // Forward declaration

class Variable {
public:
  Variable(Entity *entity);
  Variable(const std::string &name, const std::string &type,
           const std::string &label);

  std::string GetData() const;
  void OpenData();
  bool DataFileExists() const;
  std::string ResolveDataFilename() const;
  void CloseData();
  long CalculateCharSize() const;
  long CalculateBitsSize() const;
  long GetExpectedFileSize() const;
  bool FileSizeFails(long &expectedSize, long &actual) const;

  std::string Name;
  std::string Label;
  std::string Type;
  std::string Range;
  int Decimals;
  std::string Declaration;
  std::string Group;
  std::string ValuesLabelsRaw;
  std::vector<ValueLabel> ValueLabels;
  std::string Filter;
  int Size;
  std::string Filename;
  bool BinaryDataSet = false;
  bool Selected = true;

private:
  Entity *entity;
  std::unique_ptr<ICursorReader> reader;
};

} // namespace RedatamLib

#endif // REDATAMLIB_VARIABLE_HPP
