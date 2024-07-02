#ifndef VARIABLE_HPP
#define VARIABLE_HPP

#include <string>
#include <vector>
#include <memory>
#include <any>

namespace RedatamLib {
class Entity;
class ICursorReader;

class Variable {
public:
  Variable(Entity *entity);
  Variable(const std::string &name, const std::string &type,
           const std::string &filename);

  std::string ResolveDataFilename() const;
  bool FileSizeFails(long &expectedSize, long &actual) const;
  void OpenData();
  bool DataFileExists() const;
  void CloseData();
  long CalculateCharSize() const;
  long CalculateBitsSize() const;
  long GetExpectedFileSize() const;
  std::any GetData();

  std::string Name;
  std::string Type;
  std::string Filename;
  int Size;
  bool Selected;
  bool BinaryDataSet;

  std::string Label;
  std::vector<std::pair<std::string, std::string>> ValueLabels;

private:
  Entity *entity;
  std::unique_ptr<ICursorReader> reader;
};
} // namespace RedatamLib

#endif // VARIABLE_HPP
