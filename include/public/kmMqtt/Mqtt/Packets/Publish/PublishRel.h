// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.


#ifndef KMMQTT_MQTT_PACKETS_PUBLISH_PUBLISHREL_H
#define KMMQTT_MQTT_PACKETS_PUBLISH_PUBLISHREL_H

#include "kmMqtt/Mqtt/Packets/BasePacket.h"
#include "kmMqtt/Mqtt/Packets/Publish/Headers/PubRelVariableHeader.h"

namespace kmMqtt
{
	namespace mqtt
	{
		/**
		 * @brief Represents an MQTT PUBREL packet.
		 * Used in QoS 2 message flow to release a previously received PUBLISH packet.
		 * Contains a variable header with packet identifier, reason code, and properties.
		 */
		class PublishRel : public BasePacket
		{
		public:
			PublishRel(PubRelVariableHeader&& variableHeader) noexcept;
			PublishRel(ByteBuffer&& dataBuffer) noexcept;
			PublishRel(PublishRel&& other) noexcept;
			~PublishRel() override;

			PacketType getPacketType() const noexcept override;

			const PubRelVariableHeader& getVariableHeader() const;

		protected:
			void setUpHeaders() noexcept;
			void onFixedHeaderDecoded() const override;

		private:
			PubRelVariableHeader* m_variableHeader{ nullptr };
		};
	}
}

#endif // KMMQTT_MQTT_PACKETS_PUBLISH_PUBLISHREL_H
