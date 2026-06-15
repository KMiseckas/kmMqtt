// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MQTT_TOPIC_ALIASES_H
#define INCLUDE_KMMQTT_MQTT_TOPIC_ALIASES_H

#include <kmMqtt/STL/KmUnorderedMap.h>
#include <cstdint>
#include <kmMqtt/STL/KmString.h>

namespace kmMqtt
{
	namespace mqtt
	{
		class TopicAliases
		{
		public:
			bool tryAddTopicAlias(const char* topicName, std::uint16_t topicAlias);
			bool tryFindTopicName(std::uint16_t topicAlias, const char*& outTopicName) const;

		protected:
			kmStd::unordered_multimap<std::uint16_t, kmStd::string> m_topicAliasToNameMap;
		};
	}

}

#endif //INCLUDE_KMMQTT_MQTT_TOPIC_ALIASES_H 