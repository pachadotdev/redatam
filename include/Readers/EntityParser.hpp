#ifndef REDATAMLIB_ENTITYPARSER_HPP
#define REDATAMLIB_ENTITYPARSER_HPP

#include <memory>
#include <vector>
#include <unordered_map>
#include "RedatamDatabase.hpp"
#include "DataBlock.hpp"
#include "Entity.hpp"
#include "MsDOSEncoder.hpp"

namespace RedatamLib {

class EntityParser {
public:
  EntityParser(std::shared_ptr<RedatamDatabase> db);

  void Parse(const std::string &fileName);
  std::vector<std::shared_ptr<Entity>>
  ParseEntities(std::shared_ptr<Entity> parent,
                const std::vector<std::shared_ptr<Entity>> &entityNames,
                std::unordered_map<std::string, DataBlock> &dataParts);
  bool checkDataType(DataBlock &dataBlock);
  std::unordered_map<std::string, DataBlock>
  SplitDataBlocks(DataBlock &dataBlock,
                  const std::vector<std::shared_ptr<Entity>> &entityNames);
  int ParseBeginning(DataBlock &dataBlock, const std::string &entity,
                     const std::string &parent);

private:
  std::shared_ptr<RedatamDatabase> db;

  bool
  JumpToDataSet(DataBlock &dataBlock); // Assuming this is a method from C# code
};

} // namespace RedatamLib

#endif // REDATAMLIB_ENTITYPARSER_HPP
