#ifndef INCLUDE_CLEANMQTT_MQTT_PACKETS_CONNECT_HEADERS_DISCONNECTACKVARIABLEHEADER_H
#define INCLUDE_CLEANMQTT_MQTT_PACKETS_CONNECT_HEADERS_DISCONNECTACKVARIABLEHEADER_H

#include <cleanMqtt/Interfaces/IDecodeHeader.h>
#include <cleanMqtt/Interfaces/IEncodeHeader.h>
#include "cleanMqtt/Mqtt/Packets/Connection/Codes/DisconnectReasonCode.h"
#include <cleanMqtt/Mqtt/Packets/Properties.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		namespace packets
		{
			struct DisconnectVariableHeader : public interfaces::IDecodeHeader, public interfaces::IEncodeHeader
			{
			public:
				DisconnectVariableHeader() noexcept;
				DisconnectVariableHeader(DisconnectReasonCode disconnectReasonCode, Properties&& disconnectProperties) noexcept;

				void encode(ByteBuffer& buffer) const override;
				std::size_t getEncodedBytesSize() const noexcept override;

				void decode(const ByteBuffer& buffer) noexcept override;

				DisconnectReasonCode reasonCode;
				Properties properties;
			};
		}
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_PACKETS_CONNECT_HEADERS_DISCONNECTACKVARIABLEHEADER_H
