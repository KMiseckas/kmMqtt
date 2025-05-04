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

			EncodeResult BasePacket::encode()
			{
				EncodeResult result;
				result.packetType = getPacketType();

				delete m_dataBuffer;

				m_fixedHeader.packetType = getPacketType();
				m_fixedHeader.remainingLength = static_cast<std::uint32_t>(calculateFixedHeaderRemainingLength());
				const std::size_t bufferCapacity = m_fixedHeader.getEncodedBytesSize() + static_cast<std::size_t>(m_fixedHeader.remainingLength.uint32Value());

				try
				{
					if (bufferCapacity < 2)
					{
						LogException("BasePacket", std::runtime_error("Not enough packet data. Cannot encode packet with less than 2 bytes of data."));
					}

					m_dataBuffer = new ByteBuffer(bufferCapacity);

					m_fixedHeader.encode(*m_dataBuffer);
					for (const interfaces::IEncodeHeader* header : m_otherEncodeHeaders)
					{
						header->encode(*m_dataBuffer);
					}
				}
				catch (const std::exception& e)
				{
					result.code = EncodeErrorCode::INTERNAL_ERROR;
					result.reason = e.what();
				}

				return result;
			}

			DecodeResult BasePacket::decode()
			{
				DecodeResult result;
				result.packetType = getPacketType();

				if (m_dataBuffer == nullptr)
				{
					LogError("BasePacket", "Cannot decode packet. Is `nullptr` for data buffer.");
					result.code = DecodeErrorCode::INTERNAL_ERROR;
					result.reason = "Null ref buffer in packet.";
					return result;
				}

				if (m_dataBuffer->size() < 2)
				{
					LogError("BasePacket", "Cannot decode packet with less than 2 bytes of data.");
					result.code = DecodeErrorCode::MALFORMED_PACKET;
					result.reason = "Received packet data is less than 2 bytes.";
					return result;
				}

				try
				{
					result = std::move(m_fixedHeader.decode(*m_dataBuffer));

					if (result.isSuccess())
					{
						for (interfaces::IDecodeHeader* header : m_otherDecodeHeaders)
						{
							result = std::move(header->decode(*m_dataBuffer));

							if (!result.isSuccess())
							{
								result.packetType = getPacketType();
								return result;
							}
						}
					}
				}
				catch(const std::exception& e)
				{
					result.code = DecodeErrorCode::INTERNAL_ERROR;
					result.reason = e.what();
				}

				result.packetType = getPacketType();
				return result;
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

			void BasePacket::onFixedHeaderDecoded() const
			{
				//Do nothing by default.
			}

			void BasePacket::addEncodeHeader(const interfaces::IEncodeHeader* header)
			{
				m_otherEncodeHeaders.push_back(header);
			}

			void BasePacket::addDecodeHeader(interfaces::IDecodeHeader* header)
			{
				m_otherDecodeHeaders.push_back(header);
			}
		}
	}
}

