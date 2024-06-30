#ifndef CSVEXPORTER_HPP
#define CSVEXPORTER_HPP

#include <memory>
#include <vector>
#include <string>
#include "CSVDoc.hpp"
#include "Variable.hpp"
#include "Entity.hpp"

namespace RedatamLib {

class CSVExporter {
public:
  void WriteVariableValueLabels(CSVDoc &labels,
                                const std::shared_ptr<Variable> &variable) {
    for (const auto &item : variable->ValueLabels) {
      labels.WriteLine(std::vector<std::string>{item.Key, item.Value});
    }
  }

  void Export(const std::string &folder) {
    for (const auto &entity : entities) {
      std::string filename = CreateFilename(folder, entity->getName(), "csv");
      CSVDoc doc(filename);

      WriteHeader(doc, entity);

      for (const auto &variable : entity->getVariables()) {
        WriteVariable(doc, variable);
      }

      WriteFooter(doc, entity);

      // Close the document
      doc.Close();
    }
  }

private:
  std::vector<std::shared_ptr<Entity>> entities;

  std::string CreateFilename(const std::string &folder, const std::string &name,
                             const std::string &extension) {
    return folder + "/" + name + "." + extension;
  }

  void WriteHeader(CSVDoc &doc, const std::shared_ptr<Entity> &entity) {
    doc.WriteLine({"Entity Name", "Description", "Level"});
    doc.WriteLine({entity->getName(), entity->Description,
                   std::to_string(entity->Level)});
  }

  void WriteVariable(CSVDoc &doc, const std::shared_ptr<Variable> &variable) {
    doc.WriteLine({"Variable Name", "Type", "Label", "Range", "Size"});
    doc.WriteLine({variable->Name, variable->Type, variable->Label,
                   variable->Range, std::to_string(variable->Size)});
    WriteVariableValueLabels(doc, variable);
  }

  void WriteFooter(CSVDoc &doc, const std::shared_ptr<Entity> &entity) {
    doc.WriteLine({"Total Variables", std::to_string(entity->VariableCount)});
  }
};

} // namespace RedatamLib

#endif // CSVEXPORTER_HPP
