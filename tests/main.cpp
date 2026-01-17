// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest.h>
#include <kmMqtt/Logger/DefaultLogger.h>
#include <kmMqtt/Logger/LoggerInstance.h>

int main(int argc, char** argv)
{
    doctest::Context context;
    context.applyCommandLine(argc, argv);

    kmMqtt::DefaultLogger logger;
    kmMqtt::setLogger(&logger, true);

    // Run the tests
    int res = context.run();

    if (context.shouldExit()) 
    {
        return res;
    }

    return res;
}