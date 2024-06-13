#include "entity_descriptor.h"

#include <iostream>

#include "primitives.h"

namespace Redatam {

EntityDescriptor EntityDescriptor::fread(std::istream& stream, bool is_root) {
  EntityDescriptor d;

  d.name1 = fread_string(stream);
  std::cout << "Read name1: " << d.name1 << std::endl;

  if (!is_root) {
    d.name2 = fread_string(stream);
    std::cout << "Read name2: " << d.name2 << std::endl;
  }

  d.parent = fread_string(stream);
  std::cout << "Read parent: " << d.parent << std::endl;

  d.description = fread_string(stream);
  std::cout << "Read description: " << d.description << std::endl;

  d.ptr_path = fread_path(stream);  // Corrected line
  std::cout << "Read ptr_path: " << d.ptr_path << std::endl;

  d.unknown1 = fread_uint16_t(stream);
  std::cout << "Read unknown1: " << d.unknown1 << std::endl;

  d.id_varname = fread_string(stream);
  std::cout << "Read id_varname: " << d.id_varname << std::endl;

  d.name_varname = fread_string(stream);
  std::cout << "Read name_varname: " << d.name_varname << std::endl;

  d.documentation = fread_string(stream);
  std::cout << "Read documentation: " << d.documentation << std::endl;

  d.num_entities = fread_uint16_t(stream);
  std::cout << "Read num_entities: " << d.num_entities << std::endl;

  for (auto& c : d.unknown2) c = stream.get();
  std::cout << "Read unknown2" << std::endl;

  d.num_vars = fread_uint16_t(stream);
  std::cout << "Read num_vars: " << d.num_vars << std::endl;

  d.unknown3 = fread_uint16_t(stream);
  std::cout << "Read unknown3: " << d.unknown3 << std::endl;

  d.is_selectable = fread_uint16_t(stream);
  std::cout << "Read is_selectable: " << d.is_selectable << std::endl;

  for (auto& c : d.unknown6) c = stream.get();
  std::cout << "Read unknown6" << std::endl;

  d.weight_varname = fread_string(stream);
  std::cout << "Read weight_varname: " << d.weight_varname << std::endl;

  d.label = fread_string(stream);
  std::cout << "Read label: " << d.label << std::endl;

  d.is_sensible = fread_uint16_t(stream);
  std::cout << "Read is_sensible: " << d.is_sensible << std::endl;

  d.sensible_threshold = fread_uint16_t(stream);
  std::cout << "Read sensible_threshold: " << d.sensible_threshold << std::endl;

  std::cout << "Reading variables for entity: " << d.name1 << std::endl;
  for (uint16_t i = 0; i < d.num_vars; ++i) {
    VariableDescriptor variable = VariableDescriptor::fread(stream);
    std::cout << "Variable name: " << variable.name << std::endl;
    d.variables.push_back(variable);
  }

  return d;
}

std::ostream& operator<<(std::ostream& stream, const EntityDescriptor& d) {
  stream << "Name1: " << d.name1 << "\nName2: " << d.name2
         << "\nParent: " << d.parent << "\nDescription: " << d.description
         << "\nPTR file: " << d.ptr_path << "\nVar ID: " << d.id_varname
         << "\nVar Name: " << d.name_varname
         << "\nDocumentation: " << d.documentation
         << "\nNum Vars: " << d.num_vars
         << "\nWeighting variable: " << d.weight_varname
         << "\nLabel: " << d.label
         << "\nSelectable: " << (d.is_selectable ? "Yes" : "No") << "\nUNK2: ";
  for (const auto& c : d.unknown2) stream << c;
  stream << "\nUNK6: ";
  for (const auto& c : d.unknown6) stream << c;
  return stream;
}

}  // namespace Redatam
