// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MQTT_PACKETS_CONNECT_HEADERS_CONNECTACKVARIABLEHEADER_H
#define INCLUDE_KMMQTT_MQTT_PACKETS_CONNECT_HEADERS_CONNECTACKVARIABLEHEADER_H

#include <kmMqtt/Interfaces/IDecodeHeader.h>
#include "kmMqtt/Mqtt/Packets/Connection/Flags/ConnectAknowledgeFlags.h"
#include "kmMqtt/Mqtt/Packets/Connection/Codes/ConnectReasonCode.h"
#include <kmMqtt/Mqtt/Packets/Properties.h>

namespace kmMqtt
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

#endif //INCLUDE_KMMQTT_MQTT_PACKETS_CONNECT_HEADERS_CONNECTACKVARIABLEHEADER_H
