#ifndef INCLUDE_CLEANMQTT_MQTT_PACKETS_PUBLISH_PUBLISHREC_H
#define INCLUDE_CLEANMQTT_MQTT_PACKETS_PUBLISH_PUBLISHREC_H

#include <cleanMqtt/Mqtt/Packets/BasePacket.h>
#include <cleanMqtt/Mqtt/Packets/Publish/Headers/PubRecVariableHeader.h>

namespace cleanMqtt
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

#endif //INCLUDE_CLEANMQTT_MQTT_PACKETS_PUBLISH_PUBLISHREC_H