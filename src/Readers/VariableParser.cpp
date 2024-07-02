#include "VariableParser.hpp"

namespace RedatamLib {

void VariableParser::ParseValueLabels() {
  std::vector<std::string> items = split(variable->ValuesLabelsRaw, '\t');
  variable->ValueLabels.clear();
  if (variable->ValuesLabelsRaw.empty())
    return;

  for (const std::string &item : items) {
    size_t i = item.find(' ');
    if (i == std::string::npos)
      break;
    std::string part1 = item.substr(0, i);
    std::string part2 = item.substr(i + 1);
    int x;
    if (!tryParse(part1, x)) {
      variable->ValueLabels.emplace_back("0", item);
    } else {
      variable->ValueLabels.emplace_back(part1, part2);
    }
  }
}

void VariableParser::ParseDecimals(std::string &extras) {
  if (extras.find("DECIMALS ") == 0) {
    std::string label = eatStringFromString(extras);
    std::string value = eatStringFromString(extras);
    variable->Decimals = std::stoi(value);
  }
}

std::string VariableParser::GetMissingLabel(std::string &extras,
                                            const std::string &tag) {
  if (extras.find(tag) == 0) {
    std::string label = eatStringFromString(extras);
    std::string value = eatStringFromString(extras);
    variable->ValueLabels.emplace_back(value, label);
  }
  return extras;
}

void VariableParser::ParseDeclaration() {
  std::string info = variable->Declaration;
  std::string dataset = eatStringFromString(info);
  std::string type = eatStringFromString(info);
  std::string fileRaw = eatStringFromString(info);
  std::string sizeLabel = eatStringFromString(info);
  std::string size = eatStringFromString(info);

  if (type == "BIN") {
    variable->Size = std::stoi(size);
    variable->BinaryDataSet = true;
  } else if (type == "PCK" || type == "CHR") {
    variable->Size = std::stoi(size);
  } else {
    throw std::runtime_error("Data type '" + type + "' is not supported.");
  }
  variable->Filename = fileRaw;
}

void VariableParser::ParseMissingAndPrecision() {
  std::string extras = variable->Group;
  extras = GetMissingLabel(extras, "MISSING");
  extras = GetMissingLabel(extras, "NOTAPPLICABLE");
  ParseDecimals(extras);
}

std::vector<std::string> VariableParser::split(const std::string &s,
                                               char delimiter) {
  std::vector<std::string> tokens;
  std::string token;
  std::istringstream tokenStream(s);
  while (std::getline(tokenStream, token, delimiter)) {
    tokens.push_back(token);
  }
  return tokens;
}

bool VariableParser::tryParse(const std::string &str, int &result) {
  try {
    result = std::stoi(str);
    return true;
  } catch (const std::invalid_argument &) {
    return false;
  } catch (const std::out_of_range &) {
    return false;
  }
}

std::string VariableParser::eatStringFromString(std::string &extras) {
  if (extras.find("'") == 0) {
    size_t nEnd = extras.find("'", 1);
    if (nEnd != std::string::npos) {
      std::string retQ = extras.substr(1, nEnd - 1);
      extras = extras.substr(nEnd + 1).erase(0, extras.find_first_not_of(" "));
      return retQ;
    }
  }

  size_t n = extras.find(" ");
  if (n == std::string::npos)
    n = extras.size();
  std::string ret = extras.substr(0, n);
  if (n < extras.size())
    extras = extras.substr(n + 1);
  else
    extras.clear();
  return ret;
}

} // namespace RedatamLib
