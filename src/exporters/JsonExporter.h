#pragma once

#include <fstream>
#include <string>

#include "../model/DeviceReport.h"

namespace diag {

    class JsonExporter 
  {
    public: 
    static bool exportToFile(
      const DeviceReport& report,
      const std::string& filePath
    )//bool exportToFile

    {
      std::ofstream out(filePath);
      if(!out.is_open())
        return false;
      
      out << "{\n";
      out << "  \"toolName\": \"" << report.toolName << "\",\n";
      out << "  \"toolVersion\": \"" << report.toolVersion << "\",\n";
      out << "  \"generatedAt\": \"" << report.generatedAt << "\"\n";
      out << "}";
  
      out.close();

      return true;
    }//don't forget we removed the , of the last line in the json output if you add new lines add it back
    
  
  };// class jsonExporter
}// namespace diag