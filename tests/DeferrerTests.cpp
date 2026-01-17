// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <doctest.h>
#include <kmMqtt/Utils/Deferrer.h>

TEST_SUITE("Deferrer Tests")
{
	using kmMqtt::events::Deferrer;

	TEST_CASE("Deferrer defer and invokeEvents")
	{
		Deferrer deferrer;
		int callCount = 0;

		deferrer.defer([&]() { ++callCount; });
		deferrer.defer([&]() { ++callCount; });

		CHECK(callCount == 0);

		deferrer.invokeEvents();
		CHECK(callCount == 2);
	}

	TEST_CASE("Deferrer operator() invokes events")
	{
		Deferrer deferrer;
		int value = 0;

		deferrer.defer([&]() { value = 42; });
		deferrer();

		CHECK(value == 42);
	}

	TEST_CASE("Deferrer events invoked in order")
	{
		Deferrer deferrer;
		int sequence = 0;
		int first = 0;
		int second = 0;

		deferrer.defer([&]() { first = ++sequence; });
		deferrer.defer([&]() { second = ++sequence; });

		deferrer.invokeEvents();

		CHECK(first == 1);
		CHECK(second == 2);
	}

	TEST_CASE("Deferrer clear removes all events")
	{
		Deferrer deferrer;
		int callCount = 0;

		deferrer.defer([&]() { ++callCount; });
		deferrer.defer([&]() { ++callCount; });
		deferrer.clear();
		deferrer.invokeEvents();

		CHECK(callCount == 0);
	}

	TEST_CASE("Deferrer invokeEvents on empty queue")
	{
		Deferrer deferrer;
		//Should not crash
		deferrer.invokeEvents();
		deferrer();
		CHECK(true);
	}

	TEST_CASE("Deferrer clear on empty queue")
	{
		Deferrer deferrer;
		//Should not crash
		deferrer.clear();
		CHECK(true);
	}

	TEST_CASE("Deferrer with captured values")
	{
		Deferrer deferrer;
		int result = 0;
		int capturedValue = 100;

		deferrer.defer([&, capturedValue]() { result = capturedValue; });
		capturedValue = 200;
		deferrer.invokeEvents();

		CHECK(result == 100);
	}

	TEST_CASE("Deferrer multiple invoke calls")
	{
		Deferrer deferrer;
		int count = 0;

		deferrer.defer([&]() { ++count; });
		deferrer.invokeEvents();
		CHECK(count == 1);

		deferrer.defer([&]() { ++count; });
		deferrer.invokeEvents();
		CHECK(count == 2);
	}
}