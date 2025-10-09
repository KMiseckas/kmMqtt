#include <cleanMqtt/Environments/DefaultWinEnv.h>
#include <cleanMqtt/Logger/LoggerInstance.h>

namespace cleanMqtt
{
	Config DefaultWinEnv::createConfig() const noexcept
	{
		return Config{};
	}

	std::unique_ptr<IWebSocket> DefaultWinEnv::createWebSocket() const noexcept
	{
		return std::make_unique<DefaultWinWebsocket>();
	}
} // namespace cleanMqtt
