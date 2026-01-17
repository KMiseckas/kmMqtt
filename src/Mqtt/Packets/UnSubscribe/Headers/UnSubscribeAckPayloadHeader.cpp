#include <kmMqtt/Mqtt/Packets/UnSubscribe/Headers/UnSubscribeAckPayloadHeader.h>

namespace kmMqtt
{
	namespace mqtt
	{
		DecodeResult UnSubscribeAckPayloadHeader::decode(const ByteBuffer& buffer) noexcept
		{
			reasonCodes.clear();

			while (buffer.readHeadroom() > 0)
			{
				reasonCodes.push_back(static_cast<UnSubAckReasonCode>(buffer.readUint8()));
			}

			return DecodeResult{ DecodeErrorCode::NO_ERROR };
		}
	}
}