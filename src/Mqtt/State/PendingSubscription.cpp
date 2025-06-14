#include <cleanMqtt/Mqtt/State/PendingSubscription.h>
#include <cleanMqtt/Mqtt/Packets/Subscribe/Codes/SubAckReasonCode.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		PendingSubscription::PendingSubscription(std::uint16_t packetId, std::vector<Topic> topics) noexcept
			: packetId(packetId)
		{
			subscriptionResults.setTopics(std::move(topics));
		}
	}
}
