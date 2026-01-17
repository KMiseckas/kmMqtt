#include <kmMqtt/Mqtt/Packets/UnSubscribe/UnSubscribeAck.h>

namespace kmMqtt
{
	namespace mqtt
	{
		UnSubscribeAck::UnSubscribeAck(ByteBuffer&& dataBuffer) noexcept
			: BasePacket(std::move(dataBuffer))
		{
			setUpHeaders();
		}

		UnSubscribeAck::UnSubscribeAck(UnSubscribeAck&& other) noexcept
			: BasePacket(std::move(other))
		{
			m_variableHeader = other.m_variableHeader;
			m_payloadHeader = other.m_payloadHeader;
			other.m_variableHeader = nullptr;
			other.m_payloadHeader = nullptr;

			setUpHeaders();
		}

		UnSubscribeAck::~UnSubscribeAck()
		{
			delete m_variableHeader;
			delete m_payloadHeader;
		}

		PacketType UnSubscribeAck::getPacketType() const noexcept
		{
			return PacketType::UNSUBSCRIBE_ACKNOWLEDGE;
		}

		const UnSubscribeAckVariableHeader& UnSubscribeAck::getVariableHeader() const
		{
			return *m_variableHeader;
		}

		const UnSubscribeAckPayloadHeader& UnSubscribeAck::getPayloadHeader() const
		{
			return *m_payloadHeader;
		}

		void UnSubscribeAck::setUpHeaders() noexcept
		{
			if (m_variableHeader == nullptr)
			{
				m_variableHeader = new UnSubscribeAckVariableHeader();
			}

			if (m_payloadHeader == nullptr)
			{
				m_payloadHeader = new UnSubscribeAckPayloadHeader();
			}

			addDecodeHeader(m_variableHeader);
			addDecodeHeader(m_payloadHeader);
		}
	}
}