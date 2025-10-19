#include <cleanMqtt/Mqtt/Packets/Connection/Connect.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		Connect::Connect(ConnectVariableHeader&& variableHeader, ConnectPayloadHeader&& payloadHeader) noexcept
			: BasePacket(FixedHeaderFlags(k_ConnectFixedHeaderFlags)),
			m_variableHeader(new ConnectVariableHeader(std::move(variableHeader))),
			m_payloadHeader(new ConnectPayloadHeader(std::move(payloadHeader)))
		{
			setUpHeaders();
		}

		Connect::Connect(ByteBuffer&& dataBuffer) noexcept
			: BasePacket(std::move(dataBuffer))
		{
			setUpHeaders();
		}

		Connect::Connect(Connect&& other) noexcept
			: BasePacket{ std::move(other) },
			m_variableHeader{ other.m_variableHeader },
			m_payloadHeader{ other.m_payloadHeader }
		{
			other.m_variableHeader = nullptr;
			other.m_payloadHeader = nullptr;
		}

		Connect::~Connect()
		{
			delete m_payloadHeader;
			delete m_variableHeader;
		}

		PacketType Connect::getPacketType() const noexcept
		{
			return PacketType::CONNECT;
		}

		const ConnectVariableHeader& Connect::getVariableHeader() const
		{
			return *m_variableHeader;
		}

		const ConnectPayloadHeader& Connect::getPayloadHeader() const
		{
			return *m_payloadHeader;
		}

		void Connect::setUpHeaders() noexcept
		{
			if (m_variableHeader == nullptr)
			{
				m_variableHeader = new ConnectVariableHeader();
			}

			if (m_payloadHeader == nullptr)
			{
				m_payloadHeader = new ConnectPayloadHeader();
			}

			addEncodeHeader(m_variableHeader);
			addEncodeHeader(m_payloadHeader);
		}
	}
}