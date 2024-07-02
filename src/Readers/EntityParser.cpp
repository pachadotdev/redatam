#include "EntityParser.hpp"
#include "RedatamDatabase.hpp"
#include "Entity.hpp"
#include "DataBlock.hpp"
#include <unordered_map>
#include <vector>
#include <string>

namespace RedatamLib {

void EntityParser::Parse(const std::string &dataBlock) {
  auto dataParts = SplitDataBlocks(dataBlock, db->getEntityNames());
  auto parsedEntities = ParseEntities(nullptr, db->getEntityNames(), dataParts);
  db->addEntities(parsedEntities);
}

bool EntityParser::checkDataType(DataBlock &dataBlock) {
  if (dataBlock.getN() + 3 > static_cast<int>(dataBlock.getData().size())) {
    return false;
  }
  return true;
}

std::unordered_map<std::string, DataBlock> EntityParser::SplitDataBlocks(
    DataBlock &dataBlock,
    const std::vector<std::shared_ptr<Entity>> &entityNames) {
  std::unordered_map<std::string, DataBlock> dataBlocks;
  int iStart = 0;

  for (const auto &entity : entityNames) {
    dataBlocks[entity->Name] = dataBlock.getPart(iStart, dataBlock.getN());
    iStart = static_cast<int>(dataBlock.getData().size());
  }

  dataBlocks[""] =
      dataBlock.getPart(iStart, static_cast<int>(dataBlock.getData().size()));
  return dataBlocks;
}

int EntityParser::ParseBeginning(DataBlock &dataBlock,
                                 const std::string &entity,
                                 const std::string &parent) {
  auto block = dataBlock.makeString(entity);
  auto blockParent = dataBlock.makeString(parent);

  if (!dataBlock.getData().empty()) {
    return dataBlock.getN();
  }
  return 0;
}

std::vector<std::shared_ptr<Entity>> EntityParser::ParseEntities(
    std::shared_ptr<Entity> parent,
    const std::vector<std::shared_ptr<Entity>> &entityNames,
    std::unordered_map<std::string, DataBlock> &dataParts) {
  std::vector<std::shared_ptr<Entity>> parsedEntities;

  for (const auto &entityName : entityNames) {
    auto entity = std::make_shared<Entity>();
    entity->Name = entityName->Name;
    entity->Parent = parent;

    if (dataParts.find(entity->Name) != dataParts.end()) {
      entity->Data = dataParts[entity->Name];
    }

    parsedEntities.push_back(entity);
  }

  return parsedEntities;
}

} // namespace RedatamLib
