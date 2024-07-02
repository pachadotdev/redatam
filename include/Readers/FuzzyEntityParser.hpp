#ifndef REDATAMLIB_FUZZYENTITYPARSER_HPP
#define REDATAMLIB_FUZZYENTITYPARSER_HPP

#include <vector>
#include <string>
#include <memory>
#include <filesystem>
#include "DataBlock.hpp"
#include "Entity.hpp"

namespace RedatamLib {

class RedatamDatabase; // Forward declaration

class FuzzyEntityParser {
public:
  FuzzyEntityParser(RedatamDatabase *db);

  void ParseEntities(const std::string &path);

private:
  RedatamDatabase *db;

  bool ends_with(const std::string &str, const std::string &suffix);
  void TryEntities(DataBlock &dataBlock, const std::string &parent,
                   std::vector<std::shared_ptr<Entity>> &entitiesNames);
  bool checkEntityStart(DataBlock &dataBlock);
  void ProcessOcurrence(DataBlock &dataBlock,
                        std::vector<std::shared_ptr<Entity>> &leaves,
                        std::vector<std::shared_ptr<Entity>> &entitiesNames);
  std::vector<std::shared_ptr<Entity>>
  GetBest(const std::vector<std::vector<std::shared_ptr<Entity>>> &leaves);
  int CalculateTreeSize(const std::vector<std::shared_ptr<Entity>> &list);
};

} // namespace RedatamLib

#endif // REDATAMLIB_FUZZYENTITYPARSER_HPP
