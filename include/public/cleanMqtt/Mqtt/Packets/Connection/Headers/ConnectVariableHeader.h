#ifndef INTERFACE_CLEANMQTT_MQTT_PACKETS_CONNECT_HEADERS_CONNECTVARIABLEHEADER_H
#define INTERFACE_CLEANMQTT_MQTT_PACKETS_CONNECT_HEADERS_CONNECTVARIABLEHEADER_H

#include <cleanMqtt/Interfaces/IEncodeHeader.h>
#include <cleanMqtt/Mqtt/Packets/Connection/Flags/ConnectFlags.h>
#include <cleanMqtt/Mqtt/Packets/Properties.h>
#include <cleanMqtt/Mqtt/Enums/MqttVersion.h>
#include <cstdint>

namespace cleanMqtt
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

#endif //INTERFACE_CLEANMQTT_MQTT_PACKETS_CONNECT_HEADERS_CONNECTVARIABLEHEADER_H 