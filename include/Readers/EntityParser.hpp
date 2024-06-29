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

class EntityParser
{
private:
    RedatamDatabase* db;
    std::string rootPath;

    static inline const std::vector<std::string> validTypes = { "DBL", "LNG", "INT", "BIN", "PCK", "CHR" };

    Variable ParseVariable(DataBlock& dataBlock, std::shared_ptr<Entity> e);
    bool JumptToDataSet(DataBlock& dataBlock);
    bool checkDataType(DataBlock& dataBlock);
    int ParseBeginning(DataBlock& dataBlock, const std::string& entity, const std::string& parent);
    std::unordered_map<std::string, DataBlock> SplitDataBlocks(DataBlock& dataBlock, std::vector<std::shared_ptr<Entity>>& entitiesNames);

public:
    EntityParser(RedatamDatabase* db) : db(db) {}

    void Parse(const std::string& path)
    {
        rootPath = std::filesystem::path(path).parent_path().string();
        DataBlock dataBlock(path);

        // separa en bloques los datos de cada entidad...
        auto dataParts = SplitDataBlocks(dataBlock, db->entityNames);

        for (auto& entity : ParseEntities(nullptr, db->entityNames, dataParts))
        {
            db->Entities.push_back(entity);
        }

        MsDOSEncoder enc(db);
        if (enc.RequiresProcessing())
        {
            enc.ReencodeLabels();
        }
    }

    std::vector<std::shared_ptr<Entity>> ParseEntities(std::shared_ptr<Entity> parent, std::vector<std::shared_ptr<Entity>>& entitiesNames, std::unordered_map<std::string, DataBlock>& dataParts)
    {
        std::vector<std::shared_ptr<Entity>> ret;
        for (auto& entityName : entitiesNames)
        {
            std::string parentName = (parent == nullptr ? "" : parent->Name);
            auto entity = ParseEntity(dataParts[entityName->Name], entityName->Name, parentName);
            ret.push_back(entity);
            auto children = ParseEntities(entityName, entityName->Children, dataParts);
            if (!children.empty())
                entity->Children.insert(entity->Children.end(), children.begin(), children.end());
        }
        return ret;
    }

    std::shared_ptr<Entity> ParseEntity(DataBlock& dataBlock, const std::string& entity, const std::string& parent)
    {
        auto e = std::make_shared<Entity>();
        e->rootPath = rootPath;
        // construye el encabezado esperable
        auto block = dataBlock.makeStringBlock(entity);
        auto blockParent = dataBlock.makeStringBlock(parent);
        std::vector<uint8_t> full;
        if (!parent.empty())
            full = dataBlock.addArrays(block, block, blockParent);
        else
            full = dataBlock.addArrays(block, blockParent);

        if (!dataBlock.moveTo(full))
            throw std::runtime_error("Sequence not found.");
        // lee la entidad
        e->Name = dataBlock.eatShortString();
        e->RelationChild = dataBlock.eatShortString();
        if (!e->RelationChild.empty())
            e->RelationParent = dataBlock.eatShortString();
        e->Description = dataBlock.eatShortString();
        e->IndexFilename = dataBlock.eatShortString();
        e->s1 = dataBlock.eat16int();
        if (!e->IndexFilename.empty())
        {
            e->CodesVariable = dataBlock.eatShortString();
            e->LabelVariable = dataBlock.eatShortString();
            e->Level = dataBlock.eat32int();
            e->b1 = dataBlock.eatByte();
            //e.VariableCount = no es confiable...que guarde rangos... eat32int();
            // en base al count de variables, que busque los "dataset"
            while (true)
            {
                auto v = ParseVariable(dataBlock, e);
                if (v != nullptr)
                    e->Variables.push_back(v);
                else
                    break;
            }
        }
        e->VariableCount = e->Variables.size();
        return e;
    }
};

#endif // ENTITYPARSER_HPP
