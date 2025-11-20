#include <cleanMqtt/Environments/DefaultEnvironmentFactory.h>

#if defined(_WIN32) || defined(_WIN64)
#include <cleanMqtt/Environments/DefaultWinEnv.h>
using ENV = cleanMqtt::DefaultWinEnv;
#elif defined(__linux__)
#include <cleanMqtt/Environments/DefaultLinuxEnv.h>
using ENV = cleanMqtt::DefaultLinuxEnv;
#endif

namespace cleanMqtt
{
    IMqttEnvironment* DefaultEnvironmentFactory::createEnvironment()
    {
        return new ENV();
    }
}
