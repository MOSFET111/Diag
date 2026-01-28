#pragma once
#include <string>

namespace diag {
  class Logger;
  
  class Application {
    public:
    int run (Logger& logger, const std::string& outPath);
  };

}//namespace diag