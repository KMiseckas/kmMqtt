#include <doctest.h>
#include <cleanMqtt/GlobalMacros.h>
#include <cleanMqtt/Utils/Utils.h>

TEST_SUITE("Utils Tests")
{
	using namespace cleanMqtt;

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

	TEST_CASE("Split by Delimiter")
	{
		auto tokens{ splitByDelimiter("hello there steve", " ") };
		CHECK(tokens[0] == "hello");
		CHECK(tokens[1] == "there");
		CHECK(tokens[2] == "steve");

		tokens = splitByDelimiter("hello,there,steve", ",");
		CHECK(tokens[0] == "hello");
		CHECK(tokens[1] == "there");
		CHECK(tokens[2] == "steve");

		tokens = splitByDelimiter("hello,there.steve", ",");
		CHECK(tokens[0] == "hello");
		CHECK(tokens[1] == "there.steve");
	}


    TEST_CASE("Split Up MQTT packets - separateMqttPacketByteBuffers")
    {
		using namespace cleanMqtt;

		// Helper to create a simple MQTT packet: [fixed header][remaining length][payload]
		auto makePacket = [](std::uint8_t type, const std::vector<std::uint8_t>& payload) -> ByteBuffer
			{
				ByteBuffer buf(1 + 1 + payload.size()); // type + remLen + payload
				buf.append(&type, 1);
				std::uint8_t remLen = static_cast<std::uint8_t>(payload.size());
				buf.append(&remLen, 1);
				if (!payload.empty())
				{
					buf.append(payload.data(), payload.size());
				}
				return buf;
			};

		// Create two packets and concatenate them
		ByteBuffer packet1 = makePacket(0x30, { 0x01, 0x02, 0x03 }); // type=0x30, payload=3 bytes
		ByteBuffer packet2 = makePacket(0x32, { 0xAA, 0xBB });       // type=0x32, payload=2 bytes

		ByteBuffer combined{ packet1.size() + packet2.size() };
		combined.append(packet1.bytes(), packet1.size());
		combined.append(packet2.bytes(), packet2.size());

		std::vector<ByteBuffer> packets;
		std::size_t leftOverPos{ 0 };

		bool result = separateMqttPacketByteBuffers(combined, packets, leftOverPos);

		CHECK(result == true);
		CHECK(packets.size() == 2);

		// Check first packet
		CHECK(packets[0].size() == 5); // 1 (type) + 1 (rem len) + 3 (payload)
		CHECK(packets[0].bytes()[0] == 0x30);
		CHECK(packets[0].bytes()[1] == 0x03);
		CHECK(packets[0].bytes()[2] == 0x01);
		CHECK(packets[0].bytes()[3] == 0x02);
		CHECK(packets[0].bytes()[4] == 0x03);

		// Check second packet
		CHECK(packets[1].size() == 4); // 1 (type) + 1 (rem len) + 2 (payload)
		CHECK(packets[1].bytes()[0] == 0x32);
		CHECK(packets[1].bytes()[1] == 0x02);
		CHECK(packets[1].bytes()[2] == 0xAA);
		CHECK(packets[1].bytes()[3] == 0xBB);

		CHECK(leftOverPos == 0);

		// Test with incomplete packet at the end
		ByteBuffer incomplete{ packet1.size() + packet2.size() };
		incomplete.append(packet1.bytes(), packet1.size());
		incomplete.append(packet2.bytes(), packet2.size() - 1); // remove last byte

		packets.clear();
		result = separateMqttPacketByteBuffers(incomplete, packets, leftOverPos);

		CHECK(result == true);
		CHECK(packets.size() == 1);
		CHECK(leftOverPos == packet2.size() - 1);
	}
}