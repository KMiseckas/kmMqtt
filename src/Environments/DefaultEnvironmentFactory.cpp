// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <kmMqtt/Environments/DefaultEnvironmentFactory.h>

#if defined(_WIN32) || defined(_WIN64)
#include <kmMqtt/Environments/DefaultWinEnv.h>
using ENV = kmMqtt::DefaultWinEnv;
#elif defined(__linux__)
#include <kmMqtt/Environments/DefaultLinuxEnv.h>
using ENV = kmMqtt::DefaultLinuxEnv;
#endif

namespace kmMqtt
{
    IMqttEnvironment* DefaultEnvironmentFactory::createEnvironment()
    {
        return new ENV();
    }
}
