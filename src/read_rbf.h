#pragma once

#include "primitives.h"

namespace Redatam {

template <typename OutputIterator>
void read_ptr(const boost::filesystem::path &ptr_path, OutputIterator output) {
  // Open the file in binary mode
  std::fstream ptr_file(ptr_path, std::ios_base::in | std::ios_base::binary);

  // Throw an exception if the file cannot be opened
  if (!ptr_file)
    throw Exception(std::string("Could not read ptr file: ") +
                    ptr_path.string());

  // Initialize variables
  int parent_id = 0;
  unsigned current_accum = 0;
  unsigned previous_accum =
      fread_uint32_t(ptr_file); // always reads 0 at offset 0

  // Read the file and output the parent_id for each record
  for (; ptr_file; previous_accum = current_accum) {
    current_accum = fread_uint32_t(ptr_file);
    ++parent_id;
    for (unsigned i = 0; i < current_accum - previous_accum; ++i)
      *(output++) = parent_id;
  }
}

} // namespace Redatam