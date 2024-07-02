#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <string>
#include <vector>
#include <memory>
#include "CursorReader.hpp"
#include "NullCursorReader.hpp"
#include "Variable.hpp"

namespace RedatamLib {
class Entity {
public:
  Entity();
  Entity(const std::string &name, const std::string &parentName);

  std::string GetData() const;
  std::string ResolveDataFilename() const;
  void OpenPointer();
  void ClosePointer();
  long CalculateRowCount(std::shared_ptr<Entity> parentEntity);
  static std::vector<std::pair<std::string, std::string>>
  Linealize(std::shared_ptr<Entity> parent,
            const std::vector<std::shared_ptr<Entity>> &entitiesNames);
  bool DataFileExists() const;
  std::vector<std::shared_ptr<Variable>> SelectedVariables() const;

  std::string rootPath;
  std::string Name;
  std::vector<std::shared_ptr<Entity>> Children;
  std::string Description;
  std::string IndexFilename;
  int i1;
  std::string Alias;
  int s1;
  char b1;
  std::string RelationChild;
  std::string RelationParent;
  std::string CodesVariable;
  std::string LabelVariable;
  int Level;
  long RowsCount;
  int VariableCount;
  std::string c1;
  std::vector<std::shared_ptr<Variable>> Variables;
  std::string filename;

private:
  std::shared_ptr<ICursorReader> reader;
};
} // namespace RedatamLib

#endif // ENTITY_HPP
