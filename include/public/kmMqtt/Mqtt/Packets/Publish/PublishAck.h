// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MQTT_PACKETS_PUBLISH_PUBLISHACK_H
#define INCLUDE_KMMQTT_MQTT_PACKETS_PUBLISH_PUBLISHACK_H

#include <kmMqtt/Mqtt/Packets/BasePacket.h>
#include <kmMqtt/Mqtt/Packets/Publish/Headers/PubAckVariableHeader.h>

namespace kmMqtt
{
	namespace mqtt
	{
		class PublishAck : public BasePacket
		{
		public:
			PublishAck(PubAckVariableHeader&& variableHeader) noexcept;
			PublishAck(ByteBuffer&& dataBuffer) noexcept;
			PublishAck(PublishAck&& other) noexcept;
			~PublishAck() override;

			PacketType getPacketType() const noexcept override;

			const PubAckVariableHeader& getVariableHeader() const;

		protected:
			void setUpHeaders() noexcept;
			void onFixedHeaderDecoded() const override;

			PubAckVariableHeader* m_variableHeader{ nullptr };
		};
	}
}

#endif //INCLUDE_KMMQTT_MQTT_PACKETS_PUBLISH_PUBLISHACK_H
