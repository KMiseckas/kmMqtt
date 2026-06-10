// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include "TestEnvironment.h"
#include <kmMqtt/Logger/DefaultLogger.h>

namespace kmMqtt
{
    Config TestEnvironment::createConfig() const noexcept
    {
        return config;
    }

    kmStd::shared_ptr<IWebSocket> TestEnvironment::createWebSocket() const noexcept
    {
		auto newSocket{kmStd::make_shared<MockWebSocket>()};
        socketPtr = newSocket.get();
        return newSocket;
    }
}
