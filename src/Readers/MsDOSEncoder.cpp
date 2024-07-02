#include "MsDOSEncoder.hpp"
#include "DataBlock.hpp"
#include <stdexcept>
#include <iostream>
#include <codecvt>

namespace RedatamLib {

MsDOSEncoder::MsDOSEncoder(RedatamDatabase *database, bool testingInit)
    : db(database), testingInit(testingInit) {
  if (testingInit) {
    std::cout << "Testing initialization of MsDOSEncoder" << std::endl;
  }
}

void MsDOSEncoder::ReencodeLabels() {
  testing = false;
  ProcessEntities(db->Entities);
}

bool MsDOSEncoder::RequiresProcessing() {
  testing = true;
  matches.clear();
  sumMatches = 0;
  return ProcessEntities(db->Entities);
}

bool MsDOSEncoder::ProcessEntities(
    const std::vector<std::shared_ptr<Entity>> &entities) {
  for (const auto &entity : entities) {
    for (const auto &variable : entity->Variables) {
      Process(variable->Label);
      for (const auto &valueLabel : variable->ValueLabels) {
        Process(valueLabel->Value);
      }
    }
    if (ProcessEntities(entity->Children)) {
      if (testing)
        return true;
    }
  }
  return (matches.size() >= 3 && sumMatches >= 6);
}

void MsDOSEncoder::Process(std::string &value) {
  if (testing) {
    CountMsDOSChars(value);
  } else {
    value = ReplaceMsDOSChars(value);
  }
}

std::string MsDOSEncoder::ReplaceMsDOSChars(const std::string &value) {
  std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
  std::wstring wide = converter.from_bytes(value);
  std::vector<char> text(wide.size());
  for (size_t i = 0; i < wide.size(); ++i) {
    text[i] = static_cast<char>(wide[i]);
  }
  return std::string(text.begin(), text.end());
}

void MsDOSEncoder::CountMsDOSChars(const std::string &value) {
  for (char c : value) {
    if (std::find(oldChars.begin(), oldChars.end(), static_cast<int>(c)) !=
        oldChars.end()) {
      matches[c]++;
      sumMatches++;
    }
  }
}

} // namespace RedatamLib
