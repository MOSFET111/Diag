#pragma once

#include <string>

namespace diag {

  struct DeviceReport {
    std::string toolName;
    std::string toolVersion;
    std::string generatedAt;

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
}// namespace diag