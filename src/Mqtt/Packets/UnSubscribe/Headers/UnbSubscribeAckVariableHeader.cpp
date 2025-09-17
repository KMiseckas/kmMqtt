#include <cleanMqtt/Mqtt/Packets/UnSubscribe/Headers/UnSubscribeAckVariableHeader.h>

namespace cleanMqtt
{
    namespace mqtt
    {
        namespace packets
        {
            DecodeResult UnSubscribeAckVariableHeader::decode(const ByteBuffer& buffer) noexcept
            {
                DecodeResult result;

                packetId = buffer.readUInt16();

                if (packetId == 0)
                {
                    result.code = DecodeErrorCode::PROTOCOL_ERROR;
                    result.reason = "Packet ID cannot be zero in UNSUBACK";
                    return result;
                }

                if (buffer.readHeadroom() > 0)
                {
                    result = properties.decode(buffer);
                }

                return result;
            }
        }
    }
}
