#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include "ICursorReader.hpp"
#include "Variable.hpp"
#include "CursorReader.hpp"
#include "NullCursorReader.hpp"
#include "RedatamDatabase.hpp"

class Entity
{
private:
    std::unique_ptr<ICursorReader> reader;

    std::string ResolveDataFilename();

public:
    std::string rootPath;
    std::string Name;
    std::vector<std::shared_ptr<Entity>> Children;
    std::string Description;
    std::string IndexFilename;
    int i1;
    std::string Alias;
    int s1;
    uint8_t b1;
    std::string RelationChild;
    std::string RelationParent;
    std::string CodesVariable;
    std::string LabelVariable;
    int Level;
    int64_t RowsCount;
    int VariableCount;
    std::string c1;
    std::vector<std::shared_ptr<Variable>> Variables;

    Entity() = default;
    ~Entity() = default;

    std::string ToString() const;
    std::vector<std::shared_ptr<Variable>> SelectedVariables() const;
    void OpenPointer();
    bool HasData() const;
    int GetPointerData() const;
    void ClosePointer();
    int64_t CalculateRowCount(std::shared_ptr<Entity> parentEntity);
    static std::vector<std::pair<std::string, std::string>> Linealize(std::shared_ptr<Entity> parent, const std::vector<std::shared_ptr<Entity>>& entitiesNames);
    bool DataFileExists() const;
};

#endif // ENTITY_HPP
