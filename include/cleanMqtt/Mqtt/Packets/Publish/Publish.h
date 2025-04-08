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
		namespace packets
		{
			class Publish : public BasePacket
			{
			public:
				Publish(const char* topic, const char* msg, const EncodedPublishFlags& flags) noexcept;
				~Publish() override;

				PacketType getPacketType() const noexcept override;

			protected:
				PublishVariableHeader m_variableHeader;
				PublishPayloadHeader m_payloadHeader;
			};
		}
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_PACKETS_PUBLISH_PUBLISH_H