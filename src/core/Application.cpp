#include "Application.h"
#include "logging/Logger.h"
#include "model/DeviceReport.h"
#include "exporters/JsonExporter.h"
#include <iomanip>
#include <sstream>
#include <ctime>

static std::string fileSafeTimestamp()
{
  std::time_t now = std::time(nullptr);
  std::tm tm{};
  localtime_s(&tm, &now);

  std::ostringstream oss;
  oss << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S");
  return oss.str();
}

namespace diag{
  int Application::run (Logger& logger){
    logger.info("Starting Diag tool");

    diag::DeviceReport report(
      "Diag",
      "0.1.0",
      fileSafeTimestamp()
    );

    const std::string outputPath("report.json");
    logger.info("Exporting report.json to " + outputPath);
    if(diag::JsonExporter::exportToFile(report, outputPath)){
      logger.info("Export finished successfully to: " + outputPath);
    } else {  
      logger.error("Failed to export report.json");
      return 1;
    }
    logger.info("application finished successfully");
    return 0;
  }
}//namespace diag