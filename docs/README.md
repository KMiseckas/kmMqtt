# CleanMQTT Documentation

<div align="center">

![CleanMQTT](https://img.shields.io/badge/MQTT-5.0%20%7C%203.1.1-blue)
![C++](https://img.shields.io/badge/C++-14-orange)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-lightgrey)
![License](https://img.shields.io/badge/License-Check%20Repository-green)

**A modern, lightweight C++ MQTT client library**

[Getting Started](GETTING_STARTED.md) • [API Reference](API_REFERENCE.md) • [Examples](EXAMPLES.md) • [Contributing](CONTRIBUTING.md)

</div>

---

## 📚 Table of Contents

- [Overview](#overview)
- [Key Features](#key-features)
- [Quick Start](#quick-start)
- [Documentation Index](#documentation-index)
- [Community & Support](#community--support)

## Overview

CleanMQTT is a high-performance, cross-platform C++ library for MQTT protocol communication. It provides a clean, modern API for connecting to MQTT brokers, publishing messages, and subscribing to topics with support for both MQTT 5.0 and MQTT 3.1.1 protocols.

### What is MQTT?

MQTT (Message Queuing Telemetry Transport) is a lightweight, publish-subscribe messaging protocol designed for constrained devices and low-bandwidth, high-latency networks. It's widely used in IoT applications, mobile messaging, and real-time data streaming.

## Key Features

### 🚀 Core Capabilities

- **Full MQTT Support**: Compatible with MQTT 5.0 and MQTT 3.1.1 protocols
- **Cross-Platform**: Works seamlessly on Windows and Linux
- **Dual Operation Modes**: Choose between synchronous and asynchronous operation
- **Event-Driven Architecture**: React to connection, publish, and subscription events
- **WebSocket Support**: Connect via WebSocket for browser-friendly deployments

### 🎯 Advanced Features

- **Small Buffer Optimization (SBO)**: Configurable for ByteBuffer and UniqueFunction
- **Comprehensive Logging**: Built-in logging system with configurable levels
- **Sanitizer Support**: AddressSanitizer, ThreadSanitizer, MemorySanitizer, and UndefinedBehaviorSanitizer
- **Quality of Service**: Full QoS 0, 1, and 2 support
- **Automatic Reconnection**: Built-in reconnection logic with customizable strategies
- **Session Persistence**: Clean and persistent session support

### 🛠️ Developer-Friendly

- **Modern C++14**: Clean, type-safe API
- **Header-Based**: Easy integration into existing projects
- **CMake Integration**: Simple build system integration
- **Extensive Examples**: Real-world usage examples included
- **Comprehensive Testing**: Unit tests and benchmarks included

## Quick Start

### Installation

```bash
# Clone the repository
git clone https://github.com/KMiseckas/CleanMQTT.git
cd CleanMQTT

# Configure with CMake
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# Build the library
cmake --build build --config Release
```

### Basic Usage

```cpp
#include <cleanMqtt/MqttClient.h>

using namespace cleanMqtt::mqtt;

int main() {
    // Create MQTT client
    MqttClient client;
    
    // Set up event handlers
    client.onConnectEvent() += [](const ConnectEventArgs& args) {
        std::cout << "Connected to broker!" << std::endl;
    };
    
    client.onPublishEvent() += [](const PublishEventArgs& args) {
        std::cout << "Message received on topic: " 
                  << args.topic << std::endl;
    };
    
    // Connect to broker
    ConnectArgs connectArgs;
    connectArgs.clientId = "my_client";
    
    ConnectAddress address;
    address.host = "broker.hivemq.com";
    address.port = 1883;
    
    client.connect(std::move(connectArgs), std::move(address));
    
    // Subscribe to topic
    std::vector<Topic> topics = {{"test/topic", QoS::QoS0}};
    client.subscribe(topics, SubscribeOptions{});
    
    // Publish message
    ByteBuffer payload("Hello, MQTT!");
    client.publish("test/topic", std::move(payload), PublishOptions{});
    
    // Process events (synchronous mode)
    while (true) {
        client.tick();
    }
    
    return 0;
}
```

## Documentation Index

| Document | Description |
|----------|-------------|
| [Getting Started](GETTING_STARTED.md) | Installation, build instructions, and first steps |
| [API Reference](API_REFERENCE.md) | Complete API documentation for all classes and methods |
| [Examples](EXAMPLES.md) | Practical examples and common usage patterns |
| [Architecture](ARCHITECTURE.md) | Internal design and architecture overview |
| [Contributing](CONTRIBUTING.md) | Guidelines for contributing to the project |
| [FAQ](FAQ.md) | Frequently asked questions and troubleshooting |

## Community & Support

### Getting Help

- **Documentation**: Start with our comprehensive guides above
- **Examples**: Check out the [examples](../examples/) directory for working code
- **Issues**: Report bugs or request features via [GitHub Issues](https://github.com/KMiseckas/CleanMQTT/issues)

### Contributing

We welcome contributions! Please see our [Contributing Guide](CONTRIBUTING.md) for details on:
- Code style and conventions
- Submitting pull requests
- Running tests
- Adding documentation

## Project Status

CleanMQTT is actively maintained and under continuous development. Check the [project roadmap](../ReadMe.txt) for upcoming features and improvements.

### Current Version

- **Version**: 0.0.1
- **C++ Standard**: C++14 or higher
- **Build System**: CMake 3.5+

---

<div align="center">

**[⬆ Back to Top](#cleanmqtt-documentation)**

Made with ❤️ by the CleanMQTT team

</div>
