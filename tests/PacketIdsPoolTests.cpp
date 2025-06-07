#include <doctest.h>
#include <cleanMqtt/Utils/PacketIdPool.h>

TEST_SUITE("Packet Utils Tests")
{
	using cleanMqtt::PacketIdPool;
	using cleanMqtt::PacketID;

	TEST_CASE("Basic allocation")
	{
		PacketIdPool pool;
		PacketID id1 = pool.getId();
		PacketID id2 = pool.getId();
		PacketID id3 = pool.getId();

		CHECK(id1 == 1);
		CHECK(id2 == 2);
		CHECK(id3 == 3);
	}

	TEST_CASE("Release and reuse")
	{
		PacketIdPool pool;
		PacketID id1 = pool.getId();
		PacketID id2 = pool.getId();
		PacketID id3 = pool.getId();

		pool.releaseId(id2);
		PacketID id4 = pool.getId();

		CHECK(id4 == id2); // Should reuse released ID
	}

	TEST_CASE("Dont reuse 0 or invalid IDs")
	{
		PacketIdPool pool;
		pool.releaseId(0); // Should do nothing, 0 is not valid
		PacketID id1 = pool.getId();
		CHECK(id1 == 1);
	}

	TEST_CASE("Multiple releases and allocations")
	{
		PacketIdPool pool;
		PacketID ids[5];
		for (int i = 0; i < 5; ++i)
		{
			ids[i] = pool.getId();
		}

		// Release 2 and 4`
		pool.releaseId(ids[1]); // 2
		pool.releaseId(ids[3]); // 4

		PacketID reused1 = pool.getId();
		PacketID reused2 = pool.getId();
		PacketID next = pool.getId();

		// Released IDs should be reused before incrementing
		CHECK((reused1 == ids[3] || reused1 == ids[1]));
		CHECK((reused2 == ids[3] || reused2 == ids[1]));
		CHECK(reused1 != reused2);
		CHECK(next == 6);
	}
	
	TEST_CASE("Release same ID multiple times")
	{
		PacketIdPool pool;
		PacketID id1 = pool.getId();
		PacketID id2 = pool.getId();

		pool.releaseId(id1);
		pool.releaseId(id1); // Should have no effect the second time

		PacketID reused = pool.getId();
		PacketID next = pool.getId();

		CHECK(reused == id1); // id1 should be reused once
		CHECK(next == 3);     // Next should be the next incremented value
	}
}