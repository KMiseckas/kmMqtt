#include <cleanMqtt/Mqtt/Packets/Publish/PublishAck.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		PublishAck::PublishAck(PubAckVariableHeader&& variableHeader) noexcept
			: BasePacket(FixedHeaderFlags(0U)), m_variableHeader{ new PubAckVariableHeader(std::move(variableHeader)) }
		{
			setUpHeaders();
		}

		PublishAck::PublishAck(ByteBuffer&& dataBuffer) noexcept
			: BasePacket(std::move(dataBuffer))
		{
			setUpHeaders();
		}

		PublishAck::PublishAck(PublishAck&& other) noexcept
			: BasePacket(std::move(other)),
			m_variableHeader(other.m_variableHeader)
		{
			other.m_variableHeader = nullptr;
		}

		PublishAck::~PublishAck()
		{
			delete m_variableHeader;
		}

		PacketType PublishAck::getPacketType() const noexcept
		{
			return PacketType::PUBLISH_ACKNOWLEDGE;
		}

		const PubAckVariableHeader& PublishAck::getVariableHeader() const
		{
			return *m_variableHeader;
		}

		void PublishAck::setUpHeaders() noexcept
		{
			if (m_variableHeader == nullptr)
			{
				m_variableHeader = new PubAckVariableHeader();
			}

			addEncodeHeader(m_variableHeader);
			addDecodeHeader(m_variableHeader);
		}

		void PublishAck::onFixedHeaderDecoded() const
		{
			// No additional logic required for PUBACK fixed header
		}
	}
}