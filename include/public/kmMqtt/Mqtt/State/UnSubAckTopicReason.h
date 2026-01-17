#ifndef KMMQTT_MQTT_PACKETS_UNSUBSCRIBE_UNSUBACKTOPICREASON_H
#define KMMQTT_MQTT_PACKETS_UNSUBSCRIBE_UNSUBACKTOPICREASON_H

#include <kmMqtt/Mqtt/Params/Topic.h>
#include <kmMqtt/Mqtt/Packets/UnSubscribe/Codes/UnSubAckReasonCode.h>

#include <vector>
#include <cassert>

namespace kmMqtt
{
	namespace mqtt
	{
		struct PUBLIC_API UnSubAckTopicReason
		{
			Topic topic;
			UnSubAckReasonCode reasonCode{ UnSubAckReasonCode::UNSPECIFIED_ERROR };
		};

		struct PUBLIC_API UnSubAckResults
		{
			inline bool allUnSubscribedSuccesfully() const
			{
				return m_allUnSubscribedSuccesfully;
			}

			void setTopicReasons(const std::vector<UnSubAckReasonCode> reasonCodes)
			{
				assert(reasonCodes.size() == m_topicReasonList.size() && "Reason codes size must match topic list size.");

				m_allUnSubscribedSuccesfully = true;

				for (size_t i = 0; i < reasonCodes.size(); ++i)
				{
					const auto reasonCode{ reasonCodes[i] };

					if (reasonCode != UnSubAckReasonCode::SUCCESS)
					{
						m_allUnSubscribedSuccesfully = false;
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
					m_topicReasonList.push_back({ std::move(topic), UnSubAckReasonCode::UNSPECIFIED_ERROR });
				}
			}

			const std::vector<UnSubAckTopicReason>& getTopicReasons() const
			{
				return m_topicReasonList;
			}

		private:
			std::vector<UnSubAckTopicReason> m_topicReasonList;
			bool m_allUnSubscribedSuccesfully{ false };
		};
	}
}

#endif // KMMQTT_MQTT_PACKETS_UNSUBSCRIBE_UNSUBACKTOPICREASON_H
