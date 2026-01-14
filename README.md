# CleanMQTT

A C++14 MQTT client library supporting MQTT 3.1.1 and 5.0 protocols.

## Overview

CleanMQTT provides an MQTT client implementation designed with cross-platform support in mind. The library handles connection management, message publishing, topic subscriptions, and event-driven message handling.

## Features

- MQTT 3.1.1 and 5.0 protocol support
- TCP and WebSocket transport (with optional TLS/WSS support via IXWebSocket)
- Synchronous and asynchronous operation modes
- Event-driven callback system
- Automatic reconnection handling
- QoS 0, 1, and 2 message delivery
- Topic aliases and user properties (MQTT 5.0)
- Session state management
- Small buffer optimization for reduced heap allocations

## Supported Platforms

The library targets C++14 and is intended for use on platforms supporting standard C++14 features. Platform-specific implementations are provided for:

- Windows (via `DefaultWinEnv`)
- Linux (via `DefaultLinuxEnv`)

Custom platform support can be added by implementing the `IMqttEnvironment` interface.

## Dependencies

### Required

- C++14 compatible compiler
- CMake 3.5 or later

### Optional

- **OpenSSL**: Required when `BUILD_IXWEBSOCKET=ON` for WebSocket Secure (WSS) support
  - Windows: vcpkg or manual installation
  - Linux: `libssl-dev` package
  - macOS: `brew install openssl`

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

### Basic connection and publish

```cpp
#include <cleanMqtt/MqttClient.h>

using namespace cleanMqtt::mqtt;

// Create client with default environment
MqttClient client;

// Set up connection arguments
ConnectArgs connectArgs;
connectArgs.setClientId("my-client");
connectArgs.setCleanStart(true);

// Create broker address
Address brokerAddr = Address::createURL("mqtt", "broker.example.com", "1883", "");
ConnectAddress address(brokerAddr);

ReqResult result = client.connect(std::move(connectArgs), std::move(address));
if (result.isError()) {
    // Handle connection error
}

// Publish a message
ByteBuffer payload(256);
payload.append("Hello MQTT");

PublishOptions pubOpts;
pubOpts.qos = Qos::QOS_1;

client.publish("test/topic", std::move(payload), std::move(pubOpts));
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

## API Reference

See [API_REFERENCE.md](API_REFERENCE.md) for complete API documentation.

## Notes

- When using `TickMode::ASYNC`, the library manages its own thread for message processing
- When using `TickMode::SYNC`, the application must call `tick()` regularly
- Session state persistence to disk is not currently implemented
- The library does not include an MQTT broker implementation

## References

- [MQTT 3.1.1 Specification](https://docs.oasis-open.org/mqtt/mqtt/v3.1.1/mqtt-v3.1.1.html)
- [MQTT 5.0 Specification](https://docs.oasis-open.org/mqtt/mqtt/v5.0/mqtt-v5.0.html)
