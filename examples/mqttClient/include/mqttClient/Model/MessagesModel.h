#ifndef INCLUDE_MQTTCLIENT_MODEL_MESSAGESMODEL_H
#define INCLUDE_MQTTCLIENT_MODEL_MESSAGESMODEL_H

#include "mqttClient/Model/ViewModel.h"
#include <cleanMqtt/Mqtt/Params/PublishOptions.h>
#include <string>
#include <vector>
#include <memory>
#include <chrono>

namespace cleanMqtt
{
    namespace mqtt
    {
        class MqttClient;
        struct PublishEventDetails;
        class Publish;
    }
}

enum class MessageType
{
    RECEIVED,
    SENT
};

enum class SentMessageStatus
{
    PENDING,
    ACKNOWLEDGED, 
    FAILED
};

struct MqttMessage
{
    std::string topic;
    std::string payload;
    MessageType type;
    std::chrono::system_clock::time_point timestamp;

    cleanMqtt::mqtt::Qos receivedQos{ cleanMqtt::mqtt::Qos::QOS_0 };
    bool receivedRetain{ false };

    SentMessageStatus sentStatus{ SentMessageStatus::PENDING };
    cleanMqtt::mqtt::PublishOptions sentOptions;
    std::string lastError{ "" };

    MqttMessage() noexcept = default;

    MqttMessage(const std::string& topic_, const std::string& payload_, cleanMqtt::mqtt::Qos qos, bool retain) noexcept
        : topic(topic_),
        payload(payload_),
        type(MessageType::RECEIVED),
        timestamp(std::chrono::system_clock::now()),
        receivedQos(qos),
        receivedRetain(retain) {
    }

    MqttMessage(const std::string& topic_, const std::string& payload_, const cleanMqtt::mqtt::PublishOptions& options) noexcept
        : topic(topic_),
        payload(payload_),
        type(MessageType::SENT),
        timestamp(std::chrono::system_clock::now()),
        sentStatus(SentMessageStatus::PENDING),
        sentOptions(options) {
    }
};

class MessagesModel : public ViewModel
{
public:
    MessagesModel() noexcept;
    ~MessagesModel() override;

    void setMqttClient(cleanMqtt::mqtt::MqttClient* client) noexcept;

    void addReceivedMessage(const std::string& topic,
        const std::string& payload,
        cleanMqtt::mqtt::Qos qos,
        bool retain);

    void addSentMessage(const std::string& topic,
        const std::string& payload,
        const cleanMqtt::mqtt::PublishOptions opts,
        bool isSuccessRequest,
        const std::string& errMsg = "");

    void clearAllMessages();
    void removeMessage(size_t index);

    const std::vector<MqttMessage>& getAllMessages() const noexcept;

    size_t getReceivedCount() const noexcept;
    size_t getSentCount() const noexcept;
    size_t getPendingCount() const noexcept;
    size_t getFailedCount() const noexcept;

    struct UIData
    {
        int messageFilter{ 0 };
        char topicFilterBuffer[512]{ "" };

        int selectedMessageIndex{ -1 };
        bool showMessageDetails{ false };
        bool showClearConfirmation{ false };

        int maxDisplayedMessages{ 100 };
        int messageViewMode{ 0 };
    } uiData;

private:
    void setupEventHandlers();
    void onPublishReceived(const cleanMqtt::mqtt::PublishEventDetails& details, const cleanMqtt::mqtt::Publish& publish);
    void updateSentMessageStatus(const std::string& topic, SentMessageStatus status, const std::string& error = "");

    std::vector<MqttMessage> m_messages;
    cleanMqtt::mqtt::MqttClient* m_mqttClient{ nullptr };
};

#endif //INCLUDE_MQTTCLIENT_MODEL_MESSAGESMODEL_H