#include <kmMqtt/Mqtt/State/PendingSubscription.h>
#include <kmMqtt/Mqtt/Packets/Subscribe/Codes/SubAckReasonCode.h>

namespace kmMqtt
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
