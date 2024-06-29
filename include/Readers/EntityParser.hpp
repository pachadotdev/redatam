#ifndef ENTITYPARSER_HPP
#define ENTITYPARSER_HPP

#include <vector>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <filesystem>
#include "RedatamDatabase.hpp"
#include "Entity.hpp"
#include "DataBlock.hpp"
#include "DOSEncoder.hpp"
#include "VariableParser.hpp"

class EntityParser {
private:
    RedatamDatabase* db;
    std::string rootPath;

    static inline const std::vector<std::string> validTypes = { "DBL", "LNG", "INT", "BIN", "PCK", "CHR" };

public:
    EntityParser(RedatamDatabase* db) : db(db) {}

    void Parse(const std::string& path) {
        rootPath = std::filesystem::path(path).parent_path().string();
        DataBlock dataBlock(path);

        // Separate into blocks the data of each entity
        auto dataParts = SplitDataBlocks(dataBlock, db->entityNames);

        for (auto& entity : ParseEntities(nullptr, db->entityNames, dataParts)) {
            db->Entities.push_back(entity);
        }

        MsDOSEncoder enc(db);
        if (enc.RequiresProcessing()) {
            enc.ReencodeLabels();
        }
    }

    std::vector<std::shared_ptr<Entity>> ParseEntities(std::shared_ptr<Entity> parent, std::vector<std::shared_ptr<Entity>>& entitiesNames, std::unordered_map<std::string, DataBlock>& dataParts) {
        std::vector<std::shared_ptr<Entity>> ret;
        for (auto& entityName : entitiesNames) {
            std::string parentName = (parent == nullptr ? "" : parent->Name);
            auto entity = ParseEntity(dataParts[entityName->Name], entityName->Name, parentName);
            entity->rootPath = rootPath;
            entity->Parent = parent;
            ret.push_back(entity);
            if (!entityName->SubEntities.empty()) {
                auto subEntities = ParseEntities(entity, entityName->SubEntities, dataParts);
                entity->SubEntities = subEntities;
                ret.insert(ret.end(), subEntities.begin(), subEntities.end());
            }
        }
        return ret;
    }

    inline Variable ParseVariable(DataBlock& dataBlock, std::shared_ptr<Entity> e) {
        Variable var;
        var.Name = dataBlock.readString(8);
        var.Label = dataBlock.readString(40);
        var.Type = dataBlock.readString(3);
        var.Decimals = dataBlock.readInt();
        var.Length = dataBlock.readInt();
        var.MissingValues = dataBlock.readInt();
        var.MissingValuesFormat = dataBlock.readString(8);
        var.VariableData = dataBlock.readBytes(var.Length);

        if (var.Type == "CHR") {
            var.VariableData = dataBlock.readString(var.Length);
        } else if (var.Type == "PCK" || var.Type == "BIN") {
            var.VariableData = dataBlock.readBytes(var.Length);
        }

        e->Variables.push_back(var);
        return var;
    }

    inline bool JumptToDataSet(DataBlock& dataBlock) {
        while (!dataBlock.eof()) {
            std::string datasetMarker = dataBlock.readString(11);
            if (datasetMarker == "DATASET   ") {
                return true;
            }
            dataBlock.move(-10);
        }
        return false;
    }

    inline bool checkDataType(DataBlock& dataBlock) {
        std::string type = dataBlock.readString(3);
        if (std::find(validTypes.begin(), validTypes.end(), type) == validTypes.end()) {
            // Not valid, look for more
            return JumptToDataSet(dataBlock);
        }
        // Move back to the beginning of DATASET
        dataBlock.move(-11);
        return true;
    }

    inline std::unordered_map<std::string, DataBlock> SplitDataBlocks(DataBlock& dataBlock, std::vector<std::shared_ptr<Entity>>& entitiesNames) {
        std::unordered_map<std::string, DataBlock> dataParts;

        int prevStart = -1;
        int iStart = 0;
        auto linealEntityParentNames = Entity::Linealize(nullptr, entitiesNames);
        for (int i = 0; i < linealEntityParentNames.size(); ++i) {
            std::string entity = linealEntityParentNames[i].second;
            iStart = ParseBeginning(dataBlock, linealEntityParentNames[i].second, linealEntityParentNames[i].first);
            if (prevStart != -1) {
                dataParts[linealEntityParentNames[i - 1].second] = dataBlock.getPart(prevStart, iStart);
            }
            prevStart = iStart;
        }
        // Save the last one
        iStart = dataBlock.data.size();
        dataParts[linealEntityParentNames[linealEntityParentNames.size() - 1].second] = dataBlock.getPart(prevStart, iStart);
        return dataParts;
    }

    inline int ParseBeginning(DataBlock& dataBlock, const std::string& entity, const std::string& parent) {
        auto e = std::make_shared<Entity>();
        e->rootPath = rootPath;
        // Build the expected header
        auto block = dataBlock.makeStringBlock(entity);
        auto blockParent = dataBlock.makeStringBlock(parent);
        std::vector<uint8_t> full;
        if (!parent.empty())
            full = dataBlock.addArrays(block, block, blockParent);
        else
            full = dataBlock.addArrays(block, blockParent);

        if (!dataBlock.moveTo(full))
            throw std::runtime_error("Sequence not found.");
        return dataBlock.n;
    }
};

#endif // ENTITYPARSER_HPP
