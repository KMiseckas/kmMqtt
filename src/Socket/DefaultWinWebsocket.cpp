#include "cleanMqtt/Sockets/DefaultWinWebsocket.h"

namespace cleanMqtt
{
	DefaultWinWebsocket::DefaultWinWebsocket()
		: m_socket(INVALID_SOCKET), m_event(WSA_INVALID_EVENT)
	{
		m_connected = false;
	}

	DefaultWinWebsocket::~DefaultWinWebsocket()
	{
		close();
		WSACleanup();
	}

	bool DefaultWinWebsocket::connect(const std::string& hostname, const std::string& port) noexcept
	{
		logInfo("Starting connect().");
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		{
			logError("Failed to initialize Winsock");
			return false;
		}

		struct addrinfo hints = {};
		struct addrinfo* result = nullptr;
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;

		//Resolve hostname.
		if (getaddrinfo(hostname.c_str(), port.c_str(), &hints, &result) != 0)
		{
			logError("getaddinfo() failed, error: ");
			WSACleanup();
			return false;
		}
		logInfo("DNS resolution done.");

		//Create socket
		m_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (m_socket == INVALID_SOCKET)
		{
			logError("socket creation failed, error: ");
			freeaddrinfo(result);
			WSACleanup();
			return false;
		}
		logInfo("Socket created.");

		//Set non-blocking.
		u_long mode = 1;
		ioctlsocket(m_socket, FIONBIO, &mode);
		logInfo("Socket set to non-blocking.");

		//Create an event
		m_event = WSACreateEvent();
		if (m_event == WSA_INVALID_EVENT)
		{
			freeaddrinfo(result);
			WSACleanup();
			logError("WSACreateEvent(), failed to create WSA event: ");
			return false;
		}
		logInfo("Socket events created.");

		//Associate the socket with the event
		if (WSAEventSelect(m_socket, m_event, FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR)
		{
			freeaddrinfo(result);
			WSACleanup();
			logError("WSAEventSelect() failed, error: ");
			return false;
		}

		//Connect socket
		int connResult = ::connect(m_socket, result->ai_addr, static_cast<int>(result->ai_addrlen));
		if (connResult == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
			{
				freeaddrinfo(result);
				logError("winsoc connect() failed, error: ");
				WSACleanup();
				return false;
			}
		}
		logInfo("Connection pending.");

		freeaddrinfo(result);

		return true;
	}

	bool DefaultWinWebsocket::send(const ByteBuffer& data) noexcept
	{
		if (!m_connected || m_socket == INVALID_SOCKET)
		{
			return false;
		}

		auto result = ::send(m_socket, reinterpret_cast<const char*>(data.bytes()), static_cast<int>(data.size()), 0);

		if (result == SOCKET_ERROR)
		{
			return false;
		}

		if (result != data.size())
		{
			return false;
		}

		return true;
	}

	bool DefaultWinWebsocket::close() noexcept
	{
		if (m_socket != INVALID_SOCKET)
		{
			closesocket(m_socket);
			m_socket = INVALID_SOCKET;
		}

		if (m_event != WSA_INVALID_EVENT)
		{
			WSACloseEvent(m_event);
			m_event = WSA_INVALID_EVENT;
		}

		WSACleanup();

		m_connected = false;
		return true;
	}

	void DefaultWinWebsocket::tick() noexcept
	{
		if (WSAWaitForMultipleEvents(1, &m_event, FALSE, 0, FALSE) == WSA_WAIT_TIMEOUT)
		{
			return;
		}

		WSANETWORKEVENTS networkEvents;
		if (WSAEnumNetworkEvents(m_socket, m_event, &networkEvents) == SOCKET_ERROR)
		{
			logError("WSAEnumNetworkEvents() failed, error: ");
			return;
		}

		if (networkEvents.lNetworkEvents & FD_CONNECT)
		{
			if (networkEvents.iErrorCode[FD_CONNECT_BIT] == 0)
			{
				if (m_onConnectCallback)
				{
					m_onConnectCallback(true);
				}
				m_connected = true;

				logInfo("socket connection established.");
			}
			else
			{
				if (m_onConnectCallback)
				{
					m_onConnectCallback(false);
				}

				if (m_onErrorCallback)
				{
					m_onErrorCallback(static_cast<std::int16_t>(networkEvents.iErrorCode[FD_CONNECT_BIT]));
				}
				m_connected = false;

				logInfo("socket connection failed to establish.");
			}
		}

		if (networkEvents.lNetworkEvents & FD_READ)
		{
			char buffer[65536];
			const int bytesReceived = recv(m_socket, buffer, sizeof(buffer), 0);

			if (bytesReceived > 0)
			{
				ByteBuffer byteBuffer{ static_cast<std::size_t>(bytesReceived) };
				byteBuffer.append(reinterpret_cast<const std::uint8_t*>(buffer), bytesReceived);

				if (m_onRecvdCallback)
				{
					m_onRecvdCallback(std::move(byteBuffer));
				}
			}
			else if (bytesReceived == 0)
			{
				if (m_onDisconnectCallback)
				{
					m_onDisconnectCallback();
				}
				m_connected = false;
				logInfo("recv event with 0 bytes. Disconnected.");
			}
			else
			{
				if (m_onErrorCallback)
				{
					m_onErrorCallback(static_cast<std::int16_t>(WSAGetLastError()));
				}
				m_connected = false;
				logInfo("recv below 0 bytes. Disconnected.");
			}
		}

		if (networkEvents.lNetworkEvents & FD_CLOSE)
		{
			if (m_onDisconnectCallback)
			{
				m_onDisconnectCallback();
			}

			m_connected = false;
			logInfo("socket closed.");
		}
	}

	bool DefaultWinWebsocket::isConnected() const noexcept
	{
		return m_connected;
	}

	int DefaultWinWebsocket::getLastError() const noexcept
	{
		return WSAGetLastError();
	}

	int DefaultWinWebsocket::getLastCloseCode() const noexcept
	{
		return 0;
	}

	const char* DefaultWinWebsocket::getLastCloseReason() const noexcept
	{
		return "N/A";
	}
}
