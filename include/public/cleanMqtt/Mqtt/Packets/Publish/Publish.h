#ifndef INCLUDE_CLEANMQTT_MQTT_PACKETS_PUBLISH_PUBLISH_H
#define INCLUDE_CLEANMQTT_MQTT_PACKETS_PUBLISH_PUBLISH_H

#include <cleanMqtt/Mqtt/Packets/BasePacket.h>
#include <cleanMqtt/Mqtt/Packets/Publish/Flags/EncodedPublishFlags.h>
#include <cleanMqtt/Mqtt/Packets/Publish/Headers/PublishVariableHeader.h>
#include <cleanMqtt/Mqtt/Packets/Publish/Headers/PublishPayloadHeader.h>

namespace cleanMqtt
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

			PublishVariableHeader* m_variableHeader{ nullptr };
			PublishPayloadHeader* m_payloadHeader{ nullptr };
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_PACKETS_PUBLISH_PUBLISH_H