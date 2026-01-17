// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#if defined(_WIN32) || defined(_WIN64)
#include <kmMqtt/Environments/DefaultWinEnv.h>
#include <kmMqtt/Sockets/DefaultWebsocket.h>

namespace kmMqtt
{
	Config DefaultWinEnv::createConfig() const noexcept
	{
		return Config{};
	}

	std::shared_ptr<IWebSocket> DefaultWinEnv::createWebSocket() const noexcept
	{
		return std::make_shared<DefaultWebsocket>();
	}
} // namespace kmMqtt

#endif // defined(_WIN32) || defined(_WIN64)
