#ifndef ENTITYPARSER_HPP
#define ENTITYPARSER_HPP

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <filesystem>
#include <algorithm>
#include "Entity.hpp"
#include "DataBlock.hpp"
#include "VariableParser.hpp"
#include "RedatamDatabase.hpp"
#include "MsDOSEncoder.hpp"

namespace RedatamLib {

class EntityParser {
private:
  RedatamDatabase *db;
  std::string rootPath;
  std::vector<std::string> validTypes = {"DBL", "LNG", "INT",
                                         "BIN", "PCK", "CHR"};

public:
  EntityParser(RedatamDatabase *database) : db(database) {}

  void Parse(const std::string &path) {
    rootPath = std::filesystem::path(path).parent_path().string();
    DataBlock dataBlock(path);

    auto dataParts = SplitDataBlocks(dataBlock, db->entityNames);

    db->Entities.insert(
        db->Entities.end(),
        ParseEntities(nullptr, db->entityNames, dataParts).begin(),
        ParseEntities(nullptr, db->entityNames, dataParts).end());

    MsDOSEncoder enc(db, false); // Pass the second argument as false
    if (enc.RequiresProcessing()) {
      enc.ReencodeLabels();
    }
  }

  std::vector<std::shared_ptr<Entity>>
  ParseEntities(std::shared_ptr<Entity> parent,
                const std::vector<std::shared_ptr<Entity>> &entitiesNames,
                std::unordered_map<std::string, DataBlock> &dataParts) {
    std::vector<std::shared_ptr<Entity>> ret;
    for (const auto &entityName : entitiesNames) {
      std::string parentName = parent ? parent->getName() : "";
      auto entity = ParseEntity(dataParts.at(entityName->getName()),
                                entityName->getName(), parentName);
      ret.push_back(entity);
      auto children =
          ParseEntities(entity, entityName->getChildren(), dataParts);
      if (!children.empty()) {
        entity->setChildren(children);
      }
    }
    return ret;
  }

  std::shared_ptr<Entity> ParseEntity(DataBlock &dataBlock,
                                      const std::string &entity,
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

    auto e = std::make_shared<Entity>();
    e->rootPath = rootPath;
    e->setName(dataBlock.eatShortString());

    e->RelationChild = dataBlock.eatShortString();
    if (!e->RelationChild.empty()) {
      e->RelationParent = dataBlock.eatShortString();
    }
    e->Description = dataBlock.eatShortString();
    e->setIndexFilename(dataBlock.eatShortString());
    e->s1 = dataBlock.eat16int();
    if (!e->getIndexFilename().empty()) {
      e->CodesVariable = dataBlock.eatShortString();
      e->LabelVariable = dataBlock.eatShortString();
      e->Level = dataBlock.eat32int();
      e->b1 = dataBlock.eatByte();

      while (true) {
        auto variable = ParseVariable(dataBlock, e);
        if (variable) {
          e->addVariable(variable);
        } else {
          break;
        }
      }
    }
    e->VariableCount = e->getVariables().size();
    return e;
  }

  std::shared_ptr<Variable> ParseVariable(DataBlock &dataBlock,
                                          std::shared_ptr<Entity> entity) {
    if (!JumptToDataSet(dataBlock)) {
      return nullptr;
    }

    auto v = std::make_shared<Variable>(entity);
    v->setName(dataBlock.eatShortString());
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
  SplitDataBlocks(const DataBlock &dataBlock,
                  const std::vector<std::shared_ptr<Entity>> &entitiesNames) {
    std::unordered_map<std::string, DataBlock> dataParts;
    int prevStart = -1;
    int iStart = 0;
    auto linealEntityParentNames = Entity::Linealize(nullptr, entitiesNames);
    for (size_t i = 0; i < linealEntityParentNames.size(); ++i) {
      const auto &entity = linealEntityParentNames[i].second;
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

  int ParseBeginning(const DataBlock &dataBlock, const std::string &entity,
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

#endif // ENTITYPARSER_HPP
