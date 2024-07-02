#include "Variable.hpp"
#include "Entity.hpp"
#include "CursorReader.hpp"
#include "NullCursorReader.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <any>

namespace RedatamLib {
Variable::Variable(Entity *entity) : Selected(true), entity(entity) {}

Variable::Variable(const std::string &name, const std::string &type,
                   const std::string &filename)
    : Name(name), Type(type), Filename(filename), Selected(true) {}

std::string Variable::ResolveDataFilename() const {
  std::string filename = Filename;
  size_t pos = filename.find("'");
  if (pos != std::string::npos) {
    filename.replace(pos, 1, "");
  }
  return std::filesystem::path(entity->rootPath) / filename;
}

bool Variable::FileSizeFails(long &expectedSize, long &actual) const {
  expectedSize = GetExpectedFileSize();
  actual = reader->Length();
  return expectedSize > actual;
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
  std::ifstream infile(file);
  return infile.good();
}

void Variable::CloseData() { reader->Close(); }

long Variable::CalculateCharSize() const { return Size * entity->RowsCount; }

long Variable::CalculateBitsSize() const {
  long bits = Size * entity->RowsCount;
  return (bits / 8) + ((bits % 8) > 0 ? 1 : 0);
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

std::any Variable::GetData() {
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
} // namespace RedatamLib
