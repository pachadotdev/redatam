#ifndef VARIABLE_HPP
#define VARIABLE_HPP

#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <variant>
#include <exception>
#include "tinyxml2.h"
#include "ValueLabel.hpp"
#include "ICursorReader.hpp"
#include "CursorReader.hpp"
#include "NullCursorReader.hpp"
#include "Entity.hpp"

namespace RedatamLib {

class Variable {
private:
  std::unique_ptr<ICursorReader> reader;
  std::shared_ptr<Entity> entity;
  std::string Name;
  std::string Label;
  std::string Type;
  std::string Range;
  int Decimals = 0;
  std::string Declaration;
  std::string Group;
  std::string ValuesLabelsRaw;
  std::vector<ValueLabel> ValueLabels;
  std::string Filter;
  int Size = 0;
  std::string Filename;
  bool BinaryDataSet = false;
  bool Selected = true;

  std::string ResolveDataFilename() {
    return Entity::OptimisticCombine(
        entity->GetRootPath(),
        Filename.erase(std::remove(Filename.begin(), Filename.end(), '\''),
                       Filename.end()));
  }

  long CalculateCharSize() {
    long entityRows = entity->RowsCount();
    long bytes = Size * entityRows;
    return bytes;
  }

  long CalculateBitsSize() {
    long entityRows = entity->RowsCount();
    long bits = Size * entityRows;
    long bytes = bits / 8;
    if (bits % 8 > 0)
      bytes++;
    return bytes;
  }

  long GetExpectedFileSize() {
    if (Type == "STRING") {
      return CalculateCharSize();
    } else if (Type == "INTEGER" || Type == "INT16" || Type == "REAL") {
      return CalculateBitsSize();
    } else {
      throw std::runtime_error("Unsupported data type: " + Type);
    }
  }

public:
  explicit Variable(const std::shared_ptr<Entity> &entity) : entity(entity) {}

  Variable(const std::string &name, const std::string &type,
           const std::string &label)
      : Name(name), Type(type), Label(label) {}

  std::string ToString() const { return Name; }

  std::variant<std::string, uint64_t, int, double> GetData() {
    if (Type == "STRING") {
      return reader->ReadString();
    } else if (Type == "INTEGER") {
      return reader->ReadNumber();
    } else if (Type == "INT16") {
      return reader->ReadInt16();
    } else if (Type == "REAL") {
      return reader->ReadDouble();
    } else {
      throw std::runtime_error("Unsupported data type: " + Type);
    }
  }

  bool FileSizeFails(long &expectedSize, long &actual) {
    expectedSize = GetExpectedFileSize();
    actual = reader->Length();
    return expectedSize > actual;
  }

  void OpenData() {
    if (DataFileExists()) {
      std::string file = ResolveDataFilename();
      reader = std::make_unique<CursorReader>(file, Type == "STRING",
                                              BinaryDataSet, Size);
      reader->Open();
    } else {
      reader = std::make_unique<NullCursorReader>();
    }
  }

  bool DataFileExists() {
    std::string file = ResolveDataFilename();
    return std::filesystem::exists(file);
  }

  void CloseData() { reader->Close(); }
};

} // namespace RedatamLib

#endif // VARIABLE_HPP
