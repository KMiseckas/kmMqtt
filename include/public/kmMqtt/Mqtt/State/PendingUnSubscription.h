#ifndef INCLUDE_KMMQTT_MQTT_STATE_PENDINGUNSUBSCRIPTION_H
#define INCLUDE_KMMQTT_MQTT_STATE_PENDINGUNSUBSCRIPTION_H

#include <kmMqtt/Mqtt/Packets/Subscribe/Codes/SubAckReasonCode.h>
#include <kmMqtt/Mqtt/State/UnSubAckTopicReason.h>

namespace kmMqtt
{
	namespace mqtt
	{
		struct PUBLIC_API PendingUnSubscription
		{
			PendingUnSubscription(std::uint16_t packetId, std::vector<Topic> topics) noexcept;

			std::uint16_t packetId;
			UnSubAckResults unSubscriptionResults{};
		};
	}
}

#endif // INCLUDE_KMMQTT_MQTT_STATE_PENDINGUNSUBSCRIPTION_H
