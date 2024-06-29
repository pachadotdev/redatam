#ifndef REDATAMDATABASE_HPP
#define REDATAMDATABASE_HPP

#include <vector>
#include <memory>
#include <string>
#include <filesystem>
#include <stdexcept>
#include "Entity.hpp"
#include "FuzzyEntityParser.hpp"
#include "XmlEntityParser.hpp"
#include "EntityParser.hpp"

class RedatamDatabase
{
public:
    std::vector<std::shared_ptr<Entity>> entityNames;
    std::vector<std::shared_ptr<Entity>> Entities;
    std::unique_ptr<FuzzyEntityParser> FuzzyEntityParser;
    std::unique_ptr<XmlEntityParser> XmlEntityParser;
    std::string DictionaryFile;

    RedatamDatabase()
        : FuzzyEntityParser(std::make_unique<class FuzzyEntityParser>(this)),
          XmlEntityParser(std::make_unique<class XmlEntityParser>(this))
    {
    }

    int64_t GetTotalDataItems()
    {
        return GetEntitiesTotalDataItems(Entities, nullptr);
    }

private:
    int64_t GetEntitiesTotalDataItems(const std::vector<std::shared_ptr<Entity>>& entities, std::shared_ptr<Entity> parent)
    {
        int64_t ret = 0;
        for (const auto& entity : entities)
        {
            ret += (entity->SelectedVariables().size() + 2) * entity->RowsCount;
            ret += GetEntitiesTotalDataItems(entity->Children, entity);
        }
        return ret;
    }

public:
    int64_t GetTotalRowsSize()
    {
        return GetEntitiesRowsSize(Entities, nullptr);
    }

private:
    int64_t GetEntitiesRowsSize(const std::vector<std::shared_ptr<Entity>>& entities, std::shared_ptr<Entity> parent)
    {
        int64_t ret = 0;
        for (const auto& entity : entities)
        {
            entity->OpenPointer();
            int64_t entityCount = entity->CalculateRowCount(parent);
            ret += entityCount;
            entity->ClosePointer();
            ret += GetEntitiesRowsSize(entity->Children, entity);
        }
        return ret;
    }

public:
    static std::string OptimisticCombine(const std::string& path, const std::string& file)
    {
        std::filesystem::path filefull = std::filesystem::path(path) / file;
        if (std::filesystem::exists(filefull)) return filefull.string();
        std::filesystem::path fOnly = std::filesystem::path(file).filename();
        filefull = std::filesystem::path(path) / fOnly;
        return filefull.string();
    }

    void OpenDictionary(const std::string& file)
    {
        try
        {
            auto ext = std::filesystem::path(file).extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            if (ext == ".dic")
            {
                FuzzyEntityParser->ParseEntities(file);
                // Parse de entidades y variables
                try
                {
                    EntityParser parser(this);
                    parser.Parse(file);
                }
                catch (const std::exception& e)
                {
                    throw std::runtime_error("An error occurred while parsing the dictionary variables and labels (" + std::string(e.what()) + ").");
                }
            }
            else if (ext == ".dicx")
                XmlEntityParser->Parse(file);
            else
                throw std::runtime_error("Dictionary files must by .dic or .dicx files.");
        }
        catch (const std::exception& e)
        {
            throw std::runtime_error("An error occurred while discovering the dictionary entities (" + std::string(e.what()) + ").");
        }
    }

    std::vector<std::shared_ptr<Entity>> GetEntitiesList()
    {
        std::vector<std::shared_ptr<Entity>> ret;
        doGetEntitiesList(ret, Entities);
        return ret;
    }

private:
    void doGetEntitiesList(std::vector<std::shared_ptr<Entity>>& ret, const std::vector<std::shared_ptr<Entity>>& list)
    {
        for (const auto& e : list)
        {
            ret.push_back(e);
            doGetEntitiesList(ret, e->Children);
        }
    }
};

#endif // REDATAMDATABASE_HPP
