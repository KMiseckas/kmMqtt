#include <cleanMqtt/Mqtt/Packets/BasePacket.h>
#include <cleanMqtt/Interfaces/ILogger.h>
#include <stdexcept>

namespace cleanMqtt
{
	namespace mqtt
	{
		namespace packets
		{
			BasePacket::BasePacket(const FixedHeaderFlags& flags) noexcept
			{
				m_fixedHeader.flags = flags;
			}

			BasePacket::BasePacket(ByteBuffer&& dataBuffer) noexcept
				: m_dataBuffer(new ByteBuffer(std::move(dataBuffer)))
			{
			}

			BasePacket::BasePacket(BasePacket&& other) noexcept
				: m_fixedHeader{ std::move(other.m_fixedHeader) },
				m_otherDecodeHeaders{std::move(other.m_otherDecodeHeaders)},
				m_otherEncodeHeaders{std::move(other.m_otherEncodeHeaders)},
				m_dataBuffer{std::move(other.m_dataBuffer)}
			{
				other.m_dataBuffer = nullptr;
			}

			BasePacket::~BasePacket()
			{
				m_otherDecodeHeaders.clear();
				m_otherEncodeHeaders.clear();
				delete m_dataBuffer;
			}

			void BasePacket::encode()
			{
				delete m_dataBuffer;

				m_fixedHeader.packetType = getPacketType();
				m_fixedHeader.remainingLength = static_cast<std::uint32_t>(calculateFixedHeaderRemainingLength());
				const std::size_t bufferCapacity = m_fixedHeader.getEncodedBytesSize() + static_cast<std::size_t>(m_fixedHeader.remainingLength.uint32Value());

				if (bufferCapacity < 2)
				{
					Exception(LogLevel::Error, "BasePacket", std::runtime_error("Not enough packet data. Cannot encode packet with less than 2 bytes of data."));
					return;
				}

				m_dataBuffer = new ByteBuffer(bufferCapacity);

				m_fixedHeader.encode(*m_dataBuffer);
				for (const interfaces::IEncodeHeader* header : m_otherEncodeHeaders)
				{
					header->encode(*m_dataBuffer);
				}
			}

			void BasePacket::decode()
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

			const FixedHeader& BasePacket::getFixedHeader() const
			{
				return m_fixedHeader;
			}

			const ByteBuffer* BasePacket::getDataBuffer() const
			{
				return m_dataBuffer;
			}

			std::size_t BasePacket::calculateFixedHeaderRemainingLength() const
			{
				std::size_t remainingLength{ 0 };

				for (const interfaces::IEncodeHeader* header : m_otherEncodeHeaders)
				{
					remainingLength += header->getEncodedBytesSize();
				}

				return remainingLength;
			}

			void BasePacket::addEncodeHeader(const interfaces::IEncodeHeader* header)
			{
				m_otherEncodeHeaders.push_back(header);
			}

			void BasePacket::addDecodeHeader(const interfaces::IDecodeHeader* header)
			{
				m_otherDecodeHeaders.push_back(header);
			}
		}
	}
}

