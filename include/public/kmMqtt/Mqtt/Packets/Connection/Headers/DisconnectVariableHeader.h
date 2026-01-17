// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MQTT_PACKETS_CONNECT_HEADERS_DISCONNECTACKVARIABLEHEADER_H
#define INCLUDE_KMMQTT_MQTT_PACKETS_CONNECT_HEADERS_DISCONNECTACKVARIABLEHEADER_H

#include "kmMqtt/GlobalMacros.h"
#include <kmMqtt/Interfaces/IDecodeHeader.h>
#include <kmMqtt/Interfaces/IEncodeHeader.h>
#include "kmMqtt/Mqtt/Packets/Connection/Codes/DisconnectReasonCode.h"
#include <kmMqtt/Mqtt/Packets/Properties.h>

namespace kmMqtt
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

#endif //INCLUDE_KMMQTT_MQTT_PACKETS_CONNECT_HEADERS_DISCONNECTACKVARIABLEHEADER_H
