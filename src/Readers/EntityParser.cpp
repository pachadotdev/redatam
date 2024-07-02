#include "EntityParser.hpp"

namespace RedatamLib {

EntityParser::EntityParser(std::shared_ptr<RedatamDatabase> db) : db(db) {}

void EntityParser::Parse(const std::string &fileName) {
  DataBlock dataBlock(fileName);
  auto dataParts = SplitDataBlocks(dataBlock, db->entityNames);
  auto parsedEntities = ParseEntities(nullptr, db->entityNames, dataParts);
  db->Entities.insert(db->Entities.end(), parsedEntities.begin(),
                      parsedEntities.end());

  MsDOSEncoder enc(db.get());
}

std::vector<std::shared_ptr<Entity>> EntityParser::ParseEntities(
    std::shared_ptr<Entity> parent,
    const std::vector<std::shared_ptr<Entity>> &entityNames,
    std::unordered_map<std::string, DataBlock> &dataParts) {
  std::vector<std::shared_ptr<Entity>> entities;
  for (const auto &entityName : entityNames) {
    std::string parentName = parent ? parent->Name : "";
    auto entity = std::make_shared<Entity>(entityName->Name, parentName);
    entities.push_back(entity);
  }
  return entities;
}

bool EntityParser::checkDataType(DataBlock &dataBlock) {
  if (dataBlock.n + 3 > static_cast<int>(dataBlock.data.size())) {
    return false;
  }
  return JumpToDataSet(dataBlock);
}

std::unordered_map<std::string, DataBlock> EntityParser::SplitDataBlocks(
    DataBlock &dataBlock,
    const std::vector<std::shared_ptr<Entity>> &entityNames) {
  std::unordered_map<std::string, DataBlock> dataParts;
  std::vector<std::pair<std::string, std::string>> linealEntityParentNames;
  int iStart = 0;
  for (size_t i = 0; i < entityNames.size(); ++i) {
    std::string entity = linealEntityParentNames[i].second;
    iStart = ParseBeginning(dataBlock, linealEntityParentNames[i].second,
                            linealEntityParentNames[i].first);
    dataParts[linealEntityParentNames[i - 1].second] =
        dataBlock.getPart(iStart, dataBlock.n);
  }
  iStart = static_cast<int>(dataBlock.data.size());
  dataParts[linealEntityParentNames.back().second] =
      dataBlock.getPart(iStart, static_cast<int>(dataBlock.data.size()));
  return dataParts;
}

int EntityParser::ParseBeginning(DataBlock &dataBlock,
                                 const std::string &entity,
                                 const std::string &parent) {
  auto block = dataBlock.makeString(entity);
  auto blockParent = dataBlock.makeString(parent);
  if (dataBlock.moveTo(block)) {
    dataBlock.move(-block.size());
  } else if (!parent.empty() && dataBlock.moveTo(blockParent)) {
    dataBlock.move(-blockParent.size());
  }
  return dataBlock.n;
}

bool EntityParser::JumpToDataSet(DataBlock &dataBlock) {
  if (!dataBlock.moveTo("\x00\x00\x00\x80")) {
    return false;
  }
  dataBlock.move(-2);
  if (!dataBlock.moveTo("\xFF\xFF\xFF\xFF")) {
    return false;
  }
  dataBlock.move(6);
  return true;
}

} // namespace RedatamLib
