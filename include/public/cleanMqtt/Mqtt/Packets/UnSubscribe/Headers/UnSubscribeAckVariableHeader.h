#ifndef INTERFACE_CLEANMQTT_MQTT_PACKETS_UNSUBSCRIBE_HEADERS_UNSUBSCRIBEACKVARIABLEHEADER_H
#define INTERFACE_CLEANMQTT_MQTT_PACKETS_UNSUBSCRIBE_HEADERS_UNSUBSCRIBEACKVARIABLEHEADER_H

#include <cleanMqtt/Interfaces/IDecodeHeader.h>
#include <cleanMqtt/Mqtt/Packets/Properties.h>
#include <cstdint>

namespace cleanMqtt
{
	namespace mqtt
	{
		namespace packets
		{
			struct UnSubscribeAckVariableHeader : interfaces::IDecodeHeader
			{
				UnSubscribeAckVariableHeader() noexcept = default;

				DecodeResult decode(const ByteBuffer& buffer) noexcept override;

				std::uint16_t packetId{ 0U };
				Properties properties;
			};
		}
	}
}

#endif // INTERFACE_CLEANMQTT_MQTT_PACKETS_UNSUBSCRIBE_HEADERS_UNSUBSCRIBEACKVARIABLEHEADER_H
