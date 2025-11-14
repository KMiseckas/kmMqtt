#ifndef INTERFACE_CLEANMQTT_MQTT_PACKETS_PUBLISH_HEADERS_PUBLISHPAYLOADHEADER_H
#define INTERFACE_CLEANMQTT_MQTT_PACKETS_PUBLISH_HEADERS_PUBLISHPAYLOADHEADER_H

#include <cleanMqtt/Interfaces/IDecodeHeader.h>
#include <cleanMqtt/Interfaces/IEncodeHeader.h>
#include <cleanMqtt/Mqtt/Packets/Properties.h>

namespace cleanMqtt
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

#endif //INTERFACE_CLEANMQTT_MQTT_PACKETS_PUBLISH_HEADERS_PUBLISHPAYLOADHEADER_H 