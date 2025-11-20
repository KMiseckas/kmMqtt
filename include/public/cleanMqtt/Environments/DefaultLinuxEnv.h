#ifndef INCLUDE_CLEANMQTT_ENVIRONMENTS_DEFAULTLINUXENV_H
#define INCLUDE_CLEANMQTT_ENVIRONMENTS_DEFAULTLINUXENV_H

#include "cleanMqtt/GlobalMacros.h"
#include <cleanMqtt/Interfaces/IMqttEnvironment.h>
#include <cleanMqtt/Config.h>
#include <memory>

namespace cleanMqtt
{
    class PUBLIC_API DefaultLinuxEnv : public IMqttEnvironment
    {
    public:
        ~DefaultLinuxEnv() override = default;

        Config createConfig() const noexcept override;

        std::shared_ptr<IWebSocket> createWebSocket() const noexcept override;
    };
}

#endif // INCLUDE_CLEANMQTT_ENVIRONMENTS_DEFAULTLINUXENV_H
