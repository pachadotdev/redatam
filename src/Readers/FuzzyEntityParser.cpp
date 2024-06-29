#include "FuzzyEntityParser.hpp"

void FuzzyEntityParser::TryEntities(DataBlock& dataBlock, const std::string& parent, std::vector<std::shared_ptr<Entity>>& entitiesNames)
{
    // lee el length
    std::string entityNameString;
    if (!dataBlock.PlausibleString(entityNameString) || entityNameString.empty())
        return;
    //
    if (!checkEntityStart(dataBlock))
        return;
    auto entity = std::make_shared<Entity>();
    entity->Name = entityNameString;
    entitiesNames.push_back(entity);
    // ahora se va a buscar a sus hijos
    auto block = dataBlock.makeStringBlock(entityNameString);
    std::vector<std::shared_ptr<Entity>> variants;
    auto allBlockOccurrences = dataBlock.GetAllMatches(block);
    
    for (int startN : allBlockOccurrences)
    {
        dataBlock.n = startN;
        ProcessOcurrence(dataBlock, variants, entity->Children);
    }
    if (!variants.empty())
    {
        entity->Children.clear();
        entity->Children.insert(entity->Children.end(), variants.begin(), variants.end());
    }
}

bool FuzzyEntityParser::checkEntityStart(DataBlock& dataBlock)
{
    std::string relationChild;
    std::string name = dataBlock.eatShortString();
    if (!dataBlock.eatPlausibleString(relationChild))
        return false;
    if (!relationChild.empty())
    {
        //if (eatPlausibleString(out relationParent) == false)
        //  return false;
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

void FuzzyEntityParser::ProcessOcurrence(DataBlock& dataBlock, std::vector<std::shared_ptr<Entity>>& leaves, std::vector<std::shared_ptr<Entity>>& entitiesNames)
{
    if (dataBlock.moveBackString() == -1)
        return;
    
    int keepN = dataBlock.n;
    std::string child = dataBlock.eatShortString();
    dataBlock.n = keepN;
    if (!entitiesNames.empty() && entitiesNames.back()->Name == child)
        return;
    if (!dataBlock.IsText(child) || child.empty())
        return;
    // avanzó ok...
    std::vector<std::shared_ptr<Entity>> names;
    TryEntities(dataBlock, child, names);
    if (!names.empty())
        leaves.insert(leaves.end(), names.begin(), names.end());
}

std::vector<std::shared_ptr<Entity>> FuzzyEntityParser::GetBest(const std::vector<std::vector<std::shared_ptr<Entity>>>& leaves)
{
    if (leaves.empty()) return std::vector<std::shared_ptr<Entity>>();
    int max = 0;
    int nMax = 0;
    for (int n = 0; n < leaves.size(); n++)
    {
        int current = CalculateTreeSize(leaves[n]);
        if (current > max)
        {
            max = current;
            nMax = n;
        }
    }
    return leaves[nMax];
}

int FuzzyEntityParser::CalculateTreeSize(const std::vector<std::shared_ptr<Entity>>& list)
{
    int ret = 0;
    for (const auto& entity : list)
    {
        ret++;
        ret += CalculateTreeSize(entity->Children);
    }
    return ret;
}
