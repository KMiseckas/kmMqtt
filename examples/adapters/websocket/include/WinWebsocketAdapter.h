#ifndef INCLUDE_ADAPTERS_WEBSOCKETS_WINWEBSOCKETADAPTER_H
#define INCLUDE_ADAPTERS_WEBSOCKETS_WINWEBSOCKETADAPTER_H

#include <cleanMqtt/Interfaces/IWebSocket.h>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <functional>
#include <vector>
#include <iostream>
#include <mutex>
#include <thread>

#pragma comment(lib, "Ws2_32.lib")

namespace adapter
{
	using namespace cleanMqtt::interfaces;
	using namespace cleanMqtt;

	class WinWebsocketAdapter : public IWebSocket
	{
	public:
		WinWebsocketAdapter();
		~WinWebsocketAdapter() override;

		bool connect(const std::string& hostname, const std::string& port = "80") noexcept override;
		bool send(const ByteBuffer& data) noexcept override;
		bool close() noexcept override;
		void tick() noexcept override;

		bool isConnected() const noexcept override;
		int getLastError() const noexcept override;
		int getLastCloseCode() const noexcept override;
		const char* getLastCloseReason() const noexcept override;

		void setOnConnectCallback(OnConnectCallback callback) noexcept override { m_onConnectCallback = std::move(callback); }
		void setOnDisconnectCallback(OnDisconnectCallback callback) noexcept override { m_onDisconnectCallback = std::move(callback); }
		void setOnPacketRecvdCallback(OnPacketRecvdCallback callback) noexcept override { m_onPacketRecvdCallback = std::move(callback); }
		void setOnErrorCallback(OnErrorCallback callback) noexcept override { m_onErrorCallback = std::move(callback); }

	private:

		inline void logInfo(const char* msg)
		{
			std::string logMsg{ "[WinSocket]" };
			logMsg.append(msg).append("\n");

#ifdef _WIN32
			OutputDebugStringA(logMsg.c_str());
#endif
		}

		inline void logError(const char* prefixMsg)
		{
			std::string msg{ "[WinSocket]" };
			msg.append(prefixMsg);
			msg.append(std::to_string(WSAGetLastError())).append("\n");

#ifdef _WIN32
			OutputDebugStringA(msg.c_str());
#endif
		}

		WSAEVENT m_event{};

		SOCKET m_socket{};
		bool m_connected{ false };

		OnConnectCallback m_onConnectCallback;
		OnDisconnectCallback m_onDisconnectCallback;
		OnPacketRecvdCallback m_onPacketRecvdCallback;
		OnErrorCallback m_onErrorCallback;
	};
}

#endif //INCLUDE_ADAPTERS_WEBSOCKETS_WINWEBSOCKETADAPTER_H 
