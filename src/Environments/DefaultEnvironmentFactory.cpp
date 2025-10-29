#include <cleanMqtt/Environments/DefaultEnvironmentFactory.h>
#include <cleanMqtt/Environments/DefaultWinEnv.h>

namespace cleanMqtt
{
    IMqttEnvironment* DefaultEnvironmentFactory::createEnvironment()
    {
        return new DefaultWinEnv();
    }
}
