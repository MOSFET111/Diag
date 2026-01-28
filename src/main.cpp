#include <iostream>
#include <ctime>

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





int main() {
  diag::Logger logger("logs/run.log", true);
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