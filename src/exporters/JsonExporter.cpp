#include "JsonExporter.h"
#include "../model/DeviceReport.h"
#include "../model/SystemInfo.h"
#include <string>

bool JsonExporter::exportToFile(const DeviceReport& report, const std::string& path)
{
  std::ofstream out(path);
  if (!out) return false;

  out << "{\n";
  out << "  \"toolName\": \"" << report.toolName << "\",\n";
  out << "  \"toolVersion\": \"" << report.toolVersion << "\",\n";
  out << "  \"generatedAt\": \"" << report.generatedAt << "\"\n";
  out << "}\n";
  return true;
}



static std::string jsonEscape(const std::string& s)
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
