# API Reference

This document describes the public API of CleanMQTT.

## Core Classes

### MqttClient

The main client interface for MQTT operations.

**Header**: `<cleanMqtt/MqttClient.h>`

**Namespace**: `cleanMqtt::mqtt`

#### Construction

```cpp
MqttClient();
MqttClient(const IMqttEnvironment* const env, const MqttClientOptions& clientOptions);
```

- Default constructor creates a client with default environment and asynchronous ticking
- Custom constructor allows specifying environment and options

#### Connection Management

```cpp
ReqResult connect(ConnectArgs&& args, ConnectAddress&& address) noexcept;
ReqResult disconnect(DisconnectArgs&& args = {}) noexcept;
ClientError shutdown() noexcept;
```

- `connect()`: Initiates connection to broker. Returns request result indicating success or error
- `disconnect()`: Closes connection to broker with optional disconnect parameters
- `shutdown()`: Releases all resources and stops operations. Safe to call without prior `disconnect()`

#### Publishing

```cpp
ReqResult publish(const char* topic, ByteBuffer&& payload, PublishOptions&& options) noexcept;
```

Publishes a message to the specified topic.

**Parameters**:
- `topic`: Topic name as null-terminated string
- `payload`: Message payload as ByteBuffer
- `options`: Publishing options including QoS, retain flag, and MQTT 5.0 properties

**Returns**: `ReqResult` indicating success or error

#### Subscribing

```cpp
ReqResult subscribe(const std::vector<Topic>& topics, SubscribeOptions&& options) noexcept;
ReqResult unSubscribe(const std::vector<Topic>& topics, UnSubscribeOptions&& options) noexcept;
```

- `subscribe()`: Subscribes to one or more topics
- `unSubscribe()`: Unsubscribes from one or more topics

#### Tick

```cpp
ClientError tick() noexcept;
```

Processes messages and events. Must be called regularly when using `TickMode::SYNC`. Calling this in `TickMode::ASYNC` is an error.

#### Event Accessors

```cpp
ErrorEvent& onErrorEvent() noexcept;
ConnectEvent& onConnectEvent() noexcept;
DisconnectEvent& onDisconnectEvent() noexcept;
ReconnectEvent& onReconnectEvent() noexcept;
PublishEvent& onPublishEvent() noexcept;
PublishCompletedEvent& onPublishCompletedEvent() noexcept;
SubscribeAckEvent& onSubscribeAckEvent() noexcept;
UnSubscribeAckEvent& onUnSubscribeAckEvent() noexcept;
```

Each method returns a reference to an event object that supports `.subscribe(callback)` for registering handlers.

**Event firing behavior**:
- `onConnectEvent()`: Fired on successful connection or initial connection failure
- `onDisconnectEvent()`: Fired on disconnection, excluding initial connection failures
- `onReconnectEvent()`: Fired when reconnection starts or succeeds after unexpected disconnection
- `onPublishEvent()`: Fired when a publish message is received from the broker
- `onPublishCompletedEvent()`: Fired for QoS 1 and 2 acknowledgments (not for QoS 0)

#### Status Query

```cpp
ConnectionStatus getConnectionStatus() const noexcept;
const MqttConnectionInfo& getConnectionInfo() const noexcept;
bool getIsTickAsync() const noexcept;
```

- `getConnectionStatus()`: Returns current connection state
- `getConnectionInfo()`: Returns connection details from CONNACK
- `getIsTickAsync()`: Returns true if client is in asynchronous tick mode

---

## Configuration Types

### MqttClientOptions

Configuration for client behavior.

**Header**: `<cleanMqtt/MqttClientOptions.h>`

```cpp
MqttClientOptions(TickMode mode = TickMode::ASYNC);

MqttClientOptions& tickMode(TickMode mode);
MqttClientOptions& callbackDispatcher(const std::shared_ptr<ICallbackDispatcher>& callbackDispatcher);
```

**Tick Modes**:
- `TickMode::ASYNC`: Library manages its own thread for message processing (default)
- `TickMode::SYNC`: Application must call `tick()` regularly

**Callback Dispatching**:
- In `ASYNC` mode, callbacks use `ImmediateDispatcher` by default
- In `SYNC` mode, callbacks use `DefaultDispatcher` and are invoked during `tick()`

### Config

Global configuration parameters.

**Header**: `<cleanMqtt/Config.h>`

```cpp
struct Config {
    std::uint32_t connectTimeOutMS{ 15000U };
    std::uint32_t pingTimeOutMS{ 10000U };
    bool pingAlways{ true };
    std::uint32_t defaultPingInterval{ 15000U };
    std::uint32_t retryPublishIntervalMS{ 10000U };
    std::uint32_t tickAsyncWaitForMS{ 50U };
};
```

