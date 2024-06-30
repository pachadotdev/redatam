#ifndef DOSENCODER_HPP
#define DOSENCODER_HPP

#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <sstream>
#include "RedatamDatabase.hpp"
#include "Entity.hpp"

namespace RedatamLib {

class MsDOSEncoder {
private:
  std::vector<int> oldChars = {160, 130, 162, 163, 164};
  RedatamDatabase *db;
  std::unordered_map<char, int> matches;
  int sumMatches;
  bool testing;

  void Process(std::string &value) {
    if (testing) {
      CountMsDOSChars(value);
    } else {
      value = ReplaceMsDOSChars(value);
    }
  }

  void CountMsDOSChars(const std::string &value) {
    for (auto ch : value) {
      if (std::find(oldChars.begin(), oldChars.end(), ch) != oldChars.end()) {
        matches[ch]++;
        sumMatches++;
      }
    }
  }

  std::string ReplaceMsDOSChars(const std::string &value) {
    std::string result = value;
    for (auto &ch : result) {
      if (std::find(oldChars.begin(), oldChars.end(), ch) != oldChars.end()) {
        ch = ReencodeChar(ch);
      }
    }
    return result;
  }

  char ReencodeChar(unsigned char ch) {
    switch (ch) {
    case 160:
      return static_cast<char>(225);
    case 130:
      return static_cast<char>(233);
    case 162:
      return static_cast<char>(243);
    case 163:
      return static_cast<char>(250);
    case 164:
      return static_cast<char>(241);
    default:
      return ch;
    }
  }

public:
  MsDOSEncoder(RedatamDatabase *database, bool testingInit)
      : db(database), matches(), sumMatches(0), testing(testingInit) {}

  void ReencodeLabels() {
    testing = false;
    ProcessEntities(db->Entities);
  }

  bool RequiresProcessing() {
    testing = true;
    matches.clear();
    sumMatches = 0;
    return ProcessEntities(db->Entities);
  }

  bool ProcessEntities(const std::vector<std::shared_ptr<Entity>> &entities) {
    for (const auto &entity : entities) {
      for (const auto &variable : entity->getVariables()) {
        std::string label = variable->Label;
        Process(label);
        variable->Label = label;

        for (auto &valueLabel : variable->ValueLabels) {
          std::string value = valueLabel.Value;
          Process(value);
          valueLabel.Value = value;
        }
      }
      if (ProcessEntities(entity->getChildren())) {
        if (testing)
          return true;
      }
    }
    return (matches.size() >= 3 && sumMatches >= 6);
  }
};

} // namespace RedatamLib

#endif // DOSENCODER_HPP
