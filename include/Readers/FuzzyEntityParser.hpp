#ifndef FUZZYENTITYPARSER_HPP
#define FUZZYENTITYPARSER_HPP

#include <vector>
#include <string>
#include <memory>
#include <filesystem>
#include "DataBlock.hpp"
#include "Entity.hpp"
#include "RedatamDatabase.hpp"

namespace RedatamLib {

class FuzzyEntityParser {
private:
  RedatamDatabase *db;

  bool ends_with(const std::string &str, const std::string &suffix) {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
                   ::tolower);
    std::string lowerSuffix = suffix;
    std::transform(lowerSuffix.begin(), lowerSuffix.end(), lowerSuffix.begin(),
                   ::tolower);
    return lowerStr.size() >= lowerSuffix.size() &&
           lowerStr.compare(lowerStr.size() - lowerSuffix.size(),
                            lowerSuffix.size(), lowerSuffix) == 0;
  }

  void TryEntities(DataBlock &dataBlock, const std::string &parent,
                   std::vector<std::shared_ptr<Entity>> &entitiesNames) {
    std::string entityNameString;
    if (!dataBlock.PlausibleString(entityNameString) ||
        entityNameString.empty())
      return;

    if (!checkEntityStart(dataBlock))
      return;

    auto entity = std::make_shared<Entity>();
    entity->setName(entityNameString);
    entitiesNames.push_back(entity);

    auto block = dataBlock.makeStringBlock(entityNameString);
    auto allBlockOccurrences = dataBlock.GetAllMatches(block);

    for (auto startN : allBlockOccurrences) {
      dataBlock.n = startN;
      ProcessOcurrence(dataBlock, entitiesNames, entity->getChildren());
    }
  }

  bool checkEntityStart(DataBlock &dataBlock) {
    std::string relationChild, description, indexfilename;
    if (!dataBlock.PlausibleString(relationChild) || relationChild.empty())
      return false;

    if (!dataBlock.PlausibleString(description, false) ||
        !dataBlock.PlausibleString(indexfilename, false))
      return false;

    return ends_with(description, ".ptr") || ends_with(indexfilename, ".ptr");
  }

  void ProcessOcurrence(DataBlock &dataBlock,
                        std::vector<std::shared_ptr<Entity>> &leaves,
                        std::vector<std::shared_ptr<Entity>> &entitiesNames) {
    if (dataBlock.moveBackString() == -1)
      return;

    int keepN = dataBlock.n;
    std::string child = dataBlock.eatShortString();
    dataBlock.n = keepN;
    if (!entitiesNames.empty() && entitiesNames.back()->getName() == child)
      return;
    if (!dataBlock.IsText(child) || child.empty())
      return;

    std::vector<std::shared_ptr<Entity>> names;
    TryEntities(dataBlock, child, names);
    if (!names.empty())
      leaves.insert(leaves.end(), names.begin(), names.end());
  }

  std::vector<std::shared_ptr<Entity>>
  GetBest(const std::vector<std::vector<std::shared_ptr<Entity>>> &leaves) {
    if (leaves.empty())
      return {};
    int max = 0;
    size_t nMax = 0;
    for (size_t n = 0; n < leaves.size(); n++) {
      int current = CalculateTreeSize(leaves[n]);
      if (current > max) {
        max = current;
        nMax = n;
      }
    }
    return leaves[nMax];
  }

  int CalculateTreeSize(const std::vector<std::shared_ptr<Entity>> &list) {
    int ret = 0;
    for (const auto &entity : list) {
      ret++;
      ret += CalculateTreeSize(entity->getChildren());
    }
    return ret;
  }

public:
  FuzzyEntityParser(RedatamDatabase *db) : db(db) {}

  void ParseEntities(const std::string &path) {
    std::vector<std::vector<std::shared_ptr<Entity>>> candidates;

    DataBlock dataBlock(path);

    for (size_t i = 0; i < dataBlock.data.size(); i++) {
      dataBlock.n = i;
      std::vector<std::shared_ptr<Entity>> entitiesNames;
      TryEntities(dataBlock, "", entitiesNames);
      if (!entitiesNames.empty())
        candidates.push_back(entitiesNames);
    }
    db->entityNames = GetBest(candidates);
  }
};

} // namespace RedatamLib

#endif // FUZZYENTITYPARSER_HPP
