Licensed under Apache 2.0. See LICENSE and NOTICE.

# kmMqtt

[![CI](https://github.com/KMiseckas/kmMqtt/actions/workflows/release_ci.yml/badge.svg?branch=master)](https://github.com/KMiseckas/kmMqtt/actions/workflows/release_ci.yml)
[![Sanitizers](https://github.com/KMiseckas/kmMqtt/actions/workflows/sanitizers.yml/badge.svg?branch=master)](https://github.com/KMiseckas/kmMqtt/actions/workflows/sanitizers.yml)
[![Coverage](https://img.shields.io/endpoint?url=https://kmiseckas.github.io/kmMqtt/coverage.json)](https://kmiseckas.github.io/kmMqtt/)
[![Docs](https://img.shields.io/badge/docs-doxygen-blue)](https://kmiseckas.github.io/kmMqtt/)

[![C++14](https://img.shields.io/badge/C%2B%2B-14-blue?logo=cplusplus&logoColor=white)](https://en.cppreference.com/w/cpp/14)
[![MQTT](https://img.shields.io/badge/MQTT-5.0-orange)](https://docs.oasis-open.org/mqtt/mqtt/v5.0/mqtt-v5.0.html)
[![License](https://img.shields.io/github/license/KMiseckas/kmMqtt)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20Adaptable-lightgrey)](https://github.com/KMiseckas/kmMqtt#supported-platforms)

A C++14 MQTT 5.0 client library designed for game development and cross-platform applications.
kmMqtt started as a hobby project to deepen my understanding of MQTT 5.0 and to practice designing and implementing a clean, cross-platform C++ core SDK while learning modern build and tooling workflows.

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Protocol Support Notes](#protocol-support-notes)
- [Supported Platforms](#supported-platforms)
- [Dependencies](#dependencies)
- [Building](#building)
- [Quick Start](#quick-start)
- [Documentation](#documentation)
- [Notes](#notes)
- [References](#references)

## Overview

kmMqtt provides an MQTT 5.0 client implementation with game development and game engines in mind, but works across any C++14 application. The library handles connection management, message publishing, topic subscriptions, and event-driven message handling with a focus on cross-platform compatibility.

## Features

- **MQTT 5.0 protocol support** - Broad MQTT 5.0 client coverage for connect/publish/subscribe/session workflows, with known gaps documented below
- **Cross-platform socket support** - Uses adapter pattern for platform-specific socket implementations
  - Included: IXWebSocket-based implementation for Windows & Linux
  - Extendable to other closed-source platforms via `IWebSocket` interface
- **Flexible operation modes** - Synchronous and asynchronous tick modes
- **Adaptable event dispatching** - Customize callback execution via `ICallbackDispatcher` to sync with your application's event loop
- **Automatic reconnection handling** - Built-in reconnection logic
- **Full QoS support** - QoS 0, 1, and 2 message delivery
- **Session state management** - In-memory session state tracking\*
- **SBO** - Small buffer optimization for reduced heap allocations in critical paths.
- **CMake** - Uses cmake for build file generation.

\*Disk saved session states currently not-included and WIP.

## Protocol Support Notes

| Area                                                                                      | Status                        | Notes                                                                                                                                                     |
| ----------------------------------------------------------------------------------------- | ----------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Core MQTT client workflows (connect, publish, subscribe, QoS 0/1/2, keepalive, reconnect) | Implemented                   | Covered by unit/API tests and integration smoke/full suites.                                                                                              |
| CONNECT username/password authentication                                                  | Implemented                   | Standard MQTT CONNECT username/password fields are supported and can be used for broker authentication.                                                   |
| MQTT 5 AUTH packet exchange (enhanced auth flow)                                          | Partial / not fully supported | `AUTH` is currently treated as unsupported in the receive queue, so full enhanced-auth exchange is not implemented yet.                                   |
| `ConnectArgs` extended auth fields                                                        | Partial                       | `extendedAuthenticationMethod` and `extendedAuthenticationData` are exposed in API, but are not fully wired end-to-end for production AUTH workflows yet. |

If your broker uses standard CONNECT username/password auth, that path is supported. If your broker or deployment requires enhanced AUTH packet exchange, treat that path as not fully supported for now.

## Supported Platforms

The library targets C++14 and is designed for cross-platform use. Platform-specific socket implementations can be provided through the `IWebSocket` interface:

- **Tested**: Windows and Linux
- **Expected to work**: Android, macOS
- **Extendable**: By implementing `IMqttEnvironment` and `IWebSocket` interfaces

Default IXWebSocket-based socket implementation (`DefaultWebsocket`) works on Windows & Linux (and should on Mac, iOS, and Android).
The library hasn't been tested on other platforms besides Windows and Linux yet.

## Dependencies

### Required

- C++14 compatible compiler
- CMake 3.5 or later

### Optional

- **OpenSSL**: Required when `BUILD_IXWEBSOCKET=ON` for WebSocket Secure (WSS) support
  - Automatically installed by vcpkg on Windows and Linux when using CMake with vcpkg integration
  - Manual installation: `libssl-dev` package on Linux, `brew install openssl` on macOS
- **Doxygen**: Required when building documentation.

## Building

See [BUILDING.md](BUILDING.md) for detailed build instructions and configuration options.

## Quick Start

### Non-blocking connection and publish (recommended for game engines)

```cpp
#include <kmMqtt/MqttClient.h>

using namespace kmMqtt::mqtt;

// Create client with default environment
MqttClient client;

// Register connection callback (required for non-blocking)
client.onConnectEvent().add([&client](const ConnectEventDetails& details) {
    if (details.isSuccessful) {
        // Connection successful, now we can publish
        std::string payloadStr{"Hello MQTT"};
        ByteBuffer payload(payloadStr.length());
        payload.append(payloadStr.data(), payloadStr.length());

        PublishOptions pubOpts;
        pubOpts.qos = Qos::QOS_1;

        client.publish("test/topic", std::move(payload), std::move(pubOpts));
    } else {
        // Handle connection error
    }
});

// Set up connection arguments
ConnectArgs connectArgs;
connectArgs.clientId = "my-client";
connectArgs.cleanStart = true;

// Create broker address
ConnectAddress address(Address::createURL("mqtt", "broker.example.com", "1883", ""));

// Initiate connection (non-blocking)
ReqResult result = client.connect(std::move(connectArgs), std::move(address));
if (result.isError())
{
    // Handle immediate errors (e.g., invalid parameters)
}
// Connection result will arrive via onConnectEvent callback
```

### Blocking connection (for non-game environments)

```cpp
#include <kmMqtt/MqttClient.h>
#include <atomic>
#include <chrono>
#include <thread>

using namespace kmMqtt::mqtt;

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

// Set up connection arguments
ConnectArgs connectArgs;
connectArgs.clientId = "my-client";
connectArgs.cleanStart = true;

// Create broker address
ConnectAddress address(Address::createURL("mqtt", "broker.example.com", "1883", ""));

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
    std::string payloadStr{"Hello MQTT"};
    ByteBuffer payload(payloadStr.length());
    payload.append(payloadStr.data(), payloadStr.length());

    PublishOptions pubOpts;
    pubOpts.qos = Qos::QOS_1;

    client.publish("test/topic", std::move(payload), std::move(pubOpts));
}
```

### Subscribing and receiving messages

```cpp
// Register callback for received messages
client.onPublishEvent().add([](const Publish& message) {
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

- **[Coverage](https://kmiseckas.github.io/kmMqtt/)** - Click Coverage top right corner.
- **[API Documentation](https://kmiseckas.github.io/kmMqtt/)** - Complete API reference (generated with Doxygen - see build instructions)
- **[BUILDING.md](BUILDING.md)** - Build instructions, CMake options, and platform-specific setup
- **[docs/CI.md](docs/CI.md)** - CI workflow overview, triggers, and quality-pipeline map
- **[docs/INTEGRATION_TESTS.md](docs/INTEGRATION_TESTS.md)** - Integration suite structure, labels, broker overrides, and local run commands

> **Note**: API documentation must be generated using Doxygen. The HTML documentation will be available at `docs/API/index.html` after generation.

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
