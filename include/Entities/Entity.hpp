#pragma once

#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <fstream>
#include "Variable.hpp"
#include "CursorReader.hpp"

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

  Entity();
  void setName(const std::string &name);
  std::string getName() const;
  void setIndexFilename(const std::string &indexFilename);
  const std::string &getIndexFilename() const;
  std::vector<std::shared_ptr<Entity>> &getChildren();
  void setChildren(const std::vector<std::shared_ptr<Entity>> &children);
  std::vector<std::shared_ptr<Variable>> &getVariables();
  const std::vector<std::shared_ptr<Variable>> &getVariables() const;
  void setVariables(const std::vector<std::shared_ptr<Variable>> &variables);
  void addVariable(const std::shared_ptr<Variable> &variable);
  std::string ToString() const;
  std::vector<std::shared_ptr<Variable>> SelectedVariables() const;
  void OpenPointer();
  std::string ResolveDataFilename();
  bool HasData() const;
  int GetPointerData() const;
  void ClosePointer();
  int64_t CalculateRowCount(std::shared_ptr<Entity> parentEntity);
  static std::vector<std::pair<std::string, std::string>>
  Linealize(std::shared_ptr<Entity> parent,
            const std::vector<std::shared_ptr<Entity>> &entitiesNames);
  bool DataFileExists() const;
};
} // namespace RedatamLib
