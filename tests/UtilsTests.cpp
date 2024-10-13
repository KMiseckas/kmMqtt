#include <doctest.h>
#include <cleanMqtt/GlobalMacros.h>

TEST_SUITE("Utils Tests")
{
	namespace
	{
		enum class FlagEnum : std::uint8_t
		{
			FLAG_1 = 1 << 0,
			FLAG_2 = 1 << 1,
			FLAG_3 = 1 << 2,
			FLAG_4 = 1 << 3,
			FLAG_5 = 1 << 4,
		};

		ENUM_FLAG_OPERATORS(FlagEnum)
	}

	TEST_CASE("Flag operator overloads")
	{
		CHECK(static_cast<std::uint8_t>(FlagEnum::FLAG_1 | FlagEnum::FLAG_2) == 0b00000011);
		CHECK(static_cast<std::uint8_t>(FlagEnum::FLAG_1 | FlagEnum::FLAG_1) == 0b00000001);
		CHECK(static_cast<std::uint8_t>(FlagEnum::FLAG_1 | FlagEnum::FLAG_5) == 0b00010001);
		CHECK(static_cast<std::uint8_t>(FlagEnum::FLAG_1 | FlagEnum::FLAG_1 | FlagEnum::FLAG_1) == 0b00000001);
		CHECK(static_cast<std::uint8_t>(FlagEnum::FLAG_1 | FlagEnum::FLAG_2 | FlagEnum::FLAG_2) == 0b00000011);
		CHECK(static_cast<std::uint8_t>(FlagEnum::FLAG_1 | FlagEnum::FLAG_2 | FlagEnum::FLAG_3) == 0b00000111);
		CHECK(static_cast<std::uint8_t>(FlagEnum::FLAG_1 | FlagEnum::FLAG_2 | FlagEnum::FLAG_5) == 0b00010011);

		FlagEnum bitmask{ FlagEnum::FLAG_5 };
		bitmask |= FlagEnum::FLAG_1;
		CHECK(static_cast<std::uint8_t>(bitmask) == 0b00010001);
		bitmask |= FlagEnum::FLAG_2;
		CHECK(static_cast<std::uint8_t>(bitmask) == 0b00010011);
		bitmask = bitmask & FlagEnum::FLAG_2;
		CHECK(static_cast<std::uint8_t>(bitmask) == 0b00000010);

		CHECK(static_cast<std::uint8_t>(FlagEnum::FLAG_1 & FlagEnum::FLAG_2) == 0b00000000);
		CHECK(static_cast<std::uint8_t>(FlagEnum::FLAG_1 & FlagEnum::FLAG_1) == 0b00000001);
		CHECK(static_cast<std::uint8_t>(FlagEnum::FLAG_1 & FlagEnum::FLAG_5) == 0b00000000);
	}
}