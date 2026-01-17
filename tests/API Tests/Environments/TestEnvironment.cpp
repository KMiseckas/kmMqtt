#include "TestEnvironment.h"
#include <kmMqtt/Logger/DefaultLogger.h>

namespace kmMqtt
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
