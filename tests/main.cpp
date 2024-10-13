#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest.h>
#include <cleanMqtt/Logger/CleanLogger.h>
#include <cleanMqtt/Settings.h>

int main(int argc, char** argv)
{
    doctest::Context context;
    context.applyCommandLine(argc, argv);

    cleanMqtt::logger::CleanLogger logger;
    cleanMqtt::settings::setLogger(&logger, true);

    // Run the tests
    int res = context.run();

    if (context.shouldExit()) 
    {
        return res;
    }

    return res;
}