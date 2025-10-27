#ifndef INCLUDE_CLEANMQTT_MQTT_PACKETS_CONNECT_HEADERS_DISCONNECTACKVARIABLEHEADER_H
#define INCLUDE_CLEANMQTT_MQTT_PACKETS_CONNECT_HEADERS_DISCONNECTACKVARIABLEHEADER_H

#include "cleanMqtt/GlobalMacros.h"
#include <cleanMqtt/Interfaces/IDecodeHeader.h>
#include <cleanMqtt/Interfaces/IEncodeHeader.h>
#include "cleanMqtt/Mqtt/Packets/Connection/Codes/DisconnectReasonCode.h"
#include <cleanMqtt/Mqtt/Packets/Properties.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		struct DisconnectVariableHeader : public IDecodeHeader, public IEncodeHeader
		{
		public:
			DisconnectVariableHeader() noexcept;
			DisconnectVariableHeader(DisconnectReasonCode disconnectReasonCode, Properties&& disconnectProperties) noexcept;

			void encode(ByteBuffer& buffer) const override;
			std::size_t getEncodedBytesSize() const noexcept override;

			DecodeResult decode(const ByteBuffer& buffer) noexcept override;

			DisconnectReasonCode reasonCode;
			Properties properties;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_PACKETS_CONNECT_HEADERS_DISCONNECTACKVARIABLEHEADER_H
