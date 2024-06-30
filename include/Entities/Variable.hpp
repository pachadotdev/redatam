#ifndef REDATAMLIB_VARIABLE_HPP
#define REDATAMLIB_VARIABLE_HPP

#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <filesystem>

namespace RedatamLib {

class Variable {
public:
  Variable(Entity *entity) : entity(entity) {}

  Variable(const std::string &name, const std::string &type,
           const std::string &label)
      : Name(name), Type(type), Label(label) {}

  std::string ToString() const { return Name; }

  auto GetData() const {
    if (Type == "STRING") {
      return reader.ReadString();
    } else if (Type == "INTEGER") {
      return reader.ReadNumber();
    } else if (Type == "INT16") {
      return reader.ReadInt16();
    } else if (Type == "REAL") {
      return reader.ReadDouble();
    } else {
      throw std::runtime_error("Unsupported data type: " + Type);
    }
  }

  long CalculateCharSize() const {
    long entityRows = entity->RowsCount();
    long bytes = Size * entityRows;
    return bytes;
  }

  long CalculateBitsSize() const {
    long entityRows = entity->RowsCount();
    long bits = Size * entityRows;
    long bytes = bits / 8;
    if (bits % 8 > 0)
      bytes++;
    return bytes;
  }

  long GetExpectedFileSize() const {
    if (Type == "STRING") {
      return CalculateCharSize();
    } else if (Type == "INTEGER" || Type == "INT16" || Type == "REAL") {
      return CalculateBitsSize();
    } else {
      throw std::runtime_error("Unsupported data type: " + Type);
    }
  }

  bool FileSizeFails(long &expectedSize, long &actual) const {
    expectedSize = GetExpectedFileSize();
    actual = reader.Length();
    return expectedSize > actual;
  }

  void OpenData() {
    if (DataFileExists()) {
      std::string file = ResolveDataFilename();
      reader.Open(file, Type == "STRING", BinaryDataSet, Size);
    } else {
      reader = NullCursorReader();
    }
  }

  bool DataFileExists() const {
    std::string file = ResolveDataFilename();
    return std::filesystem::exists(file);
  }

  std::string ResolveDataFilename() const {
    return RedatamDatabase::OptimisticCombine(entity->rootPath, Filename);
  }

  void CloseData() {
    reader.Close();
  }

private:
  Entity *entity;
  std::string Name;
  std::string Label;
  std::string Type;
  std::string Range;
  int Decimals;
  std::string Declaration;
  std::string Group;
  std::string ValuesLabelsRaw;
  std::vector<std::string> ValueLabels;
  std::string Filter;
  int Size;
  std::string Filename;
  bool BinaryDataSet = false;
  bool Selected = true;
  CursorReader reader;
};

} // namespace RedatamLib

#endif // REDATAMLIB_VARIABLE_HPP
