# Getting Started with CleanMQTT

This guide will help you install, configure, and start using CleanMQTT in your project.

## Table of Contents

- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Building the Library](#building-the-library)
- [Integration Methods](#integration-methods)
- [Your First MQTT Application](#your-first-mqtt-application)
- [Configuration Options](#configuration-options)
- [Next Steps](#next-steps)

## Prerequisites

Before you begin, ensure you have the following installed:

### Required Tools

- **C++ Compiler**: 
  - GCC 5.0+ or Clang 3.4+ (Linux)
  - MSVC 2015+ (Windows)
- **CMake**: Version 3.5 or higher
- **Ninja**: Build system (optional but recommended)

### Supported Platforms

- **Linux**: Ubuntu 18.04+, Debian 10+, CentOS 7+, or equivalent
- **Windows**: Windows 10+ with Visual Studio 2015 or later

### Optional Dependencies

- **Git**: For cloning the repository
- **lcov/genhtml**: For code coverage reports (Linux)
- **Doxygen**: For generating API documentation (optional)

## Installation

### Option 1: Clone from GitHub

```bash
# Clone the repository
git clone https://github.com/KMiseckas/CleanMQTT.git
cd CleanMQTT
```

### Option 2: Download Release

Download the latest release from the [GitHub Releases](https://github.com/KMiseckas/CleanMQTT/releases) page and extract it to your desired location.

## Building the Library

CleanMQTT uses CMake for building. Below are platform-specific build instructions.

### Linux Build

#### Using GCC (Recommended)

```bash
# Configure with CMake using GCC
cmake -S . -B build -G Ninja --preset linux-gcc-release

# Build the library
cmake --build build --config Release

# Optional: Build examples
cmake -S . -B build -G Ninja --preset linux-gcc-release -DBUILD_EXAMPLES=ON
cmake --build build --config Release
```

#### Using Clang

```bash
# Configure with Clang
cmake -S . -B build -G Ninja --preset linux-clang-release

# Build
cmake --build build --config Release
```

### Windows Build

#### Using Visual Studio 2019/2022

```bash
# Open Visual Studio Developer Command Prompt

# Configure with CMake
cmake -S . -B build -G Ninja --preset x64-release

# Build
cmake --build build --config Release
```

### Build Options

You can customize the build with various CMake options:

```bash
# Build with examples
cmake -S . -B build -DBUILD_EXAMPLES=ON

# Build with unit tests
cmake -S . -B build -DBUILD_UNIT_TESTS=ON

# Build with benchmarks
cmake -S . -B build -DBUILD_BENCHMARKING=ON

# Enable logging
cmake -S . -B build -DENABLE_LOGS=ON -DLOG_LEVEL_TARGET=0

# Build as shared library
cmake -S . -B build -DBUILD_SHARED_LIBS=ON

# Enable sanitizers (Debug builds)
cmake -S . -B build --preset linux-gcc-ubsan-asan
```

### Available CMake Presets

CleanMQTT provides several CMake presets for different configurations:

**Linux Presets:**
- `linux-gcc-debug` - Debug build with GCC
- `linux-gcc-release` - Release build with GCC
- `linux-clang-debug` - Debug build with Clang
- `linux-clang-release` - Release build with Clang
- `linux-gcc-coverage` - Coverage build with GCC
- `linux-gcc-ubsan-asan` - Debug with sanitizers

**Windows Presets:**
- `x64-debug` - x64 Debug build
- `x64-release` - x64 Release build
- `x86-debug` - x86 Debug build
- `x86-release` - x86 Release build

View all available presets:
```bash
cmake --list-presets
```

## Integration Methods

### Method 1: Add as CMake Subdirectory

```cmake
# In your CMakeLists.txt
add_subdirectory(path/to/CleanMQTT)

# Link against your target
target_link_libraries(your_target PRIVATE cleanMqtt)
```

### Method 2: Install and Find Package

```bash
# Build and install CleanMQTT
cmake -S . -B build --preset linux-gcc-release
cmake --build build --config Release
sudo cmake --install build
```

```cmake
# In your CMakeLists.txt
find_package(cleanMqtt REQUIRED)
target_link_libraries(your_target PRIVATE cleanMqtt)
```

### Method 3: Header-Only Integration

CleanMQTT can be integrated by including the headers directly:

```cmake
target_include_directories(your_target 
    PRIVATE path/to/CleanMQTT/include/public
)
```

## Your First MQTT Application

Let's create a simple MQTT application that connects to a broker, subscribes to a topic, and publishes a message.

### Step 1: Create the Source File

Create a file named `mqtt_example.cpp`:

```cpp
#include <cleanMqtt/MqttClient.h>
#include <iostream>
#include <thread>
#include <chrono>

using namespace cleanMqtt::mqtt;
using namespace std::chrono_literals;

int main() {
    // Create an MQTT client with default settings
    MqttClient client;
    
    // Variable to track connection status
    bool connected = false;
    
    // Set up connection event handler
    client.onConnectEvent() += [&](const ConnectEventArgs& args) {
        if (args.status == ConnectionStatus::Connected) {
            std::cout << "✓ Successfully connected to broker!" << std::endl;
            connected = true;
        } else {
            std::cout << "✗ Connection failed: " 
                      << static_cast<int>(args.status) << std::endl;
        }
    };
    
    // Set up disconnect event handler
    client.onDisconnectEvent() += [](const DisconnectEventArgs& args) {
        std::cout << "Disconnected from broker" << std::endl;
    };
    
    // Set up message received handler
    client.onPublishEvent() += [](const PublishEventArgs& args) {
        std::cout << "\n📨 Message received!" << std::endl;
        std::cout << "   Topic: " << args.topic << std::endl;
        std::cout << "   Payload: " << args.payload.toString() << std::endl;
        std::cout << "   QoS: " << static_cast<int>(args.qos) << std::endl;
    };
    
    // Set up publish acknowledgment handler
    client.onPublishAckEvent() += [](const PublishAckEventArgs& args) {
        std::cout << "✓ Message published successfully!" << std::endl;
    };
    
    // Set up subscribe acknowledgment handler
    client.onSubscribeAckEvent() += [](const SubscribeAckEventArgs& args) {
        std::cout << "✓ Subscribed to topic(s)" << std::endl;
    };
    
    // Configure connection parameters
    ConnectArgs connectArgs;
    connectArgs.clientId = "CleanMQTT_Example";
    connectArgs.keepAlive = Seconds(60);
    connectArgs.cleanSession = true;
    
    // Set broker address (using public HiveMQ broker)
    ConnectAddress address;
    address.host = "broker.hivemq.com";
    address.port = 1883;
    
    std::cout << "Connecting to " << address.host 
              << ":" << address.port << "..." << std::endl;
    
    // Connect to the broker
    ClientError error = client.connect(std::move(connectArgs), 
                                       std::move(address));
    
    if (error != ClientError::Success) {
        std::cerr << "Failed to initiate connection!" << std::endl;
        return 1;
    }
    
    // Wait for connection (in a real app, use events)
    for (int i = 0; i < 50 && !connected; ++i) {
        client.tick();
        std::this_thread::sleep_for(100ms);
    }
    
    if (!connected) {
        std::cerr << "Connection timeout!" << std::endl;
        return 1;
    }
    
    // Subscribe to a topic
    std::vector<Topic> topics = {
        {"cleanmqtt/example/test", QoS::QoS0}
    };
    
    SubscribeOptions subOptions;
    client.subscribe(topics, std::move(subOptions));
    
    // Process subscription acknowledgment
    for (int i = 0; i < 20; ++i) {
        client.tick();
        std::this_thread::sleep_for(50ms);
    }
    
    // Publish a test message
    std::cout << "\n📤 Publishing message..." << std::endl;
    
    PublishOptions pubOptions;
    pubOptions.qos = QoS::QoS0;
    pubOptions.retain = false;
    
    ByteBuffer payload("Hello from CleanMQTT!");
    client.publish("cleanmqtt/example/test", 
                   std::move(payload), 
                   std::move(pubOptions));
    
    // Keep processing messages for 10 seconds
    std::cout << "\nListening for messages (10 seconds)...\n" << std::endl;
    
    for (int i = 0; i < 100; ++i) {
        client.tick();
        std::this_thread::sleep_for(100ms);
    }
    
    // Gracefully disconnect
    std::cout << "\nDisconnecting..." << std::endl;
    client.disconnect(DisconnectArgs{});
    client.shutdown();
    
    std::cout << "Done!" << std::endl;
    return 0;
}
```

### Step 2: Create CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.5)
project(mqtt_example)

set(CMAKE_CXX_STANDARD 14)

# Add CleanMQTT
add_subdirectory(path/to/CleanMQTT)

# Create executable
add_executable(mqtt_example mqtt_example.cpp)

# Link CleanMQTT
target_link_libraries(mqtt_example PRIVATE cleanMqtt)
```

### Step 3: Build and Run

```bash
# Build
cmake -S . -B build
cmake --build build

# Run
./build/mqtt_example
```

## Configuration Options

### ByteBuffer Small Buffer Optimization (SBO)

Enable SBO for ByteBuffer to avoid heap allocations for small buffers:

```bash
cmake -S . -B build \
  -DENABLE_BYTEBUFFER_SBO=ON \
  -DBYTEBUFFER_SBO_MAX_SIZE=256
```

### UniqueFunction Small Buffer Optimization

Enable SBO for UniqueFunction:

```bash
cmake -S . -B build \
  -DENABLE_UNIQUEFUNCTION_SBO=ON \
  -DUNIQUEFUNCTION_SBO_MAX_SIZE=32
```

### Logging Configuration

Enable logging with custom buffer size:

```bash
cmake -S . -B build \
  -DENABLE_LOGS=ON \
  -DLOG_LEVEL_TARGET=0 \
  -DLOG_BUFFER_SIZE=2048
```

Log levels:
- `0` - TRACE (most verbose)
- `1` - DEBUG
- `2` - INFO
- `3` - WARNING
- `4` - ERROR (least verbose)

### Warnings and Error Handling

Enable strict compilation:

```bash
cmake -S . -B build -DENABLE_WARNINGS_AS_ERRORS=ON
```

## Next Steps

Now that you have CleanMQTT set up, explore these resources:

1. **[API Reference](API_REFERENCE.md)** - Detailed documentation of all classes and methods
2. **[Examples](EXAMPLES.md)** - More complex usage scenarios
3. **[Architecture](ARCHITECTURE.md)** - Understanding the library design
4. **[FAQ](FAQ.md)** - Common questions and troubleshooting

### Try These Examples

- **Asynchronous Mode**: Run the client in async mode with automatic ticking
- **WebSocket Connection**: Connect using WebSocket transport
- **QoS 2 Messaging**: Use guaranteed message delivery
- **Session Persistence**: Maintain session state across reconnections
- **TLS/SSL**: Secure your MQTT connections (requires SSL socket implementation)

---

[◀ Back to Documentation Index](README.md) | [API Reference ▶](API_REFERENCE.md)
