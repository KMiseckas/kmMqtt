// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <doctest.h>
#include <kmMqtt/GlobalMacros.h>
#include <kmMqtt/Utils/UniqueFunction.h>
#include <iostream>
#include <string>

TEST_SUITE("UniqueFunction Tests")
{
    using namespace kmMqtt;

    TEST_CASE("UniqueFunction can store and invoke a lambda")
    {
        UniqueFunction func = []() { std::cout << "Hello, UniqueFunction!" << std::endl; };
        CHECK_NOTHROW(func());
    }

    TEST_CASE("UniqueFunction can be moved")
    {
        UniqueFunction func1 = []() { std::cout << "Function 1" << std::endl; };
        UniqueFunction func2 = std::move(func1);

        CHECK_NOTHROW(func2());
    }

    TEST_CASE("UniqueFunction handles destruction correctly")
    {
        bool destroyed = false;

        {
            UniqueFunction func = [&]() { destroyed = true; };
            CHECK_NOTHROW(func());
        }

        CHECK(destroyed);
    }
}