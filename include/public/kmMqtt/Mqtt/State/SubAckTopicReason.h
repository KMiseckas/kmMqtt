// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef KMMQTT_MQTT_PACKETS_SUBSCRIBE_SUBACKTOPICREASON_H
#define KMMQTT_MQTT_PACKETS_SUBSCRIBE_SUBACKTOPICREASON_H

#include <kmMqtt/Mqtt/Params/Topic.h>
#include <kmMqtt/Mqtt/Packets/Subscribe/Codes/SubAckReasonCode.h>

#include <vector>
#include <cassert>

namespace kmMqtt 
{
	namespace mqtt
	{
		struct PUBLIC_API SubAckTopicReason
		{
			Topic topic;
			SubAckReasonCode reasonCode{ SubAckReasonCode::UNSPECIFIED_ERROR };
		};

		struct PUBLIC_API SubAckResults
		{
			inline bool allSubscribedSuccesfully() const
			{
				return m_allSubscribedSuccesfully;
			}

			void setTopicReasons(const std::vector<SubAckReasonCode> reasonCodes)
			{
				assert(reasonCodes.size() == m_topicReasonList.size() && "Reason codes size must match topic list size.");

				m_allSubscribedSuccesfully = true;

				for (size_t i = 0; i < reasonCodes.size(); ++i)
				{
					const auto reasonCode{ reasonCodes[i] };

					if(reasonCode != SubAckReasonCode::GRANTED_QOS_0 &&
						reasonCode != SubAckReasonCode::GRANTED_QOS_1 &&
						reasonCode != SubAckReasonCode::GRANTED_QOS_2)
					{
						m_allSubscribedSuccesfully = false;
					}

					m_topicReasonList[i].reasonCode = reasonCode;
				}
			}

			void setTopics(std::vector<Topic> topics)
			{
				m_topicReasonList.clear();
				m_topicReasonList.reserve(topics.size());
				for (auto& topic : topics)
				{
					m_topicReasonList.push_back({ std::move(topic), SubAckReasonCode::UNSPECIFIED_ERROR });
				}
			}

			const std::vector<SubAckTopicReason>& getTopicReasons() const
			{
				return m_topicReasonList;
			}

		private:
			std::vector<SubAckTopicReason> m_topicReasonList;
			bool m_allSubscribedSuccesfully{ false };
		};
	}
}

#endif // KMMQTT_MQTT_PACKETS_SUBSCRIBE_SUBACKTOPICREASON_H
