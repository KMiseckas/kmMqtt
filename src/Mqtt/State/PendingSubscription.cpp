// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

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
