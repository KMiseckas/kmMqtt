# Examples

Practical examples demonstrating common usage patterns with CleanMQTT.

## Table of Contents

- [Basic Examples](#basic-examples)
  - [Simple Publish/Subscribe](#simple-publishsubscribe)
  - [Asynchronous Mode](#asynchronous-mode)
  - [QoS Levels](#qos-levels)
- [Advanced Examples](#advanced-examples)
  - [Last Will and Testament](#last-will-and-testament)
  - [Session Persistence](#session-persistence)
  - [Automatic Reconnection](#automatic-reconnection)
  - [WebSocket Connection](#websocket-connection)
- [Real-World Scenarios](#real-world-scenarios)
  - [IoT Sensor Data Publisher](#iot-sensor-data-publisher)
  - [Command and Control](#command-and-control)
  - [Request-Response Pattern](#request-response-pattern)
- [Error Handling](#error-handling)

---

## Basic Examples

### Simple Publish/Subscribe

A minimal example showing how to connect, subscribe, and publish.

```cpp
#include <cleanMqtt/MqttClient.h>
#include <iostream>
#include <thread>

using namespace cleanMqtt::mqtt;

int main() {
    MqttClient client;
    
    // Handle incoming messages
    client.onPublishEvent() += [](const PublishEventArgs& args) {
        std::cout << "Received: " << args.payload.toString() 
                  << " on " << args.topic << std::endl;
    };
    
    // Connect to broker
    ConnectArgs connectArgs;
    connectArgs.clientId = "simple_client";
    
    ConnectAddress address;
    address.host = "broker.hivemq.com";
    address.port = 1883;
    
    client.connect(std::move(connectArgs), std::move(address));
    
    // Wait for connection
    bool connected = false;
    client.onConnectEvent() += [&](const ConnectEventArgs& args) {
        connected = (args.status == ConnectionStatus::Connected);
    };
    
    while (!connected) {
        client.tick();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Subscribe to topic
    std::vector<Topic> topics = {{"test/topic", QoS::QoS0}};
    client.subscribe(topics, SubscribeOptions{});
    
    // Publish a message
    ByteBuffer payload("Hello, MQTT!");
    PublishOptions pubOptions;
    pubOptions.qos = QoS::QoS0;
    client.publish("test/topic", std::move(payload), std::move(pubOptions));
    
    // Process messages
    for (int i = 0; i < 50; ++i) {
        client.tick();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    client.disconnect();
    client.shutdown();
    
    return 0;
}
```

---

### Asynchronous Mode

Running the client in asynchronous mode with automatic message processing.

```cpp
#include <cleanMqtt/MqttClient.h>
#include <cleanMqtt/Environments/DefaultEnvironmentFactory.h>
#include <iostream>
#include <thread>
#include <chrono>

using namespace cleanMqtt::mqtt;
using namespace cleanMqtt;

int main() {
    // Configure for asynchronous operation
    MqttClientOptions options;
    options.asyncTicking = true;
    options.tickInterval = Milliseconds(10);
    
    // Create client with async options
    MqttClient client(DefaultEnvironmentFactory::create(), options);
    
    // Set up event handlers
    client.onConnectEvent() += [](const ConnectEventArgs& args) {
        if (args.status == ConnectionStatus::Connected) {
            std::cout << "Connected to broker" << std::endl;
        }
    };
    
    client.onPublishEvent() += [](const PublishEventArgs& args) {
        std::cout << "Message: " << args.payload.toString() << std::endl;
    };
    
    // Connect
    ConnectArgs connectArgs;
    connectArgs.clientId = "async_client";
    connectArgs.keepAlive = Seconds(60);
    
    ConnectAddress address;
    address.host = "broker.emqx.io";
    address.port = 1883;
    
    client.connect(std::move(connectArgs), std::move(address));
    
    // No need to call tick() - client processes automatically
    
    // Subscribe
    std::vector<Topic> topics = {{"async/test", QoS::QoS1}};
    client.subscribe(topics, SubscribeOptions{});
    
    // Publish periodically
    for (int i = 0; i < 10; ++i) {
        std::string message = "Message " + std::to_string(i);
        ByteBuffer payload(message);
        
        PublishOptions options;
        options.qos = QoS::QoS1;
        
        client.publish("async/test", std::move(payload), std::move(options));
        
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    
    // Clean shutdown
    client.shutdown();
    
    return 0;
}
```

---

### QoS Levels

Demonstrating different Quality of Service levels.

```cpp
#include <cleanMqtt/MqttClient.h>
#include <iostream>

using namespace cleanMqtt::mqtt;

void demonstrateQoS() {
    MqttClient client;
    
    // Track publish acknowledgments
    client.onPublishAckEvent() += [](const PublishAckEventArgs& args) {
        std::cout << "Message " << args.packetId << " acknowledged" << std::endl;
    };
    
    // Connect to broker
    ConnectArgs connectArgs;
    connectArgs.clientId = "qos_demo";
    
    ConnectAddress address;
    address.host = "test.mosquitto.org";
    address.port = 1883;
    
    client.connect(std::move(connectArgs), std::move(address));
    
    // Wait for connection...
    
    // QoS 0 - Fire and forget (no acknowledgment)
    {
        PublishOptions options;
        options.qos = QoS::QoS0;
        ByteBuffer payload("QoS 0 message");
        client.publish("qos/demo", std::move(payload), std::move(options));
        std::cout << "QoS 0: Sent without waiting for ack" << std::endl;
    }
    
    // QoS 1 - At least once (acknowledged with PUBACK)
    {
        PublishOptions options;
        options.qos = QoS::QoS1;
        ByteBuffer payload("QoS 1 message");
        client.publish("qos/demo", std::move(payload), std::move(options));
        std::cout << "QoS 1: Waiting for PUBACK..." << std::endl;
    }
    
    // QoS 2 - Exactly once (acknowledged with PUBREC, PUBREL, PUBCOMP)
    {
        PublishOptions options;
        options.qos = QoS::QoS2;
        ByteBuffer payload("QoS 2 message");
        client.publish("qos/demo", std::move(payload), std::move(options));
        std::cout << "QoS 2: Four-way handshake for guaranteed delivery" << std::endl;
    }
    
    // Process acknowledgments
    for (int i = 0; i < 100; ++i) {
        client.tick();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    client.shutdown();
}
```

---

## Advanced Examples

### Last Will and Testament

Setting up a Last Will message that's published when the client disconnects unexpectedly.

```cpp
#include <cleanMqtt/MqttClient.h>
#include <iostream>

using namespace cleanMqtt::mqtt;

void setupLastWill() {
    MqttClient client;
    
    ConnectArgs connectArgs;
    connectArgs.clientId = "device_sensor_01";
    connectArgs.keepAlive = Seconds(30);
    connectArgs.cleanSession = false;
    
    // Configure Last Will
    connectArgs.willTopic = "devices/sensor_01/status";
    connectArgs.willPayload = ByteBuffer("offline");
    connectArgs.willQoS = QoS::QoS1;
    connectArgs.willRetain = true;  // Broker retains the status
    
    std::cout << "Last Will configured: " << std::endl;
    std::cout << "  Topic: " << connectArgs.willTopic << std::endl;
    std::cout << "  Message: offline" << std::endl;
    std::cout << "  QoS: 1, Retain: true" << std::endl;
    
    ConnectAddress address;
    address.host = "broker.hivemq.com";
    address.port = 1883;
    
    client.connect(std::move(connectArgs), std::move(address));
    
    // When connected, publish "online" status
    client.onConnectEvent() += [&](const ConnectEventArgs& args) {
        if (args.status == ConnectionStatus::Connected) {
            std::cout << "Connected! Publishing online status..." << std::endl;
            
            PublishOptions options;
            options.qos = QoS::QoS1;
            options.retain = true;
            
            ByteBuffer payload("online");
            client.publish("devices/sensor_01/status", 
                          std::move(payload), 
                          std::move(options));
        }
    };
    
    // Simulate sensor work
    // If connection is lost, broker will publish the Last Will message
    
    for (int i = 0; i < 100; ++i) {
        client.tick();
        
        // Simulate sensor reading every 5 seconds
        if (i % 50 == 0) {
            std::string data = "{ \"temp\": 23.5, \"humidity\": 65 }";
            ByteBuffer payload(data);
            
            PublishOptions options;
            options.qos = QoS::QoS1;
            
            client.publish("devices/sensor_01/data", 
                          std::move(payload), 
                          std::move(options));
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Graceful disconnect (publishes "offline" ourselves)
    PublishOptions options;
    options.qos = QoS::QoS1;
    options.retain = true;
    ByteBuffer payload("offline");
    client.publish("devices/sensor_01/status", 
                  std::move(payload), 
                  std::move(options));
    
    client.disconnect();
    client.shutdown();
}
```

---

### Session Persistence

Using persistent sessions to maintain subscriptions across reconnections.

```cpp
#include <cleanMqtt/MqttClient.h>
#include <iostream>

using namespace cleanMqtt::mqtt;

void persistentSession() {
    MqttClient client;
    
    ConnectArgs connectArgs;
    connectArgs.clientId = "persistent_client_123";  // Must be same across connections
    connectArgs.cleanSession = false;  // Enable session persistence
    connectArgs.keepAlive = Seconds(60);
    
    ConnectAddress address;
    address.host = "broker.emqx.io";
    address.port = 1883;
    
    // On first connect
    client.onConnectEvent() += [&](const ConnectEventArgs& args) {
        if (args.status == ConnectionStatus::Connected) {
            if (args.sessionPresent) {
                std::cout << "Resumed existing session!" << std::endl;
                std::cout << "Previous subscriptions are still active" << std::endl;
            } else {
                std::cout << "New session started" << std::endl;
                
                // Subscribe on new session
                std::vector<Topic> topics = {
                    {"persistent/data", QoS::QoS1},
                    {"persistent/commands", QoS::QoS2}
                };
                
                client.subscribe(topics, SubscribeOptions{});
                std::cout << "Subscribed to topics" << std::endl;
            }
        }
    };
    
    client.onPublishEvent() += [](const PublishEventArgs& args) {
        std::cout << "Received (QoS " << static_cast<int>(args.qos) << "): " 
                  << args.payload.toString() << std::endl;
    };
    
    client.connect(std::move(connectArgs), std::move(address));
    
    // Process messages
    for (int i = 0; i < 200; ++i) {
        client.tick();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    std::cout << "\nDisconnecting..." << std::endl;
    client.disconnect();
    client.shutdown();
    
    std::cout << "\nReconnecting with same client ID..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // Reconnect - session should be preserved
    MqttClient client2;
    
    ConnectArgs reconnectArgs;
    reconnectArgs.clientId = "persistent_client_123";  // Same client ID
    reconnectArgs.cleanSession = false;
    reconnectArgs.keepAlive = Seconds(60);
    
    ConnectAddress address2;
    address2.host = "broker.emqx.io";
    address2.port = 1883;
    
    client2.onConnectEvent() += [](const ConnectEventArgs& args) {
        if (args.sessionPresent) {
            std::cout << "✓ Session resumed! Subscriptions preserved." << std::endl;
        }
    };
    
    client2.connect(std::move(reconnectArgs), std::move(address2));
    
    // Process messages - should receive messages from subscriptions
    for (int i = 0; i < 100; ++i) {
        client2.tick();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    client2.shutdown();
}
```

---

### Automatic Reconnection

Configuring automatic reconnection with custom retry logic.

```cpp
#include <cleanMqtt/MqttClient.h>
#include <cleanMqtt/Environments/DefaultEnvironmentFactory.h>
#include <iostream>

using namespace cleanMqtt::mqtt;
using namespace cleanMqtt;

void autoReconnect() {
    // Configure reconnection options
    MqttClientOptions options;
    options.enableReconnection = true;
    options.reconnectionDelay = Milliseconds(2000);  // 2 seconds between attempts
    options.maxReconnectionAttempts = 5;  // Try 5 times (0 = infinite)
    
    MqttClient client(DefaultEnvironmentFactory::create(), options);
    
    // Track connection state
    client.onConnectEvent() += [](const ConnectEventArgs& args) {
        if (args.status == ConnectionStatus::Connected) {
            std::cout << "✓ Connected to broker" << std::endl;
        } else {
            std::cout << "✗ Connection failed: " << args.returnCode << std::endl;
        }
    };
    
    // Track reconnection attempts
    client.onReconnectEvent() += [](const ReconnectEventArgs& args) {
        if (args.status == ReconnectionStatus::Reconnecting) {
            std::cout << "⟳ Reconnection attempt " << args.attemptCount 
                      << "..." << std::endl;
        } else if (args.status == ReconnectionStatus::Reconnected) {
            std::cout << "✓ Reconnected successfully!" << std::endl;
        } else {
            std::cout << "✗ Reconnection failed after " 
                      << args.attemptCount << " attempts" << std::endl;
        }
    };
    
    client.onDisconnectEvent() += [](const DisconnectEventArgs& args) {
        std::cout << "Disconnected: " << args.reason << std::endl;
    };
    
    // Initial connection
    ConnectArgs connectArgs;
    connectArgs.clientId = "reconnect_demo";
    connectArgs.keepAlive = Seconds(30);
    
    ConnectAddress address;
    address.host = "broker.hivemq.com";
    address.port = 1883;
    
    client.connect(std::move(connectArgs), std::move(address));
    
    // Simulate network disruption scenario
    std::cout << "Running for 60 seconds. Try disconnecting network..." << std::endl;
    
    for (int i = 0; i < 600; ++i) {
        client.tick();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    client.shutdown();
}
```

---

### WebSocket Connection

Connecting to an MQTT broker using WebSocket transport.

```cpp
#include <cleanMqtt/MqttClient.h>
#include <iostream>

using namespace cleanMqtt::mqtt;

void webSocketConnection() {
    MqttClient client;
    
    ConnectArgs connectArgs;
    connectArgs.clientId = "websocket_client";
    connectArgs.keepAlive = Seconds(60);
    
    ConnectAddress address;
    address.host = "broker.hivemq.com";
    address.port = 8000;  // WebSocket port
    address.useWebSocket = true;  // Enable WebSocket transport
    
    std::cout << "Connecting via WebSocket to " 
              << address.host << ":" << address.port << std::endl;
    
    client.onConnectEvent() += [](const ConnectEventArgs& args) {
        if (args.status == ConnectionStatus::Connected) {
            std::cout << "✓ Connected via WebSocket!" << std::endl;
        }
    };
    
    client.connect(std::move(connectArgs), std::move(address));
    
    // Wait for connection
    for (int i = 0; i < 50; ++i) {
        client.tick();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Use WebSocket connection normally
    std::vector<Topic> topics = {{"ws/test", QoS::QoS0}};
    client.subscribe(topics, SubscribeOptions{});
    
    ByteBuffer payload("Hello via WebSocket!");
    PublishOptions options;
    options.qos = QoS::QoS0;
    client.publish("ws/test", std::move(payload), std::move(options));
    
    for (int i = 0; i < 50; ++i) {
        client.tick();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    client.shutdown();
}
```

---

## Real-World Scenarios

### IoT Sensor Data Publisher

Publishing sensor data at regular intervals.

```cpp
#include <cleanMqtt/MqttClient.h>
#include <iostream>
#include <random>
#include <sstream>
#include <iomanip>

using namespace cleanMqtt::mqtt;

struct SensorData {
    double temperature;
    double humidity;
    double pressure;
    
    std::string toJson() const {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2);
        oss << "{"
            << "\"temperature\":" << temperature << ","
            << "\"humidity\":" << humidity << ","
            << "\"pressure\":" << pressure << ","
            << "\"timestamp\":" << std::time(nullptr)
            << "}";
        return oss.str();
    }
};

void iotSensorPublisher() {
    MqttClient client;
    
    const std::string deviceId = "sensor_001";
    const std::string dataTopic = "devices/" + deviceId + "/data";
    const std::string statusTopic = "devices/" + deviceId + "/status";
    
    // Random number generation for simulated sensor data
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> tempDist(18.0, 28.0);
    std::uniform_real_distribution<> humidDist(40.0, 80.0);
    std::uniform_real_distribution<> pressDist(980.0, 1020.0);
    
    // Setup Last Will
    ConnectArgs connectArgs;
    connectArgs.clientId = deviceId;
    connectArgs.keepAlive = Seconds(60);
    connectArgs.cleanSession = false;
    connectArgs.willTopic = statusTopic;
    connectArgs.willPayload = ByteBuffer("{\"status\":\"offline\"}");
    connectArgs.willQoS = QoS::QoS1;
    connectArgs.willRetain = true;
    
    ConnectAddress address;
    address.host = "broker.emqx.io";
    address.port = 1883;
    
    // Publish online status when connected
    client.onConnectEvent() += [&](const ConnectEventArgs& args) {
        if (args.status == ConnectionStatus::Connected) {
            std::cout << "✓ Sensor " << deviceId << " online" << std::endl;
            
            PublishOptions options;
            options.qos = QoS::QoS1;
            options.retain = true;
            
            ByteBuffer status("{\"status\":\"online\"}");
            client.publish(statusTopic.c_str(), 
                          std::move(status), 
                          std::move(options));
        }
    };
    
    client.connect(std::move(connectArgs), std::move(address));
    
    // Wait for connection
    bool connected = false;
    client.onConnectEvent() += [&](const ConnectEventArgs& args) {
        connected = (args.status == ConnectionStatus::Connected);
    };
    
    while (!connected) {
        client.tick();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Publish sensor data every 5 seconds
    std::cout << "Publishing sensor data..." << std::endl;
    
    for (int i = 0; i < 20; ++i) {  // Run for ~100 seconds
        // Read sensor data (simulated)
        SensorData data;
        data.temperature = tempDist(gen);
        data.humidity = humidDist(gen);
        data.pressure = pressDist(gen);
        
        // Convert to JSON and publish
        std::string json = data.toJson();
        ByteBuffer payload(json);
        
        PublishOptions options;
        options.qos = QoS::QoS1;
        options.contentType = "application/json";
        
        std::cout << "Publishing: " << json << std::endl;
        client.publish(dataTopic.c_str(), 
                      std::move(payload), 
                      std::move(options));
        
        // Process for 5 seconds
        for (int j = 0; j < 50; ++j) {
            client.tick();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
    // Graceful shutdown
    PublishOptions options;
    options.qos = QoS::QoS1;
    options.retain = true;
    ByteBuffer status("{\"status\":\"offline\"}");
    client.publish(statusTopic.c_str(), std::move(status), std::move(options));
    
    client.disconnect();
    client.shutdown();
    
    std::cout << "Sensor shutdown complete" << std::endl;
}
```

---

### Command and Control

Subscribing to commands and publishing responses.

```cpp
#include <cleanMqtt/MqttClient.h>
#include <iostream>
#include <string>

using namespace cleanMqtt::mqtt;

void commandAndControl() {
    MqttClient client;
    
    const std::string deviceId = "device_001";
    const std::string commandTopic = "devices/" + deviceId + "/commands";
    const std::string responseTopic = "devices/" + deviceId + "/responses";
    
    // Handle incoming commands
    client.onPublishEvent() += [&](const PublishEventArgs& args) {
        std::string command = args.payload.toString();
        std::cout << "📥 Received command: " << command << std::endl;
        
        std::string response;
        
        if (command == "status") {
            response = "{\"status\":\"ok\",\"uptime\":12345}";
        } else if (command == "reboot") {
            response = "{\"status\":\"rebooting\"}";
            std::cout << "⚠️  Executing reboot..." << std::endl;
        } else if (command.substr(0, 4) == "set:") {
            std::string value = command.substr(4);
            response = "{\"status\":\"set\",\"value\":\"" + value + "\"}";
        } else {
            response = "{\"status\":\"error\",\"message\":\"unknown command\"}";
        }
        
        // Send response
        ByteBuffer respPayload(response);
        PublishOptions options;
        options.qos = QoS::QoS1;
        
        std::cout << "📤 Sending response: " << response << std::endl;
        client.publish(responseTopic.c_str(), 
                      std::move(respPayload), 
                      std::move(options));
    };
    
    // Connect
    ConnectArgs connectArgs;
    connectArgs.clientId = deviceId;
    connectArgs.keepAlive = Seconds(60);
    
    ConnectAddress address;
    address.host = "broker.hivemq.com";
    address.port = 1883;
    
    client.connect(std::move(connectArgs), std::move(address));
    
    // Wait for connection
    bool connected = false;
    client.onConnectEvent() += [&](const ConnectEventArgs& args) {
        connected = (args.status == ConnectionStatus::Connected);
        if (connected) {
            std::cout << "✓ Connected. Subscribing to commands..." << std::endl;
            
            // Subscribe to command topic
            std::vector<Topic> topics = {
                {commandTopic, QoS::QoS1}
            };
            client.subscribe(topics, SubscribeOptions{});
        }
    };
    
    while (!connected) {
        client.tick();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    std::cout << "Waiting for commands..." << std::endl;
    std::cout << "Test by publishing to: " << commandTopic << std::endl;
    
    // Wait for commands
    for (int i = 0; i < 600; ++i) {  // Run for 60 seconds
        client.tick();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    client.shutdown();
}
```

---

### Request-Response Pattern

Implementing request-response with correlation data.

```cpp
#include <cleanMqtt/MqttClient.h>
#include <iostream>
#include <map>
#include <string>

using namespace cleanMqtt::mqtt;

void requestResponsePattern() {
    MqttClient client;
    
    const std::string requestTopic = "service/requests";
    const std::string responseTopic = "service/responses/client_001";
    
    // Track pending requests
    std::map<std::string, std::string> pendingRequests;
    
    // Handle responses
    client.onPublishEvent() += [&](const PublishEventArgs& args) {
        // In MQTT 5.0, correlation data would be in args
        // For now, we'll parse from payload
        std::string response = args.payload.toString();
        std::cout << "Response: " << response << std::endl;
    };
    
    // Connect
    ConnectArgs connectArgs;
    connectArgs.clientId = "requester_001";
    
    ConnectAddress address;
    address.host = "broker.emqx.io";
    address.port = 1883;
    
    client.connect(std::move(connectArgs), std::move(address));
    
    bool connected = false;
    client.onConnectEvent() += [&](const ConnectEventArgs& args) {
        connected = (args.status == ConnectionStatus::Connected);
        if (connected) {
            // Subscribe to response topic
            std::vector<Topic> topics = {{responseTopic, QoS::QoS1}};
            client.subscribe(topics, SubscribeOptions{});
        }
    };
    
    while (!connected) {
        client.tick();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Send request
    std::string requestId = "req_12345";
    std::string request = "{\"id\":\"" + requestId + "\",\"data\":\"get_status\"}";
    
    ByteBuffer payload(request);
    PublishOptions options;
    options.qos = QoS::QoS1;
    options.responseTopic = responseTopic;  // MQTT 5.0 feature
    
    std::cout << "Sending request: " << request << std::endl;
    client.publish(requestTopic.c_str(), std::move(payload), std::move(options));
    
    pendingRequests[requestId] = "waiting";
    
    // Wait for response
    for (int i = 0; i < 100; ++i) {
        client.tick();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    client.shutdown();
}
```

---

## Error Handling

### Comprehensive Error Handling

```cpp
#include <cleanMqtt/MqttClient.h>
#include <iostream>

using namespace cleanMqtt::mqtt;

void errorHandlingExample() {
    MqttClient client;
    
    // Global error handler
    client.onErrorEvent() += [](const ErrorEventArgs& args) {
        std::cerr << "Error: " << args.message 
                  << " (code: " << static_cast<int>(args.error) << ")" 
                  << std::endl;
    };
    
    // Connection-specific error handling
    client.onConnectEvent() += [](const ConnectEventArgs& args) {
        if (args.status != ConnectionStatus::Connected) {
            std::cerr << "Connection failed with return code: " 
                      << static_cast<int>(args.returnCode) << std::endl;
            
            switch (args.returnCode) {
                case 1:
                    std::cerr << "Unacceptable protocol version" << std::endl;
                    break;
                case 2:
                    std::cerr << "Identifier rejected" << std::endl;
                    break;
                case 3:
                    std::cerr << "Server unavailable" << std::endl;
                    break;
                case 4:
                    std::cerr << "Bad username or password" << std::endl;
                    break;
                case 5:
                    std::cerr << "Not authorized" << std::endl;
                    break;
                default:
                    std::cerr << "Unknown error" << std::endl;
            }
        }
    };
    
    // Check return values
    ConnectArgs connectArgs;
    connectArgs.clientId = "error_demo";
    
    ConnectAddress address;
    address.host = "invalid.broker.example.com";  // Invalid host
    address.port = 1883;
    
    ClientError err = client.connect(std::move(connectArgs), std::move(address));
    
    if (err != ClientError::Success) {
        std::cerr << "Failed to initiate connection: " 
                  << static_cast<int>(err) << std::endl;
        return;
    }
    
    // Try to publish before connected
    ByteBuffer payload("test");
    PublishOptions options;
    err = client.publish("test", std::move(payload), std::move(options));
    
    if (err == ClientError::NotConnected) {
        std::cerr << "Cannot publish: not connected to broker" << std::endl;
    }
    
    client.shutdown();
}
```

---

For more examples, check the [examples](../examples/) directory in the repository.

[◀ Back to Documentation Index](README.md) | [Architecture ▶](ARCHITECTURE.md)
