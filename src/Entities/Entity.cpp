#include "Entity.hpp"
#include "CursorReader.hpp"
#include "NullCursorReader.hpp"
#include "RedatamDatabase.hpp"

std::string Entity::ToString() const
{
    return this->Name;
}

std::vector<std::shared_ptr<Variable>> Entity::SelectedVariables() const
{
    std::vector<std::shared_ptr<Variable>> selected;
    for (const auto& v : Variables)
    {
        if (v->Selected)
        {
            selected.push_back(v);
        }
    }
    return selected;
}

void Entity::OpenPointer()
{
    if (!this->IndexFilename.empty())
    {
        std::string file = ResolveDataFilename();
        reader = std::make_unique<CursorReader>(file, 16);
    }
    else
    {
        reader = std::make_unique<NullCursorReader>();
    }
    reader->Open();
}

std::string Entity::ResolveDataFilename()
{
    if (!this->IndexFilename.empty())
        return RedatamDatabase::OptimisticCombine(this->rootPath, this->IndexFilename);
    else
        return "";
}

bool Entity::HasData() const
{
    return !reader->IsLastPos();
}

int Entity::GetPointerData() const
{
    return static_cast<int>(reader->ReadInt32());
}

void Entity::ClosePointer()
{
    reader->Close();
}

int64_t Entity::CalculateRowCount(std::shared_ptr<Entity> parentEntity)
{
    int64_t ret = 0;
    if (reader->Length() > 0)
    {
        int64_t pos = 1;
        if (parentEntity != nullptr)
        {
            if (parentEntity->RowsCount > 0)
                pos = parentEntity->RowsCount;
        }
        ret = reader->ReadInt32At(pos);
    }
    this->RowsCount = ret;
    return ret;
}

std::vector<std::pair<std::string, std::string>> Entity::Linealize(std::shared_ptr<Entity> parent, const std::vector<std::shared_ptr<Entity>>& entitiesNames)
{
    std::vector<std::pair<std::string, std::string>> ret;
    for (const auto& e : entitiesNames)
    {
        if (parent == nullptr)
            ret.emplace_back("", e->Name);
        else
            ret.emplace_back(parent->Name, e->Name);
        auto children = Linealize(e, e->Children);
        if (!children.empty())
            ret.insert(ret.end(), children.begin(), children.end());
    }
    return ret;
}

bool Entity::DataFileExists() const
{
    std::string file = ResolveDataFilename();
    return std::filesystem::exists(file);
}
