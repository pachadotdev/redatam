#include <array>
#include <iomanip>
#include <iostream>

namespace redatam {
  template <typename T, size_t N>
  std::ostream& operator<<(std::ostream& stream, const std::array<T, N>& arr) {
    for (const auto& c : arr) {
      stream << "0x" << std::setbase(16) << std::setw(2) << std::setfill('0')
            << static_cast<int>(c) << ' ';
    }
    return stream << std::setbase(10) << std::setfill('0');
  }
}  // namespace redatam
