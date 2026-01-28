#pragma once

#include <fstream>
#include <string>

namespace diag {
  enum class LogLevel
  {
    Info,
    Warn,
    Error
  };//class LogLevel

  class Logger
  {
    public:
      bool isOpen() const ;
      
      Logger(const std::string& filePath);
      
      void info(const std::string& message);
      void warn(const std::string& message);
      void error(const std::string& message);

      void log(LogLevel level, const std::string& message);

    private:
      std::ofstream out_;
      std::string filePath_;
  
  };//class Logger

}//namespace diag
