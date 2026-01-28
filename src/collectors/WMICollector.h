#pragma once

#include "model/SystemInfo.h"

namespace diag
{

class Logger;

  class WmiCollector
  {
  public:
    static SystemInfo collectSystemInfo(Logger& logger);
  };

} // namespace diag
