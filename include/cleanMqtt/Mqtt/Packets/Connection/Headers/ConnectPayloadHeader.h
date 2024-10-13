#ifndef INTERFACE_CLEANMQTT_MQTT_PACKETS_CONNECT_HEADERS_CONNECTPAYLOADHEADER_H
#define INTERFACE_CLEANMQTT_MQTT_PACKETS_CONNECT_HEADERS_CONNECTPAYLOADHEADER_H

#include <cleanMqtt/Mqtt/Packets/Properties.h>
#include <cleanMqtt/Interfaces/IEncodeHeader.h>
#include <cleanMqtt/Mqtt/Packets/DataTypes.h>
#include <cstdint>

namespace cleanMqtt
{
	namespace mqtt
	{
		namespace packets
		{
			struct ConnectPayloadHeader : interfaces::IEncodeHeader
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
				std::size_t getEncodedBytesSize() const noexcept;

				UTF8String clientId;	
				Properties willProperties;
				UTF8String willTopic;
				BinaryData willPayload;
				UTF8String userName;
				BinaryData password;
			};
		}
	}
}

#endif //INTERFACE_CLEANMQTT_MQTT_PACKETS_CONNECT_HEADERS_CONNECTPAYLOADHEADER_H 