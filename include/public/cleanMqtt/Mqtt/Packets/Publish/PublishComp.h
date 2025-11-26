
#ifndef CLEANMQTT_MQTT_PACKETS_PUBLISH_PUBLISHCOMP_H
#define CLEANMQTT_MQTT_PACKETS_PUBLISH_PUBLISHCOMP_H

#include "cleanMqtt/Mqtt/Packets/BasePacket.h"
#include "cleanMqtt/Mqtt/Packets/Publish/Headers/PubCompVariableHeader.h"

namespace cleanMqtt
{
	namespace mqtt
	{
		/**
		 * @brief Represents an MQTT PUBCOMP (Publish Complete) packet.
		 * This packet is sent by the receiver of a PUBLISH packet with QoS level 2
		 * to acknowledge that it has received and processed the PUBLISH packet and QOS 2 proccess is now complete.
		 * It contains a variable header with the Packet Identifier, Reason Code, and Properties.
		 */
		class PublishComp : public BasePacket
		{
		public:
			PublishComp(PubCompVariableHeader&& variableHeader) noexcept;
			PublishComp(ByteBuffer&& dataBuffer) noexcept;
			PublishComp(PublishComp&& other) noexcept;
			~PublishComp() override;

			PacketType getPacketType() const noexcept override;

			const PubCompVariableHeader& getVariableHeader() const;

		protected:
			void setUpHeaders() noexcept;
			void onFixedHeaderDecoded() const override;

		private:
			PubCompVariableHeader* m_variableHeader{ nullptr };
		};
	}
}

#endif // CLEANMQTT_MQTT_PACKETS_PUBLISH_PUBLISHCOMP_H
