#ifndef VARIABLE_HPP
#define VARIABLE_HPP

#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <variant>
#include "tinyxml2.h"
#include "ValueLabel.hpp"
#include "ICursorReader.hpp"
#include "CursorReader.hpp"
#include "NullCursorReader.hpp"

// Forward declaration of Entity class
class Entity;

class Variable {
private:
    std::unique_ptr<ICursorReader> reader;
    std::shared_ptr<Entity> entity;

    std::string ResolveDataFilename();

    long CalculateCharSize();
    long CalculateBitsSize();
    long GetExpectedFileSize();

public:
    std::string Name;
    std::string Label;
    std::string Type;
    std::string Range;
    int Decimals;
    std::string Declaration;
    std::string Group;
    std::string ValuesLabelsRaw;
    std::vector<ValueLabel> ValueLabels;
    std::string Filter;
    int Size;
    std::string Filename;
    bool BinaryDataSet = false;
    bool Selected = true;

    Variable(const std::shared_ptr<Entity>& entity)
        : entity(entity) {}

    Variable(const std::string& name, const std::string& type, const std::string& label)
        : Name(name), Label(label), Type(type) {}

    ~Variable() = default;

    std::string ToString() const {
        return Name;
    }

    std::variant<std::string, uint64_t, int, double> GetData() {
        if (Type == "STRING")
            return reader->ReadString();
        else if (Type == "INTEGER")
            return reader->ReadNumber();
        else if (Type == "INT16")
            return reader->ReadInt16();
        else if (Type == "REAL")
            return reader->ReadDouble();
        else
            throw std::runtime_error("Unsupported data type: " + Type);
    }

    bool FileSizeFails(long& expectedSize, long& actual) {
        expectedSize = GetExpectedFileSize();
        actual = reader->Length();
        return expectedSize > actual;
    }

    void OpenData() {
        if (DataFileExists()) {
            std::string file = ResolveDataFilename();
            reader = std::make_unique<CursorReader>(file, Type == "STRING", BinaryDataSet, Size);
            reader->Open();
        } else {
            reader = std::make_unique<NullCursorReader>();
        }
    }

    bool DataFileExists() {
        std::string file = ResolveDataFilename();
        return std::filesystem::exists(file);
    }

    void CloseData() {
        reader->Close();
    }
};

#endif // VARIABLE_HPP
