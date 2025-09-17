#include <cleanMqtt/Mqtt/State/PendingUnSubscription.h>
#include <cleanMqtt/Mqtt/Packets/UnSubscribe/Codes/UnSubAckReasonCode.h>

namespace cleanMqtt
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
