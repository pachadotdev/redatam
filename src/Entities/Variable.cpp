#include "Variable.hpp"
#include "Entity.hpp"
#include "CursorReader.hpp"
#include "NullCursorReader.hpp"
#include <fstream>
#include <stdexcept>
#include <filesystem>
#include <memory>

namespace RedatamLib {

Variable::Variable(Entity *entity) : entity(entity) {}

Variable::Variable(const std::string &name, const std::string &type,
                   const std::string &label)
    : Name(name), Label(label), Type(type) {}

std::string Variable::GetData() const {
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

void Variable::OpenData() {
  if (DataFileExists()) {
    std::string file = ResolveDataFilename();
    reader = std::make_unique<CursorReader>(file, Type == "STRING",
                                            BinaryDataSet, Size);
    reader->Open();
  } else {
    reader = std::make_unique<NullCursorReader>();
  }
}

bool Variable::DataFileExists() const {
  std::string file = ResolveDataFilename();
  return std::filesystem::exists(file);
}

std::string Variable::ResolveDataFilename() const {
  return std::filesystem::path(entity->GetRootPath()) /
         Filename.substr(1, Filename.size() - 2);
}

void Variable::CloseData() { reader->Close(); }

long Variable::CalculateCharSize() const {
  long entityRows = entity->RowsCount;
  long bytes = Size * entityRows;
  return bytes;
}

long Variable::CalculateBitsSize() const {
  long entityRows = entity->RowsCount;
  long bits = Size * entityRows;
  long bytes = bits / 8;
  if (bits % 8 > 0)
    bytes++;
  return bytes;
}

long Variable::GetExpectedFileSize() const {
  if (Type == "STRING") {
    return CalculateCharSize();
  } else if (Type == "INTEGER" || Type == "INT16" || Type == "REAL") {
    return CalculateBitsSize();
  } else {
    throw std::runtime_error("Unsupported data type: " + Type);
  }
}

bool Variable::FileSizeFails(long &expectedSize, long &actual) const {
  expectedSize = GetExpectedFileSize();
  actual = reader->Length();
  return expectedSize > actual;
}

} // namespace RedatamLib
