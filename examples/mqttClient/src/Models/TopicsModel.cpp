#include <mqttClient/Model/TopicsModel.h>
#include <kmMqtt/MqttClient.h>
#include <algorithm>

TopicsModel::TopicsModel() noexcept
{
}

TopicsModel::~TopicsModel()
{
    //Event handlers are automatically cleaned up when the MqttClient is destroyed
}

void TopicsModel::setMqttClient(kmMqtt::mqtt::MqttClient* client) noexcept
{
    m_mqttClient = client;
    if (m_mqttClient != nullptr)
    {
        setupEventHandlers();
    }
}

void TopicsModel::subscribe(const std::string& topicFilter)
{
    if (m_mqttClient == nullptr || topicFilter.empty())
    {
        return;
    }

    // Check if already subscribed
    auto it = std::find_if(m_subscribedTopics.begin(), m_subscribedTopics.end(),
        [&topicFilter](const SubscribedTopic& topic) {
            return topic.topicFilter == topicFilter;
        });

    SubscribedTopic topic;

    if (it != m_subscribedTopics.end())
    {
        // Already exists, just try to resubscribe
        it->lastError = "";
        it->isSubscribed = false;
    }
    else
    {
        // Add new topic
        topic = SubscribedTopic(topicFilter);
        m_subscribedTopics.push_back(std::move(topic));
        it = m_subscribedTopics.end() - 1;
    }

    // Create subscription options from UI
    kmMqtt::mqtt::TopicSubscriptionOptions opts;
    opts.qos = static_cast<kmMqtt::mqtt::Qos>(uiData.selectedTopicQos);
    opts.noLocal = uiData.noLocal;
    opts.retainAsPublished = uiData.retainAsPublished;
    opts.retainHandling = uiData.retainHandling ? 
        kmMqtt::mqtt::RetainHandling::SendAtSubscribeIfNew : 
        kmMqtt::mqtt::RetainHandling::SendAtSubscribe;

    it->options = opts;

    // Create Topic and subscribe
    std::vector<kmMqtt::mqtt::Topic> topics;
    topics.emplace_back(topicFilter, opts);

    auto result = m_mqttClient->subscribe(topics, {});
    if (!result.noError())
    {
        it->lastError = result.errorMsg();
    }
}

void TopicsModel::unsubscribe(const std::string& topicFilter)
{
    if (topicFilter.empty())
    {
        return;
    }

    if (m_mqttClient == nullptr)
    {
        m_subscribedTopics.clear();
        return;
    }

    auto it = std::find_if(m_subscribedTopics.begin(),
        m_subscribedTopics.end(),
        [&topicFilter](const SubscribedTopic& topic) 
        {
            return topic.topicFilter == topicFilter;
        });

    if (it == m_subscribedTopics.end())
    {
        return;
    }

    std::vector<kmMqtt::mqtt::Topic> topics;
    topics.emplace_back(topicFilter);

    kmMqtt::mqtt::UnSubscribeOptions unsubscribeOptions;
    
    auto result{ m_mqttClient->unSubscribe(topics, std::move(unsubscribeOptions)) };
    if (!result.noError())
    {
        it->lastError = result.errorMsg();
    }
}

void TopicsModel::unsubscribe(size_t index)
{
    if (index >= m_subscribedTopics.size())
    {
        return;
    }

    unsubscribe(m_subscribedTopics[index].topicFilter);
}

const std::vector<SubscribedTopic>& TopicsModel::getSubscribedTopics() const noexcept
{
    return m_subscribedTopics;
}

bool TopicsModel::isSubscribed(const std::string& topicFilter) const noexcept
{
    auto it = std::find_if(m_subscribedTopics.begin(), m_subscribedTopics.end(),
        [&topicFilter](const SubscribedTopic& topic) {
            return topic.topicFilter == topicFilter && topic.isSubscribed;
        });
    
    return it != m_subscribedTopics.end();
}

void TopicsModel::clearAllTopics()
{
    // Unsubscribe from all topics first
    for (auto& topic : m_subscribedTopics)
    {
        if (topic.isSubscribed)
        {
            unsubscribe(topic.topicFilter);
        }
    }
    
    m_subscribedTopics.clear();
}

void TopicsModel::reset()
{
    m_subscribedTopics.clear();
    uiData = {};
}

void TopicsModel::setupEventHandlers()
{
    if (m_mqttClient == nullptr)
    {
        return;
    }

    //Subscribe acknowledgments
    m_mqttClient->onSubscribeAckEvent().add([this](const kmMqtt::mqtt::SubscribeAckEventDetails& details, const kmMqtt::mqtt::SubscribeAck& ack)
    {
        onSubscribeAck(details, ack);
    });

    //Unsubscribe acknowledgments  
    m_mqttClient->onUnSubscribeAckEvent().add([this](const kmMqtt::mqtt::UnSubscribeAckEventDetails& details, const kmMqtt::mqtt::UnSubscribeAck& ack)
    {
        onUnSubscribeAck(details, ack);
    });
}

void TopicsModel::onSubscribeAck(const kmMqtt::mqtt::SubscribeAckEventDetails& details, const kmMqtt::mqtt::SubscribeAck& ack)
{
	if (details.results.allSubscribedSuccesfully()) //This client only subsribes to one topic at a time so its ok to check allSubscribedSuccesfully
    {
        for (auto& topic : m_subscribedTopics)
        {
            if (topic.topicFilter == details.results.getTopicReasons()[0].topic.topicFilter)
            {
                if (!topic.isSubscribed)
                {
                    topic.isSubscribed = true;
                    topic.lastError = "";
                    break;
                }
            }
        }
    }
    else
    {
        for (auto& topic : m_subscribedTopics)
        {
            if (topic.topicFilter == details.results.getTopicReasons()[0].topic.topicFilter)
            {
                if (!topic.isSubscribed)
                {
                    topic.isSubscribed = false;
					topic.lastError = "Subscription failed with reason code: " + std::to_string(static_cast<int>(details.results.getTopicReasons()[0].reasonCode));
                    break;
                }
            }
        }
    }
}

void TopicsModel::onUnSubscribeAck(const kmMqtt::mqtt::UnSubscribeAckEventDetails& details, const kmMqtt::mqtt::UnSubscribeAck& ack)
{
    if (details.results.allUnSubscribedSuccesfully())
    {
        m_subscribedTopics.erase(
            std::remove_if(m_subscribedTopics.begin(), m_subscribedTopics.end(),
                [&details](const SubscribedTopic& topic) 
                { 
					return details.results.getTopicReasons()[0].topic.topicFilter == topic.topicFilter;
                }),
            m_subscribedTopics.end());
    }
    else
    {
        for (auto& topic : m_subscribedTopics)
        {
            const auto recvReason{ details.results.getTopicReasons()[0].reasonCode };
            if (recvReason == kmMqtt::mqtt::UnSubAckReasonCode::NOT_AUTHORIZED)
            {
                topic.lastError = "Unsubsscription failed: NOT_AUTHORIZED";
				return; // Keep the topic in the list if not authorized
			}

			m_subscribedTopics.erase(
				std::remove_if(m_subscribedTopics.begin(), m_subscribedTopics.end(),
					[&details](const SubscribedTopic& topic)
					{
						return details.results.getTopicReasons()[0].topic.topicFilter == topic.topicFilter;
					}),
				m_subscribedTopics.end());
        }
    }
}