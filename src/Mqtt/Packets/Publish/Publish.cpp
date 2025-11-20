#include <cleanMqtt/Mqtt/Packets/Publish/Publish.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		Publish::Publish(PublishPayloadHeader&& payloadHeader, PublishVariableHeader&& variableHeader, const EncodedPublishFlags& flags) noexcept
			:BasePacket(flags),
			m_payloadHeader{ new PublishPayloadHeader(std::move(payloadHeader)) },
			m_variableHeader{ new PublishVariableHeader(std::move(variableHeader)) }
		{
			setUpHeaders();
		}

		Publish::Publish(ByteBuffer&& dataBuffer) noexcept
			: BasePacket(std::move(dataBuffer))
		{
			setUpHeaders();
		}

		Publish::Publish(Publish&& other) noexcept
			: BasePacket(std::move(other)),
			m_payloadHeader(other.m_payloadHeader),
			m_variableHeader(other.m_variableHeader)
		{
			other.m_variableHeader = nullptr;
			other.m_payloadHeader = nullptr;
		}

		Publish::~Publish()
		{
			delete m_variableHeader;
		}

		PacketType Publish::getPacketType() const noexcept
		{
			return PacketType::PUBLISH;
		}

		const PublishVariableHeader& Publish::getVariableHeader() const
		{
			return *m_variableHeader;
		}

		const PublishPayloadHeader& Publish::getPayloadHeader() const
		{
			return *m_payloadHeader;
		}

		void Publish::setUpHeaders() noexcept
		{
			if (m_variableHeader == nullptr)
			{
				m_variableHeader = new PublishVariableHeader();
			}

			if (m_payloadHeader == nullptr)
			{
				m_payloadHeader = new PublishPayloadHeader();
			}

			addEncodeHeader(m_variableHeader);
			addEncodeHeader(m_payloadHeader);

			addDecodeHeader(m_variableHeader);
			addDecodeHeader(m_payloadHeader);
		}

		void Publish::onFixedHeaderDecoded() const
		{
			m_variableHeader->qos = static_cast<Qos>(getFixedHeader().flags.getFlagValue(static_cast<std::uint8_t>(PublishFlags::QOS)));
		}
	}
}