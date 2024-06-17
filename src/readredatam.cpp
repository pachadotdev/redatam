#include <cpp11.hpp>
#include <string>
#include <vector>

using namespace cpp11;

// foo <- create_dummy_df()
// bar <- as.data.frame(foo)
        
[[cpp11::register]] data_frame create_dummy_df_() {
  writable::strings names = {"a", "b"};
  writable::integers a_values = {1, 2, 3};
  writable::doubles b_values = {4.0, 5.0, 6.0};

  writable::list df({a_values, b_values});
  df.names() = names;

  return df;
}

// Export to R

[[cpp11::register]] strings read_redatam_(strings dicx_file) {
  writable::strings a = {"a"};
  return a;
}
