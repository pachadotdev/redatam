#ifndef FUZZYENTITYPARSER_HPP
#define FUZZYENTITYPARSER_HPP

#include <vector>
#include <string>
#include <memory>
#include "RedatamDatabase.hpp"
#include "Entity.hpp"
#include "DataBlock.hpp"

class FuzzyEntityParser
{
private:
    RedatamDatabase* db;

    void TryEntities(DataBlock& dataBlock, const std::string& parent, std::vector<std::shared_ptr<Entity>>& entitiesNames);
    bool checkEntityStart(DataBlock& dataBlock);
    void ProcessOcurrence(DataBlock& dataBlock, std::vector<std::shared_ptr<Entity>>& leaves, std::vector<std::shared_ptr<Entity>>& entitiesNames);
    std::vector<std::shared_ptr<Entity>> GetBest(const std::vector<std::vector<std::shared_ptr<Entity>>>& leaves);
    int CalculateTreeSize(const std::vector<std::shared_ptr<Entity>>& list);

public:
    FuzzyEntityParser(RedatamDatabase* db) : db(db) {}

    void ParseEntities(const std::string& path)
    {
        std::vector<std::vector<std::shared_ptr<Entity>>> candidates;

        DataBlock dataBlock(path);
        
        // hace la hipótesis de que es una entidad...
        for (int i = 0; i < dataBlock.data.size(); i++)
        {
            dataBlock.n = i;
            std::vector<std::shared_ptr<Entity>> entitiesNames;
            TryEntities(dataBlock, "", entitiesNames);
            if (!entitiesNames.empty())
                candidates.push_back(entitiesNames);
        }
        db->entityNames = GetBest(candidates);
    }
};

#endif // FUZZYENTITYPARSER_HPP
