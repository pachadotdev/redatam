#include <boost/filesystem.hpp>
#include <iostream>
#include <string>
#include <vector>

#include "CSVExporter.h"
#include "XmlEntityParser.h"

namespace fs = boost::filesystem;

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <input.dicx> <output_directory>\n";
    return 1;
  }

  std::string dicxFilePath = argv[1];
  std::string outputPath = argv[2];

  // Create the output directory if it doesn't exist
  fs::create_directories(outputPath);

  XmlEntityParser parser;
  parser.Parse(dicxFilePath);
  RedatamDatabase database = parser.getDatabase();

  std::cout << "Number of entities parsed: " << database.entities.size()
            << std::endl;

  for (const auto &entity : database.entities) {
    std::cout << "Exporting entity: " << entity.name << std::endl;
    CSVExporter::Export(entity, outputPath);
  }

  std::cout << "CSV files generated successfully." << std::endl;
  return 0;
}