#ifndef REDATAMDATABASE_HPP
#define REDATAMDATABASE_HPP

#include <vector>
#include <memory>
#include <string>

namespace RedatamLib {
class Entity;

class RedatamDatabase {
public:
  RedatamDatabase(const std::string &path);

  std::vector<std::shared_ptr<Entity>> getEntityNames() const;
  void addEntities(const std::vector<std::shared_ptr<Entity>> &entities);

  void LoadDatabase(const std::string &path);
  std::shared_ptr<Entity> FindEntity(const std::string &name) const;
  std::shared_ptr<Entity> GetRootEntity() const;
  std::vector<std::string>
  OptimisticCombine(const std::string &rootPath,
                    const std::vector<std::string> &paths) const;
  long GetEntitiesTotalDataItems(
      const std::vector<std::shared_ptr<Entity>> &entities,
      std::shared_ptr<Entity> rootEntity) const;

private:
  std::vector<std::shared_ptr<Entity>> entityNames;
  std::vector<std::shared_ptr<Entity>> Entities;
  std::string databasePath;

  void ParseDatabaseFile(const std::string &path);
};
} // namespace RedatamLib

#endif // REDATAMDATABASE_HPP
