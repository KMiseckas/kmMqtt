// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

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
