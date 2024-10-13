#include <doctest.h>
#include <cleanMqtt/Mqtt/Packets/Flags.h>
#include <cleanMqtt/GlobalMacros.h>

TEST_SUITE("Flags Tests")
{
	namespace
	{
		enum class TestFlagsEnum : std::uint8_t
		{
			FLAG_1 = 1 << 0,
			FLAG_2 = 1 << 1 | 1 << 2,
			FLAG_3 = 1 << 3,
		};
		ENUM_FLAG_OPERATORS(TestFlagsEnum)

		using cleanMqtt::mqtt::packets::Flags;

		struct TestFlags : public Flags<std::uint8_t, TestFlagsEnum, 15>
		{
			TestFlags(std::uint8_t flags) : Flags(flags)
			{
			}

			TestFlags(bool flag1, std::uint8_t flag2, bool flag3) : Flags(0)
			{
				setFlagValue(TestFlagsEnum::FLAG_1, flag1);
				setFlagValue(TestFlagsEnum::FLAG_2, flag2);
				setFlagValue(TestFlagsEnum::FLAG_3, flag3);
			};
		};
	}

	TEST_CASE("Construction & get raw flag")
	{
		TestFlags flagsA{ 0U };
		CHECK(flagsA.getFlags() == 0U);

		TestFlags flagsB{ 5U };
		CHECK(flagsB.getFlags() == 5U);

		Flags<std::uint16_t, std::uint8_t, 15> flagsC{ 0U };
		CHECK(flagsC.getFlags() == 0U);

		Flags<std::uint16_t, std::uint8_t, 15> flagsD{ 5U };
		CHECK(flagsD.getFlags() == 5U);
	}

	TEST_CASE("Flag - flag values")
	{
		TestFlags flags = TestFlags(true, 0, true);
		CHECK(flags.getFlags() == 0b00001001);
		CHECK(flags.getFlagValue<bool>(TestFlagsEnum::FLAG_1) == true);
		CHECK(flags.getFlagValue(TestFlagsEnum::FLAG_2) == 0);
		CHECK(flags.getFlagValue<bool>(TestFlagsEnum::FLAG_3) == true);

		flags = TestFlags(false, 0, false);
		CHECK(flags.getFlags() == 0b00000000);
		CHECK(flags.getFlagValue<bool>(TestFlagsEnum::FLAG_1) == false);
		CHECK(flags.getFlagValue(TestFlagsEnum::FLAG_2) == 0);
		CHECK(flags.getFlagValue<bool>(TestFlagsEnum::FLAG_3) == false);

		flags = TestFlags(true, 1, false);
		CHECK(flags.getFlags() == 0b00000011);
		CHECK(flags.getFlagValue<bool>(TestFlagsEnum::FLAG_1) == true);
		CHECK(flags.getFlagValue(TestFlagsEnum::FLAG_2) == 1);
		CHECK(flags.getFlagValue<bool>(TestFlagsEnum::FLAG_3) == false);

		flags = TestFlags(false, 2, false);
		CHECK(flags.getFlags() == 0b00000100);
		CHECK(flags.getFlagValue<bool>(TestFlagsEnum::FLAG_1) == false);
		CHECK(flags.getFlagValue(TestFlagsEnum::FLAG_2) == 2);
		CHECK(flags.getFlagValue<bool>(TestFlagsEnum::FLAG_3) == false);

		flags = TestFlags(false, 3, false);
		CHECK(flags.getFlags() == 0b00000110);
		CHECK(flags.getFlagValue<bool>(TestFlagsEnum::FLAG_1) == false);
		CHECK(flags.getFlagValue(TestFlagsEnum::FLAG_2) == 3);
		CHECK(flags.getFlagValue<bool>(TestFlagsEnum::FLAG_3) == false);

		flags = TestFlags(false, 4, false);
		CHECK(flags.getFlags() == 0b00000000);
		CHECK(flags.getFlagValue<bool>(TestFlagsEnum::FLAG_1) == false);
		CHECK(flags.getFlagValue(TestFlagsEnum::FLAG_2) == 0);
		CHECK(flags.getFlagValue<bool>(TestFlagsEnum::FLAG_3) == false);

		flags = TestFlags(false, 3, false);
		CHECK(flags.getFlags() == 0b00000110);
		CHECK(flags.getFlagValue<bool>(TestFlagsEnum::FLAG_1) == false);
		CHECK(flags.getFlagValue(TestFlagsEnum::FLAG_2) == 3);
		CHECK(flags.getFlagValue<bool>(TestFlagsEnum::FLAG_3) == false);
		CHECK_NOTHROW(flags.setFlagValue(TestFlagsEnum::FLAG_2, 0));
		CHECK(flags.getFlagValue(TestFlagsEnum::FLAG_2) == 0);
		CHECK_NOTHROW(flags.setFlagValue(TestFlagsEnum::FLAG_2, 1));
		CHECK(flags.getFlagValue(TestFlagsEnum::FLAG_2) == 1);
		CHECK_NOTHROW(flags.setFlagValue(TestFlagsEnum::FLAG_3, true));
		CHECK(flags.getFlagValue<bool>(TestFlagsEnum::FLAG_3) == true);
		CHECK_NOTHROW(flags.setFlagValue(TestFlagsEnum::FLAG_3, false));
		CHECK(flags.getFlagValue<bool>(TestFlagsEnum::FLAG_3) == false);

		flags = TestFlags(false, 0, false);
		CHECK_NOTHROW(flags.setMultipleFlagsValue(TestFlagsEnum::FLAG_3 | TestFlagsEnum::FLAG_1, 1));
		CHECK(flags.getFlagValue<bool>(TestFlagsEnum::FLAG_3) == true);
		CHECK(flags.getFlagValue<bool>(TestFlagsEnum::FLAG_1) == true);
		CHECK(flags.getFlagValue(TestFlagsEnum::FLAG_2) == 0);
		CHECK_NOTHROW(flags.setMultipleFlagsValue(TestFlagsEnum::FLAG_3 | TestFlagsEnum::FLAG_1, 0));
		CHECK(flags.getFlagValue<bool>(TestFlagsEnum::FLAG_3) == false);
		CHECK(flags.getFlagValue<bool>(TestFlagsEnum::FLAG_1) == false);
		CHECK(flags.getFlagValue(TestFlagsEnum::FLAG_2) == 0);

	}
}