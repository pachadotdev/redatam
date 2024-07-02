#ifndef REDATAMLIB_ENTITYPARSER_HPP
#define REDATAMLIB_ENTITYPARSER_HPP

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "DataBlock.hpp"
#include "Entity.hpp"
#include "MsDOSEncoder.hpp"

namespace RedatamLib {

class EntityParser {
public:
  EntityParser(std::shared_ptr<RedatamLib::RedatamDatabase> db) : db(db) {}

  void Parse(const std::string &path) {
    rootPath = std::filesystem::path(path).parent_path().string();
    DataBlock dataBlock(path);

    auto dataParts = SplitDataBlocks(dataBlock, db->entityNames);

    auto parsedEntities = ParseEntities(nullptr, db->entityNames, dataParts);
    db->Entities.insert(db->Entities.end(), parsedEntities.begin(),
                        parsedEntities.end());

    MsDOSEncoder enc(db);
    if (enc.RequiresProcessing()) {
      enc.ReencodeLabels();
    }
  }

private:
  std::shared_ptr<RedatamDatabase> db;
  std::string rootPath;
  static inline const std::vector<std::string> validTypes = {
      "dbl", "lng", "int", "bin", "pck", "chr"};

  std::vector<std::shared_ptr<Entity>>
  ParseEntities(std::shared_ptr<Entity> parent,
                const std::vector<std::shared_ptr<Entity>> &entityNames,
                std::unordered_map<std::string, DataBlock> &dataParts) {
    std::vector<std::shared_ptr<Entity>> ret;
    for (const auto &entityName : entityNames) {
      std::string parentName = parent ? parent->getName() : "";
      auto entity = ParseEntity(dataParts.at(entityName->getName()),
                                entityName->getName(), parentName);
      ret.push_back(entity);
      auto children =
          ParseEntities(entity, entityName->Children, dataParts);
      if (!children.empty()) {
        entity->setChildren(children);
      }
    }
    return ret;
  }

  std::shared_ptr<Entity> ParseEntity(DataBlock &dataBlock,
                                      const std::string &entity,
                                      const std::string &parent) {
    auto e = std::make_shared<Entity>();
    e->rootPath = rootPath;
    auto block = dataBlock.makeStringBlock(entity);
    auto blockParent = dataBlock.makeStringBlock(parent);
    std::vector<uint8_t> full;
    if (!parent.empty()) {
      full = dataBlock.addArrays(block, block, blockParent);
    } else {
      full = dataBlock.addArrays(block, blockParent);
    }

    if (!dataBlock.moveTo(full)) {
      throw std::runtime_error("Sequence not found.");
    }

    e->Name = dataBlock.eatShortString();
    e->RelationChild = dataBlock.eatShortString();
    if (!e->RelationChild.empty()) {
      e->RelationParent = dataBlock.eatShortString();
    }
    e->Description = dataBlock.eatShortString();
    e->IndexFilename = dataBlock.eatShortString();
    e->s1 = dataBlock.eat16int();
    if (!e->IndexFilename.empty()) {
      e->CodesVariable = dataBlock.eatShortString();
      e->LabelVariable = dataBlock.eatShortString();
      e->Level = dataBlock.eat32int();
      e->b1 = dataBlock.eatByte();
      while (true) {
        auto v = ParseVariable(dataBlock, e);
        if (v != nullptr) {
          e->Variables.push_back(v);
        } else {
          break;
        }
      }
    }
    e->VariableCount = e->Variables.size();
    return e;
  }

  std::shared_ptr<Variable> ParseVariable(DataBlock &dataBlock,
                                          std::shared_ptr<Entity> e) {
    if (!JumptToDataSet(dataBlock)) {
      return nullptr;
    }
    auto v = std::make_shared<Variable>(e);
    v->Name = dataBlock.eatShortString();
    v->Declaration = dataBlock.eatShortString();
    v->Filter = dataBlock.eatShortString();
    v->Range = dataBlock.eatShortString();
    v->Type = dataBlock.eatShortString();
    v->ValuesLabelsRaw = dataBlock.eatShortString();
    v->Label = dataBlock.eatShortString();
    v->Group = dataBlock.eatShortString();

    VariableParser parser(v);
    parser.ParseDeclaration();
    parser.ParseValueLabels();
    parser.ParseMissingAndPrecision();
    return v;
  }

  bool JumptToDataSet(DataBlock &dataBlock) {
    if (!dataBlock.moveTo("DATASET")) {
      return false;
    }
    if (!checkDataType(dataBlock)) {
      return false;
    }
    dataBlock.move(-2);
    if (dataBlock.moveBackString(32) < 1) {
      dataBlock.move(6);
      return JumptToDataSet(dataBlock);
    } else {
      return true;
    }
  }

  bool checkDataType(DataBlock &dataBlock) {
    dataBlock.move(8); // "DATASET "
    if (dataBlock.n + 3 > dataBlock.data.size()) {
      return false;
    }
    std::string type = dataBlock.eatChars(3); // "DBL", "LNG", etc
    if (std::find(validTypes.begin(), validTypes.end(), type) ==
        validTypes.end()) {
      return JumptToDataSet(dataBlock);
    }
    dataBlock.move(-11);
    return true;
  }

  std::unordered_map<std::string, DataBlock>
  SplitDataBlocks(DataBlock &dataBlock,
                  const std::vector<std::shared_ptr<Entity>> &entitiesNames) {
    std::unordered_map<std::string, DataBlock> dataParts;

    int prevStart = -1;
    int iStart = 0;
    auto linealEntityParentNames = Entity::Linealize(nullptr, entitiesNames);
    for (size_t i = 0; i < linealEntityParentNames.size(); ++i) {
      std::string entity = linealEntityParentNames[i].second;
      iStart = ParseBeginning(dataBlock, linealEntityParentNames[i].second,
                              linealEntityParentNames[i].first);
      if (prevStart != -1) {
        dataParts[linealEntityParentNames[i - 1].second] =
            dataBlock.getPart(prevStart, iStart);
      }
      prevStart = iStart;
    }
    iStart = dataBlock.data.size();
    dataParts[linealEntityParentNames.back().second] =
        dataBlock.getPart(prevStart, iStart);
    return dataParts;
  }

  int ParseBeginning(DataBlock &dataBlock, const std::string &entity,
                     const std::string &parent) {
    auto block = dataBlock.makeStringBlock(entity);
    auto blockParent = dataBlock.makeStringBlock(parent);
    std::vector<uint8_t> full;
    if (!parent.empty()) {
      full = dataBlock.addArrays(block, block, blockParent);
    } else {
      full = dataBlock.addArrays(block, blockParent);
    }

    if (!dataBlock.moveTo(full)) {
      throw std::runtime_error("Sequence not found.");
    }
    return dataBlock.n;
  }
};

} // namespace RedatamLib

#endif // REDATAMLIB_ENTITYPARSER_HPP
