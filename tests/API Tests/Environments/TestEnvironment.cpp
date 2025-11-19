#include "TestEnvironment.h"
#include <cleanMqtt/Logger/DefaultLogger.h>

namespace cleanMqtt
{
    Config TestEnvironment::createConfig() const noexcept
    {
        return config;
    }

    std::shared_ptr<IWebSocket> TestEnvironment::createWebSocket() const noexcept
    {
        auto newSocket{ std::make_shared<MockWebSocket>() };
        socketPtr = newSocket.get();
        return newSocket;
    }
}
