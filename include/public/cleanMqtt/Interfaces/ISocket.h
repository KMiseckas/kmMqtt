#pragma once

#include <cleanMqtt/GlobalMacros.h>
#include <string>

namespace cleanMqtt
{
	enum class SocketState : uint8_t
	{
		_Not_Set = 0U,
		Connected,

	};

	class PUBLIC_API ISocket
	{
	public:
		virtual ~ISocket() = 0;

		virtual bool connect(const char* ip, const char* port) = 0;
		virtual bool send(const void* data, const int size, int& outBytesSent) = 0;
		virtual bool receive(void* outBuffer, const int maxBufferLength, int& outBytesReceived) = 0;
		virtual bool kill() = 0;

		virtual bool setOption(const int option, const void* val, const int length) = 0;
		virtual bool disableBlocking() = 0;
	};
}
