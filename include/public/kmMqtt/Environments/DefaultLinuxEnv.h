#ifndef INCLUDE_KMMQTT_ENVIRONMENTS_DEFAULTLINUXENV_H
#define INCLUDE_KMMQTT_ENVIRONMENTS_DEFAULTLINUXENV_H

#include "kmMqtt/GlobalMacros.h"
#include <kmMqtt/Interfaces/IMqttEnvironment.h>
#include <kmMqtt/Config.h>
#include <memory>

namespace kmMqtt
{
    class PUBLIC_API DefaultLinuxEnv : public IMqttEnvironment
    {
    public:
        ~DefaultLinuxEnv() override = default;

        Config createConfig() const noexcept override;

        std::shared_ptr<IWebSocket> createWebSocket() const noexcept override;
    };
}

#endif // INCLUDE_KMMQTT_ENVIRONMENTS_DEFAULTLINUXENV_H
