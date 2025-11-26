
#ifndef CLEANMQTT_MQTT_PACKETS_PUBLISH_PUBLISHREL_H
#define CLEANMQTT_MQTT_PACKETS_PUBLISH_PUBLISHREL_H

#include "cleanMqtt/Mqtt/Packets/BasePacket.h"
#include "cleanMqtt/Mqtt/Packets/Publish/Headers/PubRelVariableHeader.h"

namespace cleanMqtt
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

#endif // CLEANMQTT_MQTT_PACKETS_PUBLISH_PUBLISHREL_H
