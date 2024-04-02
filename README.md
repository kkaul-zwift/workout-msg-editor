# Workout Message Editor

Short-term tool to edit textevent messages in workout XMLs.

# Running the app

Download the [latest release](https://github.com/kkaul-zwift/workout-msg-editor/releases) for the target system, extract and run it.

## Requirements

### Windows

- Microsoft [VC++ redistributable](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist?view=msvc-170). (In most cases this will already be installed.)
- Vulkan capable GPU and driver. (The driver `vulkan1.dll` will be pre-installed by the OS.)

The app should Just Work on most Windows systems.

### MacOS

- [Vulkan SDK for MacOS](https://vulkan.lunarg.com/sdk/home).
- If using an x64 release on Apple Silicon (ARM), [Rosetta emulation](https://support.apple.com/en-in/102527).

Since MacOS only supports Metal, MoltenVk is required to translate Vulkan calls to Metal, and to advertise itself as a Vulkan driver. This is automatically setup by installing the Vulkan SDK.

# Building

| Aspect | Property |
|--|--|
| Language | C++20 |
| Build setup | [CMake](https://cmake.org/) |
| Windowing | [GLFW](https://github.com/glfw/glfw) |
| Rendering | Vulkan |
| GUI | [Dear ImGui](https://github.com/ocornut/imgui) |
| XML | [tinyxml2](https://github.com/leethomason/tinyxml2) |

## Requirements

- CMake 3.22+.
- C++20 compiler and standard library.
- Vulkan SDK (for validation layers).

## Steps

### IDE

- Open the project directory in an IDE that supports C++, CMake, and Ninja.
  - Visual Studio CMake mode or VSCode with CMake Tools is recommended.
- Select a preset / configuration to build (Debug) by default.
- Build and run.

### CLI

Refer to [ci.yml](.github/workflows/ci.yml) for concrete steps.

- Configure a build via `cmake -S . --preset=<desired> -B <build_dir>`.
  - This is only needed once before the first build.
- Build via `cmake --build <build_dir> [options...]`.
- If desired, install via `cmake --install <build_dir> --prefix=<install_dir>`.
- Navigate to where the binary is located (eg `<install_dir>`) and run `./workout-msg-editor[.exe]`.
