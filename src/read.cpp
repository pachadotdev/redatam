#include <cpp11.hpp>
#include <fstream>
#include <string>

#include "primitives.h"
#include "read_dic.h"
#include "read_ptr.h"
#include "read_rbf.h"
#include "redatam.h"
#include "redatam_exception.h"
#include "redatam_reader.h"

using namespace cpp11;
using namespace Redatam;

[[cpp11::register]] strings read_redatam_(std::string file_path) {
  RedatamReader reader(file_path);
  reader.read_data();  // Use the correct method to read the data

  // Assuming the reader has a method to get the result as a string
  std::string result = reader.get_result();  // Retrieve the result

  std::vector<std::string> result_vector;
  result_vector.push_back(result);

  // Return the result as a cpp11 string
  return cpp11::writable::strings(result_vector);
}
