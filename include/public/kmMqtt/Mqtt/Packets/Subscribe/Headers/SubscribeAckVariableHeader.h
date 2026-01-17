#ifndef INTERFACE_KMMQTT_MQTT_PACKETS_SUBSCRIBE_HEADERS_SUBSCRIBEACKVARIABLEHEADER_H
#define INTERFACE_KMMQTT_MQTT_PACKETS_SUBSCRIBE_HEADERS_SUBSCRIBEACKVARIABLEHEADER_H

#include <kmMqtt/Interfaces/IDecodeHeader.h>
#include <kmMqtt/Mqtt/Packets/Properties.h>
#include <cstdint>

namespace kmMqtt
{
	namespace mqtt
	{
		struct SubscribeAckVariableHeader : IDecodeHeader
		{
			SubscribeAckVariableHeader() noexcept = default;

			DecodeResult decode(const ByteBuffer& buffer) noexcept override;

			std::uint16_t packetId{ 0U };
			Properties properties;
		};
	}
}

#endif // INTERFACE_KMMQTT_MQTT_PACKETS_SUBSCRIBE_HEADERS_SUBSCRIBEACKVARIABLEHEADER_H
