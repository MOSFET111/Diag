#include "WmiCollector.h"

#include "logging/logger.h"

#include <Windows.h>
#include <comdef.h>
#include <Wbemidl.h>

#include <cstdint>
#include <string>
#include <variant>

#include <vector>
#include <cstdint>


// Link with WMI library (MSVC)
#pragma comment(lib, "wbemuuid.lib")

namespace diag
{

// Small RAII helper: initializes COM on construction, uninitializes on destruction.
class ComInit
{
public:
  explicit ComInit(DWORD coInitFlags)
  {
    hr_ = CoInitializeEx(nullptr, coInitFlags);
    initialized_ = SUCCEEDED(hr_);
  }

  ~ComInit()
  {
    if (initialized_)
      CoUninitialize();
  }

  HRESULT hr() const { return hr_; }
  bool ok() const { return initialized_; }

private:
  HRESULT hr_{E_FAIL};
  bool initialized_{false};
};

static bool readVariantString(VARIANT& vt, std::string& out)
{
  out.clear();

  if (vt.vt == VT_BSTR && vt.bstrVal)
  {
    _bstr_t b(vt.bstrVal);
    out = static_cast<const char*>(b);
    return true;
  }

  return false;
}

static bool readVariantUInt64(VARIANT& vt, std::uint64_t& out)
{
  out = 0;

  if (vt.vt == VT_UI8)
  {
    out = vt.ullVal;
    return true;
  }

  if (vt.vt == VT_I8)
  {
    out = static_cast<std::uint64_t>(vt.llVal);
    return true;
  }

  // Many WMI numeric values come as strings
  if (vt.vt == VT_BSTR && vt.bstrVal)
  {
    out = _wcstoui64(vt.bstrVal, nullptr, 10);
    return true;
  }

  return false;
}

static void collectGpus(IWbemServices* services, Logger& logger, diag::SystemInfo& info)
{
  IEnumWbemClassObject* enumerator = nullptr;

  HRESULT hr = services->ExecQuery(
    _bstr_t(L"WQL"),
    _bstr_t(L"SELECT Name, AdapterCompatibility, AdapterRAM FROM Win32_VideoController"),
    WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
    nullptr,
    &enumerator
  );

  if (FAILED(hr) || !enumerator)
  {
    logger.warn("GPU query failed (Win32_VideoController). HRESULT: " +
                std::to_string(static_cast<long>(hr)));
    return;
  }

  while (true)
  {
    IWbemClassObject* obj = nullptr;
    ULONG returned = 0;

    hr = enumerator->Next(WBEM_INFINITE, 1, &obj, &returned);
    if (FAILED(hr) || returned == 0 || !obj)
      break;

    diag::GpuInfo gpu;

    // Name
    {
      VARIANT vt{};
      VariantInit(&vt);
      if (SUCCEEDED(obj->Get(L"Name", 0, &vt, nullptr, nullptr)))
        readVariantString(vt, gpu.name);
      VariantClear(&vt);
    }

    // Vendor
    {
      VARIANT vt{};
      VariantInit(&vt);
      if (SUCCEEDED(obj->Get(L"AdapterCompatibility", 0, &vt, nullptr, nullptr)))
        readVariantString(vt, gpu.vendor);
      VariantClear(&vt);
    }

    // VRAM
    {
      VARIANT vt{};
      VariantInit(&vt);
      if (SUCCEEDED(obj->Get(L"AdapterRAM", 0, &vt, nullptr, nullptr)))
        readVariantUInt64(vt, gpu.vramBytes);
      VariantClear(&vt);
    }

    if (!gpu.name.empty())
      logger.info("Detected GPU: " + gpu.name);
    else
      logger.warn("Detected GPU row with empty Name.");

    info.gpus.push_back(std::move(gpu));

    obj->Release();
  }

  enumerator->Release();
}


// Initialize COM security settings.
// This is necessary for WMI to allow the application to impersonate the caller to query system details.
static bool initComSecurity(Logger& logger)
{
  const HRESULT hr = CoInitializeSecurity(
    nullptr,                        // security descriptor
    -1,                             // COM negotiates services
    nullptr,                        // authentication services
    nullptr,                        // reserved
    RPC_C_AUTHN_LEVEL_DEFAULT,      // authentication
    RPC_C_IMP_LEVEL_IMPERSONATE,    // impersonation
    nullptr,                        // authentication info
    EOAC_NONE,                      // additional capabilities
    nullptr                         // reserved
  );

  if (SUCCEEDED(hr))
    return true;

  if (hr == RPC_E_TOO_LATE)
  {
    logger.warn("CoInitializeSecurity was already called (RPC_E_TOO_LATE). Continuing.");
    return true;
  }

  logger.error("CoInitializeSecurity failed. HRESULT: " + std::to_string(static_cast<long>(hr)));
  return false;
}

// execute a WQL query and retrieve the first string result for a given property.
static bool queryFirstString(IWbemServices* services,
                             Logger& logger,
                             const wchar_t* wql,
                             const wchar_t* propName,
                             std::string& out)
{
  IEnumWbemClassObject* enumerator = nullptr;

  HRESULT hr = services->ExecQuery(
    _bstr_t(L"WQL"),
    _bstr_t(wql),
    WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
    nullptr,
    &enumerator
  );

  if (FAILED(hr) || !enumerator)
  {
    logger.warn("WMI ExecQuery failed for string property. HRESULT: " +
                std::to_string(static_cast<long>(hr)));
    return false;
  }

  IWbemClassObject* obj = nullptr;
  ULONG returned = 0;
  hr = enumerator->Next(WBEM_INFINITE, 1, &obj, &returned);

  if (FAILED(hr) || returned == 0 || !obj)
  {
    enumerator->Release();
    logger.warn("WMI query returned no results.");
    return false;
  }

  VARIANT vt{};
  VariantInit(&vt);

  hr = obj->Get(propName, 0, &vt, nullptr, nullptr);
  if (SUCCEEDED(hr) && vt.vt == VT_BSTR && vt.bstrVal)
  {
    // Convert wide BSTR to narrow std::string (simple ASCII-safe path).
    _bstr_t b(vt.bstrVal);
    out = static_cast<const char*>(b);
  }
  else
  {
    logger.warn("WMI property read failed or type mismatch.");
  }

  VariantClear(&vt);
  obj->Release();
  enumerator->Release();

  return !out.empty();
}




// execute a WQL query and retrieve the first results as a 64-bit unsigned integer.
// Handles string-to-int conversion if WMI returns a string type (common behavior).
static bool queryFirstUInt64(IWbemServices* services,
                             Logger& logger,
                             const wchar_t* wql,
                             const wchar_t* propName,
                             std::uint64_t& out)
{
  IEnumWbemClassObject* enumerator = nullptr;

  HRESULT hr = services->ExecQuery(
    _bstr_t(L"WQL"),
    _bstr_t(wql),
    WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
    nullptr,
    &enumerator
  );

  if (FAILED(hr) || !enumerator)
  {
    logger.warn("WMI ExecQuery failed for uint64 property. HRESULT: " +
                std::to_string(static_cast<long>(hr)));
    return false;
  }

  IWbemClassObject* obj = nullptr;
  ULONG returned = 0;
  hr = enumerator->Next(WBEM_INFINITE, 1, &obj, &returned);

  if (FAILED(hr) || returned == 0 || !obj)
  {
    enumerator->Release();
    logger.warn("WMI query returned no results.");
    return false;
  }

  VARIANT vt{};
  VariantInit(&vt);

  hr = obj->Get(propName, 0, &vt, nullptr, nullptr);
  if (SUCCEEDED(hr))
  {
    if (vt.vt == VT_BSTR && vt.bstrVal)
    {
      // Many WMI numeric fields are returned as strings.
      out = _wcstoui64(vt.bstrVal, nullptr, 10);
    }
    else if (vt.vt == VT_UI8)
    {
      out = vt.ullVal;
    }
    else if (vt.vt == VT_I8)
    {
      out = static_cast<std::uint64_t>(vt.llVal);
    }
    else
    {
      logger.warn("WMI uint64 property type mismatch.");
    }
  }
  else
  {
    logger.warn("WMI uint64 property read failed.");
  }

  VariantClear(&vt);
  obj->Release();
  enumerator->Release();

  return out != 0;
}


SystemInfo WmiCollector::collectSystemInfo(Logger& logger)
{
  SystemInfo info;

  // 1) Initialize COM for this thread.
  ComInit com(COINIT_MULTITHREADED);
  if (!com.ok())
  {
    logger.error("CoInitializeEx failed. HRESULT: " + std::to_string(static_cast<long>(com.hr())));
    return info;
  }

  // 2) Initialize COM security (needed for WMI).
  if (!initComSecurity(logger))
    return info;

  // Next chunk will go here:
  // - Create IWbemLocator
  // - ConnectServer to ROOT\\CIMV2
  // - Set proxy blanket
  // - Run WQL queries
  logger.info("COM initialized. Next: connect to WMI ROOT\\CIMV2.");

  

    // 3) Create WMI locator
  IWbemLocator* pLocator = nullptr;
  HRESULT hr = CoCreateInstance(
    CLSID_WbemLocator,
    nullptr,
    CLSCTX_INPROC_SERVER,
    IID_IWbemLocator,
    reinterpret_cast<void**>(&pLocator)
  );

  if (FAILED(hr))
  {
    logger.error("Failed to create IWbemLocator. HRESULT: " +
                 std::to_string(static_cast<long>(hr)));
    return info;
  }

  // 4) Connect to ROOT\\CIMV2
  IWbemServices* pServices = nullptr;
  hr = pLocator->ConnectServer(
    _bstr_t(L"ROOT\\CIMV2"), // namespace
    nullptr,                // user
    nullptr,                // password
    nullptr,                // locale
    0,                      // security flags
    nullptr,                // authority
    nullptr,                // context
    &pServices
  );

  if (FAILED(hr))
  {
    logger.error("Failed to connect to ROOT\\CIMV2. HRESULT: " +
                 std::to_string(static_cast<long>(hr)));
    pLocator->Release();
    return info;
  }
    // 5) Collect GPU info (before CPU/RAM)
  collectGpus(pServices, logger, info);
  // 6) Set proxy security so WMI can impersonate us
  hr = CoSetProxyBlanket(
    pServices,
    RPC_C_AUTHN_WINNT,
    RPC_C_AUTHZ_NONE,
    nullptr,
    RPC_C_AUTHN_LEVEL_CALL,
    RPC_C_IMP_LEVEL_IMPERSONATE,
    nullptr,
    EOAC_NONE
  );

  if (FAILED(hr))
  {
    logger.error("CoSetProxyBlanket failed. HRESULT: " +
                 std::to_string(static_cast<long>(hr)));
    pServices->Release();
    pLocator->Release();
    return info;
  }

  logger.info("Connected to WMI ROOT\\CIMV2 successfully.");

  // 7) Run WQL queries (MVP)
  queryFirstString(
    pServices,
    logger,
    L"SELECT Name FROM Win32_Processor",
    L"Name",
    info.cpu.name
  );

  queryFirstString(
    pServices,
    logger,
    L"SELECT Name FROM Win32_ComputerSystem",
    L"Name",
    info.computerName
  );

  queryFirstUInt64(
    pServices,
    logger,
    L"SELECT TotalPhysicalMemory FROM Win32_ComputerSystem",
    L"TotalPhysicalMemory",
    info.memory.totalBytes
  );

  queryFirstString(
    pServices,
    logger,
    L"SELECT Caption FROM Win32_OperatingSystem",
    L"Caption",
    info.os.caption
  );

  queryFirstString(
    pServices,
    logger,
    L"SELECT Version FROM Win32_OperatingSystem",
    L"Version",
    info.os.version
  );
  // BIOS: Vendor + Version (with fallback)
queryFirstString(
  pServices,
  logger,
  L"SELECT Manufacturer FROM Win32_BIOS",
  L"Manufacturer",
  info.bios.vendor
);

if (!queryFirstString(
      pServices,
      logger,
      L"SELECT SMBIOSBIOSVersion FROM Win32_BIOS",
      L"SMBIOSBIOSVersion",
      info.bios.version))
{
  // Fallback: some systems only fill "Version"
  queryFirstString(
    pServices,
    logger,
    L"SELECT Version FROM Win32_BIOS",
    L"Version",
    info.bios.version
  );
}

if (info.bios.vendor.empty())
  logger.warn("BIOS vendor is empty (Win32_BIOS.Manufacturer).");

if (info.bios.version.empty())
  logger.warn("BIOS version is empty (Win32_BIOS.SMBIOSBIOSVersion/Version).");
  // 7) Cleanup COM objects (success path)
  pServices->Release();
  pLocator->Release();

  logger.info("WMI MVP collection completed.");
  return info;

}

} // namespace diag
