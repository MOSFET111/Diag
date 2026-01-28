#pragma once

#include <string>
#include <vector>

#include "OsInfo.h"
#include "CpuInfo.h"
#include "MemoryInfo.h"
#include "BiosInfo.h"
#include "GpuInfo.h"

namespace diag {

struct SystemInfo {
  std::string computerName;

  OsInfo os;
  CpuInfo cpu;
  MemoryInfo memory;
  BiosInfo bios;

  std::vector<GpuInfo> gpus;
};

} // namespace diag
