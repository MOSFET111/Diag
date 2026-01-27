#include <iostream>
#include <ctime>

#include "model/DeviceReport.h"
#include "exporters/JsonExporter.h"

int main() {
 
  diag::DeviceReport report(
    "Diag",
    "0.1.0",
    currentTimeStamp()
  );
  const std::string outputPath("report.json");

  if(diag::JsonExporter::exportToFile(report, outputPath)){
    
    std::cout << "Report exported to " << outputPath << std::endl;
  
  } else {  
    
    std::cerr << "Failed to export report" << std::endl;
  
  }

  return 0;
}