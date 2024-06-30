#ifndef REDATAMLIB_CSVEXPORTER_HPP
#define REDATAMLIB_CSVEXPORTER_HPP

#include <memory>
#include <vector>
#include <string>
#include <filesystem>
#include "CSVDoc.hpp"
#include "Variable.hpp"
#include "Entity.hpp"

namespace RedatamLib {

class CSVExporter {
public:
  CSVExporter(const std::shared_ptr<RedatamDatabase> &db) : db(db) {}

  void Export(const std::string &folder) {
    for (const auto &entity : entities) {
      std::string filename = CreateFilename(folder, entity->getName(), "csv");
      CSVDoc doc(filename);

      doc.Columns = {"Entity Name", "Description", "Level"};
      doc.WriteLine({entity->getName(), entity->getDescription(),
                     std::to_string(entity->getLevel())});

      for (const auto &variable : entity->getVariables()) {
        CreateVariable(doc, variable);
      }

      doc.WriteLine(
          {"Total Variables", std::to_string(entity->getVariables().size())});
      doc.Close();
    }
  }

private:
  std::shared_ptr<RedatamDatabase> db;
  std::vector<std::shared_ptr<Entity>> entities;

  std::string CreateFilename(const std::string &folder, const std::string &name,
                             const std::string &extension) {
    return folder + "/" + name + "." + extension;
  }

  void CreateVariable(CSVDoc &doc, const std::shared_ptr<Variable> &variable) {
    doc.Columns.push_back(variable->Name());
    doc.Labels.push_back(variable->Label());
    if (!variable->getValueLabels().empty()) {
      WriteVariableValueLabels(doc, variable);
    }
  }

  void WriteVariableValueLabels(CSVDoc &doc,
                                const std::shared_ptr<Variable> &variable) {
    std::filesystem::path path =
        std::filesystem::path(doc.Filename).parent_path() / "Labels";
    std::filesystem::create_directories(path);
    std::string filename =
        path.string() + "/" + variable->getName() + "-LABELS.csv";
    CSVDoc labels(filename);
    labels.Columns = {variable->getName(), "label"};
    labels.CommitDictionary();
    for (const auto &item : variable->getValueLabels()) {
      labels.WriteLine({item.first, item.second});
    }
    labels.Close();
  }
};

} // namespace RedatamLib

#endif // REDATAMLIB_CSVEXPORTER_HPP
