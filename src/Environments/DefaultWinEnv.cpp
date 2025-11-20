#if defined(_WIN32) || defined(_WIN64)
#include <cleanMqtt/Environments/DefaultWinEnv.h>
#include <cleanMqtt/Sockets/DefaultWinWebsocket.h>

namespace cleanMqtt
{
	Config DefaultWinEnv::createConfig() const noexcept
	{
		return Config{};
	}

	std::shared_ptr<IWebSocket> DefaultWinEnv::createWebSocket() const noexcept
	{
		return std::make_shared<DefaultWinWebsocket>();
	}
} // namespace cleanMqtt

#endif // defined(_WIN32) || defined(_WIN64)
