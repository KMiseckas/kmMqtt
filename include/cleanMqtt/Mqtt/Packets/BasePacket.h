#ifndef INCLUDE_CLEANMQTT_MQTT_PACKETS_PACKET_H
#define INCLUDE_CLEANMQTT_MQTT_PACKETS_PACKET_H

#include <cleanMqtt/ByteBuffer.h>
#include <cleanMqtt/Mqtt/Packets/PacketUtils.h>
#include <cleanMqtt/Mqtt/Packets/PacketType.h>
#include <cleanMqtt/Mqtt/Packets/FixedHeader.h>
#include <cleanMqtt/GlobalMacros.h>
#include <cleanMqtt/Interfaces/IEncodeHeader.h>

#include <cstdint>
#include <vector>
#include <type_traits>
#include <memory>
#include <cassert>

namespace cleanMqtt
{
	namespace mqtt
	{
		namespace packets
		{
			class BasePacket
			{
			public:
				DELETE_COPY_ASSIGNMENT_AND_CONSTRUCTOR(BasePacket)
				DELETE_MOVE_ASSIGNMENT_AND_CONSTRUCTOR(BasePacket)

				BasePacket() = delete;
				BasePacket(const FixedHeaderFlags& flags) noexcept;
				BasePacket(ByteBuffer&& dataBuffer) noexcept;
				virtual ~BasePacket();

				virtual PacketType getPacketType() const noexcept = 0;

				void encode()
				{
					delete m_dataBuffer;

					m_fixedHeader.packetType = getPacketType();
					m_fixedHeader.remainingLength = static_cast<std::uint32_t>(calculateFixedHeaderRemainingLength());
					const std::size_t bufferCapacity = m_fixedHeader.getEncodedBytesSize() + static_cast<std::size_t>(m_fixedHeader.remainingLength.uint32Value());

					if (bufferCapacity < 2)
					{
						Exception(LogLevel::Error, "BasePacket", std::runtime_error("Not enough packet data. Cannot encode packet with less than 2 bytes of data."));
					}

					m_dataBuffer = new ByteBuffer(bufferCapacity);

					m_fixedHeader.encode(*m_dataBuffer);
					for (const interfaces::IEncodeHeader* header : m_otherEncodeHeaders)
					{
						header->encode(*m_dataBuffer);
					}
				}

				void decode()
				{
					if (m_dataBuffer == nullptr)
					{
						Exception(LogLevel::Error, "Connect", std::runtime_error("Cannot decode packet with no `nullptr` for data buffer."));
						return;
					}

					if (m_dataBuffer->size() < 2)
					{
						Exception(LogLevel::Error, "Connect", std::runtime_error("Not enough packet data. Cannot decode packet with less than 2 bytes of buffer data."));
						return;
					}
				}

				const FixedHeader& getFixedHeader() const;
				const ByteBuffer* getDataBuffer() const;

			protected:
				std::size_t calculateFixedHeaderRemainingLength() const
				{
					std::size_t remainingLength{ 0 };

					for (const interfaces::IEncodeHeader* header : m_otherEncodeHeaders)
					{
						remainingLength += header->getEncodedBytesSize();
					}

					return remainingLength;
				}

				void addEncodeHeader(const interfaces::IEncodeHeader* header)
				{
					m_otherEncodeHeaders.push_back(header);
				}

				void addDecodeHeader(const interfaces::IDecodeHeader* header)
				{
					m_otherDecodeHeaders.push_back(header);
				}

			private:
				FixedHeader m_fixedHeader;

				std::vector<const interfaces::IEncodeHeader*> m_otherEncodeHeaders;
				std::vector<const interfaces::IDecodeHeader*> m_otherDecodeHeaders;
				ByteBuffer* m_dataBuffer{ nullptr };
			};
		}
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_PACKETS_PACKET_H