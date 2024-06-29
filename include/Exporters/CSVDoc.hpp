#ifndef CSV_DOC_HPP
#define CSV_DOC_HPP

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <stdexcept>

class CSVDoc
{
public:
    const std::string delimiter = ";";
    const std::string quote = "\"";
    std::ofstream Stream;
    std::vector<std::string> Columns;
    std::vector<std::string> Labels;
    std::string Filename;

    CSVDoc(const std::string& filename) : Filename(filename)
    {
        Stream.open(filename);
        if (!Stream.is_open())
        {
            throw std::runtime_error("Cannot open file: " + filename);
        }
    }

    ~CSVDoc()
    {
        Close();
    }

    void Close()
    {
        if (Stream.is_open())
        {
            Stream.close();
        }
    }

    void CommitDictionary()
    {
        WriteLine(Columns);
        if (!Labels.empty())
        {
            WriteVariableLabels();
        }
    }

    void WriteLine(const std::vector<std::string>& values)
    {
        std::stringstream ss;
        bool isFirst = true;
        for (const auto& value : values)
        {
            if (!isFirst)
            {
                ss << delimiter;
            }
            else
            {
                isFirst = false;
            }

            std::string output = value;
            if (output.find(quote) != std::string::npos)
            {
                output = quote + replaceAll(output, quote, quote + quote) + quote;
            }
            else if (output.find(delimiter) != std::string::npos)
            {
                output = quote + output + quote;
            }
            ss << output;
        }
        Stream << ss.str() << "\n";
    }

    void WriteLine(const std::unordered_map<std::string, std::string>& data)
    {
        std::vector<std::string> values;
        for (const auto& col : Columns)
        {
            auto it = data.find(col);
            if (it != data.end())
            {
                values.push_back(it->second);
            }
            else
            {
                values.push_back("");
            }
        }
        WriteLine(values);
    }

private:
    void WriteVariableLabels()
    {
        if (Columns.size() != Labels.size())
        {
            throw std::runtime_error("Variable count differs from Labels count.");
        }

        std::filesystem::path folder = std::filesystem::path(Filename).parent_path() / "Labels";
        std::filesystem::create_directories(folder);
        std::string path = (folder / std::filesystem::path(Filename).stem()).string() + "-VARIABLES.csv";

        CSVDoc labels(path);
        labels.Columns = {"variable", "label"};
        labels.CommitDictionary();

        for (size_t i = 0; i < Columns.size(); ++i)
        {
            labels.WriteLine({Columns[i], Labels[i]});
        }
    }

    static std::string replaceAll(std::string str, const std::string& from, const std::string& to)
    {
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos)
        {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
        return str;
    }
};

#endif // CSV_DOC_HPP
