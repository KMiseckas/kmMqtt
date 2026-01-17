// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INTERFACE_KMMQTT_MQTT_PACKETS_PUBLISH_HEADERS_PUBLISHVARIABLEHEADER_H
#define INTERFACE_KMMQTT_MQTT_PACKETS_PUBLISH_HEADERS_PUBLISHVARIABLEHEADER_H

#include <kmMqtt/Interfaces/IDecodeHeader.h>
#include <kmMqtt/Interfaces/IEncodeHeader.h>
#include <kmMqtt/Mqtt/Packets/Properties.h>
#include <kmMqtt/Mqtt/Enums/Qos.h>

namespace kmMqtt
{
	namespace mqtt
	{
		struct PublishVariableHeader : public IDecodeHeader, public IEncodeHeader
		{
			PublishVariableHeader() noexcept;
			PublishVariableHeader(
				const char* topicName,
				std::uint16_t packetId,
				Properties&& properties,
				Qos publishQOS) noexcept;

			DecodeResult decode(const ByteBuffer& buffer) noexcept override;
			void encode(ByteBuffer& buffer) const override;
			std::size_t getEncodedBytesSize() const noexcept override;

			UTF8String topicName{ "" };
			std::uint16_t packetIdentifier{ 0U };
			Properties properties;
			Qos qos{ Qos::QOS_0 };
		};
	}
}

#endif //INTERFACE_KMMQTT_MQTT_PACKETS_PUBLISH_HEADERS_PUBLISHVARIABLEHEADER_H 