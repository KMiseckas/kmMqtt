#include <kmMqtt/Mqtt/Packets/Publish/PublishRec.h>

namespace kmMqtt
{
	namespace mqtt
	{
		PublishRec::PublishRec(PubRecVariableHeader&& variableHeader) noexcept
			: BasePacket(FixedHeaderFlags(0U)), m_variableHeader{ new PubRecVariableHeader(std::move(variableHeader)) }
		{
			setUpHeaders();
		}

		PublishRec::PublishRec(ByteBuffer&& dataBuffer) noexcept
			: BasePacket(std::move(dataBuffer))
		{
			setUpHeaders();
		}

		PublishRec::PublishRec(PublishRec&& other) noexcept
			: BasePacket(std::move(other)),
			m_variableHeader(other.m_variableHeader)
		{
			other.m_variableHeader = nullptr;
		}

		PublishRec::~PublishRec()
		{
			delete m_variableHeader;
		}

		PacketType PublishRec::getPacketType() const noexcept
		{
			return PacketType::PUBLISH_RECEIVED;
		}

		const PubRecVariableHeader& PublishRec::getVariableHeader() const
		{
			return *m_variableHeader;
		}

		void PublishRec::setUpHeaders() noexcept
		{
			if (m_variableHeader == nullptr)
			{
				m_variableHeader = new PubRecVariableHeader();
			}

			addEncodeHeader(m_variableHeader);
			addDecodeHeader(m_variableHeader);
		}

		void PublishRec::onFixedHeaderDecoded() const
		{
			// No additional logic required for PUBREC fixed header
		}
	}
}