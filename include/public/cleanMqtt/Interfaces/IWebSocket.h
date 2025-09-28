#ifndef INCLUDE_CLEANMQTT_INTERFACES_IWEBSOCKET_H
#define INCLUDE_CLEANMQTT_INTERFACES_IWEBSOCKET_H

#include <cleanMqtt/GlobalMacros.h>
#include <cleanMqtt/ByteBuffer.h>
#include <functional>

namespace cleanMqtt
{
	namespace interfaces
	{
		using OnConnectCallback = std::function<void(bool)>;
		using OnDisconnectCallback = std::function<void()>;
		using OnPacketRecvdCallback = std::function<void(ByteBuffer&&)>;
		using OnErrorCallback = std::function<void(std::uint16_t)>;

		class PUBLIC_API IWebSocket
		{
		public:
			virtual ~IWebSocket()
			{
			};

			virtual bool connect(const std::string& hostname, const std::string& port = "80") noexcept = 0;
			virtual bool send(const ByteBuffer& data) noexcept = 0;
			virtual bool close() noexcept = 0;
			virtual void tick() noexcept = 0;

			virtual bool isConnected() const noexcept = 0;
			virtual int getLastError() const noexcept = 0;
			virtual int getLastCloseCode() const noexcept = 0;
			virtual const char* getLastCloseReason() const noexcept = 0;

			virtual void setOnConnectCallback(OnConnectCallback callback) noexcept = 0;
			virtual void setOnDisconnectCallback(OnDisconnectCallback callback) noexcept = 0;
			virtual void setOnPacketRecvdCallback(OnPacketRecvdCallback callback) noexcept = 0;
			virtual void setOnErrorCallback(OnErrorCallback callback) noexcept = 0;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_INTERFACES_IWEBSOCKET_H 