- `connectTimeOutMS`: Connection timeout duration
- `pingTimeOutMS`: Ping response timeout
- `pingAlways`: Send pings regardless of other traffic
- `defaultPingInterval`: Ping interval when keep-alive is 0
- `retryPublishIntervalMS`: Retry interval for unacknowledged publishes (0 = no retry)
- `tickAsyncWaitForMS`: Maximum wait time in async tick thread

### ConnectArgs

Connection parameters for CONNECT packet.

**Header**: `<cleanMqtt/Mqtt/Params/ConnectArgs.h>`

**Key methods**:
```cpp
ConnectArgs& setVersion(MqttVersion version);
ConnectArgs& setClientId(const std::string& clientId);
ConnectArgs& setCleanStart(bool cleanStart);
ConnectArgs& setKeepAliveSeconds(std::uint16_t seconds);
ConnectArgs& setUserName(const std::string& userName);
ConnectArgs& setPassword(const BinaryData& password);
ConnectArgs& setWill(Will&& will);
ConnectArgs& setSessionExpiryInterval(std::uint32_t seconds);  // MQTT 5.0
ConnectArgs& setReceiveMaximum(std::uint16_t max);             // MQTT 5.0
```

Supports MQTT 3.1.1 and 5.0 features including will messages, authentication, and session properties.

### ConnectAddress

Broker address and transport configuration.

**Header**: `<cleanMqtt/Mqtt/Params/ConnectAddress.h>`

```cpp
struct ConnectAddress {
    Address primaryAddress;
    std::vector<Address> otherAddresses;
};

// Address factory methods
Address Address::createIp4(const char* scheme, const char* ip, const char* port, const char* path);
Address Address::createIp6(const char* scheme, const char* ip, const char* port, const char* path);
Address Address::createURL(const char* scheme, const char* hostname, const char* port, const char* path);
```

**LocatorType values**:
- `UNKNOWN`: Unspecified type
- `IP4`: IPv4 address
- `IP6`: IPv6 address
- `HOSTNAME`: Hostname/domain

**Scheme examples**:
- `"mqtt"`: MQTT over TCP
- `"ws"`: MQTT over WebSocket (requires `BUILD_IXWEBSOCKET=ON`)
- `"wss"`: MQTT over secure WebSocket (requires `BUILD_IXWEBSOCKET=ON` with OpenSSL)

### PublishOptions

Options for publishing messages.

**Header**: `<cleanMqtt/Mqtt/Params/PublishOptions.h>`

**Fields**:
```cpp
Qos qos{ Qos::QOS_0 };
bool retain{ false };
PayloadFormatIndicator payloadFormatIndicator{ PayloadFormatIndicator::BINARY };
std::uint32_t messageExpiryInterval{ 0 };
bool addMessageExpiryInterval{ false };
std::uint16_t topicAlias{ 0 };
std::string responseTopic;
std::unique_ptr<BinaryData> correlationData;
std::map<std::string, std::string> userProperties;
```

MQTT 5.0 properties (topic alias, correlation data, etc.) are ignored when using MQTT 3.1.1.

### SubscribeOptions / UnSubscribeOptions

Options for subscription operations.

**Headers**:
- `<cleanMqtt/Mqtt/Params/SubscribeOptions.h>`
- `<cleanMqtt/Mqtt/Params/UnSubscribeOptions.h>`

Both support MQTT 5.0 user properties.

---

## Data Types

### ByteBuffer

Fixed-capacity byte buffer with optional small buffer optimization.

**Header**: `<cleanMqtt/ByteBuffer.h>`

```cpp
ByteBuffer(std::size_t capacity);

void append(std::uint8_t byte);
void append(const std::uint8_t* bytes, std::size_t length);
void append(const char* str);
void appendBE(std::uint16_t value);  // Big-endian 16-bit
void appendBE(std::uint32_t value);  // Big-endian 32-bit

std::uint8_t read();
std::uint16_t readBE16();
std::uint32_t readBE32();

std::uint8_t* bytes();
const std::uint8_t* bytes() const;
std::size_t size() const;
std::size_t capacity() const;
std::size_t headroom() const;

void resetReadCursor();
void resetSize();
```

- Capacity is fixed at construction and cannot be changed
- SBO uses stack memory for small buffers (configurable via `BYTEBUFFER_SBO_MAX_SIZE`)
- Read operations advance an internal cursor
- Append operations throw `std::out_of_range` if headroom is insufficient

### Topic

Represents a subscription topic with QoS and subscription options.

**Header**: `<cleanMqtt/Mqtt/Params/Topic.h>`

```cpp
Topic(std::string filter, TopicSubscriptionOptions opts = {});
```

**TopicSubscriptionOptions**:
```cpp
struct TopicSubscriptionOptions {
    Qos qos{ Qos::QOS_0 };
    bool noLocal{ false };
    bool retainAsPublished{ false };
    RetainHandling retainHandling{ RetainHandling::SendAtSubscribe };
};
```

