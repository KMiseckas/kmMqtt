// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_MQTTCLIENT_MODEL_TOPICSMODEL_H
#define INCLUDE_MQTTCLIENT_MODEL_TOPICSMODEL_H

#include "mqttClient/Model/ViewModel.h"
#include <kmMqtt/Mqtt/Params/Topic.h>
#include <kmMqtt/Mqtt/Params/SubscribeOptions.h>
#include <kmMqtt/Mqtt/Params/UnSubscribeOptions.h>
#include <kmMqtt/Mqtt/MqttClientEvents.h>
#include <kmMqtt/STL/KmString.h>
#include <kmMqtt/STL/KmVector.h>
#include <memory>

namespace kmMqtt
{
    namespace mqtt
    {
        class MqttClient;
    }
}

struct SubscribedTopic
{
    kmMqtt::kmStd::string topicFilter;
    kmMqtt::mqtt::TopicSubscriptionOptions options;
    bool isSubscribed{ false };
    kmMqtt::kmStd::string lastError{ "" };
    
    SubscribedTopic() noexcept = default;
    SubscribedTopic(const kmMqtt::kmStd::string& filter, const kmMqtt::mqtt::TopicSubscriptionOptions& opts = {}) noexcept
        : topicFilter(filter), options(opts), isSubscribed(false) {}
};

class TopicsModel : public ViewModel
{
public:
    TopicsModel() noexcept;
    ~TopicsModel() override;

    void setMqttClient(kmMqtt::mqtt::MqttClient* client) noexcept;
    
    void subscribe(const kmMqtt::kmStd::string& topicFilter);
    void unsubscribe(const kmMqtt::kmStd::string& topicFilter);
    void unsubscribe(size_t index);
    
    const kmMqtt::kmStd::vector<SubscribedTopic>& getSubscribedTopics() const noexcept;
    bool isSubscribed(const kmMqtt::kmStd::string& topicFilter) const noexcept;
    
    void clearAllTopics();
    void reset();

    struct UIData
    {
        char newTopicBuffer[512]{ "" };
        int selectedTopicQos{ 0 };
        bool retainHandling{ false };
        bool noLocal{ false };
        bool retainAsPublished{ false };
        
        int selectedTopicIndex{ -1 };
        bool showSubscribeOptions{ false };
        
        bool addingNewTopic{ false };
        int expandedTopicIndex{ -1 };
        bool showUnsubscribeConfirmation{ false };
        int topicToUnsubscribe{ -1 };
        kmMqtt::kmStd::string unsubscribeConfirmationText{ "" };
    } uiData;

private:
    void setupEventHandlers();
    void onSubscribeAck(const kmMqtt::mqtt::SubscribeAckEventDetails& details, const kmMqtt::mqtt::SubscribeAck& ack);
    void onUnSubscribeAck(const kmMqtt::mqtt::UnSubscribeAckEventDetails& details, const kmMqtt::mqtt::UnSubscribeAck& ack);
    
    kmMqtt::kmStd::vector<SubscribedTopic> m_subscribedTopics;
    kmMqtt::mqtt::MqttClient* m_mqttClient{ nullptr };
    
    // Store event handler IDs to properly unregister them
    size_t m_subscribeAckHandlerId{ 0 };
    size_t m_unSubscribeAckHandlerId{ 0 };
};

#endif //INCLUDE_MQTTCLIENT_MODEL_TOPICSMODEL_H