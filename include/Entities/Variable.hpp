#ifndef REDATAMLIB_VARIABLE_HPP
#define REDATAMLIB_VARIABLE_HPP

#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <filesystem>
#include <memory>
#include "CursorReader.hpp"
#include "Entity.hpp"
#include "NullCursorReader.hpp"
#include "ValueLabel.hpp"

namespace RedatamLib {

// class Entity;

class Variable {
public:
  Variable(Entity *entity) : entity(entity), reader(nullptr) {}

  Variable(const std::string &name, const std::string &type,
           const std::string &label)
      : Name(name), Type(type), Label(label), reader(nullptr) {}

  std::string GetData() const {
    if (Type == "STRING") {
      return reader->ReadString();
    } else if (Type == "INTEGER") {
      return std::to_string(reader->ReadNumber());
    } else if (Type == "INT16") {
      return std::to_string(reader->ReadInt16());
    } else if (Type == "REAL") {
      return std::to_string(reader->ReadDouble());
    } else {
      throw std::runtime_error("Unsupported data type: " + Type);
    }
  }

  void OpenData() {
    if (BinaryDataSet) {
      reader = std::make_unique<CursorReader>(ResolveDataFilename());
    } else {
      reader = std::make_unique<NullCursorReader>();
    }
  }

  std::string ResolveDataFilename() const {
    if (!std::filesystem::exists(Filename)) {
      throw std::runtime_error("File not found: " + Filename);
    }
    return Filename;
  }

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
  std::unique_ptr<CursorReader> reader;

  long CalculateCharSize() const {
    long entityRows = this->entity->RowsCount;
    long bytes = this->Size * entityRows;
    return bytes;
  }
};

} // namespace RedatamLib

#endif // REDATAMLIB_VARIABLE_HPP
