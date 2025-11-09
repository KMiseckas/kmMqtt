#include <cleanMqtt/Environments/DefaultWinEnv.h>
#include <cleanMqtt/Logger/LoggerInstance.h>

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
