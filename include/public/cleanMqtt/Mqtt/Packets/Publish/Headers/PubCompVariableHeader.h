#ifndef INTERFACE_CLEANMQTT_MQTT_PACKETS_PUBLISH_HEADERS_PUBCOMPVARIABLEHEADER_H
#define INTERFACE_CLEANMQTT_MQTT_PACKETS_PUBLISH_HEADERS_PUBCOMPVARIABLEHEADER_H

#include <cleanMqtt/Interfaces/IDecodeHeader.h>
#include <cleanMqtt/Interfaces/IEncodeHeader.h>
#include <cleanMqtt/Mqtt/Packets/Properties.h>
#include "cleanMqtt/Mqtt/Packets/Publish/Codes/PubCompReasonCode.h"

namespace cleanMqtt
{
	namespace mqtt
	{
		struct PubCompVariableHeader : public IDecodeHeader, public IEncodeHeader
		{
			PubCompVariableHeader() noexcept;
			PubCompVariableHeader(
				std::uint16_t packetId,
				PubCompReasonCode reasonCode,
				Properties&& properties) noexcept;

			DecodeResult decode(const ByteBuffer& buffer) noexcept override;
			void encode(ByteBuffer& buffer) const override;
			std::size_t getEncodedBytesSize() const noexcept override;

			std::uint16_t packetId{ 0U };
			PubCompReasonCode reasonCode{ PubCompReasonCode::SUCCESS };
			Properties properties;
		};
	}
}

#endif // INTERFACE_CLEANMQTT_MQTT_PACKETS_PUBLISH_HEADERS_PUBCOMPVARIABLEHEADER_H
