#ifndef REDATAMLIB_REDATAMDATABASE_HPP
#define REDATAMLIB_REDATAMDATABASE_HPP

#include <vector>
#include <memory>
#include <string>
#include <filesystem>
#include "Entity.hpp"
#include "FuzzyEntityParser.hpp"
#include "XmlEntityParser.hpp"

namespace RedatamLib {

class RedatamDatabase : public std::enable_shared_from_this<RedatamDatabase> {
public:
  RedatamDatabase();

  // Disable copy operations
  RedatamDatabase(const RedatamDatabase &) = delete;
  RedatamDatabase &operator=(const RedatamDatabase &) = delete;

  const std::vector<std::shared_ptr<Entity>> &GetEntities() const;
  long GetTotalDataItems();
  long GetEntitiesTotalDataItems(
      const std::vector<std::shared_ptr<Entity>> &entities,
      std::shared_ptr<Entity> parent);
  void OpenDictionary(const std::string &filename);
  void Close();

  static std::string OptimisticCombine(const std::string &path1,
                                       const std::string &path2) {
    return (std::filesystem::path(path1) / path2).string();
  }

private:
  void CloseEntity(const std::shared_ptr<Entity> &entity);

  std::vector<std::shared_ptr<Entity>> Entities;
  std::unique_ptr<FuzzyEntityParser> fuzzyEntityParserInstance;
  std::unique_ptr<XmlEntityParser> xmlEntityParserInstance;
};

} // namespace RedatamLib

#endif // REDATAMLIB_REDATAMDATABASE_HPP
