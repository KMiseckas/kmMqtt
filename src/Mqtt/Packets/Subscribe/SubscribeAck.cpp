#include <cleanMqtt/Mqtt/Packets/Subscribe/SubscribeAck.h>

namespace cleanMqtt
{
    namespace mqtt
    {
        namespace packets
        {
            SubscribeAck::SubscribeAck(ByteBuffer&& dataBuffer) noexcept
                : BasePacket(std::move(dataBuffer))
            {
                setUpHeaders();
            }

            SubscribeAck::SubscribeAck(SubscribeAck&& other) noexcept
                : BasePacket(std::move(other))
            {
                m_variableHeader = other.m_variableHeader;
                m_payloadHeader = other.m_payloadHeader;
                other.m_variableHeader = nullptr;
                other.m_payloadHeader = nullptr;

                setUpHeaders();
            }

            SubscribeAck::~SubscribeAck()
            {
                delete m_variableHeader;
                delete m_payloadHeader;
            }

            PacketType SubscribeAck::getPacketType() const noexcept
            {
                return PacketType::SUBSCRIBE_ACKNOWLEDGE;
            }

            const SubscribeAckVariableHeader& SubscribeAck::getVariableHeader() const
            {
                return *m_variableHeader;
            }

            const SubscribeAckPayloadHeader& SubscribeAck::getPayloadHeader() const
            {
                return *m_payloadHeader;
            }

            void SubscribeAck::setUpHeaders() noexcept
            {
                if (m_variableHeader == nullptr)
                {
                    m_variableHeader = new SubscribeAckVariableHeader();
                }

                if (m_payloadHeader == nullptr)
                {
                    m_payloadHeader = new SubscribeAckPayloadHeader();
                }

                addDecodeHeader(m_variableHeader);
                addDecodeHeader(m_payloadHeader);
            }
        }
    }
}
