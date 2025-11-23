# Architecture

An overview of CleanMQTT's internal design and architecture.

## Table of Contents

- [Design Philosophy](#design-philosophy)
- [Architecture Overview](#architecture-overview)
- [Core Components](#core-components)
- [Threading Model](#threading-model)
- [Memory Management](#memory-management)
- [Protocol Implementation](#protocol-implementation)
- [Extension Points](#extension-points)
- [Performance Considerations](#performance-considerations)

---

## Design Philosophy

CleanMQTT is built on these core principles:

### 🎯 Clean & Modern

- **Modern C++14**: Leverages modern C++ features for type safety and expressiveness
- **Clear API**: Intuitive, user-friendly interfaces
- **Minimal Dependencies**: Reduces external dependencies for easier integration

### ⚡ Performance-Oriented

- **Small Buffer Optimization (SBO)**: Avoids heap allocations for small buffers
- **Move Semantics**: Efficient data transfer without unnecessary copies
- **Configurable**: Performance characteristics can be tuned for specific use cases

### 🔧 Flexible & Extensible

- **Interface-Based Design**: Key components defined through interfaces
- **Platform Abstraction**: Easy to port to new platforms
- **Customizable Behavior**: Inject custom implementations for logging, threading, etc.

### 🛡️ Robust

- **Event-Driven**: Clear separation of concerns through events
- **Error Handling**: Comprehensive error reporting
- **Testing**: Extensive unit tests and sanitizer support

---

## Architecture Overview

```
┌─────────────────────────────────────────────────────────────┐
│                    Application Layer                         │
│                   (User Application)                         │
└───────────────────────┬─────────────────────────────────────┘
                        │
                        ▼
┌─────────────────────────────────────────────────────────────┐
│                      MqttClient                              │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │   Events     │  │  Connection  │  │   Messaging  │      │
│  │   System     │  │  Management  │  │   Engine     │      │
│  └──────────────┘  └──────────────┘  └──────────────┘      │
└───────────────────────┬─────────────────────────────────────┘
                        │
                        ▼
┌─────────────────────────────────────────────────────────────┐
│                  MqttClientImpl                              │
│  ┌──────────────────────────────────────────────────────┐   │
│  │             Protocol State Machine                    │   │
│  │  • Connection State   • Subscription State            │   │
│  │  • Publish State      • QoS Handling                  │   │
│  └──────────────────────────────────────────────────────┘   │
│  ┌──────────────────────────────────────────────────────┐   │
│  │             Packet Processing                         │   │
│  │  • Packet Encoding    • Packet Decoding               │   │
│  │  • Packet Validation  • Packet Routing                │   │
│  └──────────────────────────────────────────────────────┘   │
└───────────────────────┬─────────────────────────────────────┘
                        │
                        ▼
┌─────────────────────────────────────────────────────────────┐
│                 Platform Abstraction                         │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │   ISocket    │  │  IWebSocket  │  │   ILogger    │      │
│  └──────────────┘  └──────────────┘  └──────────────┘      │
│  ┌──────────────┐  ┌──────────────────────────────────┐    │
│  │ IDispatcher  │  │    IMqttEnvironment              │    │
│  └──────────────┘  └──────────────────────────────────┘    │
└───────────────────────┬─────────────────────────────────────┘
                        │
                        ▼
┌─────────────────────────────────────────────────────────────┐
│                  Operating System                            │
│       (Windows Sockets, Linux Sockets, Threads)             │
└─────────────────────────────────────────────────────────────┘
```

---

## Core Components

### MqttClient

The main public-facing API that users interact with.

**Responsibilities:**
- Provides high-level MQTT operations (connect, publish, subscribe)
- Manages event subscriptions
- Delegates to MqttClientImpl for actual work
- Ensures thread-safety for public API

**Key Design:**
- Non-copyable, non-movable (RAII pattern)
- Pimpl idiom (implementation hidden in MqttClientImpl)
- Event-based notification system

### MqttClientImpl

The internal implementation of the MQTT client logic.

**Responsibilities:**
- MQTT protocol state machine
- Packet encoding/decoding
- Connection lifecycle management
- QoS flow handling
- Keep-alive mechanism

**Components:**
```
MqttClientImpl
├── ConnectionManager
│   ├── Connection state tracking
│   ├── Keep-alive timer
│   └── Reconnection logic
├── PacketProcessor
│   ├── Packet encoder
│   ├── Packet decoder
│   └── Packet validator
├── SubscriptionManager
│   ├── Active subscriptions
│   └── Topic filtering
├── PublishManager
│   ├── Outgoing publishes
│   ├── QoS state machine
│   └── Packet ID management
└── EventDispatcher
    └── Event notification
```

### ByteBuffer

A memory buffer for holding binary data with optional Small Buffer Optimization.

**Features:**
- Move-only semantics (no copying)
- SBO for buffers ≤ configured size (default: 256 bytes)
- Automatic heap allocation for larger buffers
- Efficient for MQTT payloads

**Memory Layout (with SBO):**
```
Small buffer (≤ 256 bytes):
┌──────────────────────────────────┐
│   Stack-allocated array          │
│   [256 bytes inline storage]     │
└──────────────────────────────────┘

Large buffer (> 256 bytes):
┌──────────────────────────────────┐
│   Heap pointer                    │
│   ├──> Heap-allocated data       │
└──────────────────────────────────┘
```

### UniqueFunction

A move-only function wrapper (alternative to std::function).

**Why Not std::function?**
- std::function is copyable, which isn't needed for callbacks
- UniqueFunction uses move semantics for better performance
- Supports SBO for small functors (default: 32 bytes)

### Event System

Type-safe event subscription and notification.

**Design:**
```cpp
Event<EventArgs>
├── Subscription management
├── Thread-safe notification
└── Automatic cleanup

Usage:
event += [](const Args& args) { /* handler */ };
event.invoke(args);
```

---

## Threading Model

CleanMQTT supports two operation modes:

### Synchronous Mode (Default)

```
┌────────────────────────────────────┐
│      Application Thread            │
│                                    │
│  while (running) {                 │
│    client.tick();  ◄──── User calls│
│    // tick processes:              │
│    //   - Network I/O              │
│    //   - Event callbacks          │
│    //   - Timers                   │
│    sleep(10ms);                    │
│  }                                 │
└────────────────────────────────────┘
```

**Characteristics:**
- Single-threaded
- User controls when processing happens
- Predictable, no threading overhead
- Simple debugging

### Asynchronous Mode

```
┌────────────────────────────────────┐
│      Application Thread            │
│                                    │
│  client.publish(...)   ──┐         │
│  client.subscribe(...) ──┼──┐      │
│  // No tick() needed     │  │      │
└──────────────────────────┼──┼──────┘
                           │  │
                           ▼  ▼
┌────────────────────────────────────┐
│     Background Thread              │
│                                    │
│  while (running) {                 │
│    tick();  ◄──── Automatic        │
│    sleep(tickInterval);            │
│  }                                 │
└────────────────────────────────────┘
```

**Characteristics:**
- Background thread handles processing
- Callbacks dispatched to configured thread
- Better for responsive applications
- Requires thread synchronization

### Callback Dispatching

Callbacks can be dispatched to specific threads:

```
┌─────────────────────────────────────────────┐
│         ICallbackDispatcher                 │
│                                             │
│  DefaultDispatcher:                         │
│    - Queues callbacks                       │
│    - Dispatches on specific thread          │
│                                             │
│  ImmediateDispatcher:                       │
│    - Executes callbacks immediately         │
│    - On the calling thread                  │
└─────────────────────────────────────────────┘
```

---

## Memory Management

### RAII Principles

All resources use RAII for automatic cleanup:

```cpp
{
    MqttClient client;  // Allocates resources
    client.connect(...);
    // ...
}  // Destructor automatically cleans up
```

### Move Semantics

Heavy use of move semantics avoids copies:

```cpp
// Payload ownership transferred
ByteBuffer payload("data");
client.publish("topic", std::move(payload), ...);
// payload is now empty, no copy made
```

### Small Buffer Optimization

Configurable SBO for common cases:

```cpp
// ByteBuffer with 256-byte SBO (configurable)
ByteBuffer small("Hello");  // Uses stack storage
ByteBuffer large(hugeData); // Uses heap storage

// UniqueFunction with 32-byte SBO (configurable)
UniqueFunction<void()> func = []() { /* ... */ };
```

**Build Configuration:**
```bash
cmake -DENABLE_BYTEBUFFER_SBO=ON \
      -DBYTEBUFFER_SBO_MAX_SIZE=256 \
      -DENABLE_UNIQUEFUNCTION_SBO=ON \
      -DUNIQUEFUNCTION_SBO_MAX_SIZE=32
```

---

## Protocol Implementation

### MQTT Packet Structure

```
┌──────────────────────────────────────────┐
│       Fixed Header (2-5 bytes)           │
│  ┌─────────────┬──────────────────────┐  │
│  │ Packet Type │ Remaining Length     │  │
│  └─────────────┴──────────────────────┘  │
├──────────────────────────────────────────┤
│       Variable Header (variable)         │
│  ┌──────────────────────────────────┐    │
│  │  Protocol-specific fields         │    │
│  └──────────────────────────────────┘    │
├──────────────────────────────────────────┤
│       Payload (variable)                 │
│  ┌──────────────────────────────────┐    │
│  │  Message content                  │    │
│  └──────────────────────────────────┘    │
└──────────────────────────────────────────┘
```

### State Machines

#### Connection State Machine

```
┌──────────────┐
│ Disconnected │
└──────┬───────┘
       │ connect()
       ▼
┌──────────────┐
│  Connecting  │
└──────┬───────┘
       │ CONNACK received
       ▼
┌──────────────┐
│  Connected   │◄──────────┐
└──────┬───────┘           │
       │ disconnect()      │ reconnect
       │ or error          │
       ▼                   │
┌──────────────┐           │
│Disconnecting │           │
└──────┬───────┘           │
       │                   │
       └───────────────────┘
```

#### QoS 0 Flow

```
Publisher                    Broker                   Subscriber
    │                          │                          │
    ├─── PUBLISH (QoS 0) ─────>│                          │
    │                          ├─── PUBLISH (QoS 0) ─────>│
    │                          │                          │
```

#### QoS 1 Flow

```
Publisher                    Broker                   Subscriber
    │                          │                          │
    ├─── PUBLISH (QoS 1) ─────>│                          │
    │                          ├─── PUBLISH (QoS 1) ─────>│
    │                          │<──── PUBACK ─────────────┤
    │<──── PUBACK ─────────────┤                          │
    │                          │                          │
```

#### QoS 2 Flow

```
Publisher                    Broker                   Subscriber
    │                          │                          │
    ├─── PUBLISH (QoS 2) ─────>│                          │
    │<──── PUBREC ─────────────┤                          │
    ├─── PUBREL ──────────────>│                          │
    │                          ├─── PUBLISH (QoS 2) ─────>│
    │                          │<──── PUBREC ─────────────┤
    │                          ├─── PUBREL ──────────────>│
    │                          │<──── PUBCOMP ────────────┤
    │<──── PUBCOMP ────────────┤                          │
    │                          │                          │
```

### Keep-Alive Mechanism

```
Time ─────────────────────────────────────────>

Client │    PINGREQ     │    PINGREQ     │
       ├────────────────┼────────────────┼──>
       │                │                │
Broker │    PINGRESP    │    PINGRESP    │
       ├────────────────┼────────────────┼──>
       │                │                │
       │<─ Keep-Alive ─>│<─ Keep-Alive ─>│
```

If no PINGRESP received within timeout, connection is considered lost.

---

## Extension Points

### Custom Socket Implementation

Implement `ISocket` for custom transport:

```cpp
class CustomSocket : public ISocket {
public:
    virtual ClientError connect(const char* host, uint16_t port) override;
    virtual ClientError disconnect() override;
    virtual int send(const uint8_t* data, size_t len) override;
    virtual int receive(uint8_t* buffer, size_t len) override;
    // ...
};
```

### Custom Logger

Implement `ILogger` for custom logging:

```cpp
class CustomLogger : public ILogger {
public:
    virtual void log(LogLevel level, const char* message) override {
        // Send to custom logging system
    }
};
```

### Custom Environment

Implement `IMqttEnvironment` for new platforms:

```cpp
class CustomEnvironment : public IMqttEnvironment {
public:
    virtual ISocket* createSocket() override;
    virtual IWebSocket* createWebSocket() override;
    virtual ILogger* getLogger() override;
    virtual ICallbackDispatcher* getDispatcher() override;
    // ...
};
```

---

## Performance Considerations

### Optimization Techniques

1. **Small Buffer Optimization**
   - Reduces heap allocations for common cases
   - Configurable based on use case
   - Trade-off: Larger stack usage

2. **Move Semantics**
   - Eliminates unnecessary copies
   - Efficient data transfer
   - Zero-cost abstraction

3. **Event Batching**
   - Multiple events processed per tick
   - Reduces context switching
   - Improves throughput

4. **Packet Pooling** (Future)
   - Reuse packet buffers
   - Reduce allocation overhead
   - Better cache locality

### Build Options for Performance

```bash
# Release build with optimizations
cmake -DCMAKE_BUILD_TYPE=Release

# Enable SBO for better performance
cmake -DENABLE_BYTEBUFFER_SBO=ON \
      -DBYTEBUFFER_SBO_MAX_SIZE=256

# Disable logging in production
cmake -DENABLE_LOGS=OFF
```

### Benchmarking

CleanMQTT includes benchmarks for measuring performance:

```bash
cmake -DBUILD_BENCHMARKING=ON
cmake --build build
./build/benchmarks/mqtt_benchmarks
```

**Benchmark Areas:**
- Connection establishment time
- Message throughput (messages/second)
- Latency (round-trip time)
- Memory usage
- CPU usage

---

## Code Organization

```
CleanMQTT/
├── include/
│   ├── public/              # Public API headers
│   │   └── cleanMqtt/
│   │       ├── MqttClient.h
│   │       ├── ByteBuffer.h
│   │       ├── Interfaces/  # Interface definitions
│   │       ├── Mqtt/        # MQTT-specific types
│   │       └── ...
│   └── private/             # Internal headers
│       └── cleanMqtt/
│           ├── MqttClientImpl.h
│           └── ...
├── src/                     # Implementation files
│   ├── MqttClient.cpp
│   ├── MqttClientImpl.cpp
│   ├── Mqtt/                # Protocol implementation
│   └── ...
├── examples/                # Example applications
├── tests/                   # Unit tests
├── benchmarks/              # Performance benchmarks
└── docs/                    # Documentation
```

---

## Testing Strategy

### Unit Tests

- **Coverage Goal**: >80% code coverage
- **Framework**: Custom test framework
- **Sanitizers**: ASAN, UBSAN, TSAN, MSAN support

### Integration Tests

- Real broker connections
- Multi-client scenarios
- Network failure simulation

### Build & Test

```bash
# Build with tests
cmake -DBUILD_UNIT_TESTS=ON

# Run tests with sanitizers
cmake --preset linux-gcc-ubsan-asan
cmake --build build
./build/tests/cleanMqttTests

# Generate coverage report
cmake --preset linux-gcc-coverage
cmake --build build
cmake --build build --target coverage_report
```

---

## Future Enhancements

Planned architectural improvements:

1. **Plugin System**: Dynamic loading of custom transports
2. **Packet Pooling**: Reduce allocations for high-throughput scenarios
3. **Zero-Copy I/O**: Minimize data copying in packet processing
4. **TLS/SSL Support**: Built-in secure transport layer
5. **Compression**: Optional payload compression
6. **Message Persistence**: Local storage for offline queuing

---

[◀ Back to Documentation Index](README.md) | [Contributing ▶](CONTRIBUTING.md)
