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
      
      explicit Logger(const std::string& filePath, bool echoToConsole = false);
      
      void info(const std::string& message);
      void warn(const std::string& message);
      void error(const std::string& message);

      void log(LogLevel level, const std::string& message);

    private:
      std::ofstream out_;
      std::string filePath_;
      bool echoToConsole_;
  
  };//class Logger

}//namespace diag
