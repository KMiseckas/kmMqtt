// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INTERFACE_KMMQTT_MQTT_PACKETS_SUBSCRIBE_HEADERS_SUBSCRIBEACKPAYLOADHEADER_H
#define INTERFACE_KMMQTT_MQTT_PACKETS_SUBSCRIBE_HEADERS_SUBSCRIBEACKPAYLOADHEADER_H

#include <vector>
#include <cstdint>
#include <kmMqtt/Interfaces/IDecodeHeader.h>
#include <kmMqtt/Mqtt/Packets/Subscribe/Codes/SubAckReasonCode.h>

namespace kmMqtt
{
	namespace mqtt
	{
		struct SubscribeAckPayloadHeader : public IDecodeHeader
		{
			SubscribeAckPayloadHeader() noexcept = default;

			DecodeResult decode(const ByteBuffer& buffer) noexcept override;

			std::vector<SubAckReasonCode> reasonCodes;
		};
	}
}

#endif // INTERFACE_KMMQTT_MQTT_PACKETS_SUBSCRIBE_HEADERS_SUBSCRIBEACKPAYLOADHEADER_H
