#ifndef REDATAMLIB_ENTITY_HPP
#define REDATAMLIB_ENTITY_HPP

#include <string>
#include <vector>
#include <memory>

namespace RedatamLib {

class Variable;
class ICursorReader;

class Entity {
public:
  Entity();
  Entity(const std::string &name, const std::string &parentName);

  std::string Name;
  std::vector<std::shared_ptr<Entity>> Children;
  std::vector<std::shared_ptr<Variable>> Variables;
  long RowsCount;
  std::string IndexFilename;
  std::string rootPath;

  std::string GetData() const;
  std::string ResolveDataFilename() const;
  void OpenPointer();
  void ClosePointer();
  long CalculateRowCount(std::shared_ptr<Entity> parentEntity);
  bool DataFileExists() const;

  static std::vector<std::pair<std::string, std::string>>
  Linealize(std::shared_ptr<Entity> parent,
            const std::vector<std::shared_ptr<Entity>> &entitiesNames);

private:
  std::shared_ptr<ICursorReader> reader;
};

} // namespace RedatamLib

#endif // REDATAMLIB_ENTITY_HPP
