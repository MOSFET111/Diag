#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>
#include "model/DeviceReport.h"
#include "exporters/JsonExporter.h"
#include "logging/Logger.h"

static std::string currentTimeStamp()
{
  std::time_t now = std::time(nullptr);
  std::string ts = std::ctime(&now);

  //ctime ends with /n we need to remove it to clean the json
  if(!ts.empty() && ts.back() == '\n')
    ts.pop_back();

  return ts;
}

static std::string fileSafeTimestamp()
{
  std::time_t now = std::time(nullptr);
  std::tm tm{};
  localtime_s(&tm, &now);

  std::ostringstream oss;
  oss << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S");
  return oss.str();
}




int main() {
  const std::string logPath = "logs/run_" + fileSafeTimestamp() + ".log";
  diag::Logger logger(logPath, true);
  logger.info("Starting Diag tool");


  diag::DeviceReport report(
    "Diag",
    "0.1.0",
    currentTimeStamp()
  );
  const std::string outputPath("report.json");

  logger.info("Exporting report.json to " + outputPath);
  if(diag::JsonExporter::exportToFile(report, outputPath)){
    logger.info("Export finished successfully to: " + outputPath);
  } else {  
    logger.error("Faild to export report.json");
    return 1;
  }
  
  return 0;
}