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
		struct FixedHeader :
			public IEncodeHeader,
			public IDecodeHeader
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

			DecodeResult decode(const ByteBuffer& buffer) override
			{
				//Packet Type: MSB 4 bits
				//Flags: LSB 4 bits
				const std::uint8_t firstByte = buffer.readUint8();
				packetType = static_cast<PacketType>(firstByte >> 4);
				flags.overrideFlags(firstByte & 0b00001111);
				remainingLength.decode(buffer);

				if (remainingLength.uint32Value() + getEncodedBytesSize() != buffer.size())
				{
					return DecodeResult{ DecodeErrorCode::PROTOCOL_ERROR, "Fixed header flags & type + remaining length does not equal the buffer size received from socket." };
				}

				return DecodeResult{ DecodeErrorCode::NO_ERROR };
			}

			std::size_t getEncodedBytesSize() const noexcept override
			{
				return sizeof(std::uint8_t) + remainingLength.encodingSize();
			}

			PacketType packetType;
			FixedHeaderFlags flags{ 0U };
			VariableByteInteger remainingLength;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_PACKETS_FIXEDHEADER_H
