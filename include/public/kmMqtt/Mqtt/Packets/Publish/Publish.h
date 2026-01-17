// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MQTT_PACKETS_PUBLISH_PUBLISH_H
#define INCLUDE_KMMQTT_MQTT_PACKETS_PUBLISH_PUBLISH_H

#include <kmMqtt/Mqtt/Packets/BasePacket.h>
#include <kmMqtt/Mqtt/Packets/Publish/Flags/EncodedPublishFlags.h>
#include <kmMqtt/Mqtt/Packets/Publish/Headers/PublishVariableHeader.h>
#include <kmMqtt/Mqtt/Packets/Publish/Headers/PublishPayloadHeader.h>

namespace kmMqtt
{
	namespace mqtt
	{
		class Publish : public BasePacket
		{
		public:
			Publish(PublishPayloadHeader&& payloadHeader, PublishVariableHeader&& variableHeader, const EncodedPublishFlags& flags) noexcept;
			Publish(ByteBuffer&& dataBuffer) noexcept;
			Publish(Publish&& other) noexcept;
			~Publish() override;

			PacketType getPacketType() const noexcept override;

			const PublishVariableHeader& getVariableHeader() const;
			const PublishPayloadHeader& getPayloadHeader() const;

		protected:
			void setUpHeaders() noexcept;
			void onFixedHeaderDecoded() const override;

			PublishPayloadHeader* m_payloadHeader{ nullptr };
			PublishVariableHeader* m_variableHeader{ nullptr };
		};
	}
}

#endif //INCLUDE_KMMQTT_MQTT_PACKETS_PUBLISH_PUBLISH_H