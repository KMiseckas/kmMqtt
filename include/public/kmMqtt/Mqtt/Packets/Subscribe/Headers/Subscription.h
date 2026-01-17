#ifndef INTERFACE_KMMQTT_MQTT_PACKETS_SUBSCRIBE_HEADERS_SUBSCRIBE_HEADERS_SUBSCRIPTION_H
#define INTERFACE_KMMQTT_MQTT_PACKETS_SUBSCRIBE_HEADERS_SUBSCRIBE_HEADERS_SUBSCRIPTION_H


#include "kmMqtt/Mqtt/Packets/Subscribe/Flags/SubscribeOptionsFlags.h"
#include "kmMqtt/Mqtt/Packets/DataTypes.h"
#include <cstdint>
#include <vector>

namespace kmMqtt
{
	namespace mqtt
	{
		struct Subscription
		{
			Subscription() noexcept = default;
			Subscription(const char* topic, EncodedSubscribeOptionsFlags opts) noexcept;

			std::size_t getEncodedBytesSize() const noexcept;

			void encode(ByteBuffer& buffer) const;

			UTF8String topicFilter;
			EncodedSubscribeOptionsFlags options;
		};
	}
}

#endif //INTERFACE_KMMQTT_MQTT_PACKETS_SUBSCRIBE_HEADERS_SUBSCRIBE_HEADERS_SUBSCRIPTION_H
