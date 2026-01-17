#ifndef INCLUDE_KMMQTT_INTERFACES_IENCODEHEADER_H
#define INCLUDE_KMMQTT_INTERFACES_IENCODEHEADER_H

#include <kmMqtt/ByteBuffer.h>

namespace kmMqtt
{
	class IEncodeHeader
	{
	public:
		virtual ~IEncodeHeader() = default;
		virtual void encode(ByteBuffer& buffer) const = 0;
		virtual std::size_t getEncodedBytesSize() const noexcept = 0;
	};
}
#endif //INCLUDE_KMMQTT_INTERFACES_IENCODEHEADER_H