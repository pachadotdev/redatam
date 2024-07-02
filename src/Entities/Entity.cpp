#include "Entity.hpp"
#include "CursorReader.hpp"
#include "NullCursorReader.hpp"
#include "RedatamDatabase.hpp"
#include "Variable.hpp"

namespace RedatamLib {

Entity::Entity() {}

Entity::Entity(const std::string &name, const std::string &parentName)
    : Name(name) {
  // Implementation...
}

std::string Entity::GetData() const {
  // Implement the logic based on C# code
  return "data";
}

std::string Entity::ResolveDataFilename() const {
  if (!this->IndexFilename.empty())
    return RedatamDatabase::OptimisticCombine(this->rootPath,
                                              this->IndexFilename);
  else
    return "";
}

void Entity::OpenPointer() {
  if (!this->IndexFilename.empty()) {
    std::string file = ResolveDataFilename();
    reader = std::make_shared<CursorReader>(file, 16);
  } else {
    reader = std::make_shared<NullCursorReader>();
  }
  reader->Open();
}

void Entity::ClosePointer() { reader->Close(); }

long Entity::CalculateRowCount(std::shared_ptr<Entity> parentEntity) {
  long ret = 0;
  if (reader->Length() > 0) {
    long pos = 1;
    if (parentEntity != nullptr) {
      if (parentEntity->RowsCount > 0)
        pos = parentEntity->RowsCount;
    }
    ret = reader->ReadInt32At(pos);
  }
  this->RowsCount = ret;
  return ret;
}

std::vector<std::pair<std::string, std::string>>
Entity::Linealize(std::shared_ptr<Entity> parent,
                  const std::vector<std::shared_ptr<Entity>> &entitiesNames) {
  std::vector<std::pair<std::string, std::string>> ret;
  for (const auto &e : entitiesNames) {
    if (parent == nullptr)
      ret.emplace_back("", e->Name);
    else
      ret.emplace_back(parent->Name, e->Name);
    auto children = Linealize(e, e->Children);
    if (!children.empty())
      ret.insert(ret.end(), children.begin(), children.end());
  }
  return ret;
}

bool Entity::DataFileExists() const {
  std::string file = ResolveDataFilename();
  return std::ifstream(file).good();
}

} // namespace RedatamLib
