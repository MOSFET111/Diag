#include "Logger.h"
#include <filesystem>
#include <ctime>
#include <iomanip>
#include <sstream>

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

  Logger::Logger(const std::string& filePath)
  : filePath_(filePath)
  {
    try{

      fs::path p(filePath_);
      if(p.has_parent_path())
        fs::create_directories(p.parent_path());
      out_.open(filePath_, std::ios::app);
    }catch(...){}
  }
      
  
  bool Logger::isOpen() const{
    return out_.is_open();
  }
  
  void Logger::log(LogLevel level, const std::string& message) {
    if(!isOpen()) return;
    
    out_ << "[" << nowTimeStamp() << "] ";
    out_ <<"[" << levelToString(level) << "] ";
    out_ << message << '\n';
    out_.flush();
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