#ifndef INCLUDE_CLEANMQTT_MQTT_PACKETS_UNSUBSCRIBE_UNSUBSCRIBE_H
#define INCLUDE_CLEANMQTT_MQTT_PACKETS_UNSUBSCRIBE_UNSUBSCRIBE_H

#include <cleanMqtt/Mqtt/Packets/BasePacket.h>
#include <cleanMqtt/Mqtt/Packets/UnSubscribe/Headers/UnSubscribeVariableHeader.h>
#include <cleanMqtt/Mqtt/Packets/UnSubscribe/Headers/UnSubscribePayloadHeader.h>
#include <vector>
#include <string>

namespace cleanMqtt
{
    namespace mqtt
    {
        namespace packets
        {
            namespace
            {
                constexpr std::uint8_t k_UnSubscribeFixedHeaderFlags = 0x02U;
            }

            class UnSubscribe : public BasePacket
            {
            public:
                UnSubscribe(UnSubscribeVariableHeader&& variableHeader, UnSubscribePayloadHeader&& payloadHeader) noexcept;
                UnSubscribe(ByteBuffer&& dataBuffer) noexcept;
                UnSubscribe(UnSubscribe&& other) noexcept;
                ~UnSubscribe() override;

                PacketType getPacketType() const noexcept override;
                const UnSubscribeVariableHeader& getVariableHeader() const;
                const UnSubscribePayloadHeader& getPayloadHeader() const;

            private:
                void setUpHeaders() noexcept;

                UnSubscribeVariableHeader* m_variableHeader{ nullptr };
                UnSubscribePayloadHeader* m_payloadHeader{ nullptr };
            };
        }
    }
}

#endif //INCLUDE_CLEANMQTT_MQTT_PACKETS_UNSUBSCRIBE_UNSUBSCRIBE_H
