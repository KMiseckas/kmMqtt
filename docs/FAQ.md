# Frequently Asked Questions (FAQ)

Common questions and answers about CleanMQTT.

## Table of Contents

- [General Questions](#general-questions)
- [Installation & Setup](#installation--setup)
- [Usage & API](#usage--api)
- [Performance & Optimization](#performance--optimization)
- [Troubleshooting](#troubleshooting)
- [Platform-Specific](#platform-specific)
- [Advanced Topics](#advanced-topics)

---

## General Questions

### What is CleanMQTT?

CleanMQTT is a modern C++ library for MQTT protocol communication. It provides a clean, type-safe API for connecting to MQTT brokers, publishing messages, and subscribing to topics.

### What MQTT versions are supported?

CleanMQTT supports both MQTT 5.0 and MQTT 3.1.1 protocols, providing compatibility with most MQTT brokers.

### What platforms are supported?

Currently supported platforms:
- **Windows**: Windows 10+ with MSVC 2015+
- **Linux**: Ubuntu 18.04+, Debian 10+, CentOS 7+ (GCC 5.0+, Clang 3.4+)

### Is CleanMQTT production-ready?

CleanMQTT is currently at version 0.0.1 and under active development. While it's functional and tested, consider it for evaluation and non-critical applications. Production use should await a stable 1.0 release.

### What license is CleanMQTT under?

Check the LICENSE file in the repository for current licensing information.

### How does CleanMQTT compare to other MQTT libraries?

**Advantages:**
- Modern C++14 API
- Clean, intuitive interface
- Cross-platform support
- Event-driven architecture
- Small buffer optimization
- Both sync and async modes

**Considerations:**
- Newer, less battle-tested than mature libraries
- Smaller community (growing!)
- TLS/SSL support in development

---

## Installation & Setup

### How do I install CleanMQTT?

See the [Getting Started Guide](GETTING_STARTED.md) for detailed installation instructions. Quick start:

```bash
git clone https://github.com/KMiseckas/CleanMQTT.git
cd CleanMQTT
cmake -S . -B build
cmake --build build
```

### Do I need to install any dependencies?

CleanMQTT has minimal external dependencies. You only need:
- C++14 compatible compiler
- CMake 3.5+
- Platform-specific socket APIs (provided by OS)

### How do I integrate CleanMQTT into my CMake project?

```cmake
# Add as subdirectory
add_subdirectory(path/to/CleanMQTT)
target_link_libraries(your_target PRIVATE cleanMqtt)
```

### Can I use CleanMQTT without CMake?

Yes, but CMake is recommended. You can manually include the headers and compile the source files, ensuring proper include paths and compile definitions are set.

### Build fails with "C++14 required" error

Ensure your compiler supports C++14:

```bash
# Check GCC version (requires 5.0+)
g++ --version

# Check Clang version (requires 3.4+)
clang++ --version

# Explicitly set C++ standard
cmake -S . -B build -DCMAKE_CXX_STANDARD=14
```

---

## Usage & API

### How do I connect to a broker?

```cpp
MqttClient client;

ConnectArgs args;
args.clientId = "my_client";

ConnectAddress address;
address.host = "broker.hivemq.com";
address.port = 1883;

client.connect(std::move(args), std::move(address));
```

See [Getting Started](GETTING_STARTED.md#your-first-mqtt-application) for complete examples.

### Should I use synchronous or asynchronous mode?

**Synchronous Mode** (default):
- Better for: Simple applications, single-threaded programs
- You control: When processing happens via `tick()`
- Threading: Single-threaded, predictable

**Asynchronous Mode**:
- Better for: Responsive UIs, multi-threaded applications
- Processing: Automatic in background thread
- Threading: Multi-threaded, requires synchronization

### How do I handle received messages?

Subscribe to the publish event:

```cpp
client.onPublishEvent() += [](const PublishEventArgs& args) {
    std::cout << "Topic: " << args.topic << std::endl;
    std::cout << "Message: " << args.payload.toString() << std::endl;
};
```

### How do I use wildcards in topic subscriptions?

MQTT supports two wildcards:
- `+` - Single-level wildcard
- `#` - Multi-level wildcard

```cpp
std::vector<Topic> topics = {
    {"sensor/+/temperature", QoS::QoS0},  // Matches sensor/1/temperature, sensor/2/temperature
    {"alerts/#", QoS::QoS1}                // Matches alerts/critical, alerts/warning/temp
};
client.subscribe(topics, SubscribeOptions{});
```

### What's the difference between QoS levels?

- **QoS 0**: At most once delivery (fire and forget)
  - Fast, no acknowledgment
  - May lose messages
  - Good for: Non-critical data, high-frequency updates

- **QoS 1**: At least once delivery (acknowledged)
  - Guaranteed delivery
  - May receive duplicates
  - Good for: Important data

- **QoS 2**: Exactly once delivery (four-way handshake)
  - Guaranteed, no duplicates
  - Slowest, highest overhead
  - Good for: Critical data, billing events

### How do I handle connection errors?

```cpp
client.onErrorEvent() += [](const ErrorEventArgs& args) {
    std::cerr << "Error: " << args.message << std::endl;
};

client.onConnectEvent() += [](const ConnectEventArgs& args) {
    if (args.status != ConnectionStatus::Connected) {
        std::cerr << "Connection failed: " << args.returnCode << std::endl;
    }
};
```

### Can I connect to multiple brokers?

Yes, create multiple `MqttClient` instances:

```cpp
MqttClient client1;
MqttClient client2;

// Connect client1 to broker1
client1.connect(args1, address1);

// Connect client2 to broker2
client2.connect(args2, address2);
```

---

## Performance & Optimization

### How can I improve performance?

1. **Enable Small Buffer Optimization**:
   ```bash
   cmake -DENABLE_BYTEBUFFER_SBO=ON -DBYTEBUFFER_SBO_MAX_SIZE=256
   ```

2. **Use QoS 0** for non-critical messages

3. **Batch messages** when possible

4. **Disable logging** in production:
   ```bash
   cmake -DENABLE_LOGS=OFF
   ```

5. **Use Release build**:
   ```bash
   cmake -DCMAKE_BUILD_TYPE=Release
   ```

### What is Small Buffer Optimization (SBO)?

SBO avoids heap allocations for small buffers by using stack storage. Configure it:

```bash
# ByteBuffer SBO (default: 256 bytes)
cmake -DENABLE_BYTEBUFFER_SBO=ON -DBYTEBUFFER_SBO_MAX_SIZE=256

# UniqueFunction SBO (default: 32 bytes)
cmake -DENABLE_UNIQUEFUNCTION_SBO=ON -DUNIQUEFUNCTION_SBO_MAX_SIZE=32
```

**Trade-off**: Larger stack usage vs. fewer heap allocations.

### How much memory does CleanMQTT use?

Memory usage depends on:
- Number of active connections
- Message payload sizes
- Number of subscriptions
- SBO configuration
- Pending QoS messages

Typical usage: 10-50 KB per client instance (excluding message payloads).

### Can CleanMQTT handle high message rates?

Yes, CleanMQTT is designed for performance. For high-throughput scenarios:
- Use asynchronous mode
- Adjust tick interval
- Use QoS 0 when acceptable
- Enable SBO
- Monitor with benchmarks

### How do I benchmark my application?

CleanMQTT includes benchmarks:

```bash
cmake -DBUILD_BENCHMARKING=ON
cmake --build build
./build/benchmarks/mqtt_benchmarks
```

---

## Troubleshooting

### Client doesn't connect to broker

**Check:**
1. Broker is running and accessible
2. Correct host and port
3. Network connectivity
4. Firewall rules
5. Authentication credentials (if required)

**Debug:**
```cpp
client.onConnectEvent() += [](const ConnectEventArgs& args) {
    std::cout << "Status: " << static_cast<int>(args.status) << std::endl;
    std::cout << "Return code: " << static_cast<int>(args.returnCode) << std::endl;
};
```

### Messages not being received

**Common causes:**
1. Not subscribed to topic
2. QoS mismatch
3. Not calling `tick()` in synchronous mode
4. Topic filter doesn't match published topic

**Verify subscription:**
```cpp
client.onSubscribeAckEvent() += [](const SubscribeAckEventArgs& args) {
    std::cout << "Subscribed! Packet ID: " << args.packetId << std::endl;
    for (auto code : args.returnCodes) {
        std::cout << "Return code: " << static_cast<int>(code) << std::endl;
    }
};
```

### Connection drops unexpectedly

**Possible causes:**
1. Network issues
2. Keep-alive timeout
3. Broker limits exceeded
4. Session expired

**Enable auto-reconnect:**
```cpp
MqttClientOptions options;
options.enableReconnection = true;
options.reconnectionDelay = Milliseconds(2000);

MqttClient client(DefaultEnvironmentFactory::create(), options);
```

### Application crashes or hangs

**Debug steps:**
1. Build with debug symbols:
   ```bash
   cmake -DCMAKE_BUILD_TYPE=Debug
   ```

2. Run with sanitizers:
   ```bash
   cmake --preset linux-gcc-ubsan-asan
   cmake --build build
   ./build/your_app
   ```

3. Use debugger (GDB/LLDB):
   ```bash
   gdb ./build/your_app
   ```

4. Check for:
   - Calling `tick()` from multiple threads (not thread-safe)
   - Using moved objects
   - Memory corruption

### Compile errors

**Common issues:**

1. **C++ standard**: Ensure C++14 or higher
   ```bash
   cmake -DCMAKE_CXX_STANDARD=14
   ```

2. **Missing headers**: Check include paths
   ```cmake
   target_include_directories(your_target 
       PRIVATE path/to/CleanMQTT/include/public)
   ```

3. **Linker errors**: Link against cleanMqtt
   ```cmake
   target_link_libraries(your_target PRIVATE cleanMqtt)
   ```

---

## Platform-Specific

### Windows: WebSocket compilation fails

Ensure Windows SDK is installed and up-to-date. WebSocket support requires Windows 8+.

### Linux: Socket errors

Check if socket permissions are correct:
```bash
# May need sudo for certain operations
sudo ./your_app
```

### Cross-compilation

Set appropriate toolchain file:
```bash
cmake -DCMAKE_TOOLCHAIN_FILE=path/to/toolchain.cmake
```

---

## Advanced Topics

### How do I implement TLS/SSL connections?

TLS/SSL support is planned but not yet implemented. You can:

1. **Use a broker with TLS termination proxy**
2. **Implement custom ISocket** with SSL support
3. **Wait for official TLS support** (in development)

### Can I use CleanMQTT in embedded systems?

CleanMQTT targets resource-constrained environments but requires:
- C++14 compiler
- CMake for building
- Sufficient RAM (depends on usage)
- Platform-specific socket implementation

For very small embedded systems, consider specialized MQTT-SN libraries.

### How do I implement custom authentication?

Provide credentials in ConnectArgs:

```cpp
ConnectArgs args;
args.username = "user";
args.password = "password";
```

For advanced authentication (OAuth, JWT), handle at application level before connecting.

### Can I customize the protocol behavior?

You can customize behavior through:
1. **Custom IMqttEnvironment** implementation
2. **Custom ISocket** for transport
3. **Custom ICallbackDispatcher** for threading
4. **Configuration options** in MqttClientOptions

### How do I persist messages offline?

Currently, CleanMQTT doesn't provide built-in message persistence. You can:

1. **Use persistent sessions** (broker-side):
   ```cpp
   args.cleanSession = false;
   ```

2. **Implement application-level queuing**:
   ```cpp
   if (client.publish(topic, payload, options) != ClientError::Success) {
       // Queue message for later
       offlineQueue.push({topic, payload, options});
   }
   ```

### How do I monitor connection health?

Use events and keep-alive:

```cpp
// Monitor connection
client.onDisconnectEvent() += [](const DisconnectEventArgs& args) {
    logError("Disconnected: " + args.reason);
};

// Set keep-alive
args.keepAlive = Seconds(30);  // Send PINGREQ every 30 seconds
```

### Can I use CleanMQTT in a multi-threaded application?

**Synchronous mode**: Not thread-safe, call `tick()` from one thread only

**Asynchronous mode**: Public API is thread-safe, callbacks dispatched via ICallbackDispatcher

For maximum safety:
- Use asynchronous mode
- Use proper synchronization for shared data
- Configure appropriate callback dispatcher

---

## Still Have Questions?

- **Documentation**: Check our [comprehensive docs](README.md)
- **Examples**: See [practical examples](EXAMPLES.md)
- **Issues**: Search [GitHub Issues](https://github.com/KMiseckas/CleanMQTT/issues)
- **Discussions**: Join [GitHub Discussions](https://github.com/KMiseckas/CleanMQTT/discussions)
- **Contact**: Open a new issue with the "question" label

---

[◀ Back to Documentation Index](README.md)
