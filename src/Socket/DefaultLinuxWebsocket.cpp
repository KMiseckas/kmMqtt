#if defined(__linux__)
#include "cleanMqtt/Sockets/DefaultLinuxWebsocket.h"

namespace cleanMqtt
{
	//TODO finish linux socket.

	DefaultLinuxWebsocket::DefaultLinuxWebsocket()
	{
	}

	DefaultLinuxWebsocket::~DefaultLinuxWebsocket()
	{
	}

	bool DefaultLinuxWebsocket::connect(const std::string& /*hostname*/, const std::string& /*port*/) noexcept
	{
		return false;
	}

	int DefaultLinuxWebsocket::send(const ByteBuffer& /*data*/) noexcept
	{
		return -1;
	}

	bool DefaultLinuxWebsocket::close() noexcept
	{
		return false;
	}

	void DefaultLinuxWebsocket::tick() noexcept
	{
	}

	bool DefaultLinuxWebsocket::isConnected() const noexcept
	{
		return false;
	}

	int DefaultLinuxWebsocket::getLastError() const noexcept
	{
		return -1;
	}

	int DefaultLinuxWebsocket::getLastCloseCode() const noexcept
	{
		return -1;
	}

	const char* DefaultLinuxWebsocket::getLastCloseReason() const noexcept
	{
		return nullptr;
	}
}
#endif //defined(__linux__)