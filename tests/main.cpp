#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest.h>
#include <cleanMqtt/Logger/DefaultLogger.h>
#include <cleanMqtt/Logger/LoggerInstance.h>

int main(int argc, char** argv)
{
    doctest::Context context;
    context.applyCommandLine(argc, argv);

    cleanMqtt::DefaultLogger logger;
    cleanMqtt::setLogger(&logger, true);

    // Run the tests
    int res = context.run();

    if (context.shouldExit()) 
    {
        return res;
    }

    return res;
}