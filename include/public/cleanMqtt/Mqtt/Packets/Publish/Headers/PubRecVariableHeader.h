#ifndef INTERFACE_CLEANMQTT_MQTT_PACKETS_PUBLISH_HEADERS_PUBRECVARIABLEHEADER_H
#define INTERFACE_CLEANMQTT_MQTT_PACKETS_PUBLISH_HEADERS_PUBRECVARIABLEHEADER_H

#include <cleanMqtt/Interfaces/IDecodeHeader.h>
#include <cleanMqtt/Interfaces/IEncodeHeader.h>
#include <cleanMqtt/Mqtt/Packets/Properties.h>
#include "cleanMqtt/Mqtt/Packets/Publish/Codes/PubRecReasonCode.h"

namespace cleanMqtt
{
	namespace mqtt
	{
		struct PubRecVariableHeader : public IDecodeHeader, public IEncodeHeader
		{
			PubRecVariableHeader() noexcept;
			PubRecVariableHeader(
				std::uint16_t packetId,
				PubRecReasonCode reasonCode,
				Properties&& properties) noexcept;

			DecodeResult decode(const ByteBuffer& buffer) noexcept override;
			void encode(ByteBuffer& buffer) const override;
			std::size_t getEncodedBytesSize() const noexcept override;

			std::uint16_t packetId{ 0U };
			PubRecReasonCode reasonCode{ PubRecReasonCode::SUCCESS };
			Properties properties;
		};
	}
}

#endif // INTERFACE_CLEANMQTT_MQTT_PACKETS_PUBLISH_HEADERS_PUBRECVARIABLEHEADER_H
