#ifndef INCLUDE_CLEANMQTT_MQTT_TOPIC_ALIASES_H
#define INCLUDE_CLEANMQTT_MQTT_TOPIC_ALIASES_H

#include <unordered_map>

namespace cleanMqtt
{
	namespace mqtt
	{
		class TopicAliases
		{
		public:
			bool tryAddTopicAlias(const char* topicName, std::uint16_t topicAlias);
			bool tryFindTopicName(std::uint16_t topicAlias, const char*& outTopicName) const;

		protected:
			std::unordered_multimap<std::uint16_t, const char*> m_topicAliasToNameMap;
		};
	}

}

#endif //INCLUDE_CLEANMQTT_MQTT_TOPIC_ALIASES_H 