// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_ENVIRONMENTS_DEFAULTWINENV_H
#define INCLUDE_KMMQTT_ENVIRONMENTS_DEFAULTWINENV_H

#include "kmMqtt/GlobalMacros.h"
#include <kmMqtt/Interfaces/IMqttEnvironment.h>
#include <kmMqtt/Config.h>
#include <memory>

namespace kmMqtt
{
    class PUBLIC_API DefaultWinEnv : public IMqttEnvironment
    {
    public:
        ~DefaultWinEnv() override = default;

        Config createConfig() const noexcept override;
         
        std::shared_ptr<IWebSocket> createWebSocket() const noexcept override;
    };
}

#endif // INCLUDE_KMMQTT_ENVIRONMENTS_DEFAULTWINENV_H
