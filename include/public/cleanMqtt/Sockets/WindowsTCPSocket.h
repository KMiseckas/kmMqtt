#if defined(_WIN32) || defined(_WIN64)

#ifndef INCLUDE_ADAPTERS_WEBSOCKETS_DEFAULTWINWEBSOCKET_H
#define INCLUDE_ADAPTERS_WEBSOCKETS_DEFAULTWINWEBSOCKET_H

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

namespace cleanMqtt
{
	class PUBLIC_API WindowsTCPSocket : public IWebSocket
	{
	public:
		WindowsTCPSocket();
		~WindowsTCPSocket() override;

		bool connect(const mqtt::Address& address) noexcept override;
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
		OnRecvdCallback m_onRecvdCallback;
		OnErrorCallback m_onErrorCallback;
	};
}

#endif //INCLUDE_ADAPTERS_WEBSOCKETS_DEFAULTWINWEBSOCKET_H 
#endif //defined(_WIN32) || defined(_WIN64)