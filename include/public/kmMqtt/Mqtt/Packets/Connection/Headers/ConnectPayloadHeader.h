// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INTERFACE_KMMQTT_MQTT_PACKETS_CONNECT_HEADERS_CONNECTPAYLOADHEADER_H
#define INTERFACE_KMMQTT_MQTT_PACKETS_CONNECT_HEADERS_CONNECTPAYLOADHEADER_H

#include <kmMqtt/Mqtt/Packets/Properties.h>
#include <kmMqtt/Interfaces/IEncodeHeader.h>
#include <kmMqtt/Mqtt/Packets/DataTypes.h>
#include <cstdint>

namespace kmMqtt
{
	namespace mqtt
	{
		struct ConnectPayloadHeader : IEncodeHeader
		{
			ConnectPayloadHeader() noexcept;
			ConnectPayloadHeader(
				UTF8String&& clientId,
				Properties&& willProperties,
				UTF8String&& willTopic,
				BinaryData&& willPayload,
				UTF8String&& userName,
				BinaryData&& password) noexcept;

			void encode(ByteBuffer& buffer) const override;
			std::size_t getEncodedBytesSize() const noexcept override;

			UTF8String clientId;
			Properties willProperties;
			UTF8String willTopic;
			BinaryData willPayload;
			UTF8String userName;
			BinaryData password;
		};
	}
}

#endif //INTERFACE_KMMQTT_MQTT_PACKETS_CONNECT_HEADERS_CONNECTPAYLOADHEADER_H 