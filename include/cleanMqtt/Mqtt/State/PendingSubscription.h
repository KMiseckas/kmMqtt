#ifndef INCLUDE_CLEANMQTT_MQTT_STATE_PENDINGSUBSCRIPTION_H
#define INCLUDE_CLEANMQTT_MQTT_STATE_PENDINGSUBSCRIPTION_H

#include <cleanMqtt/Mqtt/Packets/Subscribe/Codes/SubAckReasonCode.h>
#include <cleanMqtt/Mqtt/State/SubAckTopicReason.h>

namespace cleanMqtt
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

#endif // INCLUDE_CLEANMQTT_MQTT_STATE_PENDINGSUBSCRIPTION_H
