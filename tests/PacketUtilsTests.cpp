#include <doctest.h>
#include <kmMqtt/Mqtt/Packets/PacketUtils.h>

TEST_SUITE("Packet Utils Tests")
{
	using namespace kmMqtt::mqtt;

	TEST_CASE("Are Bits Contiguous")
	{
		static constexpr std::uint8_t u8Bits_contiguous_0 = 0b00001100;
		static constexpr std::uint8_t u8Bits_contiguous_1 = 0b11111100;
		static constexpr std::uint8_t u8Bits_contiguous_2 = 0b00000011;
		static constexpr std::uint8_t u8Bits_contiguous_3 = 0b00000001;
		static constexpr std::uint8_t u8Bits_contiguous_4 = 0b11111111;
		static constexpr std::uint8_t u8Bits_contiguous_5 = 0b00000000;

		static constexpr std::uint16_t u16Bits_contiguous_0 = 0b0000110000000000;
		static constexpr std::uint16_t u16Bits_contiguous_1 = 0b1111110000000000;
		static constexpr std::uint16_t u16Bits_contiguous_2 = 0b0000001111110000;
		static constexpr std::uint16_t u16Bits_contiguous_3 = 0b0000000000000001;

		static constexpr std::uint16_t u16Bits_notContiguous_0 = 0b0000110100000000;
		static constexpr std::uint16_t u16Bits_notContiguous_1 = 0b1011111111111111;
		static constexpr std::uint16_t u16Bits_notContiguous_2 = 0b0000101111110111;
		static constexpr std::uint16_t u16Bits_notContiguous_3 = 0b1000000000000001;

		CHECK(arePositiveBitsContiguous<std::uint8_t>(u8Bits_contiguous_0) == true);
		CHECK(arePositiveBitsContiguous<std::uint8_t>(u8Bits_contiguous_1) == true);
		CHECK(arePositiveBitsContiguous<std::uint8_t>(u8Bits_contiguous_2) == true);
		CHECK(arePositiveBitsContiguous<std::uint8_t>(u8Bits_contiguous_3) == true);
		CHECK(arePositiveBitsContiguous<std::uint8_t>(u8Bits_contiguous_4) == true);
		CHECK(arePositiveBitsContiguous<std::uint8_t>(u8Bits_contiguous_5) == false);

		CHECK(arePositiveBitsContiguous<std::uint16_t>(u16Bits_contiguous_0) == true);
		CHECK(arePositiveBitsContiguous<std::uint16_t>(u16Bits_contiguous_1) == true);
		CHECK(arePositiveBitsContiguous<std::uint16_t>(u16Bits_contiguous_2) == true);
		CHECK(arePositiveBitsContiguous<std::uint16_t>(u16Bits_contiguous_3) == true);

		CHECK(arePositiveBitsContiguous<std::uint16_t>(u16Bits_notContiguous_0) == false);
		CHECK(arePositiveBitsContiguous<std::uint16_t>(u16Bits_notContiguous_1) == false);
		CHECK(arePositiveBitsContiguous<std::uint16_t>(u16Bits_notContiguous_2) == false);
		CHECK(arePositiveBitsContiguous<std::uint16_t>(u16Bits_notContiguous_3) == false);
	}

	TEST_CASE("Get First Bit Position")
	{
		static constexpr std::uint8_t bits_0 = 0b00001100;
		static constexpr std::uint8_t bits_1 = 0b11111100;
		static constexpr std::uint8_t bits_2 = 0b00000011;
		static constexpr std::uint8_t bits_3 = 0b00000001;
		static constexpr std::uint8_t bits_4 = 0b11111111;
		static constexpr std::uint8_t bits_5 = 0b00000000;

		CHECK(getFirstBitPosition<std::uint8_t>(bits_0) == 2);
		CHECK(getFirstBitPosition<std::uint8_t>(bits_1) == 2);
		CHECK(getFirstBitPosition<std::uint8_t>(bits_2) == 0);
		CHECK(getFirstBitPosition<std::uint8_t>(bits_3) == 0);
		CHECK(getFirstBitPosition<std::uint8_t>(bits_4) == 0);
		CHECK(getFirstBitPosition<std::uint8_t>(bits_5) == -1);
	}
}