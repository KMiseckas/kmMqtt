#include <kmMqtt/Mqtt/State/PendingUnSubscription.h>
#include <kmMqtt/Mqtt/Packets/UnSubscribe/Codes/UnSubAckReasonCode.h>

namespace kmMqtt
{
	namespace mqtt
	{
		PendingUnSubscription::PendingUnSubscription(std::uint16_t packetId, std::vector<Topic> topics) noexcept
			: packetId(packetId)
		{
			unSubscriptionResults.setTopics(std::move(topics));
		}
	}
}
