#include <cleanMqtt/Mqtt/Packets/Subscribe/Headers/SubscribeAckPayloadHeader.h>
#include <cleanMqtt/Mqtt/Packets/Subscribe/Codes/SubAckReasonCode.h>

namespace cleanMqtt
{
    namespace mqtt
    {
        namespace packets
        {
            DecodeResult SubscribeAckPayloadHeader::decode(const ByteBuffer& buffer) noexcept
            {
                reasonCodes.clear();

                while (buffer.readHeadroom() > 0)
                {
                    reasonCodes.push_back(static_cast<SubAckReasonCode>(buffer.readUint8()));
                }

                return DecodeResult{ DecodeErrorCode::NO_ERROR };
            }
        }
    }
}
