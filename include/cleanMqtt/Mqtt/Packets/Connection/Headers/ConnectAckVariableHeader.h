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
		namespace packets
		{
			struct ConnectAckVariableHeader : public interfaces::IDecodeHeader
			{
			public:
				ConnectAckVariableHeader() noexcept;

				void decode(const ByteBuffer& buffer) noexcept override;

				EncodedConnectAcknowledgeFlags flags;
				ConnectReasonCode reasonCode;
				Properties properties;
			};
		}
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_PACKETS_CONNECT_HEADERS_CONNECTACKVARIABLEHEADER_H
