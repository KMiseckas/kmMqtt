#include <kmMqtt/Mqtt/TopicAliases.h>
#include <kmMqtt/Logger/Log.h>

namespace kmMqtt
{
	namespace mqtt
	{
		bool TopicAliases::tryAddTopicAlias(const char* topicName, const std::uint16_t topicAlias)
		{
			LogTrace("TopicAliases", "Trying to map topic name [%s] to topic alias [%d].", topicName, topicAlias);

			if (topicName == nullptr || *topicName == '\0')
			{
				LogWarning("TopicAliases", "topicName argument is invalid. Empty topicName.");
				return false;
			}

			auto iter{ m_topicAliasToNameMap.find(topicAlias) };
			if (iter != m_topicAliasToNameMap.end())
			{
				if (std::strcmp(topicName, iter->second) == 0)
				{
					LogTrace("TopicAliases", "Topic name [%s] already mapped under requested topic alias [%d].", topicName, topicAlias);
					return true;
				}

				LogTrace("TopicAliases", "Topic alias[%d] found in existing mapping to topic name [%s]. Erasing mapping.", topicAlias, iter->second);
				m_topicAliasToNameMap.erase(topicAlias);
			}

			m_topicAliasToNameMap.insert({ topicAlias, topicName });
			LogTrace("TopicAliases", "Succesfully mapped topic name to topic alias.");

			return true;
		}

		bool TopicAliases::tryFindTopicName(std::uint16_t topicAlias, const char*& outTopicName) const
		{
			outTopicName = nullptr;

			auto iter{ m_topicAliasToNameMap.find(topicAlias) };

			if (iter == m_topicAliasToNameMap.end())
			{
				return false;
			}

			outTopicName = iter->second;
			return true;
		}
	}
}
