#ifndef CSV_EXPORTER_H
#define CSV_EXPORTER_H

#include "Entity.h"
#include <string>

class CSVExporter {
public:
  static void Export(const Entity &entity, const std::string &outputPath);
};

#endif // CSV_EXPORTER_H
