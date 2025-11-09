#ifndef INCLUDE_CLEANMQTT_ENVIRONMENTS_DEFAULTWINENV_H
#define INCLUDE_CLEANMQTT_ENVIRONMENTS_DEFAULTWINENV_H

#include "cleanMqtt/GlobalMacros.h"
#include <cleanMqtt/Interfaces/IMqttEnvironment.h>
#include <cleanMqtt/Sockets/DefaultWinWebsocket.h>
#include <cleanMqtt/Logger/DefaultLogger.h>
#include <cleanMqtt/Config.h>
#include <memory>

namespace cleanMqtt
{
    class PUBLIC_API DefaultWinEnv : public IMqttEnvironment
    {
    public:
        ~DefaultWinEnv() override = default;

        Config createConfig() const noexcept override;
         
        std::shared_ptr<IWebSocket> createWebSocket() const noexcept override;
    };
}

#endif // INCLUDE_CLEANMQTT_ENVIRONMENTS_DEFAULTWINENV_H
