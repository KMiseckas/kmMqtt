// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MQTT_SESSIONSTATE_SESSIONMESSAGE_H
#define INCLUDE_KMMQTT_MQTT_SESSIONSTATE_SESSIONMESSAGE_H

#include <kmMqtt/GlobalMacros.h>
#include <kmMqtt/GlobalTypes.h>
#include <kmMqtt/Mqtt/Params/PublishOptions.h>
#include <kmMqtt/Mqtt/Packets/PacketType.h>
#include <cassert>

namespace kmMqtt
{
	namespace mqtt
	{
		enum class PublishMessageStatus : std::uint8_t
		{
			//Ougoing QOS1
			WaitingForAck,

			//Outgoing QOS2
			WaitingForPubRec,
			WaitingForPubComp,

			//Incoming QOS2
			WaitingForPubRel,
			NeedToSendPubComp,
		};

		struct PUBLIC_API PublishMessageData
		{
			std::string topic;
			ByteBuffer payload;
			PublishOptions options;
		};

		struct PUBLIC_API SavedData
		{
			PublishMessageStatus status;
			std::uint16_t packetID{ 0U };
			PublishMessageData publishMsgData;
		};

		struct MessageContainerData
		{
			MessageContainerData(std::uint16_t packetId, PublishMessageData msgData, TimePoint nextRetryTime, bool retryEnabled = false)
				: nextRetryTime(std::move(nextRetryTime)),
				canRetry(retryEnabled),
				data{ msgData.options.qos == Qos::QOS_1 ? PublishMessageStatus::WaitingForAck : PublishMessageStatus::WaitingForPubRec, packetId, std::move(msgData) }
			{
			}

			inline PacketType getRetryPacketType() const noexcept
			{
				switch(data.status)
				{
					case PublishMessageStatus::WaitingForAck:
					case PublishMessageStatus::WaitingForPubRec:
						return PacketType::PUBLISH;
					case PublishMessageStatus::WaitingForPubRel:
						return PacketType::PUBLISH_RECEIVED;
					case PublishMessageStatus::WaitingForPubComp:
						return PacketType::PUBLISH_RELEASED;
					case PublishMessageStatus::NeedToSendPubComp:
						return PacketType::PUBLISH_COMPLETE;
					default:
						return PacketType::RESERVED; // Should not happen, but just in case
				}
			}

			TimePoint nextRetryTime;
			bool canRetry{ false };
			SavedData data;
		};
	}
}

#endif //INCLUDE_KMMQTT_MQTT_SESSIONSTATE_SESSIONMESSAGE_H
