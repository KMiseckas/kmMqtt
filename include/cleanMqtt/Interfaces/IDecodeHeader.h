#ifndef INCLUDE_CLEANMQTT_INTERFACES_IDECODEHEADER_H
#define INCLUDE_CLEANMQTT_INTERFACES_IDECODEHEADER_H

#include <cleanMqtt/ByteBuffer.h>

namespace cleanMqtt
{
	namespace interfaces
	{
		class IDecodeHeader
		{
		public:
			virtual void decode(const ByteBuffer& buffer) noexcept = 0;
		};
	}
}
#endif //INCLUDE_CLEANMQTT_INTERFACES_IDECODEHEADER_H