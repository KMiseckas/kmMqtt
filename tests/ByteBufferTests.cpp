#include <doctest.h>
#include <cleanMqtt/ByteBuffer.h>

TEST_SUITE("Byte Buffer Tests")
{
	using namespace cleanMqtt;

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
}