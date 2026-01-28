#pragma once

#include <string>
#include "../model/SystemInfo.h" // or "SystemInfo.h" depending on your include style

namespace diag {

  struct DeviceReport {
    std::string toolName;
    std::string toolVersion;
    std::string generatedAt;

    SystemInfo system; // ✅ add this

    DeviceReport() = default;

    DeviceReport(
      std::string name,
      std::string version,
      std::string timestamp
    )
    : toolName(std::move(name)),
      toolVersion(std::move(version)),
      generatedAt(std::move(timestamp))
    {}
  };

} // namespace diag
