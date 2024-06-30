#ifndef REDATAMDATABASE_HPP
#define REDATAMDATABASE_HPP

#include <memory>
#include <vector>
#include <string>
#include "Entity.hpp"

namespace RedatamLib {
    
// Forward declarations to avoid circular dependency
class FuzzyEntityParser;
class XmlEntityParser;

class RedatamDatabase {
public:
    std::vector<std::shared_ptr<Entity>> entityNames;
    std::vector<std::shared_ptr<Entity>> Entities;
    std::unique_ptr<FuzzyEntityParser> fuzzyEntityParserInstance;
    std::unique_ptr<XmlEntityParser> xmlEntityParserInstance;
    std::string DictionaryFile;

    RedatamDatabase();

    long GetTotalDataItems();
    long GetTotalRowsSize();
    void OpenDictionary(const std::string& file);
    std::vector<std::shared_ptr<Entity>> GetEntitiesList();

private:
    long GetEntitiesTotalDataItems(const std::vector<std::shared_ptr<Entity>>& entities, std::shared_ptr<Entity> parent);
    long GetEntitiesRowsSize(const std::vector<std::shared_ptr<Entity>>& entities, std::shared_ptr<Entity> parent);
    void doGetEntitiesList(std::vector<std::shared_ptr<Entity>>& ret, const std::vector<std::shared_ptr<Entity>>& list);

    static std::string OptimisticCombine(const std::string& path, const std::string& file);
};

} // namespace RedatamLib

#endif // REDATAMDATABASE_HPP
