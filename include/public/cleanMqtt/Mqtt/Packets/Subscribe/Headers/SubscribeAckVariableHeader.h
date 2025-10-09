#ifndef INTERFACE_CLEANMQTT_MQTT_PACKETS_SUBSCRIBE_HEADERS_SUBSCRIBEACKVARIABLEHEADER_H
#define INTERFACE_CLEANMQTT_MQTT_PACKETS_SUBSCRIBE_HEADERS_SUBSCRIBEACKVARIABLEHEADER_H

#include <cleanMqtt/Interfaces/IDecodeHeader.h>
#include <cleanMqtt/Mqtt/Packets/Properties.h>
#include <cstdint>

namespace cleanMqtt
{
	namespace mqtt
	{
		namespace packets
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
}

#endif // INTERFACE_CLEANMQTT_MQTT_PACKETS_SUBSCRIBE_HEADERS_SUBSCRIBEACKVARIABLEHEADER_H
