#include "Application.h"

#include "logging/Logger.h"
#include "model/DeviceReport.h"
#include "exporters/JsonExporter.h"
#include "collectors/WmiCollector.h"

#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

static std::string fileSafeTimestamp()
{
  std::time_t now = std::time(nullptr);
  std::tm tm{};
  localtime_s(&tm, &now);

  std::ostringstream oss;
  oss << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S");
  return oss.str();
}

namespace diag {

int Application::run(Logger& logger, const std::string& outPath)
{
  logger.info("Starting Diag tool");

  // 1) Create report (metadata)
  DeviceReport report(
    "Diag",
    "0.1.0",
    fileSafeTimestamp()
  );

  // 2) Collect system info
  logger.info("Starting WMI collection...");
  report.system = WmiCollector::collectSystemInfo(logger);
  logger.info("Finished WMI collection.");

  // 3) Export
  logger.info("Exporting report to: " + outPath);
  if (!JsonExporter::exportToFile(report, outPath))
  {
    logger.error("Failed to export report.");
    return 1;
  }

  logger.info("Export finished successfully.");
  return 0;
}

} // namespace diag
