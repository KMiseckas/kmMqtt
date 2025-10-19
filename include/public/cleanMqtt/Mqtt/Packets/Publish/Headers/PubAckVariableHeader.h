#ifndef INTERFACE_CLEANMQTT_MQTT_PACKETS_PUBLISH_HEADERS_PUBACKVARIABLEHEADER_H
#define INTERFACE_CLEANMQTT_MQTT_PACKETS_PUBLISH_HEADERS_PUBACKVARIABLEHEADER_H

#include <cleanMqtt/Interfaces/IDecodeHeader.h>
#include <cleanMqtt/Interfaces/IEncodeHeader.h>
#include <cleanMqtt/Mqtt/Packets/Properties.h>
#include "cleanMqtt/Mqtt/Packets/Publish/Codes/PubAckReasonCode.h"

namespace cleanMqtt
{
	namespace mqtt
	{
		struct PubAckVariableHeader : public IDecodeHeader, public IEncodeHeader
		{
			PubAckVariableHeader() noexcept;
			PubAckVariableHeader(
				std::uint16_t packetId,
				PubAckReasonCode reasonCode,
				Properties&& properties) noexcept;

			DecodeResult decode(const ByteBuffer& buffer) noexcept override;
			void encode(ByteBuffer& buffer) const override;
			std::size_t getEncodedBytesSize() const noexcept override;

			std::uint16_t packetId{ 0U };
			PubAckReasonCode reasonCode{ PubAckReasonCode::SUCCESS };
			Properties properties;
		};
	}
}

#endif // INTERFACE_CLEANMQTT_MQTT_PACKETS_PUBLISH_HEADERS_PUBACKVARIABLEHEADER_H
