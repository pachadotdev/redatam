#include "RedatamDatabase.hpp"
#include "EntityParser.hpp"
#include <stdexcept>
#include <algorithm>
#include <filesystem>

namespace RedatamLib {

RedatamDatabase::RedatamDatabase() {
  fuzzyEntityParserInstance = std::make_unique<FuzzyEntityParser>(this);
  xmlEntityParserInstance = std::make_unique<XmlEntityParser>(this);
}

const std::vector<std::shared_ptr<Entity>> &
RedatamDatabase::GetEntities() const {
  return Entities;
}

long RedatamDatabase::GetTotalDataItems() {
  return GetEntitiesTotalDataItems(Entities, nullptr);
}

long RedatamDatabase::GetEntitiesTotalDataItems(
    const std::vector<std::shared_ptr<Entity>> &entities,
    std::shared_ptr<Entity> parent) {
  long total = 0;
  for (const auto &entity : entities) {
    entity->OpenPointer();
    total += (entity->SelectedVariables().size() + 2) *
             entity->RowsCount; // Replaced GetRowsCount with RowsCount
    total += GetEntitiesTotalDataItems(entity->Children, entity);
    entity->ClosePointer();
  }
  return total;
}

void RedatamDatabase::OpenDictionary(const std::string &filename) {
  EntityParser parser(shared_from_this());
  parser.Parse(filename);
}

void RedatamDatabase::Close() {
  for (const auto &entity : Entities) {
    CloseEntity(entity);
  }
}

void RedatamDatabase::CloseEntity(const std::shared_ptr<Entity> &entity) {
  entity->ClosePointer();
  for (const auto &child : entity->Children) {
    CloseEntity(child);
  }
}

} // namespace RedatamLib
