#ifndef INTERFACE_CLEANMQTT_MQTT_PACKETS_UNSUBSCRIBE_HEADERS_UNSUBSCRIBEPAYLOADHEADER_H
#define INTERFACE_CLEANMQTT_MQTT_PACKETS_UNSUBSCRIBE_HEADERS_UNSUBSCRIBEPAYLOADHEADER_H

#include <cleanMqtt/Interfaces/IEncodeHeader.h>
#include <cleanMqtt/Mqtt/Packets/DataTypes.h>
#include <cstdint>
#include <vector>

namespace cleanMqtt
{
	namespace mqtt
	{
		namespace packets
		{
			struct UnSubscribePayloadHeader : interfaces::IEncodeHeader
			{
				UnSubscribePayloadHeader() noexcept = default;
				UnSubscribePayloadHeader(std::vector<UTF8String>&& topics) noexcept;

				void encode(ByteBuffer& buffer) const override;
				std::size_t getEncodedBytesSize() const noexcept;

				std::vector<UTF8String> topics;
			};
		}
	}
}

#endif // INTERFACE_CLEANMQTT_MQTT_PACKETS_UNSUBSCRIBE_HEADERS_UNSUBSCRIBEPAYLOADHEADER_H
