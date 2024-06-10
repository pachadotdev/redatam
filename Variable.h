#ifndef VARIABLE_H
#define VARIABLE_H

#include <string>
#include <vector>

class Variable {
public:
  std::string name;
  std::string filename;
  std::string varType;
  int fieldSize;
};

std::vector<std::string> readData(const std::string &filename, int fieldSize);

#endif // VARIABLE_H
