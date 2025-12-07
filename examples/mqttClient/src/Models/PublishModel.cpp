#include <mqttClient/Model/PublishModel.h>
#include <mqttClient/Events/EventService.h>
#include <cleanMqtt/MqttClient.h>
#include <cleanMqtt/Mqtt/Packets/DataTypes.h>
#include <algorithm>
#include <mqttClient/Events/ApplicationEvents.h>

PublishModel::PublishModel() noexcept
{
}

PublishModel::~PublishModel()
{
    // Event handlers are automatically cleaned up when the MqttClient is destroyed
}

void PublishModel::setMqttClient(cleanMqtt::mqtt::MqttClient* client) noexcept
{
    m_mqttClient = client;
    if (m_mqttClient != nullptr)
    {
        setupEventHandlers();
    }
}

void PublishModel::publish(const std::string& topic, const std::string& payload)
{
    if (m_mqttClient == nullptr || topic.empty())
    {
        return;
    }

    // Create publish options from UI
    cleanMqtt::mqtt::PublishOptions opts;
    opts.qos = static_cast<cleanMqtt::mqtt::Qos>(uiData.selectedQos);
    opts.retain = uiData.retain;
    opts.payloadFormatIndicator = uiData.payloadFormatIndicator == 0 ? 
        cleanMqtt::mqtt::PayloadFormatIndicator::UTF8 : 
        cleanMqtt::mqtt::PayloadFormatIndicator::BINARY;
    
    // Advanced options
    if (strlen(uiData.responseTopicBuffer) > 0)
    {
        opts.responseTopic = uiData.responseTopicBuffer;
    }
    
    opts.topicAlias = static_cast<std::uint16_t>(uiData.topicAlias);
    opts.messageExpiryInterval = static_cast<std::uint32_t>(uiData.messageExpiryInterval);
    opts.addMessageExpiryInterval = uiData.addMessageExpiryInterval;
    
    if (strlen(uiData.correlationDataBuffer) > 0)
    {
        opts.correlationData = std::make_unique<cleanMqtt::mqtt::BinaryData>(
            static_cast<std::uint16_t>(strlen(uiData.correlationDataBuffer)),
            reinterpret_cast<const std::uint8_t*>(uiData.correlationDataBuffer));
    }

    // Create published message entry (no longer using packetId for tracking)
    PublishedMessage message(topic, payload, opts);
    
    // Add to our tracking list
    m_publishedMessages.push_back(message);
    auto& addedMessage = m_publishedMessages.back();

    // Create ByteBuffer from payload
    cleanMqtt::ByteBuffer payloadBuffer(payload.length());
    for (char c : payload)
    {
        payloadBuffer += static_cast<std::uint8_t>(c);
    }

    // Publish the message
    auto result = m_mqttClient->publish(topic.c_str(), std::move(payloadBuffer), std::move(opts));
    
    if (!result.noError())
    {
        addedMessage.status = PublishMessageStatus::FAILED;
        addedMessage.lastError = result.errorMsg();
    }
    else
    {
        // For QoS 0, consider it immediately sent/acknowledged
        if (uiData.selectedQos == 0)
        {
            addedMessage.status = PublishMessageStatus::ACKNOWLEDGED;
        }
        else
        {
            addedMessage.status = PublishMessageStatus::SENT;
        }
    }

    {
        events::PublishMessageEvent e;
        e.result = { result.noError(), result.errorMsg()};
        e.topic = topic;
        e.message = payload;
        e.options = message.options;

        events::publish<events::PublishMessageEvent>(e);
    }
}

void PublishModel::clearAllMessages()
{
    m_publishedMessages.clear();
}

void PublishModel::removeMessage(size_t index)
{
    if (index < m_publishedMessages.size())
    {
        m_publishedMessages.erase(m_publishedMessages.begin() + index);
    }
}

const std::vector<PublishedMessage>& PublishModel::getPublishedMessages() const noexcept
{
    return m_publishedMessages;
}

size_t PublishModel::getPendingCount() const noexcept
{
    return std::count_if(m_publishedMessages.begin(), m_publishedMessages.end(),
        [](const PublishedMessage& msg) { return msg.status == PublishMessageStatus::PENDING || msg.status == PublishMessageStatus::SENT; });
}

size_t PublishModel::getSuccessCount() const noexcept
{
    return std::count_if(m_publishedMessages.begin(), m_publishedMessages.end(),
        [](const PublishedMessage& msg) { return msg.status == PublishMessageStatus::ACKNOWLEDGED; });
}

size_t PublishModel::getFailedCount() const noexcept
{
    return std::count_if(m_publishedMessages.begin(), m_publishedMessages.end(),
        [](const PublishedMessage& msg) { return msg.status == PublishMessageStatus::FAILED; });
}

void PublishModel::setupEventHandlers()
{
    if (m_mqttClient == nullptr)
    {
        return;
    }

    // Register for publish acknowledgments
    m_mqttClient->onPublishCompletedEvent().add([this](const cleanMqtt::mqtt::PublishCompleteEventDetails& details)
    {
        onPublishCompleted(details);
    });
}

void PublishModel::onPublishCompleted(const cleanMqtt::mqtt::PublishCompleteEventDetails& details)
{
    // Use FIFO approach - find the first SENT message and mark it as acknowledged
    for (auto& message : m_publishedMessages)
    {
        if (message.status == PublishMessageStatus::SENT)
        {
            if (details.isSuccess())
            {
                if (details.packetType == cleanMqtt::mqtt::PacketType::PUBLISH_ACKNOWLEDGE)
                {
                    message.status = PublishMessageStatus::ACKNOWLEDGED;
                }

                message.lastError = "";
            }
            else
            {
                message.status = PublishMessageStatus::FAILED;
                message.lastError = "Pub Ack Reason Code: " + static_cast<int>(details.reasonCode);
            }
            break; // Only update the first SENT message (FIFO)
        }
    }
}

void PublishModel::updateMessageStatus(std::uint16_t packetId, PublishMessageStatus status, const std::string& error)
{
    // This method is no longer used since we're not tracking packet IDs
    // Keeping it for potential future use when packet IDs are exposed
    (void)packetId;
    (void)status;
    (void)error;
}