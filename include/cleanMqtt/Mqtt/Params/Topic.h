#ifndef INCLUDE_CLEANMQTT_MQTT_PARAMS_TOPICS_H
#define INCLUDE_CLEANMQTT_MQTT_PARAMS_TOPICS_H

#include "cleanMqtt/GlobalMacros.h"
#include <string>
#include <cstdint>
#include <vector>
#include <cleanMqtt/Mqtt/Enums/Qos.h>

namespace cleanMqtt
{
    namespace mqtt
    {
        enum class RetainHandling : uint8_t
        {
            SendAtSubscribe = 0,
            SendAtSubscribeIfNew = 1,
            DoNotSend = 2
        };

        struct PUBLIC_API TopicSubscriptionOptions
        {
            TopicSubscriptionOptions(Qos qos = Qos::QOS_0, bool noLocal = false, bool retainAsPublished = false, RetainHandling retainHandling = RetainHandling::SendAtSubscribe) noexcept
                : qos(qos), noLocal(noLocal), retainAsPublished(retainAsPublished), retainHandling(retainHandling) {}

            Qos qos{ Qos::QOS_0 };
            bool noLocal{ false };
            bool retainAsPublished{ false };
            RetainHandling retainHandling{ RetainHandling::SendAtSubscribe };
        };

        struct PUBLIC_API Topic
        {
            Topic() noexcept = default;
            Topic(std::string filter, TopicSubscriptionOptions opts = {}) noexcept
                : topicFilter(std::move(filter)), options(std::move(opts)) {}

            std::string topicFilter;
            TopicSubscriptionOptions options;
        };

        inline std::string allTopicsToStr(std::vector<Topic> topics) noexcept
        {
            std::string topicsAsStringList;

            for (const auto& c : topics)
            {
                topicsAsStringList.append(c.topicFilter).append(",");
            }

			return topicsAsStringList.erase(topicsAsStringList.length() - 1, 1);
        }
    }
}

#endif // INCLUDE_CLEANMQTT_MQTT_PARAMS_TOPICS_H
