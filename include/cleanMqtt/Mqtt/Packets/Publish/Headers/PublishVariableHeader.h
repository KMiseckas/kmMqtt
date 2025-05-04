#ifndef INTERFACE_CLEANMQTT_MQTT_PACKETS_PUBLISH_HEADERS_PUBLISHVARIABLEHEADER_H
#define INTERFACE_CLEANMQTT_MQTT_PACKETS_PUBLISH_HEADERS_PUBLISHVARIABLEHEADER_H

#include <cleanMqtt/Interfaces/IDecodeHeader.h>
#include <cleanMqtt/Interfaces/IEncodeHeader.h>
#include <cleanMqtt/Mqtt/Packets/Properties.h>
#include <cleanMqtt/Mqtt/Enums/Qos.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		namespace packets
		{
			struct PublishVariableHeader : public interfaces::IDecodeHeader, public interfaces::IEncodeHeader
			{
				PublishVariableHeader() noexcept;
				PublishVariableHeader(
					const char* topicName,
					std::uint16_t packetId,
					Properties&& properties,
					Qos publishQOS) noexcept;

				DecodeResult decode(const ByteBuffer& buffer) noexcept override;
				void encode(ByteBuffer& buffer) const override;
				std::size_t getEncodedBytesSize() const noexcept override;
				void setQos(const Qos qos) noexcept;

				UTF8String topicName{ "" };
				std::uint16_t packetIdentifier{ 0U };
				Properties properties;

			protected:
				Qos m_Qos{ Qos::QOS_0 };
			};
		}
	}
}

#endif //INTERFACE_CLEANMQTT_MQTT_PACKETS_PUBLISH_HEADERS_PUBLISHVARIABLEHEADER_H 