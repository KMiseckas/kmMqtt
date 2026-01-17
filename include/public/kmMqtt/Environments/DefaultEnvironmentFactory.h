#ifndef INCLUDE_KMMQTT_UTILS_DEFAULTENVIRONMENTFACTORY_H
#define INCLUDE_KMMQTT_UTILS_DEFAULTENVIRONMENTFACTORY_H

#include "kmMqtt/GlobalMacros.h"
#include <kmMqtt/Interfaces/IMqttEnvironment.h>
#include <memory>

namespace kmMqtt
{
    // Abstract factory class for creating IMqttEnvironment instances
    class PUBLIC_API DefaultEnvironmentFactory
    {
    public:
        virtual ~DefaultEnvironmentFactory() = default;
        virtual IMqttEnvironment* createEnvironment();
    };
}

#endif // INCLUDE_KMMQTT_UTILS_DEFAULTENVIRONMENTFACTORY_H