// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <doctest.h>
#include <kmMqtt/Utils/Event.h>

TEST_SUITE("Events Tests")
{
	using kmMqtt::events::Event;

	TEST_CASE("Event add, invoke, and remove")
	{
		Event<int> event;
		int callCount = 0;
		int lastValue = 0;

		auto cb = [&](int v) { ++callCount; lastValue = v; };

		event.add(cb);
		event.invoke(42);

		CHECK(callCount == 1);
		CHECK(lastValue == 42);

		event.remove(cb);
		event.invoke(99);
		CHECK(callCount == 1); // Should not increment, callback removed
	}

	TEST_CASE("Event operator overloads")
	{
		Event<int> event;
		int sum = 0;

		auto cb = [&](int v) { sum += v; };

		event += cb;
		event(10);
		CHECK(sum == 10);

		event -= cb;
		event(5);
		CHECK(sum == 10); // No change, callback removed
	}

	TEST_CASE("Event removeAll")
	{
		Event<int> event;
		int count = 0;

		auto cb1 = [&](int) { ++count; };
		auto cb2 = [&](int) { ++count; };

		event.add(cb1);
		event.add(cb2);
		event.removeAll();
		event.invoke(1);
		CHECK(count == 0);
	}

	TEST_CASE("Event add duplicate callback")
	{
		Event<int> event;
		int count = 0;
		auto cb = [&](int) { ++count; };

		event.add(cb);
		event.add(cb); // Should not add duplicate
		event.invoke(1);
		CHECK(count == 1);
	}
}