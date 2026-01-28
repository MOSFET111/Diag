#pragma once

#include <fstream>
#include <string>
#include "../model/DeviceReport.h"
#include "../utils/SizeUtils.h"
namespace diag {

  // ✅ Header-only helper must be inline to avoid multiple definition issues
  static inline std::string jsonEscape(const std::string& s)
  {
    std::string out;
    out.reserve(s.size() + 8);

    for (char c : s)
    {
      switch (c)
      {
        case '\\': out += "\\\\"; break;
        case '"':  out += "\\\""; break;
        case '\n': out += "\\n";  break;
        case '\r': out += "\\r";  break;
        case '\t': out += "\\t";  break;
        default:   out += c;      break;
      }
    }
    return out;
  }

  class JsonExporter
  {
  public:
    static bool exportToFile(
      const DeviceReport& report,
      const std::string& filePath
    )
    {
      std::ofstream out(filePath);
      if (!out.is_open())
        return false;

      out << "{\n";
      out << "  \"toolName\": \"" << jsonEscape(report.toolName) << "\",\n";
      out << "  \"toolVersion\": \"" << jsonEscape(report.toolVersion) << "\",\n";
      out << "  \"generatedAt\": \"" << jsonEscape(report.generatedAt) << "\",\n";

    // systemInfo{}
      out << "  \"system\": {\n";

      // os
      out << "    \"os\": {\n";
      out << "      \"caption\": \"" << jsonEscape(report.system.os.caption) << "\",\n";
      out << "      \"version\": \"" << jsonEscape(report.system.os.version) << "\"\n";
      out << "    },\n";

      // cpu
      out << "    \"cpu\": {\n";
      out << "      \"name\": \"" << jsonEscape(report.system.cpu.name) << "\"\n";
      out << "    },\n";

      // memory
      out << "    \"memory\": {\n";
      out << "      \"totalBytes\": " << report.system.memory.totalBytes << ",\n";
      out << "      \"totalMB\": " << diag::utils::bytesToMB(report.system.memory.totalBytes) << ",\n";
      out << "      \"totalGB\": " << diag::utils::bytesToGB(report.system.memory.totalBytes) << "\n";
      out << "    },\n";

      // bios
      out << "    \"bios\": {\n";
      out << "      \"vendor\": \"" << jsonEscape(report.system.bios.vendor) << "\",\n";
      out << "      \"version\": \"" << jsonEscape(report.system.bios.version) << "\"\n";
      out << "    },\n";


      //gpus
      out << "    \"gpus\": [\n";
      for (std::size_t i = 0; i < report.system.gpus.size(); ++i)
      {
        const auto& gpu = report.system.gpus[i];

        out << "      {\n";
        out << "        \"name\": \"" << jsonEscape(gpu.name) << "\",\n";
        out << "        \"vendor\": \"" << jsonEscape(gpu.vendor) << "\",\n";
        out << "        \"vramBytes\": " << gpu.vramBytes << ",\n";
        out << "        \"vramMB\": " << diag::utils::bytesToMB(gpu.vramBytes) << ",\n";
        out << "        \"vramGB\": " << diag::utils::bytesToGB(gpu.vramBytes) << "\n";
        out << "      }";

        if (i + 1 < report.system.gpus.size())
          out << ",";
        out << "\n";
      }
      out << "    ]\n";
      out << "  }\n";
    out << "}\n";
    return true;
  }
  };

} // namespace diag
