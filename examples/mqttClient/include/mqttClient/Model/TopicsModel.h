#ifndef INCLUDE_MQTTCLIENT_MODEL_TOPICSMODEL_H
#define INCLUDE_MQTTCLIENT_MODEL_TOPICSMODEL_H

#include "mqttClient/Model/ViewModel.h"
#include <kmMqtt/Mqtt/Params/Topic.h>
#include <kmMqtt/Mqtt/Params/SubscribeOptions.h>
#include <kmMqtt/Mqtt/Params/UnSubscribeOptions.h>
#include <kmMqtt/Mqtt/MqttClientEvents.h>
#include <string>
#include <vector>
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
    std::string topicFilter;
    kmMqtt::mqtt::TopicSubscriptionOptions options;
    bool isSubscribed{ false };
    std::string lastError{ "" };
    
    SubscribedTopic() noexcept = default;
    SubscribedTopic(const std::string& filter, const kmMqtt::mqtt::TopicSubscriptionOptions& opts = {}) noexcept
        : topicFilter(filter), options(opts), isSubscribed(false) {}
};

class TopicsModel : public ViewModel
{
public:
    TopicsModel() noexcept;
    ~TopicsModel() override;

    void setMqttClient(kmMqtt::mqtt::MqttClient* client) noexcept;
    
    void subscribe(const std::string& topicFilter);
    void unsubscribe(const std::string& topicFilter);
    void unsubscribe(size_t index);
    
    const std::vector<SubscribedTopic>& getSubscribedTopics() const noexcept;
    bool isSubscribed(const std::string& topicFilter) const noexcept;
    
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
        std::string unsubscribeConfirmationText{ "" };
    } uiData;

private:
    void setupEventHandlers();
    void onSubscribeAck(const kmMqtt::mqtt::SubscribeAckEventDetails& details, const kmMqtt::mqtt::SubscribeAck& ack);
    void onUnSubscribeAck(const kmMqtt::mqtt::UnSubscribeAckEventDetails& details, const kmMqtt::mqtt::UnSubscribeAck& ack);
    
    std::vector<SubscribedTopic> m_subscribedTopics;
    kmMqtt::mqtt::MqttClient* m_mqttClient{ nullptr };
    
    // Store event handler IDs to properly unregister them
    size_t m_subscribeAckHandlerId{ 0 };
    size_t m_unSubscribeAckHandlerId{ 0 };
};

#endif //INCLUDE_MQTTCLIENT_MODEL_TOPICSMODEL_H