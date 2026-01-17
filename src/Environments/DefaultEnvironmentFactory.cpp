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
