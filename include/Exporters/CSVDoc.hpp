#ifndef REDATAMLIB_CSVDOC_HPP
#define REDATAMLIB_CSVDOC_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>

namespace RedatamLib {

class CSVDoc {
public:
  CSVDoc(const std::string &filename);
  ~CSVDoc();

  void Close();
  void CommitDictionary();
  void WriteLine(const std::vector<std::string> &values);
  void WriteVariableLabels();
  std::string replaceAll(std::string str, const std::string &from,
                         const std::string &to);

  std::vector<std::string> Columns;
  std::vector<std::string> Labels;

private:
  std::string Filename;
  std::ofstream Stream;
  const char delimiter = ',';
  const char quote = '"';
};

} // namespace RedatamLib

#endif // REDATAMLIB_CSVDOC_HPP
