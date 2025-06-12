#ifndef INTERFACE_CLEANMQTT_MQTT_PACKETS_SUBSCRIBE_HEADERS_SUBSCRIBEPAYLOADHEADER_H
#define INTERFACE_CLEANMQTT_MQTT_PACKETS_SUBSCRIBE_HEADERS_SUBSCRIBEPAYLOADHEADER_H

#include <cleanMqtt/Interfaces/IEncodeHeader.h>
#include <cleanMqtt/Mqtt/Packets/DataTypes.h>
#include "cleanMqtt/Mqtt/Packets/Subscribe/Headers/Subscription.h"
#include <cstdint>
#include <vector>

namespace cleanMqtt
{
	namespace mqtt
	{
		namespace packets
		{
			struct SubscribePayloadHeader : interfaces::IEncodeHeader
			{
				SubscribePayloadHeader() noexcept = default;
				SubscribePayloadHeader(std::vector<Subscription>&& subscriptions) noexcept;

				void encode(ByteBuffer& buffer) const override;
				std::size_t getEncodedBytesSize() const noexcept;

				std::vector<Subscription> subscriptions;
			};
		}
	}
}

#endif // INTERFACE_CLEANMQTT_MQTT_PACKETS_SUBSCRIBE_HEADERS_SUBSCRIBEPAYLOADHEADER_H
