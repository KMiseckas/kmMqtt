// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MQTT_PACKETS_PUBLISH_PUBLISHREC_H
#define INCLUDE_KMMQTT_MQTT_PACKETS_PUBLISH_PUBLISHREC_H

#include <kmMqtt/Mqtt/Packets/BasePacket.h>
#include <kmMqtt/Mqtt/Packets/Publish/Headers/PubRecVariableHeader.h>

namespace kmMqtt
{
	namespace mqtt
	{
		/**
		 * @brief Represents an MQTT PUBREC packet.
		 * Used in QoS 2 message flow to acknowledge receipt of a PUBLISH packet.
		 * Contains a variable header with packet identifier, reason code, and properties.
		 */
		class PublishRec : public BasePacket
		{
		public:
			PublishRec(PubRecVariableHeader&& variableHeader) noexcept;
			PublishRec(ByteBuffer&& dataBuffer) noexcept;
			PublishRec(PublishRec&& other) noexcept;
			~PublishRec() override;

			PacketType getPacketType() const noexcept override;

			const PubRecVariableHeader& getVariableHeader() const;

		protected:
			void setUpHeaders() noexcept;
			void onFixedHeaderDecoded() const override;

		private:
			PubRecVariableHeader* m_variableHeader{ nullptr };
		};
	}
}

#endif //INCLUDE_KMMQTT_MQTT_PACKETS_PUBLISH_PUBLISHREC_H