#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <vector>

#include "Variable.h"

class Entity {
public:
  std::string name;
  std::vector<Variable> variables;
  std::vector<std::vector<std::string>> data;
};

#endif // ENTITY_H
