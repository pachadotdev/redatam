#ifndef REDATAMLIB_MS_DOS_ENCODER_HPP
#define REDATAMLIB_MS_DOS_ENCODER_HPP

#include "RedatamDatabase.hpp"
#include "DataBlock.hpp"
#include <unordered_map>
#include <vector>
#include <string>

namespace RedatamLib {

class MsDOSEncoder {
public:
  MsDOSEncoder(RedatamDatabase *database, bool testingInit = false);

  void ReencodeLabels();
  bool RequiresProcessing();

private:
  RedatamDatabase *db;
  std::unordered_map<char, int> matches;
  int sumMatches;
  bool testing;
  std::vector<int> oldChars = {160, 130, 162, 163, 164};

  bool ProcessEntities(const std::vector<std::shared_ptr<Entity>> &entities);
  void Process(std::string &value);
  static std::string ReplaceMsDOSChars(const std::string &value);
  void CountMsDOSChars(const std::string &value);
};

} // namespace RedatamLib

#endif // REDATAMLIB_MS_DOS_ENCODER_HPP
