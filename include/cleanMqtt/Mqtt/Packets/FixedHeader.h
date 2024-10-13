#ifndef INCLUDE_CLEANMQTT_MQTT_PACKETS_FIXEDHEADER_H
#define INCLUDE_CLEANMQTT_MQTT_PACKETS_FIXEDHEADER_H

#include <cleanMqtt/Interfaces/IDecodeHeader.h>
#include <cleanMqtt/Interfaces/IEncodeHeader.h>
#include <cleanMqtt/Mqtt/Packets/PacketType.h>
#include <cleanMqtt/Mqtt/Packets/FixedHeaderFlags.h>
#include <cleanMqtt/Mqtt/Packets/DataTypes.h>
#include <cstdint>

namespace cleanMqtt
{
	namespace mqtt
	{
		namespace packets
		{
			struct FixedHeader : 
				public interfaces::IEncodeHeader, 
				public interfaces::IDecodeHeader
			{
				void encode(ByteBuffer& buffer) const override
				{
					//Packet Type: MSB 4 bits
					//Flags: LSB 4 bits
					const std::uint8_t firstByte = ((0 | static_cast<std::uint8_t>(packetType)) << 4) | flags.getFlags();
					buffer += firstByte;
				
					//Remaining length of packet data excluding fixed header bytes as variable byte integer. (Max 4 bytes) 
					const std::uint32_t length = remainingLength.uint32EncodedBytes();
					static constexpr std::size_t maxBytesShift = sizeof(std::uint32_t) - 1;
					for (size_t i = 0; i < remainingLength.encodingSize(); ++i)
					{
						buffer += ((length >> ((maxBytesShift - i) * 8)) & 0xff);
					}
				}

				void decode(const ByteBuffer& /*buffer*/) noexcept override
				{
					assert(false);
				}

				std::size_t getEncodedBytesSize() const noexcept
				{
					return sizeof(std::uint8_t) + remainingLength.encodingSize();
				}

				PacketType packetType;
				FixedHeaderFlags flags{ 0U };
				VariableByteInteger remainingLength{ 0 };
			};
		}
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_PACKETS_FIXEDHEADER_H
