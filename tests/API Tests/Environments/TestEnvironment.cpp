#include "TestEnvironment.h"
#include <cleanMqtt/Logger/DefaultLogger.h>

namespace cleanMqtt
{
    Config TestEnvironment::createConfig() const noexcept
    {
        return Config{};
    }

    std::unique_ptr<IWebSocket> TestEnvironment::createWebSocket() const noexcept
    {
        auto newSocket{ std::make_unique<MockWebSocket>() };
        socketPtr = newSocket.get();
        return newSocket;
    }
}
