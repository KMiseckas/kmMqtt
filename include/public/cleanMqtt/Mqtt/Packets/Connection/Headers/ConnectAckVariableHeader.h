#ifndef INCLUDE_CLEANMQTT_MQTT_PACKETS_CONNECT_HEADERS_CONNECTACKVARIABLEHEADER_H
#define INCLUDE_CLEANMQTT_MQTT_PACKETS_CONNECT_HEADERS_CONNECTACKVARIABLEHEADER_H

#include <cleanMqtt/Interfaces/IDecodeHeader.h>
#include "cleanMqtt/Mqtt/Packets/Connection/Flags/ConnectAknowledgeFlags.h"
#include "cleanMqtt/Mqtt/Packets/Connection/Codes/ConnectReasonCode.h"
#include <cleanMqtt/Mqtt/Packets/Properties.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		struct ConnectAckVariableHeader : public IDecodeHeader
		{
		public:
			ConnectAckVariableHeader() noexcept;

			DecodeResult decode(const ByteBuffer& buffer) noexcept override;

			EncodedConnectAcknowledgeFlags flags;
			ConnectReasonCode reasonCode;
			Properties properties;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_PACKETS_CONNECT_HEADERS_CONNECTACKVARIABLEHEADER_H
