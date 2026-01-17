#if defined(__linux__)
#include <kmMqtt/Environments/DefaultLinuxEnv.h>
#include <kmMqtt/Sockets/DefaultWebsocket.h>

namespace kmMqtt
{
	Config DefaultLinuxEnv::createConfig() const noexcept
	{
		return Config{};
	}

	std::shared_ptr<IWebSocket> DefaultLinuxEnv::createWebSocket() const noexcept
	{
		return std::make_shared<DefaultWebsocket>();
	}
} // namespace kmMqtt

#endif // defined(__linux__)
