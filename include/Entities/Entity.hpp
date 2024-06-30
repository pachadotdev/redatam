#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <string>
#include <vector>
#include <memory>
#include "Variable.hpp"

class Entity {
private:
    std::string Name;
    std::vector<std::shared_ptr<Variable>> Variables;
    std::string IndexFilename;
    std::unique_ptr<CursorReader> reader;
    int64_t RowsCount;
    std::vector<std::shared_ptr<Entity>> Children;

public:
    std::string RelationChild;
    std::string RelationParent;
    std::string Description;
    std::string CodesVariable;
    std::string LabelVariable;
    int Level;
    uint8_t b1;
    int s1; // Add s1 as an integer member
    int VariableCount;
    std::string rootPath;

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

    void setName(const std::string& name) {
        Name = name;
    }

    std::string getName() const {
        return Name;
    }

    void setIndexFilename(const std::string& indexFilename) {
        IndexFilename = indexFilename;
    }

    std::string getIndexFilename() const {
        return IndexFilename;
    }

    std::vector<std::shared_ptr<Entity>>& getChildren() {
        return Children;
    }

    void setChildren(const std::vector<std::shared_ptr<Entity>>& children) {
        Children = children;
    }

    std::vector<std::shared_ptr<Variable>>& getVariables() {
        return Variables;
    }

    void setVariables(const std::vector<std::shared_ptr<Variable>>& variables) {
        Variables = variables;
    }
};

#endif // ENTITY_HPP
