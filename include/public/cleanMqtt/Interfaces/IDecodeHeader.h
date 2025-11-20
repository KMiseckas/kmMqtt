#ifndef INCLUDE_CLEANMQTT_INTERFACES_IDECODEHEADER_H
#define INCLUDE_CLEANMQTT_INTERFACES_IDECODEHEADER_H

#include <cleanMqtt/ByteBuffer.h>
#include <cleanMqtt/Mqtt/Packets/ErrorCodes.h>

namespace cleanMqtt
{
	class IDecodeHeader
	{
	public:
		virtual ~IDecodeHeader() = default;
		virtual mqtt::DecodeResult decode(const ByteBuffer& buffer) = 0;
	};
}
#endif //INCLUDE_CLEANMQTT_INTERFACES_IDECODEHEADER_H