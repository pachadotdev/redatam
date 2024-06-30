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
      ReencodeMsDOSChars(value);
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

  char ReencodeChar(char ch) {
    switch (ch) {
    case 160:
      return 'á';
    case 130:
      return 'é';
    case 162:
      return 'ó';
    case 163:
      return 'ú';
    case 164:
      return 'ñ';
    default:
      return ch;
    }
  }

public:
  MsDOSEncoder(RedatamDatabase *database, bool testing)
      : db(database), testing(testing), sumMatches(0) {}

  void ReencodeLabels() {
    for (auto &entity : db->Entities) {
      for (auto &variable : entity.Variables) {
        if (variable.ValueLabels != nullptr) {
          for (auto &label : *variable.ValueLabels) {
            Process(label.Value);
          }
        }
        Process(variable.Label);
      }
      Process(entity.Label);
    }
  }

  bool RequiresProcessing() const { return sumMatches > 3; }
};

}

#endif // DOSENCODER_HPP
