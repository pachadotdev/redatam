#ifndef REDATAMLIB_CSVEXPORTER_HPP
#define REDATAMLIB_CSVEXPORTER_HPP

#include <memory>
#include <string>
#include <filesystem>
#include <vector>

namespace RedatamLib {

class RedatamDatabase;
class CSVDoc;
class Variable;

class CSVExporter {
public:
  CSVExporter(const std::shared_ptr<RedatamDatabase> &db);

  void Export(const std::string &folder);

private:
  std::shared_ptr<RedatamDatabase> db;

  std::string CreateFilename(const std::string &folder,
                             const std::string &entityName,
                             const std::string &extension);
  void CreateVariable(CSVDoc &doc, const std::shared_ptr<Variable> &variable);
  void WriteVariableValueLabels(CSVDoc &doc,
                                const std::shared_ptr<Variable> &variable);
};

} // namespace RedatamLib

#endif // REDATAMLIB_CSVEXPORTER_HPP
