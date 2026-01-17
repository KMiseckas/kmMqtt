// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <doctest.h>
#include <kmMqtt/Utils/PacketIdPool.h>

TEST_SUITE("Packet Utils Tests")
{
	using kmMqtt::PacketIdPool;

	TEST_CASE("Basic allocation")
	{
		PacketIdPool pool;
		std::uint16_t id1 = pool.getId();
		std::uint16_t id2 = pool.getId();
		std::uint16_t id3 = pool.getId();

		CHECK(id1 == 1);
		CHECK(id2 == 2);
		CHECK(id3 == 3);
	}

	TEST_CASE("Release and reuse")
	{
		PacketIdPool pool;
		std::uint16_t id1 = pool.getId();
		std::uint16_t id2 = pool.getId();
		std::uint16_t id3 = pool.getId();

		pool.releaseId(id2);
		std::uint16_t id4 = pool.getId();

		CHECK(id4 == id2); // Should reuse released ID
	}

	TEST_CASE("Dont reuse 0 or invalid IDs")
	{
		PacketIdPool pool;
		pool.releaseId(0); // Should do nothing, 0 is not valid
		std::uint16_t id1 = pool.getId();
		CHECK(id1 == 1);
	}

	TEST_CASE("Multiple releases and allocations")
	{
		PacketIdPool pool;
		std::uint16_t ids[5];
		for (int i = 0; i < 5; ++i)
		{
			ids[i] = pool.getId();
		}

		// Release 2 and 4`
		pool.releaseId(ids[1]); // 2
		pool.releaseId(ids[3]); // 4

		std::uint16_t reused1 = pool.getId();
		std::uint16_t reused2 = pool.getId();
		std::uint16_t next = pool.getId();

		// Released IDs should be reused before incrementing
		CHECK((reused1 == ids[3] || reused1 == ids[1]));
		CHECK((reused2 == ids[3] || reused2 == ids[1]));
		CHECK(reused1 != reused2);
		CHECK(next == 6);
	}
	
	TEST_CASE("Release same ID multiple times")
	{
		PacketIdPool pool;
		std::uint16_t id1 = pool.getId();
		std::uint16_t id2 = pool.getId();

		pool.releaseId(id1);
		pool.releaseId(id1); // Should have no effect the second time

		std::uint16_t reused = pool.getId();
		std::uint16_t next = pool.getId();

		CHECK(reused == id1); // id1 should be reused once
		CHECK(next == 3);     // Next should be the next incremented value
	}
}