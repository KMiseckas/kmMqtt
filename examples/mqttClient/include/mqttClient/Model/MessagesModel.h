// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_MQTTCLIENT_MODEL_MESSAGESMODEL_H
#define INCLUDE_MQTTCLIENT_MODEL_MESSAGESMODEL_H

#include "mqttClient/Model/ViewModel.h"
#include <kmMqtt/Mqtt/Params/PublishOptions.h>
#include <kmMqtt/STL/KmString.h>
#include <kmMqtt/STL/KmVector.h>
#include <memory>
#include <chrono>

namespace kmMqtt
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
    kmMqtt::kmStd::string topic;
    kmMqtt::kmStd::string payload;
    MessageType type;
    std::chrono::system_clock::time_point timestamp;

    kmMqtt::mqtt::Qos receivedQos{ kmMqtt::mqtt::Qos::QOS_0 };
    bool receivedRetain{ false };

    SentMessageStatus sentStatus{ SentMessageStatus::PENDING };
    kmMqtt::mqtt::PublishOptions sentOptions;
    kmMqtt::kmStd::string lastError{ "" };

    MqttMessage() noexcept = default;

    MqttMessage(const kmMqtt::kmStd::string& topic_, const kmMqtt::kmStd::string& payload_, kmMqtt::mqtt::Qos qos, bool retain) noexcept
        : topic(topic_),
        payload(payload_),
        type(MessageType::RECEIVED),
        timestamp(std::chrono::system_clock::now()),
        receivedQos(qos),
        receivedRetain(retain) {
    }

    MqttMessage(const kmMqtt::kmStd::string& topic_, const kmMqtt::kmStd::string& payload_, const kmMqtt::mqtt::PublishOptions& options) noexcept
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

    void setMqttClient(kmMqtt::mqtt::MqttClient* client) noexcept;

    void addReceivedMessage(const kmMqtt::kmStd::string& topic,
        const kmMqtt::kmStd::string& payload,
        kmMqtt::mqtt::Qos qos,
        bool retain);

    void addSentMessage(const kmMqtt::kmStd::string& topic,
        const kmMqtt::kmStd::string& payload,
        const kmMqtt::mqtt::PublishOptions opts,
        bool isSuccessRequest,
        const kmMqtt::kmStd::string& errMsg = "");

    void clearAllMessages();
    void removeMessage(size_t index);

    const kmMqtt::kmStd::vector<MqttMessage>& getAllMessages() const noexcept;

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
    void onPublishReceived(const kmMqtt::mqtt::PublishEventDetails& details, const kmMqtt::mqtt::Publish& publish);
    void updateSentMessageStatus(const kmMqtt::kmStd::string& topic, SentMessageStatus status, const kmMqtt::kmStd::string& error = "");

    kmMqtt::kmStd::vector<MqttMessage> m_messages;
    kmMqtt::mqtt::MqttClient* m_mqttClient{ nullptr };
};

#endif //INCLUDE_MQTTCLIENT_MODEL_MESSAGESMODEL_H