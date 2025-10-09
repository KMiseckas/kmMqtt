#ifndef INTERFACE_CLEANMQTT_MQTT_PACKETS_PUBLISH_HEADERS_PUBLISHPAYLOADHEADER_H
#define INTERFACE_CLEANMQTT_MQTT_PACKETS_PUBLISH_HEADERS_PUBLISHPAYLOADHEADER_H

#include <cleanMqtt/Interfaces/IDecodeHeader.h>
#include <cleanMqtt/Interfaces/IEncodeHeader.h>
#include <cleanMqtt/Mqtt/Packets/Properties.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		namespace packets
		{
			struct PublishPayloadHeader : public IDecodeHeader, public IEncodeHeader
			{
				PublishPayloadHeader() noexcept;
				PublishPayloadHeader(BinaryData&& payload) noexcept;

				DecodeResult decode(const ByteBuffer& buffer) noexcept override;
				void encode(ByteBuffer& buffer) const override;
				std::size_t getEncodedBytesSize() const noexcept override;

				BinaryData payload;
			};
		}
	}
}

#endif //INTERFACE_CLEANMQTT_MQTT_PACKETS_PUBLISH_HEADERS_PUBLISHPAYLOADHEADER_H 