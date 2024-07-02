#include "CSVExporter.hpp"
#include "CSVDoc.hpp"
#include "Variable.hpp"

namespace RedatamLib {
void CSVExporter::CreateVariable(CSVDoc &doc,
                                 const std::shared_ptr<Variable> &variable) {
  doc.Labels.push_back(variable->Label);
  if (!variable->ValueLabels.empty()) {
    for (const auto &item : variable->ValueLabels) {
      doc.WriteLine({item.first, item.second});
    }
  }
}

void CSVExporter::WriteVariableValueLabels(
    CSVDoc &labels, const std::shared_ptr<Variable> &variable) {
  for (const auto &item : variable->ValueLabels) {
    labels.WriteLine({item.first, item.second});
  }
}
} // namespace RedatamLib
