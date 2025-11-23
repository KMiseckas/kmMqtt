# CleanMQTT

<div align="center">

![MQTT](https://img.shields.io/badge/MQTT-5.0%20%7C%203.1.1-blue?style=flat-square)
![C++](https://img.shields.io/badge/C++-14-orange?style=flat-square)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-lightgrey?style=flat-square)
![Version](https://img.shields.io/badge/Version-0.0.1-green?style=flat-square)

**A modern, lightweight, cross-platform C++ MQTT client library**

[📚 Documentation](docs/README.md) • [🚀 Getting Started](docs/GETTING_STARTED.md) • [📖 API Reference](docs/API_REFERENCE.md) • [💡 Examples](docs/EXAMPLES.md)

</div>

---

## ✨ Features

- 🔌 **Full MQTT Support** - Compatible with MQTT 5.0 and MQTT 3.1.1
- 🖥️ **Cross-Platform** - Works on Windows and Linux
- ⚡ **High Performance** - Small Buffer Optimization (SBO) for reduced allocations
- 🔄 **Dual Modes** - Synchronous and asynchronous operation
- 📡 **Event-Driven** - Clean event-based architecture
- 🌐 **WebSocket Support** - Connect via WebSocket transport
- 🛡️ **Type-Safe** - Modern C++14 API
- 🧪 **Well-Tested** - Comprehensive unit tests and sanitizer support

## 🚀 Quick Start

```cpp
#include <cleanMqtt/MqttClient.h>

using namespace cleanMqtt::mqtt;

int main() {
    MqttClient client;
    
    // Handle incoming messages
    client.onPublishEvent() += [](const PublishEventArgs& args) {
        std::cout << "Message: " << args.payload.toString() << std::endl;
    };
    
    // Connect to broker
    ConnectArgs args;
    args.clientId = "my_client";
    
    ConnectAddress address;
    address.host = "broker.hivemq.com";
    address.port = 1883;
    
    client.connect(std::move(args), std::move(address));
    
    // Subscribe and publish
    client.subscribe({{"test/topic", QoS::QoS0}}, SubscribeOptions{});
    client.publish("test/topic", ByteBuffer("Hello!"), PublishOptions{});
    
    // Process events
    while (true) {
        client.tick();
    }
    
    return 0;
}
```

## 📦 Installation

```bash
# Clone the repository
git clone https://github.com/KMiseckas/CleanMQTT.git
cd CleanMQTT

# Configure and build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

**Integration with CMake:**

```cmake
add_subdirectory(path/to/CleanMQTT)
target_link_libraries(your_target PRIVATE cleanMqtt)
```

See the [Getting Started Guide](docs/GETTING_STARTED.md) for detailed instructions.

## 📚 Documentation

| Document | Description |
|----------|-------------|
| [📘 Getting Started](docs/GETTING_STARTED.md) | Installation, build, and first steps |
| [📖 API Reference](docs/API_REFERENCE.md) | Complete API documentation |
| [💡 Examples](docs/EXAMPLES.md) | Practical examples and patterns |
| [🏗️ Architecture](docs/ARCHITECTURE.md) | Design and architecture |
| [🤝 Contributing](docs/CONTRIBUTING.md) | Contribution guidelines |
| [❓ FAQ](docs/FAQ.md) | Common questions and troubleshooting |

## 🎯 Use Cases

CleanMQTT is perfect for:

- 🏭 **Industrial IoT** - Sensor data collection and device control
- 🏠 **Smart Home** - Home automation and monitoring
- 📱 **Mobile Apps** - Real-time messaging and notifications  
- 🎮 **Gaming** - Multiplayer game state synchronization
- 📊 **Data Streaming** - Real-time data pipelines
- 🔔 **Event Systems** - Publish-subscribe event architectures

## 🛠️ Building

### Requirements

- C++14 compatible compiler (GCC 5.0+, Clang 3.4+, MSVC 2015+)
- CMake 3.5 or higher
- Platform: Windows 10+ or Linux (Ubuntu 18.04+, Debian 10+, CentOS 7+)

### Build Options

```bash
# Build with examples
cmake -S . -B build -DBUILD_EXAMPLES=ON

# Build with tests
cmake -S . -B build -DBUILD_UNIT_TESTS=ON

# Enable sanitizers (debug builds)
cmake -S . -B build --preset linux-gcc-ubsan-asan

# Enable logging
cmake -S . -B build -DENABLE_LOGS=ON
```

## 🤝 Contributing

We welcome contributions! Please see our [Contributing Guide](docs/CONTRIBUTING.md) for details on:

- Code style and conventions
- Development workflow
- Testing requirements
- Pull request process

## 📄 License

See the [LICENSE](LICENSE) file for license information.

## 🙏 Acknowledgments

CleanMQTT is built with modern C++ best practices and draws inspiration from the MQTT community.

## 📞 Support

- 📖 **Documentation**: [docs/](docs/)
- 🐛 **Issues**: [GitHub Issues](https://github.com/KMiseckas/CleanMQTT/issues)
- 💬 **Discussions**: [GitHub Discussions](https://github.com/KMiseckas/CleanMQTT/discussions)

---

<div align="center">

**Made with ❤️ for the IoT and messaging community**

⭐ Star this repo if you find it useful!

</div>
