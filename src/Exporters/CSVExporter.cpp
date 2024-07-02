#include "CSVExporter.hpp"
#include "CSVDoc.hpp"
#include "Entity.hpp"
#include "Variable.hpp"
#include "RedatamDatabase.hpp" // Include the full definition of RedatamDatabase
#include <filesystem>
#include <vector>

namespace RedatamLib {

CSVExporter::CSVExporter(const std::shared_ptr<RedatamDatabase> &db) : db(db) {}

void CSVExporter::Export(const std::string &folder) {
  for (const auto &entity : db->GetEntities()) {
    std::string filename = CreateFilename(folder, entity->Name, "csv");
    CSVDoc doc(filename);
    doc.WriteLine(
        std::vector<std::string>{"Entity Name", "Description", "Level"});
    doc.WriteLine(std::vector<std::string>{entity->Name, entity->Description,
                                           std::to_string(entity->Level)});
    for (const auto &variable : entity->Variables) {
      CreateVariable(doc, variable);
      WriteVariableValueLabels(doc, variable);
    }
    doc.WriteLine(std::vector<std::string>{
        "Total Variables", std::to_string(entity->Variables.size())});
  }
}

std::string CSVExporter::CreateFilename(const std::string &folder,
                                        const std::string &entityName,
                                        const std::string &extension) {
  return folder + "/" + entityName + "." + extension;
}

void CSVExporter::CreateVariable(CSVDoc &doc,
                                 const std::shared_ptr<Variable> &variable) {
  doc.Columns.push_back(variable->Name);
  doc.Labels.push_back(variable->Label);
  if (!variable->ValueLabels.empty()) {
    for (const auto &item : variable->ValueLabels) {
      doc.WriteLine(std::vector<std::string>{item.GetKey(), item.GetValue()});
    }
  }
}

void CSVExporter::WriteVariableValueLabels(
    CSVDoc &doc, const std::shared_ptr<Variable> &variable) {
  std::filesystem::path path("value_labels");
  std::filesystem::create_directory(path);
  CSVDoc labels(path.string() + "/" + variable->Name + "-LABELS.csv");
  labels.Columns = {variable->Name, "label"};
  for (const auto &item : variable->ValueLabels) {
    labels.WriteLine(std::vector<std::string>{item.GetKey(), item.GetValue()});
  }
}

} // namespace RedatamLib
