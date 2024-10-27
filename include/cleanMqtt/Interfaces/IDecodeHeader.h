#ifndef INCLUDE_CLEANMQTT_INTERFACES_IDECODEHEADER_H
#define INCLUDE_CLEANMQTT_INTERFACES_IDECODEHEADER_H

#include <cleanMqtt/ByteBuffer.h>
#include <cleanMqtt/Mqtt/Packets/ErrorCodes.h>

namespace cleanMqtt
{
	namespace interfaces
	{
		using namespace mqtt::packets;

		class IDecodeHeader
		{
		public:
			virtual DecodeResult decode(const ByteBuffer& buffer) noexcept = 0;
		};
	}
}
#endif //INCLUDE_CLEANMQTT_INTERFACES_IDECODEHEADER_H