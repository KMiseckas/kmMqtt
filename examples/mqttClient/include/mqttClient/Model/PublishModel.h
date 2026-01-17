#ifndef INCLUDE_MQTTCLIENT_MODEL_PUBLISHMODEL_H
#define INCLUDE_MQTTCLIENT_MODEL_PUBLISHMODEL_H

#include "mqttClient/Model/ViewModel.h"
#include <kmMqtt/Mqtt/Params/PublishOptions.h>
#include <kmMqtt/ByteBuffer.h>
#include <kmMqtt/Mqtt/MqttClientEvents.h>
#include <string>
#include <vector>
#include <memory>
#include <chrono>

namespace kmMqtt
{
    namespace mqtt
    {
        class MqttClient;
    }
}

enum class PublishMessageStatus
{
    PENDING,        // Waiting to be sent
    SENT,           // Sent to broker (QoS 0) or waiting for ack (QoS 1/2)
    ACKNOWLEDGED,   // Received PUBACK/PUBCOMP
    FAILED          // Error occurred
};

struct PublishedMessage
{
    std::string topic;
    std::string payload;
    kmMqtt::mqtt::PublishOptions options;
    PublishMessageStatus status{ PublishMessageStatus::PENDING };
    std::string lastError{ "" };
    std::chrono::system_clock::time_point timestamp;
    
    PublishedMessage() noexcept = default;
    PublishedMessage(const std::string& topic_, const std::string& payload_, const kmMqtt::mqtt::PublishOptions& opts = {}) noexcept
        : topic(topic_), payload(payload_), options(opts), timestamp(std::chrono::system_clock::now()) {}
};

class PublishModel : public ViewModel
{
public:
    PublishModel() noexcept;
    ~PublishModel() override;

    void setMqttClient(kmMqtt::mqtt::MqttClient* client) noexcept;
    
    void publish(const std::string& topic, const std::string& payload);
    void clearAllMessages();
    void removeMessage(size_t index);
    
    const std::vector<PublishedMessage>& getPublishedMessages() const noexcept;
    size_t getPendingCount() const noexcept;
    size_t getSuccessCount() const noexcept;
    size_t getFailedCount() const noexcept;

    struct UIData
    {
        char topicBuffer[512]{ "" };
        char payloadBuffer[16384]{ "" };
        int selectedQos{ 0 };
        bool retain{ false };
        bool showAdvancedOptions{ false };
        
        // Advanced options
        char responseTopicBuffer[512]{ "" };
        int topicAlias{ 0 };
        int messageExpiryInterval{ 0 };
        bool addMessageExpiryInterval{ false };
        int payloadFormatIndicator{ 0 }; // 0 = UTF8, 1 = Binary
        char correlationDataBuffer[1024]{ "" };
        
        // UI state
        int selectedMessageIndex{ -1 };
        int expandedMessageIndex{ -1 };
        bool showMessageDetails{ false };
        bool showClearConfirmation{ false };
        
        // Filter options
        int statusFilter{ 0 }; // 0 = All, 1 = Pending, 2 = Sent, 3 = Acknowledged, 4 = Failed
    } uiData;

private:
    void setupEventHandlers();
    void onPublishCompleted(const kmMqtt::mqtt::PublishCompleteEventDetails& details);
    void updateMessageStatus(std::uint16_t packetId, PublishMessageStatus status, const std::string& error = "");
    
    std::vector<PublishedMessage> m_publishedMessages;
    kmMqtt::mqtt::MqttClient* m_mqttClient{ nullptr };
    
    // Store event handler IDs to properly unregister them
    size_t m_publishAckHandlerId{ 0 };
};

#endif //INCLUDE_MQTTCLIENT_MODEL_PUBLISHMODEL_H