#include <cleanMqtt/Sockets/CleanSocket.h>
#include <cleanMqtt/Interfaces/ILogger.h>

namespace cleanMqtt
{
	namespace socket
	{
		CleanSocket::~CleanSocket()
		{
			if (IS_VALID_SOCKET(m_socket))
			{
				kill();
			}
		}

		const bool CleanSocket::connect(const char* ip, const char* port)
		{
#if _WIN32
			printf("Starting socket initialization.\n");

			WSADATA data;
			if (WSAStartup(MAKEWORD(2, 2), &data) != SUCCESS)
			{
				Log(LogLevel::Info, "Failed initialize.");
				fprintf(stderr, "Failed initialize.\n");
				return false;
			}

			printf("Finished socket initialization.\n");
#endif

			if (ip == nullptr || ip[0] == '\0')
			{
				fprintf(stderr, "Invalid ip parameter.\n");
			}

			printf("Starting remote address configuration.\n");

			struct addrinfo* remoteAddrInfo;
			struct addrinfo hints;

			memset(&hints, 0, sizeof(hints)); //Empty hints.
			hints.ai_socktype = SOCK_STREAM; //Set as Stream (TCP)

			if (getaddrinfo(ip, port, &hints, &remoteAddrInfo) != SUCCESS)
			{
				fprintf(stderr, "getaddrinfo failed, check node name and service.\n");
				freeaddrinfo(remoteAddrInfo);
				return false;
			}

			printf("Finished remote address configuration.\n");

			char addressBuffer[100];
			char serviceBuffer[100];
			getnameinfo(remoteAddrInfo->ai_addr,
				static_cast<socklen_t>(remoteAddrInfo->ai_addrlen),
				addressBuffer,
				sizeof(addressBuffer),
				serviceBuffer,
				sizeof(serviceBuffer),
				NI_NUMERICHOST);

			printf("Remote Address: %s %s\n", addressBuffer, serviceBuffer);
			printf("Starting socket creation.\n");

			m_socket = ::socket(remoteAddrInfo->ai_family, remoteAddrInfo->ai_socktype, remoteAddrInfo->ai_protocol);

			if (!IS_VALID_SOCKET(m_socket))
			{
				fprintf(stderr, "Failed to create socket - Error: %d.\n", GET_SOCKET_ERROR_NUM());
				freeaddrinfo(remoteAddrInfo);
				return false;
			}

			printf("Finished socket creation.\n");
			printf("Starting connection.\n");

			if (::connect(m_socket, remoteAddrInfo->ai_addr, static_cast<int>(remoteAddrInfo->ai_addrlen)) != SUCCESS)
			{
				fprintf(stderr, "Failed to connect - Error: %d.\n", GET_SOCKET_ERROR_NUM());
				freeaddrinfo(remoteAddrInfo);
				return false;
			}

			printf("Connected!.\n");

			freeaddrinfo(remoteAddrInfo);

			return true;
		}

		const bool CleanSocket::send(const void* data, const int size, int& outBytesSent)
		{
			if (data == nullptr || size == 0)
			{
				fprintf(stderr, "send() data failed! Invalid arguments.");
				return false;
			}

			outBytesSent = ::send(m_socket, static_cast<const char*>(data), size, 0);

			if (outBytesSent < 0)
			{
				fprintf(stderr, "send() data failed! - Error %d - Data: %s", GetLastError(), static_cast<const char*>(data));
				return false;
			}

			return true;
		}

		const bool CleanSocket::receive(void* outBuffer, const int maxBufferLength, int& outBytesReceived)
		{
			if (outBuffer == nullptr)
			{
				fprintf(stderr, "receive() data failed! Invalid arguments.");
				return false;
			}

			outBytesReceived = ::recv(m_socket, static_cast<char*>(outBuffer), maxBufferLength, outBytesReceived);

			if (outBytesReceived < 0)
			{
				fprintf(stderr, "recv() data failed! - Error %d", GetLastError());
				return false;
			}

			return true;
		}

		const bool CleanSocket::kill()
		{
			if (!IS_VALID_SOCKET(m_socket))
			{
				return false;
			}

#ifdef _WIN32
			closesocket(m_socket);
			WSACleanup();
#else
			close(m_socket);
#endif

			m_socket = INVALID_SOCKET;

			return true;
		}

		const bool CleanSocket::setOption(const int option, const void* val, const int length)
		{
#ifdef _WIN32
			if (setsockopt(m_socket, SOL_SOCKET, option, static_cast<const char*>(val), length) != SUCCESS)
			{
				fprintf(stderr,
					"setsockopt() failed! - Error: %d - Option: %d - Option Value: %s - Length: %d",
					GetLastError(),
					option,
					static_cast<const char*>(val),
					length);
				return false;
			}
#else
			if(setsockopt(m_socket, SOL_SOCKET, option, static_cast<const char*>(val), length) != SUCCESS)
			{
				fprintf(stderr,
					"setsockopt() failed! - Error: %d - Option: %d - Option Value: %s - Length: %d",
					GetLastError(),
					option,
					static_cast<const char*>(val),
					length);
				return false;
			}
#endif

			return true;
		}

		const bool CleanSocket::disableBlocking()
		{
#ifdef _WIN32
			u_long nonBlockingMode = 1;
			if (ioctlsocket(m_socket, FIONBIO, &nonBlockingMode) != SUCCESS)
			{
				return false;
			}
#else
			if (fcntl(m_socket, F_SETFL, O_NONBLOCK) != SUCCESS)
			{
				return false;
			}
#endif

			return true;
		}
	}
}
