#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>
#include "core/Application.h"
#include "logging/Logger.h"
#include "cli/Args.h"

static std::string fileSafeTimestamp()
{
  std::time_t now = std::time(nullptr);
  std::tm tm{};
  localtime_s(&tm, &now);

  std::ostringstream oss;
  oss << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S");
  return oss.str();
}


int main(int argc, char** argv)
{
  const diag::Args args = diag::parseArgs(argc, argv);
  if (args.showHelp)
  {
    std::cout << diag::helpText();
    return 0;
  }

  const std::string logPath = "logs/run_" + fileSafeTimestamp() + ".log";
  diag::Logger logger(logPath, args.echoLogs);

  diag::Application app;
  return app.run(logger, args.outPath);
}
