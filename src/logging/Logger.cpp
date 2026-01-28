#include "Logger.h"
#include <filesystem>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <iostream>

namespace fs = std::filesystem;

static std::string nowTimeStamp() {
  std::time_t now = std::time(nullptr);
  std::tm tm{};
  localtime_s(&tm, &now);
  std::ostringstream oss;
  oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
  return oss.str();
}

static const char* levelToString(diag::LogLevel level) {
  switch (level) {
    case diag::LogLevel::Info: return "INFO";
    case diag::LogLevel::Warn: return "WARN";
    case diag::LogLevel::Error: return "ERROR";
    default: return "INFO";
  }
}

namespace diag
{

  Logger::Logger(const std::string& filePath, bool echoToConsole)
  : filePath_(filePath), echoToConsole_(echoToConsole)
  {
    try{
      fs::path p(filePath_);
      if(p.has_parent_path())
        fs::create_directories(p.parent_path());
      out_.open(filePath_, std::ios::app);
    }catch(const std::exception& e){
      if(echoToConsole_)
        std::cerr << "Failed to initialize logger: " << e.what() << "\n";
    }catch(...){
      if(echoToConsole_)
        std::cerr << "Failed to initialize logger: Unknown error\n";
    }
  }
      
  
  bool Logger::isOpen() const{
    return out_.is_open();
  }
  
  void Logger::log(LogLevel level, const std::string& message) {
    const std::string line =
      "[" + nowTimeStamp() + "] [" + levelToString(level) + "] " + message;
    
    if(isOpen()) {
       out_ << line << '\n';
       out_.flush();
    }
    
    if(echoToConsole_){
      if(level == LogLevel::Error)
        std::cerr << line << "\n";
      else
        std::cout << line << "\n";
    }
  }
    
  void Logger::info(const std::string& message) {
    log(LogLevel::Info, message);
  }
  
  void Logger::warn(const std::string& message) {
    log(LogLevel::Warn, message);
  }
    
  void Logger::error(const std::string& message) {
    log(LogLevel::Error, message);
  }
    
}