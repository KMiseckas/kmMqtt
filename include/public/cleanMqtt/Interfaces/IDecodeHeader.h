#ifndef INCLUDE_CLEANMQTT_INTERFACES_IDECODEHEADER_H
#define INCLUDE_CLEANMQTT_INTERFACES_IDECODEHEADER_H

#include <cleanMqtt/ByteBuffer.h>
#include <cleanMqtt/Mqtt/Packets/ErrorCodes.h>

namespace cleanMqtt
{
	namespace interfaces
	{
		class IDecodeHeader
		{
		public:
			virtual mqtt::packets::DecodeResult decode(const ByteBuffer& buffer) = 0;
		};
	}
}
#endif //INCLUDE_CLEANMQTT_INTERFACES_IDECODEHEADER_H