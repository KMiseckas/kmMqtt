#ifndef INCLUDE_CLEANMQTT_UTILS_DEFAULTENVIRONMENTFACTORY_H
#define INCLUDE_CLEANMQTT_UTILS_DEFAULTENVIRONMENTFACTORY_H

#include "cleanMqtt/GlobalMacros.h"
#include <cleanMqtt/Interfaces/IMqttEnvironment.h>
#include <memory>

namespace cleanMqtt
{
    // Abstract factory class for creating IMqttEnvironment instances
    class PUBLIC_API DefaultEnvironmentFactory
    {
    public:
        virtual ~DefaultEnvironmentFactory() = default;
        virtual IMqttEnvironment* createEnvironment();
    };
}

#endif // INCLUDE_CLEANMQTT_UTILS_DEFAULTENVIRONMENTFACTORY_H