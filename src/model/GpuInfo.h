#pragma once
#include <string>
#include <cstdint>

namespace diag {

struct GpuInfo {
  std::string name;
  std::string vendor;
  std::uint64_t vramBytes{0};
};

} // namespace diag
