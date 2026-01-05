#if defined(__linux__)
#include <cleanMqtt/Environments/DefaultLinuxEnv.h>
#include <cleanMqtt/Sockets/DefaultWebsocket.h>

namespace cleanMqtt
{
	Config DefaultLinuxEnv::createConfig() const noexcept
	{
		return Config{};
	}

	std::shared_ptr<IWebSocket> DefaultLinuxEnv::createWebSocket() const noexcept
	{
		return std::make_shared<DefaultWebsocket>();
	}
} // namespace cleanMqtt

#endif // defined(__linux__)
