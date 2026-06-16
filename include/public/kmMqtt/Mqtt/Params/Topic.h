// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MQTT_PARAMS_TOPICS_H
#define INCLUDE_KMMQTT_MQTT_PARAMS_TOPICS_H

#include "kmMqtt/GlobalMacros.h"
#include <kmMqtt/STL/KmString.h>
#include <cstdint>
#include <kmMqtt/STL/KmVector.h>
#include <kmMqtt/Mqtt/Enums/Qos.h>

namespace kmMqtt
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
            Topic(kmStd::string filter, TopicSubscriptionOptions opts = {}) noexcept
                : topicFilter(std::move(filter)), options(std::move(opts)) {}

            kmStd::string topicFilter;
            TopicSubscriptionOptions options;
        };

        inline kmStd::string allTopicsToStr(kmStd::vector<Topic> topics) noexcept
        {
            kmStd::string topicsAsStringList;

            for (const auto& c : topics)
            {
                topicsAsStringList.append(c.topicFilter).append(",");
            }

			return topicsAsStringList.erase(topicsAsStringList.length() - 1, 1);
        }
    }
}

#endif // INCLUDE_KMMQTT_MQTT_PARAMS_TOPICS_H
