#include "RedatamDatabase.hpp"
#include "Entity.hpp"
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <sstream>

namespace RedatamLib {

RedatamDatabase::RedatamDatabase(const std::string &path) : databasePath(path) {
  LoadDatabase(path);
}

std::vector<std::shared_ptr<Entity>> RedatamDatabase::getEntityNames() const {
  return entityNames;
}

void RedatamDatabase::addEntities(
    const std::vector<std::shared_ptr<Entity>> &entities) {
  Entities.insert(Entities.end(), entities.begin(), entities.end());
}

void RedatamDatabase::LoadDatabase(const std::string &path) {
  // Call the parse function to load the database
  ParseDatabaseFile(path);
}

std::shared_ptr<Entity>
RedatamDatabase::FindEntity(const std::string &name) const {
  for (const auto &entity : Entities) {
    if (entity->Name == name) {
      return entity;
    }
  }
  return nullptr;
}

std::shared_ptr<Entity> RedatamDatabase::GetRootEntity() const {
  if (!Entities.empty()) {
    return Entities.front();
  }
  return nullptr;
}

std::vector<std::string> RedatamDatabase::OptimisticCombine(
    const std::string &rootPath, const std::vector<std::string> &paths) const {
  std::vector<std::string> combinedPaths;
  for (const auto &path : paths) {
    combinedPaths.push_back(rootPath + "/" + path);
  }
  return combinedPaths;
}

long RedatamDatabase::GetEntitiesTotalDataItems(
    const std::vector<std::shared_ptr<Entity>> &entities,
    std::shared_ptr<Entity> rootEntity) const {
  long total = 0;
  for (const auto &entity : entities) {
    total += (entity->SelectedVariables().size() + 2) * rootEntity->RowsCount;
  }
  return total;
}

void RedatamDatabase::ParseDatabaseFile(const std::string &path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open database file: " + path);
  }

  std::string line;
  while (std::getline(file, line)) {
    std::istringstream iss(line);
    std::string name, type;
    if (!(iss >> name >> type)) {
      continue; // Skip lines that don't have the correct format
    }

    auto entity = std::make_shared<Entity>();
    entity->Name = name;
    entity->Type = type;
    entityNames.push_back(entity);
    Entities.push_back(entity);
  }
}
} // namespace RedatamLib
