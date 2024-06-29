#ifndef FUZZYENTITYPARSER_HPP
#define FUZZYENTITYPARSER_HPP

#include <vector>
#include <string>
#include <memory>
#include <filesystem>
#include "DataBlock.hpp"
#include "Entity.hpp"
#include "RedatamDatabase.hpp"

class FuzzyEntityParser {
private:
    RedatamDatabase* db;

    void TryEntities(DataBlock& dataBlock, const std::string& entityNameString, std::vector<std::shared_ptr<Entity>>& entitiesNames) {
        std::string entityName;
        if (!dataBlock.PlausibleString(entityName) || entityName.empty())
            return;

        if (!checkEntityStart(dataBlock))
            return;

        auto entity = std::make_shared<Entity>();
        entity->setName(entityNameString);
        entitiesNames.push_back(entity);
        ProcessOcurrence(dataBlock, entity->getChildren(), entitiesNames);
    }

    bool checkEntityStart(DataBlock& dataBlock) {
        if (dataBlock.n + 10 >= dataBlock.data.size())
            return false;
        std::string relationChild = dataBlock.eatShortString();
        if (!dataBlock.PlausibleString(relationChild))
            return false;
        if (!relationChild.empty()) {
            // if (eatPlausibleString(out relationParent) == false)
            // return false;
        }
        std::string description;
        std::string indexfilename;
        if (!dataBlock.eatPlausibleString(description, false))
            return false;
        if (!dataBlock.eatPlausibleString(indexfilename, false))
            return false;
        return description.ends_with(".ptr") ||
               indexfilename.ends_with(".ptr") || description.ends_with(".PTR") ||
               indexfilename.ends_with(".PTR");
    }

    void ProcessOcurrence(DataBlock& dataBlock, std::vector<std::shared_ptr<Entity>>& leaves, std::vector<std::shared_ptr<Entity>>& entitiesNames) {
        if (dataBlock.moveBackString() == -1)
            return;
        
        int keepN = dataBlock.n;
        std::string child = dataBlock.eatShortString();
        dataBlock.n = keepN;
        if (!entitiesNames.empty() && entitiesNames.back()->getName() == child)
            return;
        if (!dataBlock.IsText(child) || child.empty())
            return;

        std::vector<std::shared_ptr<Entity>> names;
        TryEntities(dataBlock, child, names);
        if (!names.empty())
            leaves.insert(leaves.end(), names.begin(), names.end());
    }

    std::vector<std::shared_ptr<Entity>> GetBest(const std::vector<std::vector<std::shared_ptr<Entity>>>& leaves) {
        if (leaves.empty()) return std::vector<std::shared_ptr<Entity>>();
        int max = 0;
        int nMax = 0;
        for (int n = 0; n < leaves.size(); n++) {
            int current = CalculateTreeSize(leaves[n]);
            if (current > max) {
                max = current;
                nMax = n;
            }
        }
        return leaves[nMax];
    }

    int CalculateTreeSize(const std::vector<std::shared_ptr<Entity>>& list) {
        int ret = 0;
        for (const auto& entity : list) {
            ret++;
            ret += CalculateTreeSize(entity->getChildren());
        }
        return ret;
    }

public:
    FuzzyEntityParser(RedatamDatabase* db) : db(db) {}

    void ParseEntities(const std::string& path) {
        std::vector<std::vector<std::shared_ptr<Entity>>> candidates;

        DataBlock dataBlock(path);
        
        for (int i = 0; i < dataBlock.data.size(); i++) {
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
