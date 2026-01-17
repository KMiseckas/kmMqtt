// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INTERFACE_KMMQTT_MQTT_PACKETS_PUBLISH_HEADERS_PUBACKVARIABLEHEADER_H
#define INTERFACE_KMMQTT_MQTT_PACKETS_PUBLISH_HEADERS_PUBACKVARIABLEHEADER_H

#include <kmMqtt/Interfaces/IDecodeHeader.h>
#include <kmMqtt/Interfaces/IEncodeHeader.h>
#include <kmMqtt/Mqtt/Packets/Properties.h>
#include "kmMqtt/Mqtt/Packets/Publish/Codes/PubAckReasonCode.h"

namespace kmMqtt
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

#endif // INTERFACE_KMMQTT_MQTT_PACKETS_PUBLISH_HEADERS_PUBACKVARIABLEHEADER_H
