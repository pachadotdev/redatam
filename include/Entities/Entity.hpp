#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include "Variable.hpp"

// Forward declarations to avoid circular dependencies
class CursorReader;
class NullCursorReader;

class Entity {
private:
    std::string Name;
    std::vector<std::shared_ptr<Variable>> Variables;
    std::string IndexFilename;
    std::unique_ptr<CursorReader> reader;
    int64_t RowsCount;
    std::vector<std::shared_ptr<Entity>> Children;

public:
    std::string ToString() const;
    std::vector<std::shared_ptr<Variable>> SelectedVariables() const;
    void OpenPointer();
    std::string ResolveDataFilename();
    bool HasData() const;
    int GetPointerData() const;
    void ClosePointer();
    int64_t CalculateRowCount(std::shared_ptr<Entity> parentEntity);
    std::vector<std::pair<std::string, std::string>> Linealize(std::shared_ptr<Entity> parent, const std::vector<std::shared_ptr<Entity>>& entitiesNames);
    bool DataFileExists() const;

    std::vector<std::shared_ptr<Entity>> SubEntities;
    std::shared_ptr<Entity> Parent;
    std::string rootPath;  // Declared only once

    // Public setter for Name
    void setName(const std::string& name) {
        Name = name;
    }

    // Public getter for Name
    std::string getName() const {
        return Name;
    }

    // Public getter for Children
    std::vector<std::shared_ptr<Entity>>& getChildren() {
        return Children;
    }

    // Public setter for Children
    void setChildren(const std::vector<std::shared_ptr<Entity>>& children) {
        Children = children;
    }
};

#endif // ENTITY_HPP
