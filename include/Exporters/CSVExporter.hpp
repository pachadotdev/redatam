#ifndef CSV_EXPORTER_HPP
#define CSV_EXPORTER_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <filesystem>
#include "RedatamDatabase.hpp"
#include "Entity.hpp"
#include "Variable.hpp"
#include "CSVDoc.hpp"

namespace RedatamLib {

class CSVExporter
{
private:
    RedatamDatabase* db;

    std::string CreateFilename(const std::string& folder, const std::string& name, const std::string& extension)
    {
        std::string filename = folder + "/" + name + "." + extension;
        return filename;
    }

    void CreateVariable(CSVDoc& doc, const std::shared_ptr<Variable>& v)
    {
        doc.Columns.push_back(v->Name);
        doc.Labels.push_back(v->Label);
        if (!v->ValueLabels.empty())
        {
            WriteVariableValueLabels(doc, v);
        }
    }

    void WriteVariableValueLabels(CSVDoc& doc, const std::shared_ptr<Variable>& v)
    {
        std::filesystem::path path = std::filesystem::path(doc.Filename).parent_path() / "Labels" / std::filesystem::path(doc.Filename).stem();
        std::string file = path.string() + "-" + v->Name + "-LABELS.CSV";
        CSVDoc labels(file);
        labels.Columns = {v->Name, "label"};
        labels.CommitDictionary();
        for (const auto &item : v->ValueLabels) {
          labels.WriteLine({item.first, item.second});
        }
        labels.Close();
    }

    void WriteVariablesData(CSVDoc& doc, const std::unordered_map<std::string, std::string>& data)
    {
        doc.WriteLine(data);
    }

public:
    CSVExporter(RedatamDatabase* db) : db(db) {}

    void Export(const std::string& folder)
    {
        for (const auto& entity : db->Entities)
        {
            std::string filename = CreateFilename(folder, entity->Name, "csv");
            CSVDoc doc(filename);
            for (const auto& variable : entity->Variables)
            {
                CreateVariable(doc, variable);
            }
            doc.CommitDictionary();

            for (const auto& row : entity->Rows)
            {
                WriteVariablesData(doc, row);
            }

            doc.Close();
        }
    }
};

}

#endif // CSV_EXPORTER_HPP
