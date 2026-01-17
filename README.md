# kmMqtt

A C++14 MQTT 5.0 client library designed for game development and cross-platform applications.

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Supported Platforms](#supported-platforms)
- [Dependencies](#dependencies)
- [Building](#building)
- [Quick Start](#quick-start)
- [Documentation](#documentation)
- [Notes](#notes)
- [References](#references)

## Overview

kmMqtt provides an MQTT 5.0 client implementation developed by a game development engineer with game engines in mind, but works across any C++14 application. The library handles connection management, message publishing, topic subscriptions, and event-driven message handling with a focus on cross-platform compatibility.

## Features

- **MQTT 5.0 protocol support** - Full implementation of MQTT 5.0 specification
- **Cross-platform socket support** - Uses adapter pattern for platform-specific socket implementations
  - Included: IXWebSocket-based implementation for Windows, Linux, and Android (macOS not tested but should work)
  - Extendable to other closed-source platforms via `IWebSocket` interface
- **Flexible operation modes** - Synchronous and asynchronous tick modes
- **Adaptable event dispatching** - Customize callback execution via `ICallbackDispatcher` to sync with your application's event loop
- **Automatic reconnection handling** - Built-in reconnection logic
- **Full QoS support** - QoS 0, 1, and 2 message delivery
- **MQTT 5.0 features** - Topic aliases, user properties, and more
- **Session state management** - In-memory session state tracking
- **Performance optimizations** - Small buffer optimization for reduced heap allocations

## Supported Platforms

The library targets C++14 and is designed for cross-platform use. Platform-specific socket implementations can be provided through the `IWebSocket` interface:

- **Tested**: Windows and Linux
- **Expected to work**: Android, macOS
- **Extendable**: Any platform by implementing `IMqttEnvironment` and `IWebSocket` interfaces

The included IXWebSocket-based implementation (`DefaultWebsocket`) works on Windows, Linux, and Android.

## Dependencies

### Required

- C++14 compatible compiler
- CMake 3.5 or later

### Optional

- **OpenSSL**: Required when `BUILD_IXWEBSOCKET=ON` for WebSocket Secure (WSS) support
  - Automatically installed by vcpkg on Windows and Linux when using CMake with vcpkg integration
  - Manual installation: `libssl-dev` package on Linux, `brew install openssl` on macOS

## Building

### Basic build

```bash
cmake -B build
cmake --build build
```

### Common build options

```bash
cmake -B build \
  -DBUILD_SHARED_LIBS=OFF \
  -DBUILD_UNIT_TESTS=ON \
  -DBUILD_EXAMPLES=ON \
  -DBUILD_IXWEBSOCKET=ON \
  -DENABLE_BYTEBUFFER_SBO=ON
cmake --build build
```

See [BUILDING.md](BUILDING.md) for detailed build instructions and configuration options.

## Quick Start

### Non-blocking connection and publish (recommended for game engines)

```cpp
#include <cleanMqtt/MqttClient.h>

using namespace cleanMqtt::mqtt;

// Create client with default environment
MqttClient client;

// Register connection callback (required for non-blocking)
client.onConnectEvent().subscribe([&client](const ConnectEventDetails& details) {
    if (details.isSuccessful) {
        // Connection successful, now we can publish
        ByteBuffer payload(256);
        payload.append("Hello MQTT");
        
        PublishOptions pubOpts;
        pubOpts.qos = Qos::QOS_1;
        
        client.publish("test/topic", std::move(payload), std::move(pubOpts));
    } else {
        // Handle connection error
    }
});

// Set up connection arguments
ConnectArgs connectArgs;
connectArgs.setClientId("my-client");
connectArgs.setCleanStart(true);

// Create broker address
Address brokerAddr = Address::createURL("mqtt", "broker.example.com", "1883", "");
ConnectAddress address(brokerAddr);

// Initiate connection (non-blocking)
ReqResult result = client.connect(std::move(connectArgs), std::move(address));
if (result.isError()) {
    // Handle immediate errors (e.g., invalid parameters)
}
// Connection result will arrive via onConnectEvent callback
```

### Blocking connection (for non-game environments)

```cpp
#include <cleanMqtt/MqttClient.h>
#include <atomic>
#include <chrono>
#include <thread>

using namespace cleanMqtt::mqtt;

// Create client
MqttClient client;

// Track connection state
std::atomic<bool> connected{false};
std::atomic<bool> connectionFailed{false};

// Register connection callback
client.onConnectEvent().subscribe([&](const ConnectEventDetails& details) {
    if (details.isSuccessful) {
        connected = true;
    } else {
        connectionFailed = true;
    }
});

// Set up connection
ConnectArgs connectArgs;
connectArgs.setClientId("my-client");
connectArgs.setCleanStart(true);

Address brokerAddr = Address::createURL("mqtt", "broker.example.com", "1883", "");
ConnectAddress address(brokerAddr);

// Initiate connection
ReqResult result = client.connect(std::move(connectArgs), std::move(address));
if (result.isError()) {
    // Handle immediate error
}

// Wait for connection callback
while (!connected && !connectionFailed) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

if (connected) {
    // Now we can publish
    ByteBuffer payload(256);
    payload.append("Hello MQTT");
    
    PublishOptions pubOpts;
    pubOpts.qos = Qos::QOS_1;
    
    client.publish("test/topic", std::move(payload), std::move(pubOpts));
}
```

### Subscribing and receiving messages

```cpp
// Register callback for received messages
client.onPublishEvent().subscribe([](const Publish& message) {
    // Process received message
    const char* topic = message.getVariableHeader().getTopicName();
    const ByteBuffer& payload = message.getPayloadHeader().getPayload();
    // ... handle message
});

// Subscribe to topics
std::vector<Topic> topics = {
    Topic("sensor/temperature", TopicSubscriptionOptions(Qos::QOS_1)),
    Topic("sensor/humidity", TopicSubscriptionOptions(Qos::QOS_1))
};

SubscribeOptions subOpts;
client.subscribe(topics, std::move(subOpts));
```

### Synchronous mode operation

```cpp
// Create client in synchronous mode
MqttClientOptions options(TickMode::SYNC);
MqttClient client(DefaultEnvironmentFactory::create(), options);

// ... connect and subscribe as above

// Application loop
while (running) {
    client.tick();  // Process messages and events
    // ... other application work
}
```

## Documentation

- **[API Documentation](docs/API/index.html)** - Complete API reference generated with Doxygen
- **[BUILDING.md](BUILDING.md)** - Build instructions, CMake options, and platform-specific setup

## Notes

- Connection operations (`connect()`) are **non-blocking** in both `ASYNC` and `SYNC` modes
  - Use `onConnectEvent()` callback to determine when connection completes
  - Blocking pattern shown above is useful for non-game environments
- When using `TickMode::ASYNC`, the library manages its own thread for message processing
- When using `TickMode::SYNC`, the application must call `tick()` regularly
- Session state persistence to disk is not currently implemented
- The library does not include an MQTT broker implementation

## References

- [MQTT 5.0 Specification](https://docs.oasis-open.org/mqtt/mqtt/v5.0/mqtt-v5.0.html)
