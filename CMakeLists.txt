cmake_minimum_required(VERSION 3.20)
project(Diag LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Diag
  src/main.cpp
  src/logging/Logger.cpp
  src/core/Application.cpp
  src/cli/Args.cpp
  src/collectors/WMICollector.cpp
)

target_include_directories(Diag PRIVATE src)
