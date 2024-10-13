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

			BasePacket::~BasePacket()
			{
				m_otherDecodeHeaders.clear();
				m_otherEncodeHeaders.clear();
				delete m_dataBuffer;
			}

			const FixedHeader& BasePacket::getFixedHeader() const
			{
				return m_fixedHeader;
			}

			const ByteBuffer* BasePacket::getDataBuffer() const
			{
				return m_dataBuffer;
			}
		}
	}
}

