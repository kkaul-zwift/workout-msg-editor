project(ext)

add_library(${PROJECT_NAME} INTERFACE)
add_library(wme::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

include(FetchContent)

set(FETCHCONTENT_QUIET OFF)

FetchContent_Declare(
  tinyxml
  GIT_REPOSITORY https://github.com/kkaul-zwift/tinyxml2
  GIT_TAG space-before-self-closing-tag
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/tinyxml"
)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt/
  GIT_TAG 10.2.1
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/fmt"
)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw
  GIT_TAG 3.4
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/glfw"
)

FetchContent_Declare(
  vulkan-headers
  GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Headers
  GIT_TAG v1.3.280
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/vulkan_headers"
)

FetchContent_Declare(
  dear-imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui
  GIT_TAG v1.90.4
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/dear_imgui"
)

set(tinyxml2_BUILD_TESTING OFF)
set(tinyxml2_INSTALL OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
set(FMT_INSTALL OFF)

FetchContent_MakeAvailable(tinyxml fmt glfw vulkan-headers dear-imgui)

# Dear ImGui
add_library(dear_imgui)
add_library(dear_imgui::dear_imgui ALIAS dear_imgui)

target_sources(dear_imgui PRIVATE
  src/dear_imgui/imstb_truetype.h
  src/dear_imgui/imgui_draw.cpp
  src/dear_imgui/backends/imgui_impl_vulkan.h
  src/dear_imgui/backends/imgui_impl_vulkan.cpp
  src/dear_imgui/backends/imgui_impl_glfw.h
  src/dear_imgui/backends/imgui_impl_glfw.cpp
  src/dear_imgui/imconfig.h
  src/dear_imgui/imgui_internal.h
  src/dear_imgui/imgui.h
  src/dear_imgui/imgui.cpp
  src/dear_imgui/imgui_tables.cpp
  src/dear_imgui/imstb_textedit.h
  src/dear_imgui/imgui_demo.cpp
  src/dear_imgui/imgui_widgets.cpp
)

target_include_directories(dear_imgui PUBLIC SYSTEM
  src/dear_imgui
)

target_compile_definitions(dear_imgui PUBLIC
  VK_NO_PROTOTYPES
)

target_link_libraries(dear_imgui PUBLIC
  glfw
  Vulkan::Headers
)

target_link_libraries(${PROJECT_NAME} INTERFACE
  fmt::fmt
  tinyxml2::tinyxml2
  dear_imgui::dear_imgui
  glfw
)

if(CMAKE_SYSTEM_NAME STREQUAL Linux)
  target_link_libraries(${PROJECT_NAME} INTERFACE dl)
endif()

target_compile_definitions(${PROJECT_NAME} INTERFACE
  VK_NO_PROTOTYPES
  VK_ENABLE_BETA_EXTENSIONS
  $<$<CONFIG:Debug>:WME_DEBUG>
)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  target_compile_options(${PROJECT_NAME} INTERFACE
    -Wall -Wextra -Wpedantic -Wconversion -Wunused -Werror=return-type
  )
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  target_compile_options(${PROJECT_NAME} INTERFACE
    /MP
  )
  target_compile_definitions(${PROJECT_NAME} INTERFACE
    _AMD64_ NOMINMAX
  )
endif()
