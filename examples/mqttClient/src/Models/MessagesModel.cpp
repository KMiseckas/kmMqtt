#include <mqttClient/Model/MessagesModel.h>
#include <cleanMqtt/MqttClient.h>
#include <cleanMqtt/Mqtt/Packets/Publish/Publish.h>
#include <mqttClient/Events/EventService.h>
#include <algorithm>
#include <mqttClient/Events/ApplicationEvents.h>

MessagesModel::MessagesModel() noexcept
{
    events::subscribe<events::PublishMessageEvent>([this](const events::PublishMessageEvent& event)
        {
            addSentMessage(event.topic, event.message, event.options, event.result.success, event.result.errorMessage);
        });
}

MessagesModel::~MessagesModel()
{
}

void MessagesModel::setMqttClient(cleanMqtt::mqtt::MqttClient* client) noexcept
{
    m_mqttClient = client;
    if (m_mqttClient != nullptr)
    {
        setupEventHandlers();
    }
}

void MessagesModel::addReceivedMessage(const std::string& topic,
    const std::string& payload,
    cleanMqtt::mqtt::Qos qos,
    bool retain)
{
    MqttMessage message(topic, payload, qos, retain);
    m_messages.push_back(message);

    if (m_messages.size() > static_cast<size_t>(uiData.maxDisplayedMessages))
    {
        m_messages.erase(m_messages.begin(), m_messages.begin() + 1);
    }
}

void MessagesModel::addSentMessage(const std::string& topic,
    const std::string& payload,
    const cleanMqtt::mqtt::PublishOptions opts,
    bool isSuccessRequest,
    const std::string& errMsg)
{
    MqttMessage message(topic, payload, opts);

    if (opts.qos == cleanMqtt::mqtt::Qos::QOS_0)
    {
        message.sentStatus = isSuccessRequest ? SentMessageStatus::ACKNOWLEDGED : SentMessageStatus::FAILED;
    }
    else if (!isSuccessRequest)
    {
        message.sentStatus = SentMessageStatus::FAILED;
        message.lastError = errMsg;
    }

    m_messages.push_back(message);

    if (m_messages.size() > static_cast<size_t>(uiData.maxDisplayedMessages))
    {
        m_messages.erase(m_messages.begin(), m_messages.begin() + 1);
    }
}

void MessagesModel::clearAllMessages()
{
    m_messages.clear();
}

void MessagesModel::removeMessage(size_t index)
{
    if (index < m_messages.size())
    {
        m_messages.erase(m_messages.begin() + index);
    }
}

const std::vector<MqttMessage>& MessagesModel::getAllMessages() const noexcept
{
    return m_messages;
}

size_t MessagesModel::getReceivedCount() const noexcept
{
    return std::count_if(m_messages.begin(), m_messages.end(),
        [](const MqttMessage& msg) 
        { 
            return msg.type == MessageType::RECEIVED; 
        });
}

size_t MessagesModel::getSentCount() const noexcept
{
    return std::count_if(m_messages.begin(), m_messages.end(),
        [](const MqttMessage& msg) 
        { 
            return msg.type == MessageType::SENT; 
        });
}

size_t MessagesModel::getPendingCount() const noexcept
{
    return std::count_if(m_messages.begin(), m_messages.end(),
        [](const MqttMessage& msg) 
        {
            return msg.type == MessageType::SENT && msg.sentStatus == SentMessageStatus::PENDING;
        });
}

size_t MessagesModel::getFailedCount() const noexcept
{
    return std::count_if(m_messages.begin(), m_messages.end(),
        [](const MqttMessage& msg) 
        {
            return msg.type == MessageType::SENT && msg.sentStatus == SentMessageStatus::FAILED;
        });
}

void MessagesModel::setupEventHandlers()
{
    if (m_mqttClient == nullptr)
    {
        return;
    }

    //Register for received publish messages
    m_mqttClient->onPublishEvent().add([this](const cleanMqtt::mqtt::PublishEventDetails& details, const cleanMqtt::mqtt::Publish& publish)
        {
            onPublishReceived(details, publish);
        });
}

void MessagesModel::onPublishReceived(const cleanMqtt::mqtt::PublishEventDetails& details, const cleanMqtt::mqtt::Publish& publish)
{
    std::string payload;
    if (details.payload->size() > 0)
    {
        payload.assign(reinterpret_cast<const char*>(details.payload->bytes()), details.payload->size());
    }

    //Add received message
    bool isRetained{ (bool)publish.getFixedHeader().flags.getFlagValue(cleanMqtt::mqtt::PublishFlags::IS_RETAINED) };
    addReceivedMessage( details.topic, payload, publish.getVariableHeader().qos, isRetained);
}

void MessagesModel::updateSentMessageStatus(const std::string& topic, SentMessageStatus status, const std::string& error)
{
    // Find the most recent sent message with matching topic and update its status
    for (auto it = m_messages.rbegin(); it != m_messages.rend(); ++it)
    {
        if (it->type == MessageType::SENT && it->topic == topic && it->sentStatus == SentMessageStatus::PENDING)
        {
            it->sentStatus = status;
            if (!error.empty())
            {
                it->lastError = error;
            }
            break;
        }
    }
}