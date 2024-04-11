cmake_minimum_required(VERSION 3.22)

project(workout-msg-editor VERSION 0.1.5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_DEBUG_POSTFIX "-d")

add_subdirectory(ext)

configure_file(src/build_version.hpp.in "${CMAKE_CURRENT_BINARY_DIR}/include/wme/build_version.hpp" @ONLY NEWLINE_STYLE UNIX)
configure_file(src/ci.yml.in "${CMAKE_CURRENT_SOURCE_DIR}/.github/workflows/ci.yml" @ONLY NEWLINE_STYLE UNIX)

add_executable(${PROJECT_NAME} WIN32)

target_link_libraries(${PROJECT_NAME} PRIVATE
  wme::ext
)

file(GLOB_RECURSE sources LIST_DIRECTORIES false CONFIGURE_DEPENDS "src/*.[hc]pp")

target_sources(${PROJECT_NAME} PRIVATE
  ${sources}
)

target_include_directories(${PROJECT_NAME} PRIVATE
  src
  "${CMAKE_CURRENT_BINARY_DIR}/include"
)

target_precompile_headers(${PROJECT_NAME} PRIVATE
  <vulkan/vulkan.hpp>
  <fmt/format.h>
  <algorithm>
  <atomic>
  <chrono>
  <cstdint>
  <deque>
  <filesystem>
  <format>
  <functional>
  <future>
  <memory>
  <mutex>
  <optional>
  <span>
  <string>
  <string_view>
  <unordered_map>
  <unordered_set>
  <variant>
  <vector>
)

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

install(TARGETS ${PROJECT_NAME}
  RUNTIME
  DESTINATION .
)
