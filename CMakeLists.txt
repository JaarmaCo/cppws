cmake_minimum_required(VERSION 3.10..3.21)

project(proj C CXX)

include(GNUInstallDirs)
include(FetchContent)
include(cmake/update-dependencies.cmake)

find_package(CURL REQUIRED)

add_library(cppws
  src/cppws.cpp
  src/socket.cpp
  src/url.cpp
  src/http_request.cpp)

target_include_directories(cppws PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_link_libraries(cppws
  PRIVATE
    CURL::libcurl
    spdlog::spdlog_header_only
    nlohmann_json::nlohmann_json)

enable_testing()
add_subdirectory(test)

