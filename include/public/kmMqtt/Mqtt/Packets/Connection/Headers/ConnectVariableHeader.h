#ifndef INTERFACE_KMMQTT_MQTT_PACKETS_CONNECT_HEADERS_CONNECTVARIABLEHEADER_H
#define INTERFACE_KMMQTT_MQTT_PACKETS_CONNECT_HEADERS_CONNECTVARIABLEHEADER_H

#include <kmMqtt/Interfaces/IEncodeHeader.h>
#include <kmMqtt/Mqtt/Packets/Connection/Flags/ConnectFlags.h>
#include <kmMqtt/Mqtt/Packets/Properties.h>
#include <kmMqtt/Mqtt/Enums/MqttVersion.h>
#include <cstdint>

namespace kmMqtt
{
	namespace mqtt
	{
		struct ConnectVariableHeader : IEncodeHeader
		{
		public:
			ConnectVariableHeader() noexcept;
			ConnectVariableHeader(
				const char* protocolName,
				MqttVersion mqttVersion,
				std::uint16_t keepAlive,
				EncodedConnectFlags&& flags,
				Properties&& properties) noexcept;

			void encode(ByteBuffer& buffer) const override;
			std::size_t getEncodedBytesSize() const noexcept override;

			UTF8String protocolName{ "" };
			MqttVersion protocolLevel{ MqttVersion::MQTT_5_0 };
			std::uint16_t keepAliveInSec{ 0 };
			EncodedConnectFlags flags;
			Properties properties;
		};
	}
}

#endif //INTERFACE_KMMQTT_MQTT_PACKETS_CONNECT_HEADERS_CONNECTVARIABLEHEADER_H 