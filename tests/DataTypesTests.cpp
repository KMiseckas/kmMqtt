#include <doctest.h>
#include <cleanMqtt/Mqtt/Packets/DataTypes.h>

TEST_SUITE("Data Type Tests")
{
	using namespace cleanMqtt::mqtt::packets;

	TEST_CASE("Binary Data")
	{
		SUBCASE("Default Constructor")
		{
			BinaryData data;
			CHECK(data.encodingSize() == 2);
			CHECK(data.bytes() == nullptr);
			CHECK(data.size() == 0);
		}

		SUBCASE("With Args Constructor")
		{
			constexpr std::uint16_t size{ 5 };
			const std::uint8_t* intPtr = new std::uint8_t[size]{ 1,2,3,4,5 };
			static constexpr std::size_t expectedEncodingSize = 2 + 5; //2 for data size and 5 for bytes in actual data.

			BinaryData data{ 5, intPtr };
			CHECK(data.encodingSize() == expectedEncodingSize);
			CHECK(data.bytes()[0] == 1);
			CHECK(data.bytes()[1] == 2);
			CHECK(data.bytes()[2] == 3);
			CHECK(data.bytes()[3] == 4);
			CHECK(data.bytes()[4] == 5);
			CHECK(data.size() == size);

			delete[] intPtr;
			CHECK(data.bytes()[0] == 1);
			CHECK(data.bytes()[1] == 2);
			CHECK(data.bytes()[2] == 3);
			CHECK(data.bytes()[3] == 4);
			CHECK(data.bytes()[4] == 5);
		}

		SUBCASE("Move Constructor")
		{
			constexpr std::uint16_t size{ 5 };
			const std::uint8_t* intPtr = new std::uint8_t[size]{ 1,2,3,4,5 };
			static constexpr std::size_t expectedEncodingSize = 2 + 5; //2 for data size and 5 for bytes in actual data.

			BinaryData* data = new BinaryData{ 5, intPtr };
			delete[] intPtr;

			BinaryData* dataMovedTo = new BinaryData(std::move(*data));
			delete data;

			CHECK(dataMovedTo->encodingSize() == expectedEncodingSize);
			CHECK(dataMovedTo->bytes()[0] == 1);
			CHECK(dataMovedTo->bytes()[1] == 2);
			CHECK(dataMovedTo->bytes()[2] == 3);
			CHECK(dataMovedTo->bytes()[3] == 4);
			CHECK(dataMovedTo->bytes()[4] == 5);
			CHECK(dataMovedTo->size() == size);

			delete dataMovedTo;
		}

		SUBCASE("Move Assignment")
		{
			constexpr std::uint16_t size{ 5 };
			const std::uint8_t* intPtr = new std::uint8_t[size]{ 1,2,3,4,5 };
			static constexpr std::size_t expectedEncodingSize = 2 + 5; //2 for data size and 5 for bytes in actual data.

			BinaryData* const data = new BinaryData{ 5, intPtr };
			delete[] intPtr;

			BinaryData* const dataMovedTo = new BinaryData{};
			*dataMovedTo = std::move(*data);
			delete data;

			CHECK(dataMovedTo->encodingSize() == expectedEncodingSize);
			CHECK(dataMovedTo->bytes()[0] == 1);
			CHECK(dataMovedTo->bytes()[1] == 2);
			CHECK(dataMovedTo->bytes()[2] == 3);
			CHECK(dataMovedTo->bytes()[3] == 4);
			CHECK(dataMovedTo->bytes()[4] == 5);
			CHECK(dataMovedTo->size() == size);

			delete dataMovedTo;
		}
	}

	TEST_CASE("Variable Byte Integer")
	{
		SUBCASE("No Arg Constructor")
		{
			VariableByteInteger varByteInt;
			CHECK(varByteInt.encodingSize() == 1);
			CHECK(varByteInt.uint32Value() == 0);
			CHECK(varByteInt.uint32EncodedBytes() == 0);
		}

		SUBCASE("Encoding / Decoding")
		{
			std::uint32_t val;
			std::uint32_t expectedEncodedVal;
			std::size_t expectedBytesSize;

			SUBCASE("2 (1 byte)")
			{
				val = 2;
				expectedEncodedVal = 0b00000010000000000000000000000000;
				expectedBytesSize = 1;
			}

			SUBCASE("10 (1 byte)")
			{
				val = 10;
				expectedEncodedVal = 0b00001010000000000000000000000000;
				expectedBytesSize = 1;
			}

			SUBCASE("268435455 (max - 4 bytes)")
			{
				val = 268435455;
				expectedEncodedVal = 0b11111111111111111111111101111111;
				expectedBytesSize = 4;
			}

			SUBCASE("128 (2 bytes)")
			{
				val = 128;
				expectedEncodedVal = 0b10000000000000010000000000000000;
				expectedBytesSize = 2;
			}

			SUBCASE("16384 (3 bytes)")
			{
				val = 16384;
				expectedEncodedVal = 0b10000000100000000000000100000000;
				expectedBytesSize = 3;
			}

			//Encoding
			const VariableByteInteger varByteInt{VariableByteInteger::tryCreateFromValue(val)};
			CHECK(varByteInt.encodingSize() == expectedBytesSize);
			CHECK(varByteInt.uint32Value() == val);
			CHECK(varByteInt.uint32EncodedBytes() == expectedEncodedVal);

			//Asignment to byte array for decoding.
			VariableByteInteger varByteIntDecode;
			std::uint8_t* encodedVarByteInt = new std::uint8_t[4]{ 0,0,0,0 };
			encodedVarByteInt[0] = (expectedEncodedVal >> (3 * 8)) & 0xff;
			encodedVarByteInt[1] = (expectedEncodedVal >> (2 * 8)) & 0xff;
			encodedVarByteInt[2] = (expectedEncodedVal >> (1 * 8)) & 0xff;
			encodedVarByteInt[3] = (expectedEncodedVal >> (0 * 8)) & 0xff;

			varByteIntDecode.decode(encodedVarByteInt);
			CHECK(varByteIntDecode.uint32Value() == val);
			CHECK(varByteIntDecode.encodingSize() == expectedBytesSize);
			CHECK(varByteIntDecode.uint32EncodedBytes() == 0); //Doesnt encode into uint32_t when decoding array.

			delete[] encodedVarByteInt;
		}

		SUBCASE("Assignment to Value Decoding")
		{
			VariableByteInteger varByteInt;
			varByteInt.setValue(10);

			CHECK(varByteInt.encodingSize() == 1);
			CHECK(varByteInt.uint32Value() == 10);
			CHECK(varByteInt.uint32EncodedBytes() == 0b00001010000000000000000000000000);
		}
	}

	TEST_CASE("UTF8String")
	{
		SUBCASE("Constructor / Assignment Operators")
		{
			UTF8String strEmpty;
			CHECK(strEmpty.stringBytes() == nullptr);
			CHECK(strEmpty.encodingSize() == 2);
			CHECK(strEmpty.getString() == "");
			CHECK(strEmpty.stringSize() == 0);

			UTF8String strFilled_0{ 5, "Hello" };
			CHECK(strFilled_0.stringBytes() != nullptr);
			CHECK(strFilled_0.encodingSize() == 7);
			CHECK(strFilled_0.getString().compare("Hello") == 0);
			CHECK(strFilled_0.stringSize() == 5);

			UTF8String* strFilled_1 = new UTF8String{ "Hello" };
			CHECK(strFilled_1->stringBytes() != nullptr);
			CHECK(strFilled_1->encodingSize() == 7);
			CHECK(strFilled_1->getString().compare("Hello") == 0);
			CHECK(strFilled_1->stringSize() == 5);

			UTF8String strFilled_2{ std::move(*strFilled_1) };
			delete strFilled_1;
			CHECK(strFilled_2.stringBytes() != nullptr);
			CHECK(strFilled_2.encodingSize() == 7);
			CHECK(strFilled_2.getString().compare("Hello") == 0);
			CHECK(strFilled_2.stringSize() == 5);

			strFilled_1 = new UTF8String{ "Hello2" };
			CHECK(strFilled_1->stringBytes() != nullptr);
			CHECK(strFilled_1->encodingSize() == 8);
			CHECK(strFilled_1->getString().compare("Hello2") == 0);
			CHECK(strFilled_1->stringSize() == 6);

			UTF8String strFilled_Copy_1 = *strFilled_1;
			UTF8String strFilled_Copy_2 = *strFilled_1;
			UTF8String strFilled_Move_1 = std::move(*strFilled_1);
			delete strFilled_1;

			CHECK(strFilled_Copy_1.stringBytes() != nullptr);
			CHECK(strFilled_Copy_1.encodingSize() == 8);
			CHECK(strFilled_Copy_1.getString().compare("Hello2") == 0);
			CHECK(strFilled_Copy_1.stringSize() == 6);
			CHECK(strFilled_Copy_2.stringBytes() != nullptr);
			CHECK(strFilled_Copy_2.encodingSize() == 8);
			CHECK(strFilled_Copy_2.getString().compare("Hello2") == 0);
			CHECK(strFilled_Copy_2.stringSize() == 6);
			CHECK(strFilled_Move_1.stringBytes() != nullptr);
			CHECK(strFilled_Move_1.encodingSize() == 8);
			CHECK(strFilled_Move_1.getString().compare("Hello2") == 0);
			CHECK(strFilled_Move_1.stringSize() == 6);
		}

		SUBCASE("Constructor with size and char*")
		{
			const char* text = "World";
			UTF8String str(5, text);
			CHECK(str.stringBytes() != nullptr);
			CHECK(str.encodingSize() == 7);
			CHECK(str.getString() == "World");
			CHECK(str.stringSize() == 5);
		}

		SUBCASE("Constructor with std::string")
		{
			std::string text = "Test";
			UTF8String str(text);
			CHECK(str.stringBytes() != nullptr);
			CHECK(str.encodingSize() == 6);
			CHECK(str.getString() == "Test");
			CHECK(str.stringSize() == 4);
		}

		SUBCASE("Copy Constructor")
		{
			UTF8String original(4, "Copy");
			UTF8String copy(original);
			CHECK(copy.stringBytes() != nullptr);
			CHECK(copy.encodingSize() == 6);
			CHECK(copy.getString() == "Copy");
			CHECK(copy.stringSize() == 4);
		}

		SUBCASE("Move Constructor")
		{
			UTF8String original(3, "abc");
			UTF8String moved(std::move(original));
			CHECK(moved.stringBytes() != nullptr);
			CHECK(moved.encodingSize() == 5);
			CHECK(moved.getString() == "abc");
			CHECK(moved.stringSize() == 3);
		}

		SUBCASE("Copy Assignment")
		{
			UTF8String a(2, "hi");
			UTF8String b;
			b = a;
			CHECK(b.stringBytes() != nullptr);
			CHECK(b.encodingSize() == 4);
			CHECK(b.getString() == "hi");
			CHECK(b.stringSize() == 2);
		}

		SUBCASE("Move Assignment")
		{
			UTF8String a(2, "ok");
			UTF8String b;
			b = std::move(a);
			CHECK(b.stringBytes() != nullptr);
			CHECK(b.encodingSize() == 4);
			CHECK(b.getString() == "ok");
			CHECK(b.stringSize() == 2);
		}

		SUBCASE("Assignment from std::string")
		{
			UTF8String str;
			std::string s = "assign";
			str = s;
			CHECK(str.stringBytes() != nullptr);
			CHECK(str.encodingSize() == 8);
			CHECK(str.getString() == "assign");
			CHECK(str.stringSize() == 6);
		}

		SUBCASE("Max Size String")
		{
			std::string maxStr(65535, 'x');
			UTF8String str(maxStr);
			CHECK(str.stringBytes() != nullptr);
			CHECK(str.encodingSize() == 65537);
			CHECK(str.getString() == maxStr);
			CHECK(str.stringSize() == 65535);
		}

		SUBCASE("Encode/Decode Roundtrip")
		{
			std::string text = "roundtrip";
			UTF8String str(text);
			cleanMqtt::ByteBuffer buf{ str.encodingSize()};
			str.encode(buf);

			str.decode(buf);
			CHECK(str.getString() == text);
			CHECK(str.stringSize() == text.size());
		}

		SUBCASE("Encode/Decode with special UTF-8 characters")
		{
			// "Héłłø 🌍" contains multi-byte UTF-8 characters.
			std::string special = u8"Héłłø 🌍";
			UTF8String str(special);
			cleanMqtt::ByteBuffer buf{ str.encodingSize() };
			str.encode(buf);

			UTF8String decoded;
			decoded.decode(buf);

			CHECK(decoded.getString() == special);
			CHECK(decoded.stringSize() == special.size());
		}
	}

	TEST_CASE("UTF8StringPair")
	{
		SUBCASE("Constructor / Assignment Operators")
		{
			UTF8StringPair strEmpty;
			CHECK(strEmpty.first().stringBytes() == nullptr);
			CHECK(strEmpty.second().stringBytes() == nullptr);
			CHECK(strEmpty.encodingSize() == 4);
			CHECK(strEmpty.first().getString() == "");
			CHECK(strEmpty.second().getString() == "");
			CHECK(strEmpty.first().stringSize() == 0);
			CHECK(strEmpty.second().stringSize() == 0);

			UTF8StringPair strFilled_0{ 5, "Hello", 3, "bye"};
			CHECK(strFilled_0.first().stringBytes() != nullptr);
			CHECK(strFilled_0.second().stringBytes() != nullptr);
			CHECK(strFilled_0.encodingSize() == 12);
			CHECK(strFilled_0.first().getString().compare("Hello") == 0);
			CHECK(strFilled_0.second().getString().compare("bye") == 0);
			CHECK(strFilled_0.first().stringSize() == 5);
			CHECK(strFilled_0.second().stringSize() == 3);

			UTF8StringPair* strFilled_1 = new UTF8StringPair{ "Hello" , "Bye"};
			UTF8StringPair strFilled_2{ std::move(*strFilled_1) };
			delete strFilled_1;
			CHECK(strFilled_2.first().stringBytes() != nullptr);
			CHECK(strFilled_2.second().stringBytes() != nullptr);
			CHECK(strFilled_2.encodingSize() == 12);
			CHECK(strFilled_2.first().getString().compare("Hello") == 0);
			CHECK(strFilled_2.second().getString().compare("Bye") == 0);
			CHECK(strFilled_2.first().stringSize() == 5);
			CHECK(strFilled_2.second().stringSize() == 3);
		}
	}
}