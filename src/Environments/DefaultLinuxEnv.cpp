// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

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
