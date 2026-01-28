#pragma once

#include <string>

namespace diag
{

  struct Args
  {
    std::string outPath = "report.json";
    bool echoLogs = true;
    bool showHelp = false;
  };

  Args parseArgs(int argc, char** argv);
  std::string helpText();

} // namespace diag
