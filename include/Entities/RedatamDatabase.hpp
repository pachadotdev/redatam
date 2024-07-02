#ifndef REDATAMLIB_REDATAMDATABASE_HPP
#define REDATAMLIB_REDATAMDATABASE_HPP

#include <memory>
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <filesystem>
#include "Entity.hpp"
#include "FuzzyEntityParser.hpp"
#include "XmlEntityParser.hpp"

namespace RedatamLib {

class RedatamDatabase {
public:
  RedatamDatabase() {
    fuzzyEntityParserInstance = std::make_unique<FuzzyEntityParser>(this);
    xmlEntityParserInstance = std::make_unique<XmlEntityParser>(this);
  }

  const std::vector<std::shared_ptr<Entity>> &GetEntities() const {
    return Entities;
  }
  
  long GetTotalDataItems() {
    return GetEntitiesTotalDataItems(Entities, nullptr);
  }

  long GetTotalRowsSize() { return GetEntitiesRowsSize(Entities, nullptr); }

  void OpenDictionary(const std::string &file) {
    std::string ext = std::filesystem::path(file).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == ".dic") {
      fuzzyEntityParserInstance->ParseEntities(file);
      try {
        EntityParser parser(
            std::make_shared<RedatamLib::RedatamDatabase>(*this));
        parser.Parse(file);
      } catch (const std::exception &e) {
        throw std::runtime_error("An error occurred while parsing the "
                                 "dictionary variables and labels (" +
                                 std::string(e.what()) + ").");
      }
    } else if (ext == ".dicx") {
      xmlEntityParserInstance->Parse(file);
    } else {
      throw std::runtime_error("Dictionary files must be .dic or .dicx files.");
    }
  }

  std::vector<std::shared_ptr<Entity>> GetEntitiesList() {
    std::vector<std::shared_ptr<Entity>> list;
    doGetEntitiesList(list, Entities);
    return list;
  }

private:
  std::vector<std::shared_ptr<Entity>> Entities;
  std::unique_ptr<FuzzyEntityParser> fuzzyEntityParserInstance;
  std::unique_ptr<XmlEntityParser> xmlEntityParserInstance;
  std::string DictionaryFile;

  long GetEntitiesTotalDataItems(
      const std::vector<std::shared_ptr<Entity>> &entities,
      std::shared_ptr<Entity> parent) {
    long ret = 0;
    for (auto &entity : entities) {
      ret += (entity->SelectedVariables().size() + 2) * entity->RowsCount;
      ret += GetEntitiesTotalDataItems(entity->Children, entity);
    }
    return ret;
  }

  long GetEntitiesRowsSize(const std::vector<std::shared_ptr<Entity>> &entities,
                           std::shared_ptr<Entity> parent) {
    long ret = 0;
    for (auto &entity : entities) {
      entity->OpenPointer();
      long entityCount = entity->CalculateRowCount(parent);
      ret += entityCount;
      entity->ClosePointer();
      ret += GetEntitiesRowsSize(entity->Children, entity);
    }
    return ret;
  }

  void doGetEntitiesList(std::vector<std::shared_ptr<Entity>> &ret,
                         const std::vector<std::shared_ptr<Entity>> &list) {
    for (auto &e : list) {
      ret.push_back(e);
      doGetEntitiesList(ret, e->Children);
    }
  }
};

} // namespace RedatamLib

#endif // REDATAMLIB_REDATAMDATABASE_HPP
