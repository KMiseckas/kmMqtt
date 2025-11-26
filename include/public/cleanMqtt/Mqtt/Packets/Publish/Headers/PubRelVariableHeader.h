#ifndef INTERFACE_CLEANMQTT_MQTT_PACKETS_PUBLISH_HEADERS_PUBRELVARIABLEHEADER_H
#define INTERFACE_CLEANMQTT_MQTT_PACKETS_PUBLISH_HEADERS_PUBRELVARIABLEHEADER_H

#include <cleanMqtt/Interfaces/IDecodeHeader.h>
#include <cleanMqtt/Interfaces/IEncodeHeader.h>
#include <cleanMqtt/Mqtt/Packets/Properties.h>
#include "cleanMqtt/Mqtt/Packets/Publish/Codes/PubRelReasonCode.h"

namespace cleanMqtt
{
	namespace mqtt
	{
		struct PubRelVariableHeader : public IDecodeHeader, public IEncodeHeader
		{
			PubRelVariableHeader() noexcept;
			PubRelVariableHeader(
				std::uint16_t packetId,
				PubRelReasonCode reasonCode,
				Properties&& properties) noexcept;

			DecodeResult decode(const ByteBuffer& buffer) noexcept override;
			void encode(ByteBuffer& buffer) const override;
			std::size_t getEncodedBytesSize() const noexcept override;

			std::uint16_t packetId{ 0U };
			PubRelReasonCode reasonCode{ PubRelReasonCode::SUCCESS };
			Properties properties;
		};
	}
}

#endif // INTERFACE_CLEANMQTT_MQTT_PACKETS_PUBLISH_HEADERS_PUBRELVARIABLEHEADER_H
