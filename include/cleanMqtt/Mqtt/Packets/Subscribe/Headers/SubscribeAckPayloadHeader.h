#ifndef INTERFACE_CLEANMQTT_MQTT_PACKETS_SUBSCRIBE_HEADERS_SUBSCRIBEACKPAYLOADHEADER_H
#define INTERFACE_CLEANMQTT_MQTT_PACKETS_SUBSCRIBE_HEADERS_SUBSCRIBEACKPAYLOADHEADER_H

#include <vector>
#include <cstdint>
#include <cleanMqtt/Interfaces/IDecodeHeader.h>
#include <cleanMqtt/Mqtt/Packets/Subscribe/Codes/SubAckReasonCode.h>

namespace cleanMqtt
{
    namespace mqtt
    {
        namespace packets
        {
            struct SubscribeAckPayloadHeader : public interfaces::IDecodeHeader
            {
                SubscribeAckPayloadHeader() noexcept = default;

                DecodeResult decode(const ByteBuffer& buffer) noexcept override;

                std::vector<SubAckReasonCode> reasonCodes;
            };
        }
    }
}

#endif // INTERFACE_CLEANMQTT_MQTT_PACKETS_SUBSCRIBE_HEADERS_SUBSCRIBEACKPAYLOADHEADER_H
