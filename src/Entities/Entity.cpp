#include "Entity.hpp"
#include "RedatamDatabase.hpp"
#include <fstream>
#include <sstream>

namespace RedatamLib {
Entity::Entity() {}

Entity::Entity(const std::string &name, const std::string &parentName)
    : Name(name), RelationParent(parentName) {}

std::string Entity::GetData() const { return this->Name; }

std::string Entity::ResolveDataFilename() const {
  return RedatamDatabase::OptimisticCombine(this->rootPath,
                                            this->IndexFilename);
}

void Entity::OpenPointer() {
  if (!this->IndexFilename.empty()) {
    filename = this->ResolveDataFilename();
    reader = std::make_shared<CursorReader>(filename, false, false, 16);
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
    if (parentEntity && parentEntity->RowsCount > 0) {
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
    if (!parent) {
      ret.emplace_back("", e->Name);
    } else {
      ret.emplace_back(parent->Name, e->Name);
    }
    auto children = Linealize(e, e->Children);
    if (!children.empty()) {
      ret.insert(ret.end(), children.begin(), children.end());
    }
  }
  return ret;
}

bool Entity::DataFileExists() const {
  std::string file = ResolveDataFilename();
  std::ifstream infile(file);
  return infile.good();
}

std::vector<std::shared_ptr<Variable>> Entity::SelectedVariables() const {
  std::vector<std::shared_ptr<Variable>> selected;
  for (const auto &v : Variables) {
    if (v->Selected) {
      selected.push_back(v);
    }
  }
  return selected;
}
} // namespace RedatamLib
