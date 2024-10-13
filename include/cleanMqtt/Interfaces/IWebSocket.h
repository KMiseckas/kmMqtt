#ifndef INCLUDE_CLEANMQTT_INTERFACES_IWEBSOCKET_H
#define INCLUDE_CLEANMQTT_INTERFACES_IWEBSOCKET_H

#include <cleanMqtt/GlobalMacros.h>
#include <cleanMqtt/ByteBuffer.h>

namespace cleanMqtt
{
	namespace interfaces
	{
		using OnConnectCallback = void(*)(bool);
		using OnDisconnectCallback = void(*)();
		using OnMessageRecvdCallback = void(*)(ByteBuffer&&);
		using OnErrorCallback = void(*)(std::uint16_t errorCode);

		class PUBLIC_API IWebSocket
		{
		public:
			virtual ~IWebSocket();

			virtual bool connect(const std::string& url) = 0;
			virtual bool send(const ByteBuffer& data) = 0;
			virtual void close() = 0;
			virtual void tick() = 0;

			virtual bool isConnected() const noexcept = 0;
			virtual int getLastError() const noexcept = 0;

			virtual void setOnConnectCallback(const OnConnectCallback& callback) noexcept = 0;
			virtual void setOnDisconnectCallback(const OnDisconnectCallback& callback) noexcept = 0;
			virtual void setOnMessageRecvdCallback(const OnMessageRecvdCallback& callback) noexcept = 0;
			virtual void setOnErrorCallback(const OnErrorCallback& callback) noexcept = 0;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_INTERFACES_IWEBSOCKET_H 
