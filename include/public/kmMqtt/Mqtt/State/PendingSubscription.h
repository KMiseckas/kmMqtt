#ifndef INCLUDE_KMMQTT_MQTT_STATE_PENDINGSUBSCRIPTION_H
#define INCLUDE_KMMQTT_MQTT_STATE_PENDINGSUBSCRIPTION_H

#include <kmMqtt/Mqtt/Packets/Subscribe/Codes/SubAckReasonCode.h>
#include <kmMqtt/Mqtt/State/SubAckTopicReason.h>

namespace kmMqtt
{
	namespace mqtt
	{
		struct PUBLIC_API PendingSubscription
		{
			PendingSubscription(std::uint16_t packetId, std::vector<Topic> topics) noexcept;

			std::uint16_t packetId;
			SubAckResults subscriptionResults{};
		};
	}
}

#endif // INCLUDE_KMMQTT_MQTT_STATE_PENDINGSUBSCRIPTION_H
