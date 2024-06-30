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

  void Process(const std::string &value) {
    std::string mutableValue = value;
    if (testing) {
      CountMsDOSChars(mutableValue);
    } else {
      ReencodeMsDOSChars(mutableValue);
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

  void ReencodeMsDOSChars(std::string &value) {
    for (auto &ch : value) {
      if (std::find(oldChars.begin(), oldChars.end(), ch) != oldChars.end()) {
        ch = ReencodeChar(ch);
      }
    }
  }

  char ReencodeChar(unsigned char ch) { // Use unsigned char for extended ASCII
    switch (ch) {
    case 160:
      return static_cast<char>(225); // ASCII value for 'á'
    case 130:
      return static_cast<char>(233); // ASCII value for 'é'
    case 162:
      return static_cast<char>(243); // ASCII value for 'ó'
    case 163:
      return static_cast<char>(250); // ASCII value for 'ú'
    case 164:
      return static_cast<char>(241); // ASCII value for 'ñ'
    default:
      return ch;
    }
  }

public:
  MsDOSEncoder(RedatamDatabase *database, bool testingInit)
      : db(database), matches(), sumMatches(0), testing(testingInit) {}

  void ReencodeLabels() {
    for (auto &entity : db->Entities) {
      for (auto &variable : entity->getVariables()) {
        if (!variable->ValueLabels.empty()) {
          for (auto &label : variable->ValueLabels) {
            Process(label.Value);
          }
        }
        Process(variable->Label);
      }
      Process(entity->getLabel());
    }
  }

  bool RequiresProcessing() const { return sumMatches > 3; }
};

} // namespace RedatamLib

#endif // DOSENCODER_HPP
