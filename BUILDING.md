# Building kmMqtt

This document describes how to build kmMqtt from source.

## Table of Contents

- [Requirements](#requirements)
- [Basic Build](#basic-build)
- [CMake Options](#cmake-options)
- [Platform-Specific Instructions](#platform-specific-instructions)
- [Troubleshooting](#troubleshooting)
- [Cross-Compilation](#cross-compilation)
- [Integration with Other Build Systems](#integration-with-other-build-systems)
- [Build Artifacts](#build-artifacts)

## Requirements

- C++14 compatible compiler (GCC, Clang, MSVC)
- CMake 3.5 or later
- OpenSSL (required only when `BUILD_IXWEBSOCKET=ON`, auto-installed by vcpkg)

## Basic Build

```bash
# Configure
cmake -B build

# Build
cmake --build build
```

## CMake Options

### Library Options

| Option | Default | Description |
|--------|---------|-------------|
| `BUILD_SHARED_LIBS` | `OFF` | Build shared library instead of static |
| `BUILD_UNIT_TESTS` | `ON` | Build unit tests |
| `BUILD_EXAMPLES` | `ON` | Build example applications |
| `BUILD_BENCHMARKING` | `ON` | Build benchmark suite |
| `BUILD_COVERAGE` | `OFF` | Enable code coverage (GCC/Clang only) |

### Feature Options

| Option | Default | Description |
|--------|---------|-------------|
| `ENABLE_LOGS` | `ON` | Enable logging functionality |
| `LOG_BUFFER_SIZE` | `2048` | Fixed log buffer size in bytes |
| `ENABLE_BYTEBUFFER_SBO` | `ON` | Enable small buffer optimization for ByteBuffer |
| `BYTEBUFFER_SBO_MAX_SIZE` | `128` | Max stack size for ByteBuffer SBO in bytes |
| `ENABLE_UNIQUEFUNCTION_SBO` | `ON` | Enable small buffer optimization for UniqueFunction |
| `UNIQUEFUNCTION_SBO_MAX_SIZE` | `32` | Max stack size for UniqueFunction SBO in bytes |

### Build Quality Options

| Option | Default | Description |
|--------|---------|-------------|
| `ENABLE_WARNINGS_AS_ERRORS` | `ON` | Treat compiler warnings as errors |
| `ENABLE_ASAN` | `OFF` | Enable AddressSanitizer |
| `ENABLE_UBSAN` | `OFF` | Enable UndefinedBehaviorSanitizer |
| `ENABLE_TSAN` | `OFF` | Enable ThreadSanitizer |
| `ENABLE_MSAN` | `OFF` | Enable MemorySanitizer (Clang only) |

### Protocol Options

| Option | Default | Description |
|--------|---------|-------------|
| `FORCE_ADD_PROPERTIES` | `OFF` | Force encoding empty properties (non-standard) |

### WebSocket Support

| Option | Default | Description |
|--------|---------|-------------|
| `BUILD_IXWEBSOCKET` | `ON` | Build with IXWebSocket library for WebSocket support |

When `BUILD_IXWEBSOCKET=ON`, OpenSSL is required for secure WebSocket (WSS) connections. vcpkg automatically installs OpenSSL on Windows and Linux.

## Platform-Specific Instructions

Project makes use of [CMakePresets](https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html) for various configurations for solutuion generation and building. Default ones are defined in `CMakePresets.json`.
```powershell
#Show all presets
cmake --list-presets
```

Add your own cross-platform presets into a new local file named `CMakeUserPresets.json` with any options and toolchains required. See [CMakePresets Docs](https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html).
Use `CMakePresets.json` as an example for how to structure your local presets file.
```powershell
#Build using local presets same way as default presets
cmake --preset <local-preset-name>
cmake --build --preset <local-preset-name>
```

### Windows

#### Set up OpenSSL
If using IXWebsocket implementation for socket, download OpenSSL either through Vcpkg or manually.
1. Clone from Vcpkg git repo.
2. Run .bat inside vcpkg folder.
3. Set ENV variable `VCPKG_ROOT` to vcpkg folder.

```powershell
# Set up vcpkg
git clone https://github.com/microsoft/vcpkg.git
.\vcpkg\bootstrap-vcpkg.bat
$env:VCPKG_ROOT = "path\to\vcpkg"
```

OpenSSL can be defined manually by setting the `CMAKE_PREFIX_PATH` to the openssl folder. Ideal for custom or closed-source toolchains.
```powershell
# Point CMake to OpenSSL installation
cmake --preset <configure-preset-name> -DCMAKE_PREFIX_PATH="C:\path\to\openssl"
```

#### Building MQTT Lib

```powershell
cmake --preset <configure-preset-name> #Optionally: -DCMAKE_PREFIX_PATH="C:\path\to\openssl"
cmake --build --preset <build-preset-name>
```
Or using open VS IDE to generate solutions from selected configurations based on `CMakePresets.json` file.

### Linux

#### Ubuntu/Debian

#### OpenSSL

```bash
# Install dependencies
sudo apt-get update
sudo apt-get install build-essential cmake libssl-dev
```

#### Mqtt Lib

```bash
# Build
cmake --preset <configure-preset-name> #Optionally: -DCMAKE_PREFIX_PATH="C:\path\to\openssl"
cmake --build --preset <build-preset-name>
```

### macOS

#### OpenSSL

```bash
# Install dependencies
brew install cmake openssl
```

#### Mqtt Lib

```bash
# Build with OpenSSL from Homebrew
cmake --preset <configure-preset-name> #Optionally: -DCMAKE_PREFIX_PATH="C:\path\to\openssl"
cmake --build --preset <build-preset-name>
```

## Troubleshooting

### OpenSSL Not Found

**Error**: `OpenSSL not found!`

**Solutions**:
- Install OpenSSL using your package manager
- Set `CMAKE_PREFIX_PATH` to OpenSSL installation directory
- Use vcpkg and set `VCPKG_ROOT` environment variable
- Disable WebSocket support with `-DBUILD_IXWEBSOCKET=OFF`

### Compiler Warnings as Errors

If warnings block your build:

```bash
cmake -B build -DENABLE_WARNINGS_AS_ERRORS=OFF
```

### Sanitizer Conflicts

Sanitizers cannot be combined:
- MSAN is incompatible with ASAN, TSAN, and UBSAN
- TSAN is incompatible with ASAN, MSAN, and UBSAN

Use only one sanitizer at a time.

## Cross-Compilation

To cross compile build the project either directly from command line or (recommended) create a `CMakeUserPresets.json` file for your custom preset for target platform. See [CMakePresets Docs](https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html).

For direct command line build, provide a toolchain file to CMake:
```bash
cmake -B build \
  -DCMAKE_TOOLCHAIN_FILE=/path/to/toolchain.cmake \
  -DBUILD_IXWEBSOCKET=OFF #Use custom sockets for closed source OS platforms.
cmake --build build
```

## Integration with Other Build Systems

### CMake FetchContent

```cmake
#Set any kmMQtt options or cache variables here before fetching library. These will be inherited by kmMqtt.

include(FetchContent)
FetchContent_Declare(
  kmmqtt
  GIT_REPOSITORY https://github.com/KMiseckas/kmMqtt.git
  GIT_TAG main
)
FetchContent_MakeAvailable(kmmqtt)

target_link_libraries(your_target PRIVATE kmMqtt)
```

## Build Artifacts

After a successful build, find:

- **Library**: `build/libkmMqtt.a` (or `.lib`, `.so`, `.dll` depending on options)
- **Tests**: `build/tests/kmMqttTests`
- **Examples**: `build/examples/mqttClient/mqttClient`
- **Headers**: Already in source tree under `include/public/kmMqtt/`
