#ifndef INCLUDE_CLEANMQTT_INTERFACES_IENCODEHEADER_H
#define INCLUDE_CLEANMQTT_INTERFACES_IENCODEHEADER_H

#include <cleanMqtt/ByteBuffer.h>

namespace cleanMqtt
{
	class IEncodeHeader
	{
	public:
		virtual ~IEncodeHeader() = default;
		virtual void encode(ByteBuffer& buffer) const = 0;
		virtual std::size_t getEncodedBytesSize() const noexcept = 0;
	};
}
#endif //INCLUDE_CLEANMQTT_INTERFACES_IENCODEHEADER_H