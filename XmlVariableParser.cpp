#include "XmlVariableParser.h"

#include <iostream>
#include <pugixml.hpp>

void XmlVariableParser::Parse(const std::string &filePath) {
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(filePath.c_str());
  if (!result) {
    std::cerr << "XML [" << filePath << "] parsed with errors, attr value: ["
              << doc.child("node").attribute("attr").value() << "]\n";
    std::cerr << "Error description: " << result.description() << "\n";
    std::cerr << "Error offset: " << result.offset << " (error at [..."
              << (filePath.c_str() + result.offset) << "]\n";
    return;
  }

  // Parsing logic specific to XmlVariableParser
}
