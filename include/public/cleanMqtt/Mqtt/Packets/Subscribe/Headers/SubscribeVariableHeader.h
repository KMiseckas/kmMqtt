#ifndef INTERFACE_CLEANMQTT_MQTT_PACKETS_SUBSCRIBE_HEADERS_SUBSCRIBEVARIABLEHEADER_H
#define INTERFACE_CLEANMQTT_MQTT_PACKETS_SUBSCRIBE_HEADERS_SUBSCRIBEVARIABLEHEADER_H

#include <cleanMqtt/Interfaces/IEncodeHeader.h>
#include <cleanMqtt/Mqtt/Packets/Properties.h>
#include <cstdint>

namespace cleanMqtt
{
	namespace mqtt
	{
		namespace packets
		{
			struct SubscribeVariableHeader : IEncodeHeader
			{
			public:
				SubscribeVariableHeader() noexcept;
				SubscribeVariableHeader(std::uint16_t packetId, Properties&& properties) noexcept;

				void encode(ByteBuffer& buffer) const override;
				std::size_t getEncodedBytesSize() const noexcept;

				std::uint16_t packetId{ 0 };
				Properties properties;
			};
		}
	}
}

#endif //INTERFACE_CLEANMQTT_MQTT_PACKETS_SUBSCRIBE_HEADERS_SUBSCRIBEVARIABLEHEADER_H
