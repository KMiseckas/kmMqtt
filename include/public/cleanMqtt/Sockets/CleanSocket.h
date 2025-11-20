#if defined(_WIN32) || defined(__Win64)
#pragma once

#include <cleanMqtt/Interfaces/ISocket.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#endif

#ifdef _WIN32
#define IS_VALID_SOCKET(socket) (socket != INVALID_SOCKET)
#define CLOSE_SOCKET(socket) closesocket(socket)
#define GET_SOCKET_ERROR_NUM() WSAGetLastError()
#else
#define IS_VALID_SOCKET(socket) (socket >= 0)
#define CLOSE_SOCKET(socket) close(socket)
#define GET_SOCKET_ERROR_NUM() errno
#define SOCKET int
#endif

#define SUCCESS 0

#include <stdio.h>
#include <string>

namespace cleanMqtt
{
	namespace socket
	{
		class PUBLIC_API CleanSocket : public ISocket
		{
			~CleanSocket() override;

			bool connect(const char* ip, const char* port) override;
			bool send(const void* data, const int size, int& outBytesSent) override;
			bool receive(void* outBuffer, const int maxBufferLength, int& outBytesReceived) override;
			bool kill() override;
			bool setOption(const int option, const void* val, const int length) override;
			bool disableBlocking() override;

		private:
			SOCKET m_socket{ INVALID_SOCKET };
		};
	}
}
#endif 
