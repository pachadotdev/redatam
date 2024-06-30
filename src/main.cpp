#include <iostream>
#include <filesystem>
#include "RedatamDatabase.hpp"
#include "Entity.hpp"
#include "EntityParser.hpp"
#include "XmlEntityParser.hpp"
#include "CSVExporter.hpp"

void convertDicToCsv(const std::string& dicFilePath, const std::string& outputDirPath)
{
  auto db = std::make_shared<RedatamLib::RedatamDatabase>();
  RedatamLib::XmlEntityParser parser(db);
  parser.Parse(dicFilePath);

  RedatamLib::CSVExporter exporter;
  exporter.Export(outputDirPath);
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: redatam input.dic(x) output-csv/" << std::endl;
        return 1;
    }

    std::string dicFilePath = argv[1];
    std::string outputDirPath = argv[2];

    if (!std::filesystem::exists(dicFilePath))
    {
        std::cerr << "Error: Input DIC file does not exist." << std::endl;
        return 1;
    }

    if (!std::filesystem::exists(outputDirPath))
    {
        std::filesystem::create_directories(outputDirPath);
    }

    try
    {
        convertDicToCsv(dicFilePath, outputDirPath);
        std::cout << "Conversion successful!" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
