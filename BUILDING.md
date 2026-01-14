# Building CleanMQTT

This document describes how to build CleanMQTT from source.

## Requirements

- C++14 compatible compiler (GCC, Clang, MSVC)
- CMake 3.5 or later
- OpenSSL (required only when `BUILD_IXWEBSOCKET=ON`)

## Basic Build

```bash
# Configure
cmake -B build

# Build
cmake --build build

# Optional: Run tests
cd build && ctest
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

When `BUILD_IXWEBSOCKET=ON`, OpenSSL is required for secure WebSocket (WSS) connections.

## Platform-Specific Instructions

### Windows

#### Using vcpkg (Recommended)

```powershell
# Set up vcpkg
git clone https://github.com/microsoft/vcpkg.git
.\vcpkg\bootstrap-vcpkg.bat
$env:VCPKG_ROOT = "path\to\vcpkg"

# vcpkg will automatically install OpenSSL when building with BUILD_IXWEBSOCKET=ON
cmake -B build
cmake --build build
```

#### Manual OpenSSL Installation

```powershell
# Point CMake to OpenSSL installation
cmake -B build -DCMAKE_PREFIX_PATH="C:\path\to\openssl"
cmake --build build
```

#### Visual Studio

```powershell
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

### Linux

#### Ubuntu/Debian

```bash
# Install dependencies
sudo apt-get update
sudo apt-get install build-essential cmake libssl-dev

# Build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```

#### RHEL/CentOS/Fedora

```bash
# Install dependencies
sudo yum install gcc-c++ cmake openssl-devel

# Build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```

### macOS

```bash
# Install dependencies
brew install cmake openssl

# Build with OpenSSL from Homebrew
cmake -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="$(brew --prefix openssl)"
cmake --build build -j$(sysctl -n hw.ncpu)
```

## Build Examples

### Release Build

```bash
cmake -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=OFF
cmake --build build --config Release
```

### Development Build with Tests

```bash
cmake -B build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DBUILD_UNIT_TESTS=ON \
  -DBUILD_EXAMPLES=ON \
  -DENABLE_ASAN=ON \
  -DENABLE_UBSAN=ON
cmake --build build
```

### Minimal Build (No WebSocket, No Tests)

```bash
cmake -B build \
  -DBUILD_UNIT_TESTS=OFF \
  -DBUILD_EXAMPLES=OFF \
  -DBUILD_BENCHMARKING=OFF \
  -DBUILD_IXWEBSOCKET=OFF
cmake --build build
```

### Optimized for Size

```bash
cmake -B build \
  -DCMAKE_BUILD_TYPE=MinSizeRel \
  -DENABLE_BYTEBUFFER_SBO=OFF \
  -DENABLE_UNIQUEFUNCTION_SBO=OFF \
  -DENABLE_LOGS=OFF \
  -DBUILD_SHARED_LIBS=ON
cmake --build build
```

## Testing

### Run All Tests

```bash
cd build
ctest --output-on-failure
```

### Run Tests with Specific Configuration

```bash
cd build
ctest -C Release --output-on-failure
```

### Run Specific Test

```bash
cd build
./cleanMqttTests
```

## Code Coverage

Coverage is supported on GCC and Clang.

### GCC (lcov)

```bash
# Install lcov
sudo apt-get install lcov

# Build with coverage
cmake -B build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DBUILD_COVERAGE=ON \
  -DBUILD_UNIT_TESTS=ON
cmake --build build

# Run tests
cd build && ctest

# Generate report
cmake --build . --target coverage_report

# View report
xdg-open coverage/index.html
```

### Clang (llvm-cov)

```bash
# Build with coverage
cmake -B build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_COMPILER=clang++ \
  -DBUILD_COVERAGE=ON \
  -DBUILD_UNIT_TESTS=ON
cmake --build build

# Run tests and generate report
cd build
cmake --build . --target coverage_report

# View report
open coverage/index.html
```

## Installing

After building, install the library:

```bash
cmake --install build --prefix /usr/local
```

Or with custom prefix:

```bash
cmake --install build --prefix /path/to/install
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

Provide a toolchain file to CMake:

```bash
cmake -B build \
  -DCMAKE_TOOLCHAIN_FILE=/path/to/toolchain.cmake \
  -DBUILD_IXWEBSOCKET=OFF
cmake --build build
```

For console platforms, provide OpenSSL via your toolchain file by setting `OPENSSL_ROOT_DIR`.

## Integration with Other Build Systems

### vcpkg

Add to `vcpkg.json`:

```json
{
  "dependencies": [
    "cleanmqtt"
  ]
}
```

### CMake FetchContent

```cmake
include(FetchContent)
FetchContent_Declare(
  cleanmqtt
  GIT_REPOSITORY https://github.com/KMiseckas/CleanMQTT.git
  GIT_TAG main
)
FetchContent_MakeAvailable(cleanmqtt)

target_link_libraries(your_target PRIVATE cleanMqtt)
```

### CMake find_package

After installing:

```cmake
find_package(cleanMqtt REQUIRED)
target_link_libraries(your_target PRIVATE cleanMqtt)
```

## Build Artifacts

After a successful build, find:

- **Library**: `build/libcleanMqtt.a` (or `.lib`, `.so`, `.dll` depending on options)
- **Tests**: `build/tests/cleanMqttTests`
- **Examples**: `build/examples/mqttClient/mqttClient`
- **Headers**: Already in source tree under `include/public/cleanMqtt/`
