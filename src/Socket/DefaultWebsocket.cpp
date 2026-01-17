// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifdef BUILD_IXWEBSOCKET

#include "kmMqtt/Sockets/DefaultWebsocket.h"
#include "kmMqtt/Logger/Log.h"

#include <ixwebsocket/IXNetSystem.h>

namespace kmMqtt
{
	DefaultWebsocket::DefaultWebsocket()
		: m_websocket(std::make_unique<ix::WebSocket>())
		, m_connected(false)
		, m_lastError(0)
		, m_lastCloseCode(0)
		, m_lastCloseReason("")
	{
		m_websocket->disableAutomaticReconnection();
		m_websocket->setPingInterval(30);

		//Message callbacks
		m_websocket->setOnMessageCallback([this](const ix::WebSocketMessagePtr& msg)
			{
				if (msg->type == ix::WebSocketMessageType::Open)
				{
					LogInfo("DefaultWebsocket", "WebSocket connection established.");
					m_connected = true;
					m_lastError = 0;

					if (m_onConnectCallback)
					{
						m_onConnectCallback(true);
					}
				}
				else if (msg->type == ix::WebSocketMessageType::Close)
				{
					LogInfo("DefaultWebsocket", "WebSocket connection closed.");
					m_connected = false;
					m_lastCloseCode = msg->closeInfo.code;
					m_lastCloseReason = msg->closeInfo.reason;

					if (m_onDisconnectCallback)
					{
						m_onDisconnectCallback();
					}
				}
				else if (msg->type == ix::WebSocketMessageType::Message)
				{
					const std::string& data = msg->str;
					if (!data.empty())
					{
						ByteBuffer buffer(data.size());
						buffer.append(reinterpret_cast<const std::uint8_t*>(data.data()), data.size());

						if (m_onRecvdCallback)
						{
							m_onRecvdCallback(std::move(buffer));
						}
					}
				}
				else if (msg->type == ix::WebSocketMessageType::Error)
				{
					LogError("DefaultWebsocket", msg->errorInfo.reason.c_str());
					m_connected = false;
					m_lastError = static_cast<int>(msg->errorInfo.retries);

					if (m_onErrorCallback)
					{
						m_onErrorCallback(static_cast<std::uint16_t>(m_lastError));
					}

					if (m_onConnectCallback && !m_connected)
					{
						m_onConnectCallback(false);
					}
				}
			});
	}

	DefaultWebsocket::~DefaultWebsocket()
	{
		close();
	}

	bool DefaultWebsocket::connect(const mqtt::Address& address) noexcept
	{
		try
		{
#if defined(_WIN32) || defined(_WIN64)
			ix::initNetSystem();
#endif

			if (!m_websocket)
			{
				LogError("DefaultWebsocket", "WebSocket instance is null.");
				return false;
			}

			std::string url{ address.url() };
			LogInfo("DefaultWebsocket", ("Connecting to: " + url).c_str());

			m_websocket->setUrl(url);
			m_websocket->setExtraHeaders({ {"Sec-WebSocket-Protocol", "mqtt"} });

			m_websocket->start();

			LogInfo("DefaultWebsocket", "Connection initiated.");
			return true;
		}
		catch (const std::exception& e)
		{
			LogError("DefaultWebsocket", "Exception during connect: %s", e.what());
#if defined(_WIN32) || defined(_WIN64)
			ix::uninitNetSystem();
#endif
			return false;
		}
		catch (...)
		{
			LogError("DefaultWebsocket", "Unknown exception during connect.");
#if defined(_WIN32) || defined(_WIN64)
			ix::uninitNetSystem();
#endif
			return false;
		}
	}

	int DefaultWebsocket::send(const ByteBuffer& data) noexcept
	{
		try
		{
			if (!m_connected || !m_websocket)
			{
				LogError("DefaultWebsocket", "Cannot send: not connected.");
				return -1;
			}

			std::string payload(reinterpret_cast<const char*>(data.bytes()), data.size());
			ix::WebSocketSendInfo info = m_websocket->sendBinary(payload);

			if (info.success)
			{
				return static_cast<int>(data.size());
			}
			else
			{
				LogError("DefaultWebsocket", "Send failed");
				m_lastError = -1;
				return -1;
			}
		}
		catch (const std::exception& e)
		{
			LogError("DefaultWebsocket", "Exception during send: %s", e.what());
			return -1;
		}
		catch (...)
		{
			LogError("DefaultWebsocket", "Unknown exception during send.");
			return -1;
		}
	}

	bool DefaultWebsocket::close() noexcept
	{
#if defined(_WIN32) || defined(_WIN64)
		ix::uninitNetSystem();
#endif

		try
		{
			if (m_websocket)
			{
				LogInfo("DefaultWebsocket", "Closing WebSocket connection.");
				m_websocket->stop();
				m_connected = false;
			}
			return true;
		}
		catch (const std::exception& e)
		{
			LogError("DefaultWebsocket", "Exception during close: %s", e.what());
			return false;
		}
		catch (...)
		{
			LogError("DefaultWebsocket", "Unknown exception during close.");
			return false;
		}
	}

	void DefaultWebsocket::tick() noexcept
	{
		//IXWebSocket handles its own event loop internally via background threads.
		//This method is kept for interface compatibility but doesn't need to do anything.
		//All callbacks are triggered from IXWebSocket's internal threads.
	}

	bool DefaultWebsocket::isConnected() const noexcept
	{
		return m_connected;
	}

	int DefaultWebsocket::getLastError() const noexcept
	{
		return m_lastError;
	}

	int DefaultWebsocket::getLastCloseCode() const noexcept
	{
		return m_lastCloseCode;
	}

	const char* DefaultWebsocket::getLastCloseReason() const noexcept
	{
		return m_lastCloseReason.c_str();
	}
}

#else
namespace kmMqtt
{
	DefaultWebsocket::DefaultWebsocket()
		: m_connected(false)
	{
	}

	DefaultWebsocket::~DefaultWebsocket()
	{
	}

	bool DefaultWebsocket::connect(const mqtt::Address& address) noexcept
	{
		return false;
	}

	int DefaultWebsocket::send(const ByteBuffer&) noexcept
	{
		return -1;
	}

	bool DefaultWebsocket::close() noexcept
	{
		return false;
	}

	void DefaultWebsocket::tick() noexcept
	{
	}

	bool DefaultWebsocket::isConnected() const noexcept
	{
		return false;
	}

	int DefaultWebsocket::getLastError() const noexcept
	{
		return -1;
	}

	int DefaultWebsocket::getLastCloseCode() const noexcept
	{
		return 0;
	}

	const char* DefaultWebsocket::getLastCloseReason() const noexcept
	{
		return "IXWebSocket not built (BUILD_IXWEBSOCKET=OFF)";
	}
}
#endif // BUILD_IXWEBSOCKET
