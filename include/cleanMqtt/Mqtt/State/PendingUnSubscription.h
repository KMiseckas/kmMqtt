#ifndef INCLUDE_CLEANMQTT_MQTT_STATE_PENDINGUNSUBSCRIPTION_H
#define INCLUDE_CLEANMQTT_MQTT_STATE_PENDINGUNSUBSCRIPTION_H

#include <cleanMqtt/Mqtt/Packets/Subscribe/Codes/SubAckReasonCode.h>
#include <cleanMqtt/Mqtt/State/UnSubAckTopicReason.h>

namespace cleanMqtt
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

#endif // INCLUDE_CLEANMQTT_MQTT_STATE_PENDINGUNSUBSCRIPTION_H
