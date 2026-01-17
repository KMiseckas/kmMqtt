#include <doctest.h>
#include <kmMqtt/ByteBuffer.h>

TEST_SUITE("Byte Buffer Tests")
{
	using namespace kmMqtt;

	TEST_CASE("Constructors")
	{
		ByteBuffer* buffer_0 = new ByteBuffer(0);
		CHECK(buffer_0->size() == 0);
		CHECK(buffer_0->capacity() == 0);
		CHECK(buffer_0->bytes() != nullptr);

		ByteBuffer* buffer_1 = new ByteBuffer(20000);
		CHECK(buffer_1->capacity() == 20000);
		CHECK(buffer_1->bytes() != nullptr);
		CHECK(buffer_1->size() == 0);
		CHECK(buffer_1->headroom() == 20000);

		static constexpr std::uint8_t array[5]{1,2,3,4,5};
		buffer_1->append(array, 5);
		CHECK(buffer_1->capacity() == 20000);
		CHECK(buffer_1->bytes()[0] == 1);
		CHECK(buffer_1->bytes()[4] == 5);
		CHECK(buffer_1->size() == 5);
		CHECK(buffer_1->headroom() == (20000 - 5));

		ByteBuffer* buffer_2 = new ByteBuffer(std::move(*buffer_1));
		CHECK(buffer_2->capacity() == 20000);
		CHECK(buffer_2->bytes()[0] == 1);
		CHECK(buffer_2->bytes()[4] == 5);
		CHECK(buffer_2->size() == 5);

		buffer_2->append(array, 5);
		CHECK(buffer_2->capacity() == 20000);
		CHECK(buffer_2->bytes()[0] == 1);
		CHECK(buffer_2->bytes()[4] == 5);
		CHECK(buffer_2->bytes()[6] == 2);
		CHECK(buffer_2->bytes()[9] == 5);
		CHECK(buffer_2->size() == 10);

		ByteBuffer* buffer_3 = new ByteBuffer(2);
		CHECK_NOTHROW(*buffer_3 += 1);
		CHECK_NOTHROW(*buffer_3 += 1);
		CHECK(buffer_3->capacity() == 2);
		CHECK(buffer_3->bytes()[0] == 1);
		CHECK(buffer_3->bytes()[1] == 1);
		CHECK(buffer_3->size() == 2);
		CHECK(buffer_3->headroom() == 0);
		CHECK_THROWS(*buffer_3 += 1);

		ByteBuffer* buffer_4 = new ByteBuffer(2);
		static constexpr std::uint8_t arr[2] = {1,2};
		CHECK_THROWS(buffer_4->append(arr, 3));
		CHECK_NOTHROW(buffer_4->append(arr, 1));

		delete buffer_0;
		delete buffer_1;
		delete buffer_2;
		delete buffer_3;
		delete buffer_4;
	}

	TEST_CASE("Copy Constructor")
	{
		SUBCASE("Copy empty buffer")
		{
			ByteBuffer original(100);
			ByteBuffer copy(original);

			CHECK(copy.size() == 0);
			CHECK(copy.capacity() == 100);
			CHECK(copy.headroom() == 100);
			CHECK(copy.bytes() != nullptr);
			CHECK(copy.bytes() != original.bytes()); // Different memory locations
		}

		SUBCASE("Copy buffer with data")
		{
			ByteBuffer original(100);
			static constexpr std::uint8_t data[5] = {10, 20, 30, 40, 50};
			original.append(data, 5);

			ByteBuffer copy(original);

			CHECK(copy.size() == 5);
			CHECK(copy.capacity() == 100);
			CHECK(copy.headroom() == 95);
			CHECK(copy[0] == 10);
			CHECK(copy[1] == 20);
			CHECK(copy[2] == 30);
			CHECK(copy[3] == 40);
			CHECK(copy[4] == 50);
			CHECK(copy.bytes() != original.bytes()); // Deep copy
		}

		SUBCASE("Copy does not affect original")
		{
			ByteBuffer original(50);
			original.append(static_cast<std::uint16_t>(0x1234));
			
			ByteBuffer copy(original);
			copy.append(static_cast<std::uint16_t>(0x5678));

			CHECK(original.size() == 2);
			CHECK(copy.size() == 4);
			CHECK(original[0] == 0x12);
			CHECK(original[1] == 0x34);
			CHECK(copy[0] == 0x12);
			CHECK(copy[1] == 0x34);
			CHECK(copy[2] == 0x56);
			CHECK(copy[3] == 0x78);
		}

		SUBCASE("Copy buffer with read cursor position")
		{
			ByteBuffer original(50);
			static constexpr std::uint8_t data[5] = {1, 2, 3, 4, 5};
			original.append(data, 5);
			original.readUint8(); // Advances read cursor to position 1

			ByteBuffer copy(original);

			CHECK(copy.readCursor() == original.readCursor());
			CHECK(copy.readHeadroom() == original.readHeadroom());
		}

#ifdef ENABLE_BYTEBUFFER_SBO
		SUBCASE("Copy buffer using SBO")
		{
			ByteBuffer original(32); // Small enough for SBO
			static constexpr std::uint8_t data[4] = {0xAA, 0xBB, 0xCC, 0xDD};
			original.append(data, 4);

			ByteBuffer copy(original);

			CHECK(copy.size() == 4);
			CHECK(copy.capacity() == 32);
			CHECK(copy[0] == 0xAA);
			CHECK(copy[1] == 0xBB);
			CHECK(copy[2] == 0xCC);
			CHECK(copy[3] == 0xDD);
		}
#endif
	}

	TEST_CASE("Copy Assignment Operator")
	{
		SUBCASE("Copy assign to empty buffer")
		{
			ByteBuffer original(100);
			static constexpr std::uint8_t data[3] = {7, 8, 9};
			original.append(data, 3);

			ByteBuffer target(50);
			target = original;

			CHECK(target.size() == 3);
			CHECK(target.capacity() == 100);
			CHECK(target[0] == 7);
			CHECK(target[1] == 8);
			CHECK(target[2] == 9);
			CHECK(target.bytes() != original.bytes());
		}

		SUBCASE("Copy assign to buffer with existing data")
		{
			ByteBuffer original(100);
			original.append(static_cast<std::uint32_t>(0x12345678));

			ByteBuffer target(50);
			target.append(static_cast<std::uint16_t>(0xABCD));
			
			target = original;

			CHECK(target.size() == 4);
			CHECK(target.capacity() == 100);
			CHECK(target[0] == 0x12);
			CHECK(target[1] == 0x34);
			CHECK(target[2] == 0x56);
			CHECK(target[3] == 0x78);
		}

		SUBCASE("Self assignment")
		{
			ByteBuffer buffer(50);
			static constexpr std::uint8_t data[2] = {0xFF, 0xEE};
			buffer.append(data, 2);

			buffer = buffer;

			CHECK(buffer.size() == 2);
			CHECK(buffer.capacity() == 50);
			CHECK(buffer[0] == 0xFF);
			CHECK(buffer[1] == 0xEE);
		}

		SUBCASE("Chain copy assignments")
		{
			ByteBuffer original(100);
			original += static_cast<std::uint8_t>(42);

			ByteBuffer buffer1(50);
			ByteBuffer buffer2(75);

			buffer2 = buffer1 = original;

			CHECK(buffer1.size() == 1);
			CHECK(buffer1[0] == 42);
			CHECK(buffer2.size() == 1);
			CHECK(buffer2[0] == 42);
		}

		SUBCASE("Copy assignment preserves read cursor")
		{
			ByteBuffer original(50);
			static constexpr std::uint8_t data[5] = {1, 2, 3, 4, 5};
			original.append(data, 5);
			original.readUint8();
			original.readUint8(); // Read cursor at position 2

			ByteBuffer target(100);
			target = original;

			CHECK(target.readCursor() == 2);
			CHECK(target.readHeadroom() == 3);
		}
	}

	TEST_CASE("Move Constructor")
	{
		SUBCASE("Move empty buffer")
		{
			ByteBuffer original(100);
			ByteBuffer moved(std::move(original));

			CHECK(moved.size() == 0);
			CHECK(moved.capacity() == 100);
			CHECK(moved.bytes() != nullptr);
			CHECK(original.capacity() == 0);
			CHECK(original.size() == 0);
		}

		SUBCASE("Move buffer with data")
		{
			ByteBuffer original(100);
			static constexpr std::uint8_t data[4] = {11, 22, 33, 44};
			original.append(data, 4);
			const std::uint8_t* originalPtr = original.bytes();

			ByteBuffer moved(std::move(original));

			CHECK(moved.size() == 4);
			CHECK(moved.capacity() == 100);
			CHECK(moved[0] == 11);
			CHECK(moved[1] == 22);
			CHECK(moved[2] == 33);
			CHECK(moved[3] == 44);
			
			// Original should be in moved-from state
			CHECK(original.size() == 0);
			CHECK(original.capacity() == 0);

#ifndef ENABLE_BYTEBUFFER_SBO
			// Without SBO, pointer should be transferred
			CHECK(moved.bytes() == originalPtr);
#endif
		}

		SUBCASE("Move buffer with read cursor")
		{
			ByteBuffer original(50);
			static constexpr std::uint8_t data[5] = {5, 4, 3, 2, 1};
			original.append(data, 5);
			original.readUint8();
			original.readUint8(); // Read cursor at position 2

			ByteBuffer moved(std::move(original));

			CHECK(moved.readCursor() == 2);
			CHECK(moved.readHeadroom() == 3);
		}

#ifdef ENABLE_BYTEBUFFER_SBO
		SUBCASE("Move buffer using SBO")
		{
			ByteBuffer original(32); // Small enough for SBO
			static constexpr std::uint8_t data[3] = {0x11, 0x22, 0x33};
			original.append(data, 3);

			ByteBuffer moved(std::move(original));

			CHECK(moved.size() == 3);
			CHECK(moved.capacity() == 32);
			CHECK(moved[0] == 0x11);
			CHECK(moved[1] == 0x22);
			CHECK(moved[2] == 0x33);
			
			// Original should be cleared
			CHECK(original.size() == 0);
			CHECK(original.capacity() == 0);
		}
#endif
	}

	TEST_CASE("Move Assignment Operator")
	{
		SUBCASE("Move assign to empty buffer")
		{
			ByteBuffer original(100);
			static constexpr std::uint8_t data[2] = {99, 88};
			original.append(data, 2);

			ByteBuffer target(50);
			target = std::move(original);

			CHECK(target.size() == 2);
			CHECK(target.capacity() == 100);
			CHECK(target[0] == 99);
			CHECK(target[1] == 88);
			CHECK(original.size() == 0);
			CHECK(original.capacity() == 0);
		}

		SUBCASE("Move assign to buffer with existing data")
		{
			ByteBuffer original(100);
			original.append(static_cast<std::uint16_t>(0xBEEF));

			ByteBuffer target(50);
			target.append(static_cast<std::uint16_t>(0xDEAD));
			const std::uint8_t* originalPtr = original.bytes();

			target = std::move(original);

			CHECK(target.size() == 2);
			CHECK(target.capacity() == 100);
			CHECK(target[0] == 0xBE);
			CHECK(target[1] == 0xEF);

#ifndef ENABLE_BYTEBUFFER_SBO
			CHECK(target.bytes() == originalPtr);
#endif
		}

		SUBCASE("Self move assignment")
		{
			ByteBuffer buffer(50);
			static constexpr std::uint8_t data[2] = {0xAA, 0xBB};
			buffer.append(data, 2);

			buffer = std::move(buffer);

			CHECK(buffer.size() == 2);
			CHECK(buffer.capacity() == 50);
			CHECK(buffer[0] == 0xAA);
			CHECK(buffer[1] == 0xBB);
		}

		SUBCASE("Move assignment preserves read cursor")
		{
			ByteBuffer original(50);
			static constexpr std::uint8_t data[6] = {10, 20, 30, 40, 50, 60};
			original.append(data, 6);
			original.readUint8();
			original.readUint8();
			original.readUint8(); // Read cursor at position 3

			ByteBuffer target(100);
			target = std::move(original);

			CHECK(target.readCursor() == 3);
			CHECK(target.readHeadroom() == 3);
			CHECK(target.readUint8() == 40);
		}

		SUBCASE("Chain move assignments")
		{
			ByteBuffer original(100);
			original += static_cast<std::uint8_t>(123);

			ByteBuffer buffer1(50);
			ByteBuffer buffer2(75);

			buffer2 = std::move(buffer1 = std::move(original));

			CHECK(buffer2.size() == 1);
			CHECK(buffer2[0] == 123);
		}
	}
}