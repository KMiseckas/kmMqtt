#include <kmMqtt/Mqtt/Packets/BasePacket.h>
#include <kmMqtt/Interfaces/ILogger.h>
#include <stdexcept>

namespace kmMqtt
{
	namespace mqtt
	{
		BasePacket::BasePacket(const FixedHeaderFlags& flags) noexcept
		{
			m_fixedHeader.flags = flags;
		}

		BasePacket::BasePacket(ByteBuffer&& dataBuffer) noexcept
			: m_dataBuffer(std::move(dataBuffer))
		{
		}

		BasePacket::BasePacket(BasePacket&& other) noexcept
			: m_fixedHeader{ std::move(other.m_fixedHeader) },
			m_otherDecodeHeaders{ std::move(other.m_otherDecodeHeaders) },
			m_otherEncodeHeaders{ std::move(other.m_otherEncodeHeaders) },
			m_dataBuffer{ std::move(other.m_dataBuffer) }
		{
			other.m_dataBuffer.clear();
		}

		BasePacket& BasePacket::operator=(BasePacket&& other) noexcept
		{
			if (this != &other)
			{
				m_fixedHeader = std::move(other.m_fixedHeader);
				m_otherEncodeHeaders = std::move(other.m_otherEncodeHeaders);
				m_otherDecodeHeaders = std::move(other.m_otherDecodeHeaders);

				m_dataBuffer = std::move(other.m_dataBuffer);
				other.m_dataBuffer.clear();
			}

			return *this;
		}

		BasePacket::~BasePacket()
		{
			m_otherDecodeHeaders.clear();
			m_otherEncodeHeaders.clear();
		}

		EncodeResult BasePacket::encode()
		{
			EncodeResult result;
			result.packetType = getPacketType();

			m_fixedHeader.packetType = getPacketType();

			//Calculate and set remaining length in fixed header
			if (!m_fixedHeader.remainingLength.setValue(static_cast<std::uint32_t>(calculateFixedHeaderRemainingLength())))
			{
				return EncodeResult{ EncodeErrorCode::INTERNAL_ERROR, "Failed to set remaining length in fixed header. Remaing length size" };
			}

			const std::size_t bufferCapacity{ m_fixedHeader.getEncodedBytesSize() + static_cast<std::size_t>(m_fixedHeader.remainingLength.uint32Value()) };

			try
			{
				//Minimum packet size is 2 bytes (1 byte for type and flags + minimum 1 byte for remaining length)
				if (bufferCapacity < 2)
				{
					LogException("BasePacket", std::runtime_error("Not enough packet data. Cannot encode packet with less than 2 bytes of data."));
				}

				m_dataBuffer.clear();
				m_dataBuffer.expand(bufferCapacity);

				m_fixedHeader.encode(m_dataBuffer);
				for (const IEncodeHeader* header : m_otherEncodeHeaders)
				{
					header->encode(m_dataBuffer);
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
			assert(m_dataBuffer.capacity() > 0);

			DecodeResult result;
			result.packetType = getPacketType();

			//Minimum packet size is 2 bytes (1 byte for type and flags + minimum 1 byte for remaining length)
			if (m_dataBuffer.size() < 2)
			{
				LogError("BasePacket", "Cannot decode packet with less than 2 bytes of data.");
				result.code = DecodeErrorCode::MALFORMED_PACKET;
				result.reason = "Received packet data is less than 2 bytes.";
				return result;
			}

			try
			{
				result = m_fixedHeader.decode(m_dataBuffer);
				onFixedHeaderDecoded();

				if (result.isSuccess())
				{
					for (IDecodeHeader* header : m_otherDecodeHeaders)
					{
						result = header->decode(m_dataBuffer);

						if (!result.isSuccess())
						{
							result.packetType = getPacketType();
							return result;
						}
					}
				}
			}
			catch (const std::exception& e)
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

		const ByteBuffer& BasePacket::getDataBuffer() const
		{
			return m_dataBuffer;
		}

		ByteBuffer&& BasePacket::extractDataBuffer() noexcept
		{
			return std::move(m_dataBuffer);
		}

		std::size_t BasePacket::calculateFixedHeaderRemainingLength() const
		{
			std::size_t remainingLength{ 0 };

			for (const IEncodeHeader* header : m_otherEncodeHeaders)
			{
				remainingLength += header->getEncodedBytesSize();
			}

			return remainingLength;
		}

		void BasePacket::onFixedHeaderDecoded() const
		{
			//Do nothing by default.
		}

		void BasePacket::addEncodeHeader(const IEncodeHeader* header)
		{
			m_otherEncodeHeaders.push_back(header);
		}

		void BasePacket::addDecodeHeader(IDecodeHeader* header)
		{
			m_otherDecodeHeaders.push_back(header);
		}
	}
}