**RetainHandling enum** (MQTT 5.0):
- `SendAtSubscribe`: Send retained messages at subscription time
- `SendAtSubscribeIfNew`: Send retained messages only for new subscriptions
- `DoNotSend`: Do not send retained messages

### Will

Last will and testament message configuration.

**Header**: `<cleanMqtt/Mqtt/Params/ConnectArgs.h>`

```cpp
Will(const std::string& topic);

Qos willQos{ Qos::QOS_0 };
bool retainWillMessage{ false };
std::uint32_t willDelayInterval{ 0 };
std::uint32_t messageExpiryInterval{ 0 };
std::string contentType;
std::string responseTopic;
std::unique_ptr<BinaryData> correlationData;
std::string willTopic;
PayloadFormatIndicator payloadFormat{ PayloadFormatIndicator::BINARY };
std::unique_ptr<BinaryData> payload;
std::map<std::string, std::string> userProperties;
```

---

## Enumerations

### ConnectionStatus

**Header**: `<cleanMqtt/Mqtt/Enums/ConnectionStatus.h>`

```cpp
enum class ConnectionStatus : std::uint8_t {
    DISCONNECTED,
    CONNECTED,
    CONNECTING,
    RECONNECT,
    RECONNECTING
};
```

### Qos

**Header**: `<cleanMqtt/Mqtt/Enums/Qos.h>`

```cpp
enum class Qos : std::uint8_t {
    QOS_0 = 0U,  // At most once
    QOS_1 = 1U,  // At least once
    QOS_2 = 2U   // Exactly once
};
```

### MqttVersion

**Header**: `<cleanMqtt/Mqtt/Enums/MqttVersion.h>`

```cpp
enum class MqttVersion : std::uint8_t {
    MQTT_3_1_1 = 4U,
    MQTT_5_0 = 5U
};
```

### LocatorType

**Header**: `<cleanMqtt/Mqtt/Enums/LocatorType.h>`

```cpp
enum class LocatorType : std::uint8_t {
    UNKNOWN = 0U,
    IP4,
    IP6,
    HOSTNAME
};
```

Indicates the type of network address (IPv4, IPv6, or hostname).

### PayloadFormatIndicator

**Header**: `<cleanMqtt/Mqtt/Enums/PayloadFormatIndicator.h>`

```cpp
enum class PayloadFormatIndicator : std::uint8_t {
    BINARY = 0,
    UTF8 = 1
};
```

Indicates whether message payload is binary data or UTF-8 encoded text.

---

## Result Types

### ReqResult

Result type for client operation requests.

**Header**: `<cleanMqtt/Mqtt/ReqResult.h>`

```cpp
bool isError() const;
bool isSuccess() const;
ClientError getError() const;
```

### ClientError

Error information from client operations.

**Header**: `<cleanMqtt/Mqtt/ClientError.h>`

Contains error code and descriptive message. Check `ClientErrorCode` enum for specific error types.

### MqttConnectionInfo

Connection details received in CONNACK.

**Header**: `<cleanMqtt/Mqtt/MqttConnectionInfo.h>`

Provides broker-assigned session information and connection properties for MQTT 5.0.

---

## Packet Types

The library exposes packet structures for reading received messages. Key packet types:

- `Publish`: Incoming publish message
- `ConnectAck`: Connection acknowledgment
- `SubscribeAck`: Subscription acknowledgment
- `UnSubscribeAck`: Unsubscription acknowledgment

Access packet data through getter methods following MQTT specification structure (fixed header, variable header, payload).

---

## Extension Points

### IMqttEnvironment

Interface for platform-specific functionality.

**Header**: `<cleanMqtt/Interfaces/IMqttEnvironment.h>`

Implement this interface to provide custom socket, threading, and timing implementations. Default implementations are provided via `DefaultEnvironmentFactory`.

### ICallbackDispatcher

Interface for controlling callback execution.

**Header**: `<cleanMqtt/Interfaces/ICallbackDispatcher.h>`

Allows custom callback dispatching strategies. Built-in implementations:
- `ImmediateDispatcher`: Invokes callbacks immediately (used in ASYNC mode)
- `DefaultDispatcher`: Defers callbacks until `tick()` is called (used in SYNC mode)

### IWebSocket

Interface for WebSocket transport implementations.

**Header**: `<cleanMqtt/Interfaces/IWebSocket.h>`

`DefaultWebsocket` implementation uses IXWebSocket and is available when `BUILD_IXWEBSOCKET=ON`.

---

## Notes

- All operations returning `ReqResult` or `ClientError` are marked `noexcept`
- MQTT 5.0 specific properties are ignored when connected with MQTT 3.1.1
- Session persistence to disk is not implemented; session state exists in memory only
- Topic aliases are tracked per connection and reset on reconnection
