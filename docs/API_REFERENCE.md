# API Reference

Complete reference documentation for the CleanMQTT API.

## Table of Contents

- [Core Classes](#core-classes)
  - [MqttClient](#mqttclient)
  - [MqttClientOptions](#mqttclientoptions)
- [Connection Management](#connection-management)
  - [ConnectArgs](#connectargs)
  - [ConnectAddress](#connectaddress)
  - [DisconnectArgs](#disconnectargs)
- [Messaging](#messaging)
  - [PublishOptions](#publishoptions)
  - [SubscribeOptions](#subscribeoptions)
  - [UnSubscribeOptions](#unsubscribeoptions)
  - [Topic](#topic)
  - [ByteBuffer](#bytebuffer)
- [Events](#events)
  - [ConnectEvent](#connectevent)
  - [DisconnectEvent](#disconnectevent)
  - [ReconnectEvent](#reconnectevent)
  - [PublishEvent](#publishevent)
  - [PublishAckEvent](#publishackevent)
  - [SubscribeAckEvent](#subscribeackevent)
  - [UnSubscribeAckEvent](#unsubscribeackevent)
  - [ErrorEvent](#errorevent)
- [Enumerations](#enumerations)
  - [QoS](#qos)
  - [ConnectionStatus](#connectionstatus)
  - [ReconnectionStatus](#reconnectionstatus)
  - [ClientError](#clienterror)
- [Interfaces](#interfaces)
  - [IMqttEnvironment](#imqttenvironment)
  - [ISocket](#isocket)
  - [IWebSocket](#iwebsocket)
  - [ICallbackDispatcher](#icallbackdispatcher)
  - [ILogger](#ilogger)
- [Utilities](#utilities)
  - [DefaultEnvironmentFactory](#defaultenvironmentfactory)
  - [UniqueFunction](#uniquefunction)

---

## Core Classes

### MqttClient

The main class for interacting with MQTT brokers.

**Header**: `#include <cleanMqtt/MqttClient.h>`

**Namespace**: `cleanMqtt::mqtt`

#### Constructors

```cpp
// Default constructor
MqttClient();

// Constructor with custom environment and options
MqttClient(const IMqttEnvironment* const env, 
           const MqttClientOptions& clientOptions);
```

#### Methods

##### connect()

Connects to an MQTT broker.

```cpp
ClientError connect(ConnectArgs&& args, 
                   ConnectAddress&& address) noexcept;
```

**Parameters:**
- `args` - Connection arguments (client ID, keep-alive, etc.)
- `address` - Broker address and port

**Returns:** `ClientError` indicating success or failure

**Example:**
```cpp
ConnectArgs args;
args.clientId = "my_client";
args.keepAlive = Seconds(60);

ConnectAddress address;
address.host = "broker.example.com";
address.port = 1883;

client.connect(std::move(args), std::move(address));
```

##### publish()

Publishes a message to a topic.

```cpp
ClientError publish(const char* topic, 
                   ByteBuffer&& payload, 
                   PublishOptions&& options) noexcept;
```

**Parameters:**
- `topic` - Topic string (e.g., "sensor/temperature")
- `payload` - Message payload as ByteBuffer
- `options` - Publishing options (QoS, retain, etc.)

**Returns:** `ClientError` indicating success or failure

**Example:**
```cpp
ByteBuffer payload("Temperature: 23.5°C");
PublishOptions options;
options.qos = QoS::QoS1;
options.retain = false;

client.publish("sensor/temp", std::move(payload), std::move(options));
```

##### subscribe()

Subscribes to one or more topics.

```cpp
ClientError subscribe(const std::vector<Topic>& topics, 
                     SubscribeOptions&& options) noexcept;
```

**Parameters:**
- `topics` - Vector of topics with QoS levels
- `options` - Subscription options

**Returns:** `ClientError` indicating success or failure

**Example:**
```cpp
std::vector<Topic> topics = {
    {"sensor/+/temperature", QoS::QoS0},
    {"alerts/#", QoS::QoS1}
};

client.subscribe(topics, SubscribeOptions{});
```

##### unSubscribe()

Unsubscribes from topics.

```cpp
ClientError unSubscribe(const std::vector<Topic>& topics, 
                       UnSubscribeOptions&& options) noexcept;
```

**Parameters:**
- `topics` - Vector of topics to unsubscribe from
- `options` - Unsubscribe options

**Returns:** `ClientError` indicating success or failure

##### disconnect()

Disconnects from the broker.

```cpp
ClientError disconnect(DisconnectArgs&& args = {}) noexcept;
```

**Parameters:**
- `args` - Optional disconnect arguments

**Returns:** `ClientError` indicating success or failure

##### shutdown()

Shuts down the client and releases resources.

```cpp
ClientError shutdown() noexcept;
```

**Note:** Use this in both synchronous and asynchronous modes.

##### tick()

Processes messages and events (synchronous mode only).

```cpp
ClientError tick() noexcept;
```

**Returns:** `ClientError` indicating success or failure

**Note:** Do not call in asynchronous mode. Call regularly in a loop for synchronous operation.

#### Event Accessors

All event accessors return a reference to the event object for registering callbacks.

```cpp
ErrorEvent& onErrorEvent() noexcept;
ConnectEvent& onConnectEvent() noexcept;
DisconnectEvent& onDisconnectEvent() noexcept;
ReconnectEvent& onReconnectEvent() noexcept;
PublishEvent& onPublishEvent() noexcept;
PublishAckEvent& onPublishAckEvent() noexcept;
SubscribeAckEvent& onSubscribeAckEvent() noexcept;
UnSubscribeAckEvent& onUnSubscribeAckEvent() noexcept;
```

**Example:**
```cpp
client.onConnectEvent() += [](const ConnectEventArgs& args) {
    // Handle connection event
};
```

---

### MqttClientOptions

Configuration options for MqttClient.

**Header**: `#include <cleanMqtt/MqttClientOptions.h>`

#### Fields

```cpp
struct MqttClientOptions {
    bool asyncTicking;              // Enable asynchronous operation
    Milliseconds tickInterval;       // Tick interval for async mode
    bool enableReconnection;         // Enable automatic reconnection
    Milliseconds reconnectionDelay;  // Delay between reconnection attempts
    uint32_t maxReconnectionAttempts; // Max reconnection attempts (0 = infinite)
};
```

**Default Values:**
- `asyncTicking`: `false`
- `tickInterval`: `10ms`
- `enableReconnection`: `true`
- `reconnectionDelay`: `1000ms`
- `maxReconnectionAttempts`: `0` (infinite)

---

## Connection Management

### ConnectArgs

Arguments for connecting to an MQTT broker.

**Header**: `#include <cleanMqtt/Mqtt/Params/ConnectArgs.h>`

#### Fields

```cpp
struct ConnectArgs {
    std::string clientId;           // Unique client identifier
    Seconds keepAlive;              // Keep-alive interval
    bool cleanSession;              // Clean session flag
    std::string username;           // Optional username
    std::string password;           // Optional password
    ByteBuffer willPayload;         // Last Will message payload
    std::string willTopic;          // Last Will topic
    QoS willQoS;                   // Last Will QoS
    bool willRetain;               // Last Will retain flag
};
```

**Example:**
```cpp
ConnectArgs args;
args.clientId = "device_001";
args.keepAlive = Seconds(60);
args.cleanSession = true;
args.username = "user";
args.password = "pass";

// Set Last Will
args.willTopic = "devices/device_001/status";
args.willPayload = ByteBuffer("offline");
args.willQoS = QoS::QoS1;
args.willRetain = true;
```

---

### ConnectAddress

Broker address information.

**Header**: `#include <cleanMqtt/Mqtt/Params/ConnectArgs.h>`

#### Fields

```cpp
struct ConnectAddress {
    std::string host;    // Broker hostname or IP address
    uint16_t port;       // Broker port (typically 1883 or 8883)
    bool useWebSocket;   // Use WebSocket transport
};
```

---

### DisconnectArgs

Arguments for disconnecting from the broker.

**Header**: `#include <cleanMqtt/Mqtt/Params/DisconnectArgs.h>`

#### Fields

```cpp
struct DisconnectArgs {
    uint8_t reasonCode;     // MQTT 5.0 reason code
    std::string reason;     // Optional reason string
};
```

---

## Messaging

### PublishOptions

Options for publishing messages.

**Header**: `#include <cleanMqtt/Mqtt/Params/PublishOptions.h>`

#### Fields

```cpp
struct PublishOptions {
    QoS qos;                        // Quality of Service level
    bool retain;                     // Retain flag
    bool duplicate;                  // Duplicate flag (usually set by broker)
    uint32_t messageExpiryInterval; // Message expiry (MQTT 5.0)
    std::string contentType;        // Content type (MQTT 5.0)
    std::string responseTopic;      // Response topic (MQTT 5.0)
    ByteBuffer correlationData;     // Correlation data (MQTT 5.0)
};
```

**Example:**
```cpp
PublishOptions options;
options.qos = QoS::QoS1;
options.retain = false;
options.messageExpiryInterval = 3600; // 1 hour
options.contentType = "application/json";
```

---

### SubscribeOptions

Options for subscribing to topics.

**Header**: `#include <cleanMqtt/Mqtt/Params/SubscribeOptions.h>`

#### Fields

```cpp
struct SubscribeOptions {
    uint32_t subscriptionIdentifier; // MQTT 5.0 subscription ID
    // Additional MQTT 5.0 options
};
```

---

### UnSubscribeOptions

Options for unsubscribing from topics.

**Header**: `#include <cleanMqtt/Mqtt/Params/UnSubscribeOptions.h>`

---

### Topic

Represents a topic with its QoS level.

**Header**: `#include <cleanMqtt/Mqtt/Params/Topic.h>`

#### Fields

```cpp
struct Topic {
    std::string name;    // Topic name/pattern
    QoS qos;            // Quality of Service level
};
```

**Example:**
```cpp
Topic topic;
topic.name = "sensors/+/temperature";  // + is single-level wildcard
topic.qos = QoS::QoS1;

Topic topic2{"alerts/#", QoS::QoS0};   // # is multi-level wildcard
```

---

### ByteBuffer

A buffer for holding message payloads and binary data.

**Header**: `#include <cleanMqtt/ByteBuffer.h>`

#### Constructors

```cpp
ByteBuffer();                              // Empty buffer
ByteBuffer(const std::string& str);        // From string
ByteBuffer(const char* data, size_t len); // From raw data
```

#### Methods

```cpp
std::string toString() const;              // Convert to string
const uint8_t* data() const;              // Get raw data pointer
size_t size() const;                       // Get buffer size
```

---

## Events

### ConnectEvent

Triggered when connection status changes.

**Event Args:**
```cpp
struct ConnectEventArgs {
    ConnectionStatus status;  // Connection status
    uint8_t returnCode;      // MQTT return code
    bool sessionPresent;     // Session present flag
};
```

**Usage:**
```cpp
client.onConnectEvent() += [](const ConnectEventArgs& args) {
    if (args.status == ConnectionStatus::Connected) {
        std::cout << "Connected!" << std::endl;
    }
};
```

---

### DisconnectEvent

Triggered when disconnected from broker.

**Event Args:**
```cpp
struct DisconnectEventArgs {
    uint8_t reasonCode;      // Disconnect reason code
    std::string reason;      // Reason string
};
```

---

### ReconnectEvent

Triggered during reconnection attempts.

**Event Args:**
```cpp
struct ReconnectEventArgs {
    ReconnectionStatus status;   // Reconnection status
    uint32_t attemptCount;       // Current attempt number
};
```

---

### PublishEvent

Triggered when a message is received.

**Event Args:**
```cpp
struct PublishEventArgs {
    std::string topic;           // Topic name
    ByteBuffer payload;          // Message payload
    QoS qos;                    // Quality of Service
    bool retain;                // Retain flag
    bool duplicate;             // Duplicate flag
};
```

**Usage:**
```cpp
client.onPublishEvent() += [](const PublishEventArgs& args) {
    std::cout << "Topic: " << args.topic << std::endl;
    std::cout << "Message: " << args.payload.toString() << std::endl;
};
```

---

### PublishAckEvent

Triggered when publish is acknowledged (QoS 1 or 2).

**Event Args:**
```cpp
struct PublishAckEventArgs {
    uint16_t packetId;          // Packet identifier
    uint8_t reasonCode;         // Reason code (MQTT 5.0)
};
```

---

### SubscribeAckEvent

Triggered when subscription is acknowledged.

**Event Args:**
```cpp
struct SubscribeAckEventArgs {
    uint16_t packetId;              // Packet identifier
    std::vector<uint8_t> returnCodes; // Return codes for each topic
};
```

---

### UnSubscribeAckEvent

Triggered when unsubscription is acknowledged.

**Event Args:**
```cpp
struct UnSubscribeAckEventArgs {
    uint16_t packetId;              // Packet identifier
    std::vector<uint8_t> reasonCodes; // Reason codes (MQTT 5.0)
};
```

---

### ErrorEvent

Triggered when an error occurs.

**Event Args:**
```cpp
struct ErrorEventArgs {
    ClientError error;          // Error code
    std::string message;        // Error message
};
```

---

## Enumerations

### QoS

Quality of Service levels.

```cpp
enum class QoS : uint8_t {
    QoS0 = 0,  // At most once delivery
    QoS1 = 1,  // At least once delivery
    QoS2 = 2   // Exactly once delivery
};
```

---

### ConnectionStatus

Connection states.

```cpp
enum class ConnectionStatus {
    Disconnected,
    Connecting,
    Connected,
    Disconnecting,
    ConnectionFailed
};
```

---

### ReconnectionStatus

Reconnection states.

```cpp
enum class ReconnectionStatus {
    Reconnecting,
    Reconnected,
    ReconnectionFailed
};
```

---

### ClientError

Error codes returned by client operations.

```cpp
enum class ClientError {
    Success,
    NotConnected,
    InvalidParameter,
    OperationInProgress,
    Timeout,
    NetworkError,
    ProtocolError,
    UnknownError
};
```

---

## Interfaces

### IMqttEnvironment

Interface for platform-specific environment implementations.

**Header**: `#include <cleanMqtt/Interfaces/IMqttEnvironment.h>`

Provides platform-specific implementations for:
- Socket creation
- Threading
- Time management
- Logging

**Implementations:**
- `DefaultWinEnv` - Windows environment
- `DefaultLinuxEnv` - Linux environment

---

### ISocket

Interface for socket implementations.

**Header**: `#include <cleanMqtt/Interfaces/ISocket.h>`

**Implementations:**
- `CleanSocket` - Default TCP socket implementation

---

### IWebSocket

Interface for WebSocket implementations.

**Header**: `#include <cleanMqtt/Interfaces/IWebSocket.h>`

**Implementations:**
- `DefaultWinWebsocket` - Windows WebSocket
- `DefaultLinuxWebsocket` - Linux WebSocket

---

### ICallbackDispatcher

Interface for dispatching callbacks to specific threads.

**Header**: `#include <cleanMqtt/Interfaces/ICallbackDispatcher.h>`

**Implementations:**
- `DefaultDispatcher` - Dispatches to a specific thread
- `ImmediateDispatcher` - Executes callbacks immediately

---

### ILogger

Interface for logging implementations.

**Header**: `#include <cleanMqtt/Interfaces/ILogger.h>`

**Implementation:**
- `DefaultLogger` - Default logging implementation

---

## Utilities

### DefaultEnvironmentFactory

Factory for creating default platform environments.

**Header**: `#include <cleanMqtt/Environments/DefaultEnvironmentFactory.h>`

```cpp
static IMqttEnvironment* create();
```

Creates the appropriate environment for the current platform.

---

### UniqueFunction

A move-only function wrapper (similar to std::function).

**Header**: `#include <cleanMqtt/Utils/UniqueFunction.h>`

Supports Small Buffer Optimization (SBO) to avoid heap allocations for small functors.

```cpp
UniqueFunction<void(int)> func = [](int x) {
    std::cout << x << std::endl;
};

func(42);
```

---

## Usage Patterns

### Synchronous Mode

```cpp
MqttClient client;
// ... setup and connect ...

while (running) {
    client.tick();  // Process events
    std::this_thread::sleep_for(10ms);
}
```

### Asynchronous Mode

```cpp
MqttClientOptions options;
options.asyncTicking = true;
options.tickInterval = Milliseconds(10);

MqttClient client(DefaultEnvironmentFactory::create(), options);
// Client automatically ticks in background thread
// ... setup and connect ...
// No need to call tick()
```

### Error Handling

```cpp
ClientError err = client.connect(args, address);
if (err != ClientError::Success) {
    std::cerr << "Connection failed: " << static_cast<int>(err) << std::endl;
}

// Or use error event
client.onErrorEvent() += [](const ErrorEventArgs& args) {
    std::cerr << "Error: " << args.message << std::endl;
};
```

---

[◀ Back to Documentation Index](README.md) | [Examples ▶](EXAMPLES.md)
