#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <fstream>
#include "Variable.hpp"
#include "CursorReader.hpp" // Assuming CursorReader is similar to the C# version

namespace RedatamLib {

class Entity {
private:
  std::string Name;
  std::vector<std::shared_ptr<Variable>> Variables;
  std::string IndexFilename;
  std::unique_ptr<CursorReader> reader;
  int64_t RowsCount;
  std::vector<std::shared_ptr<Entity>> Children;

public:
  std::string rootPath;
  std::string Description;
  std::string RelationChild;
  std::string RelationParent;
  std::string CodesVariable;
  std::string LabelVariable;
  int Level;
  uint8_t b1;
  int s1;
  int VariableCount;

  Entity() : RowsCount(0), Level(0), b1(0), s1(0), VariableCount(0) {}

  void setName(const std::string &name) { Name = name; }

  std::string getName() const { return Name; }

  void setIndexFilename(const std::string &indexFilename) {
    IndexFilename = indexFilename;
  }

  const std::string &getIndexFilename() const { return IndexFilename; }

  std::vector<std::shared_ptr<Entity>> &getChildren() { return Children; }

  void setChildren(const std::vector<std::shared_ptr<Entity>> &children) {
    Children = children;
  }

  std::vector<std::shared_ptr<Variable>> &getVariables() { return Variables; }

  const std::vector<std::shared_ptr<Variable>> &getVariables() const {
    return Variables;
  }

  void setVariables(const std::vector<std::shared_ptr<Variable>> &variables) {
    Variables = variables;
  }

  void addVariable(const std::shared_ptr<Variable> &variable) {
    Variables.push_back(variable);
  }

  std::string ToString() const { return Name; }

  std::vector<std::shared_ptr<Variable>> SelectedVariables() const {
    std::vector<std::shared_ptr<Variable>> selected;
    std::copy_if(
        Variables.begin(), Variables.end(), std::back_inserter(selected),
        [](const std::shared_ptr<Variable> &v) { return v->isSelected(); });
    return selected;
  }

  void OpenPointer() {
    if (!IndexFilename.empty()) {
      std::string file = ResolveDataFilename();
      reader = std::make_unique<CursorReader>(file, 16);
    } else {
      reader = std::make_unique<NullCursorReader>();
    }
    reader->Open();
  }

  std::string ResolveDataFilename() {
    if (!IndexFilename.empty())
      return RedatamDatabase::OptimisticCombine(rootPath, IndexFilename);
    else
      return "";
  }

  bool HasData() const { return !reader->IsLastPos(); }

  int GetPointerData() const { return static_cast<int>(reader->ReadInt32()); }

  void ClosePointer() { reader->Close(); }

  int64_t CalculateRowCount(std::shared_ptr<Entity> parentEntity) {
    int64_t ret = 0;
    if (reader->Length() > 0) {
      int64_t pos = 1;
      if (parentEntity != nullptr && parentEntity->RowsCount > 0) {
        pos = parentEntity->RowsCount;
      }
      ret = reader->ReadInt32At(pos);
    }
    RowsCount = ret;
    return ret;
  }

  static std::vector<std::pair<std::string, std::string>>
  Linealize(std::shared_ptr<Entity> parent,
            const std::vector<std::shared_ptr<Entity>> &entitiesNames) {
    std::vector<std::pair<std::string, std::string>> ret;
    for (const auto &e : entitiesNames) {
      if (parent == nullptr)
        ret.emplace_back("", e->getName());
      else
        ret.emplace_back(parent->getName(), e->getName());
      auto children = Linealize(e, e->getChildren());
      ret.insert(ret.end(), children.begin(), children.end());
    }
    return ret;
  }

  bool DataFileExists() const {
    std::string file = ResolveDataFilename();
    return std::ifstream(file).good();
  }
};

} // namespace RedatamLib

#endif // ENTITY_HPP