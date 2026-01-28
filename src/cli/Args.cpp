#include "Args.h"
#include <sstream>
#include <string>


namespace diag {

Args args;

Args parseArgs(int argc, char** argv){

  for(int i =1; i < argc; ++i){

    const std::string token = argv[i];

    if(token == "--help" | token == "-h"){
      args.showHelp = true;
      return args;
    }

    if(token == "--out"){
      //needs a value after it, so it cant be the last member in the list
      if(i + 1 >= argc){
        args.showHelp = true;
        return args;
      }
      args.outPath = argv[i + 1];
      continue;
    }

    if (token == "--no-log-echo"){
      args.echoLogs = false;
      continue;
    }
  }
  return args;
}
  std::string helpText() {
    std::ostringstream oss;
      oss << "Diag - device diagnostics tool\n";
      oss << "\n";
      oss << "Usage:\n";
      oss << "  Diag.exe [options]\n";
      oss << "\n";
      oss << "Options:\n";
      oss << "  -h, --help           Show this help message\n";
      oss << "  --out <path>         Output JSON path (default: report.json)\n";
      oss << "  --log-echo           Echo logs to console (default: on)\n";
      oss << "  --no-log-echo        Disable console log echo\n";
      oss << "\n";
      oss << "Examples:\n";
      oss << "  Diag.exe --out out/report.json\n";
      oss << "  Diag.exe --no-log-echo\n";
    return oss.str();
  };



}//namespace diag