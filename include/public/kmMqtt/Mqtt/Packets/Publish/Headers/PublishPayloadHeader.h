#ifndef INTERFACE_KMMQTT_MQTT_PACKETS_PUBLISH_HEADERS_PUBLISHPAYLOADHEADER_H
#define INTERFACE_KMMQTT_MQTT_PACKETS_PUBLISH_HEADERS_PUBLISHPAYLOADHEADER_H

#include <kmMqtt/Interfaces/IDecodeHeader.h>
#include <kmMqtt/Interfaces/IEncodeHeader.h>
#include <kmMqtt/Mqtt/Packets/Properties.h>

namespace kmMqtt
{
	namespace mqtt
	{
		struct PublishPayloadHeader : public IDecodeHeader, public IEncodeHeader
		{
			PublishPayloadHeader() noexcept;
			PublishPayloadHeader(ByteBuffer&& payload) noexcept;

			DecodeResult decode(const ByteBuffer& buffer) noexcept override;
			void encode(ByteBuffer& buffer) const override;
			std::size_t getEncodedBytesSize() const noexcept override;

			ByteBuffer payload{ 0 };
		};
	}
}

#endif //INTERFACE_KMMQTT_MQTT_PACKETS_PUBLISH_HEADERS_PUBLISHPAYLOADHEADER_H 