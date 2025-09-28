#ifndef INCLUDE_CLEANMQTT_MQTT_PACKETS_SUBSCRIBE_SUBSCRIBE_H
#define INCLUDE_CLEANMQTT_MQTT_PACKETS_SUBSCRIBE_SUBSCRIBE_H

#include <cleanMqtt/Mqtt/Packets/BasePacket.h>
#include <cleanMqtt/Mqtt/Packets/Subscribe/Headers/SubscribeVariableHeader.h>
#include <cleanMqtt/Mqtt/Packets/Subscribe/Headers/SubscribePayloadHeader.h>

namespace cleanMqtt
{
    namespace mqtt
    {
        namespace packets
        {
            namespace
            {
                constexpr std::uint8_t k_SubscribeFixedHeaderFlags = 0x02U;
            }

            class Subscribe : public BasePacket
            {
            public:
                Subscribe(SubscribeVariableHeader&& variableHeader, SubscribePayloadHeader&& payloadHeader) noexcept;
                Subscribe(ByteBuffer&& dataBuffer) noexcept;
                Subscribe(Subscribe&& other) noexcept;
                ~Subscribe() override;

                PacketType getPacketType() const noexcept override;

                const SubscribeVariableHeader& getVariableHeader() const;
                const SubscribePayloadHeader& getPayloadHeader() const;

            private:
                void setUpHeaders() noexcept;

                SubscribeVariableHeader* m_variableHeader{ nullptr };
                SubscribePayloadHeader* m_payloadHeader{ nullptr };
            };
        }
    }
}

#endif //INCLUDE_CLEANMQTT_MQTT_PACKETS_SUBSCRIBE_SUBSCRIBE_H
