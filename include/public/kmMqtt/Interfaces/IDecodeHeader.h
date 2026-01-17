// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_INTERFACES_IDECODEHEADER_H
#define INCLUDE_KMMQTT_INTERFACES_IDECODEHEADER_H

#include <kmMqtt/ByteBuffer.h>
#include <kmMqtt/Mqtt/Packets/ErrorCodes.h>

namespace kmMqtt
{
	class IDecodeHeader
	{
	public:
		virtual ~IDecodeHeader() = default;
		virtual mqtt::DecodeResult decode(const ByteBuffer& buffer) = 0;
	};
}
#endif //INCLUDE_KMMQTT_INTERFACES_IDECODEHEADER_H