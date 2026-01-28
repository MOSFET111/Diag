#pragma once
#include <cstdint>

namespace diag::utils {

  inline double bytesToGB(std::uint64_t bytes) {
    return static_cast<double>(bytes) / (1024.0 * 1024 * 1024);
  }

  inline double bytesToMB(std::uint64_t bytes) {
    return static_cast<double>(bytes) / (1024.0 * 1024);
  }

}
