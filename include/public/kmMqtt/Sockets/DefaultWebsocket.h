// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_ADAPTERS_WEBSOCKETS_DEFAULTWEBSOCKET_H
#define INCLUDE_ADAPTERS_WEBSOCKETS_DEFAULTWEBSOCKET_H

#include <kmMqtt/Interfaces/IWebSocket.h>

#ifdef BUILD_IXWEBSOCKET
#include <ixwebsocket/IXWebSocket.h>
#include <memory>
#include <string>
#endif

namespace kmMqtt
{
	class PUBLIC_API DefaultWebsocket : public IWebSocket
	{
	public:
		DefaultWebsocket();
		~DefaultWebsocket() override;

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
#ifdef BUILD_IXWEBSOCKET
		std::unique_ptr<ix::WebSocket> m_websocket;
#endif
		bool m_connected{ false };
		int m_lastError{ 0 };
		int m_lastCloseCode{ 0 };
		std::string m_lastCloseReason;

		OnConnectCallback m_onConnectCallback;
		OnDisconnectCallback m_onDisconnectCallback;
		OnRecvdCallback m_onRecvdCallback;
		OnErrorCallback m_onErrorCallback;
	};
}

#endif //INCLUDE_ADAPTERS_WEBSOCKETS_DEFAULTWEBSOCKET_H