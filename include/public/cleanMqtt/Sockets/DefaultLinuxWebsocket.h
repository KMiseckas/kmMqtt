#ifndef INCLUDE_PUBLIC_CLEANMQTT_SOCKETS_DEFAULTLINUXWEBSOCKET_H
#define INCLUDE_PUBLIC_CLEANMQTT_SOCKETS_DEFAULTLINUXWEBSOCKET_H

#include <cleanMqtt/Interfaces/IWebSocket.h>

namespace cleanMqtt
{
	class PUBLIC_API DefaultLinuxWebsocket : public IWebSocket
	{
	public:
		DefaultLinuxWebsocket();
		~DefaultLinuxWebsocket() override;

		bool connect(const std::string& hostname, const std::string& port = "80") noexcept override;
		int send(const ByteBuffer& data) noexcept override;
		bool close() noexcept override;
		void tick() noexcept override;
		bool isConnected() const noexcept override;

		int getLastError() const noexcept override;
		int getLastCloseCode() const noexcept override;
		const char* getLastCloseReason() const noexcept override;

		void setOnConnectCallback(OnConnectCallback callback) noexcept override { m_onConnectCallback = std::move(callback); }
		void setOnDisconnectCallback(OnDisconnectCallback callback) noexcept override { m_onDisconnectCallback = std::move(callback); }
		void setOnRecvdCallback(OnRecvdCallback callback) noexcept override { m_onRecvdCallback = std::move(callback); }
		void setOnErrorCallback(OnErrorCallback callback) noexcept override { m_onErrorCallback = std::move(callback); }
	private:

		OnConnectCallback m_onConnectCallback;
		OnDisconnectCallback m_onDisconnectCallback;
		OnRecvdCallback m_onRecvdCallback;
		OnErrorCallback m_onErrorCallback;
	};
}

#endif
