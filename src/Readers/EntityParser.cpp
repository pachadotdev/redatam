#include "EntityParser.hpp"

Variable EntityParser::ParseVariable(DataBlock& dataBlock, std::shared_ptr<Entity> e)
{
    // lee las variables
    if (!JumptToDataSet(dataBlock))
        return nullptr;
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

bool EntityParser::JumptToDataSet(DataBlock& dataBlock)
{
    if (!dataBlock.moveTo("DATASET"))
        return false;
    // valida el tipo de dato
    if (!checkDataType(dataBlock))
        return false;
    //
    dataBlock.move(-2);
    // retrocede para leer el nombre
    if (dataBlock.moveBackString(32) < 1)
    {
        dataBlock.move(6);
        // este no es válido... busca si hay más...
        return JumptToDataSet(dataBlock);
    }
    else
        return true;
}

bool EntityParser::checkDataType(DataBlock& dataBlock)
{
    dataBlock.move(8); // "DATASET "            
    if (dataBlock.n + 3 > dataBlock.data.size())
        return false;
    std::string type = dataBlock.eatChars(3); // "DBL", "LNG", etc
    if (std::find(validTypes.begin(), validTypes.end(), type) == validTypes.end())
    {
        // este no es válido... busca si hay más...
        return JumptToDataSet(dataBlock);
    }
    // retrocede hasta el inicio de DATASET
    dataBlock.move(-11);
    return true;
}

std::unordered_map<std::string, DataBlock> EntityParser::SplitDataBlocks(DataBlock& dataBlock, std::vector<std::shared_ptr<Entity>>& entitiesNames)
{
    std::unordered_map<std::string, DataBlock> dataParts;

    int prevStart = -1;
    int iStart = 0;
    auto linealEntityParentNames = Entity::Linealize(nullptr, entitiesNames);
    for (int i = 0; i < linealEntityParentNames.size(); ++i)
    {
        std::string entity = linealEntityParentNames[i].second;
        iStart = ParseBeginning(dataBlock, linealEntityParentNames[i].second, linealEntityParentNames[i].first);
        if (prevStart != -1)
        {
            dataParts[linealEntityParentNames[i - 1].second] = dataBlock.getPart(prevStart, iStart);
        }
        prevStart = iStart;
    }
    // guarda el último
    iStart = dataBlock.data.size();
    dataParts[linealEntityParentNames[linealEntityParentNames.size() - 1].second] = dataBlock.getPart(prevStart, iStart);
    return dataParts;
}

int EntityParser::ParseBeginning(DataBlock& dataBlock, const std::string& entity, const std::string& parent)
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
    return dataBlock.n;
}
