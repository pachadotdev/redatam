#ifndef REDATAMLIB_DOSENCODER_HPP
#define REDATAMLIB_DOSENCODER_HPP

#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <memory>
#include "RedatamDatabase.hpp"
#include "Entity.hpp"

namespace RedatamLib {

class MsDOSEncoder {
private:
  std::vector<int> oldChars = {160, 130, 162, 163, 164};
  RedatamLib::RedatamDatabase *db;
  std::unordered_map<char, int> matches;
  int sumMatches = 0;
  bool testing = false;

  void Process(std::string &value) {
    if (testing) {
      CountMsDOSChars(value);
    } else {
      value = ReplaceMsDOSChars(value);
    }
  }

  void CountMsDOSChars(const std::string &value) {
    for (auto ch : value) {
      if (std::find(oldChars.begin(), oldChars.end(), static_cast<int>(ch)) !=
          oldChars.end()) {
        matches[ch]++;
        sumMatches++;
      }
    }
  }

  std::string ReplaceMsDOSChars(const std::string &value) {
    std::string result;
    for (auto ch : value) {
      result += ReencodeChar(ch);
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

  bool ProcessEntities(const std::vector<std::shared_ptr<Entity>> &entities) {
    for (const auto &entity : entities) {
      for (auto &variable : entity->getVariables()) {
        Process(variable->Label);
        for (auto &valueLabel : variable->ValueLabels) {
          Process(valueLabel.Value);
        }
      }
      if (ProcessEntities(db->GetEntities()) && testing) {
        return true;
      }
    }
    return (matches.size() >= 3 && sumMatches >= 6);
  }

public:
  MsDOSEncoder(RedatamDatabase *database, bool testingInit = false)
      : db(database), testing(testingInit) {}

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
};

} // namespace RedatamLib

#endif // REDATAMLIB_DOSENCODER_HPP
