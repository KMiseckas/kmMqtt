#ifndef INCLUDE_CLEANMQTT_MQTT_PACKETS_PUBLISH_PUBLISHACK_H
#define INCLUDE_CLEANMQTT_MQTT_PACKETS_PUBLISH_PUBLISHACK_H

#include <cleanMqtt/Mqtt/Packets/BasePacket.h>
#include <cleanMqtt/Mqtt/Packets/Publish/Headers/PubAckVariableHeader.h>

namespace cleanMqtt
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

#endif //INCLUDE_CLEANMQTT_MQTT_PACKETS_PUBLISH_PUBLISHACK_H
