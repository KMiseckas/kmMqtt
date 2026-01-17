#ifndef INTERFACE_KMMQTT_MQTT_PACKETS_UNSUBSCRIBE_HEADERS_UNSUBSCRIBEPAYLOADHEADER_H
#define INTERFACE_KMMQTT_MQTT_PACKETS_UNSUBSCRIBE_HEADERS_UNSUBSCRIBEPAYLOADHEADER_H

#include <kmMqtt/Interfaces/IEncodeHeader.h>
#include <kmMqtt/Mqtt/Packets/DataTypes.h>
#include <vector>

namespace kmMqtt
{
	namespace mqtt
	{
		struct UnSubscribePayloadHeader : IEncodeHeader
		{
			UnSubscribePayloadHeader() noexcept = default;
			UnSubscribePayloadHeader(std::vector<UTF8String>&& topics) noexcept;

			void encode(ByteBuffer& buffer) const override;
			std::size_t getEncodedBytesSize() const noexcept override;

			std::vector<UTF8String> topics;
		};
	}
}

#endif // INTERFACE_KMMQTT_MQTT_PACKETS_UNSUBSCRIBE_HEADERS_UNSUBSCRIBEPAYLOADHEADER_H
