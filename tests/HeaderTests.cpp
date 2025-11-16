#include <doctest.h>
#include <cleanMqtt/Mqtt/Packets/FixedHeader.h>
#include <cleanMqtt/Mqtt/Packets/Connection/Headers/ConnectVariableHeader.h>
#include <cleanMqtt/Mqtt/Packets/Connection/Headers/ConnectPayloadHeader.h>
#include <cleanMqtt/ByteBuffer.h>
#include <cleanMqtt/Mqtt/Packets/PacketUtils.h>
#include <cleanMqtt/Mqtt/Packets/Publish/Headers/PublishPayloadHeader.h>
#include <cleanMqtt/Mqtt/Packets/Publish/Headers/PublishVariableHeader.h>

TEST_SUITE("Header Tests")
{
	using cleanMqtt::mqtt::FixedHeader;
	using cleanMqtt::mqtt::FixedHeaderFlags;
	using cleanMqtt::mqtt::PacketType;
	using cleanMqtt::mqtt::VariableByteInteger;
	using cleanMqtt::mqtt::ConnectVariableHeader;
	using cleanMqtt::ByteBuffer;
	using cleanMqtt::mqtt::MqttVersion;

	TEST_CASE("Fixed Header")
	{
		SUBCASE("Encoding")
		{
			FixedHeader header;
			header.flags.setFlagValue(1, true); //-> 0b00000001
			CHECK_NOTHROW(header.flags.setFlagValue(4 | 8, 3)); //-> 0b00001101
			header.packetType = PacketType::CONNECT;
			header.remainingLength.setValue(3123); //2 bytes worth

			CHECK(header.getEncodedBytesSize() == 3);

			ByteBuffer buffer{ header.getEncodedBytesSize() };

			CHECK_NOTHROW(header.encode(buffer));
			CHECK(buffer.size() == 3);
			CHECK(buffer.capacity() == 3);
			CHECK(buffer.headroom() == 0);

			FixedHeaderFlags flags{ 0 };
			VariableByteInteger varByte;

			CHECK(cleanMqtt::mqtt::checkPacketType(buffer.bytes(), buffer.size()) == PacketType::CONNECT);
			CHECK(buffer[0] == 0b00011101);
			varByte.decode(&buffer[1]);
			CHECK(varByte.uint32Value() == 3123);
		}
	}

	TEST_CASE("Connect Variable Header")
	{
		SUBCASE("Encoding")
		{
			SUBCASE("Default")
			{
				ConnectVariableHeader header;
				header.protocolLevel = static_cast<MqttVersion>(0b00010000);

				ByteBuffer buffer{ header.getEncodedBytesSize() };

				CHECK_NOTHROW(header.encode(buffer));
				CHECK(header.flags.getFlags() == 0);
				CHECK(header.keepAliveInSec == 0);
				CHECK(header.properties.size() == 0);
				CHECK(static_cast<std::uint8_t>(header.protocolLevel) == 16);
				CHECK(header.protocolName.getString().empty());

				CHECK(buffer.capacity() == buffer.size());
				CHECK(buffer.headroom() == 0);

				CHECK(buffer[0] == 0);	//Protocol Name UTF8 Length - Byte 0
				CHECK(buffer[1] == 0);	//Protocol Name UTF8 Length - Byte 1
				CHECK(buffer[2] == 16);	//Protocol Level
				CHECK(buffer[3] == 0);	//Connect Flags
				CHECK(buffer[4] == 0);	//Keep alive in seconds - byte 0
				CHECK(buffer[5] == 0);	//Keep alive in seconds - byte 1
				CHECK(buffer[6] == 0);	//Properties Length Variable Byte Integer
			}

			SUBCASE("Params 1")
			{
				using namespace cleanMqtt::mqtt;

				EncodedConnectFlags flags;
				flags.setFlagValue(ConnectFlags::CLEAN_START, true);
				flags.setFlagValue(ConnectFlags::PASSWORD, true);
				flags.setFlagValue(ConnectFlags::USER_NAME, true);
				flags.setFlagValue(ConnectFlags::WILL_QOS, 2);
				flags.setFlagValue(ConnectFlags::WILL_RETAIN, false);

				Properties properties;
				properties.tryAddProperty<PropertyType::WILL_DELAY_INTERVAL>(200); //Size = 5 (Property Id (1) + Data Type UINT32 (4))

				ConnectVariableHeader header{
					"MQTT",
					MqttVersion::MQTT_5_0,
					43000,
					std::move(flags),
					std::move(properties)
				};

				ByteBuffer buffer{ header.getEncodedBytesSize() };

				CHECK_NOTHROW(header.encode(buffer));

				CHECK(header.flags.getFlags() == 0b11010010);
				CHECK(header.keepAliveInSec == 43000);
				CHECK(header.properties.size() == 5);
				CHECK(static_cast<std::uint8_t>(header.protocolLevel) == 5);
				CHECK(header.protocolName.getString().compare("MQTT") == 0);

				CHECK(buffer.capacity() == buffer.size());
				CHECK(buffer.headroom() == 0);

				CHECK(buffer[0] == 0);			//Protocol Name UTF8 Length - Byte 0
				CHECK(buffer[1] == 4);			//Protocol Name UTF8 Length - Byte 1
				CHECK(buffer[2] == 'M');		//Protocol Name
				CHECK(buffer[3] == 'Q');		//Protocol Name
				CHECK(buffer[4] == 'T');		//Protocol Name
				CHECK(buffer[5] == 'T');		//Protocol Name
				CHECK(buffer[6] == 5);			//Protocol Level
				CHECK(buffer[7] == 0b11010010);	//Connect Flags
				CHECK(buffer[8] == 0b10100111);	//Keep alive in seconds - byte 0
				CHECK(buffer[9] == 0b11111000);	//Keep alive in seconds - byte 1
				CHECK(buffer[10] == 5);			//Properties Length Variable Byte Integer
				VariableByteInteger propertyIdVar{ VariableByteInteger::tryCreateFromValue(buffer[11])};
				CHECK(propertyIdVar.uint32Value() == static_cast<std::uint8_t>(PropertyType::WILL_DELAY_INTERVAL));
				CHECK(buffer[12] == 0);			//200 in uint32
				CHECK(buffer[13] == 0);			//...
				CHECK(buffer[14] == 0);			//...
				CHECK(buffer[15] == 200);		//...
			}

			SUBCASE("Params - Properties")
			{
				using namespace cleanMqtt::mqtt;

				EncodedConnectFlags flags;
				flags.setFlagValue(ConnectFlags::CLEAN_START, true);
				flags.setFlagValue(ConnectFlags::PASSWORD, true);
				flags.setFlagValue(ConnectFlags::USER_NAME, true);
				flags.setFlagValue(ConnectFlags::WILL_QOS, 2);
				flags.setFlagValue(ConnectFlags::WILL_RETAIN, false);

				Properties properties;
				properties.tryAddProperty<PropertyType::WILL_DELAY_INTERVAL>(200); //Size = 5 (Property Id (1) + Data Type UINT32 (4))
				properties.tryAddProperty<PropertyType::REASON_STRING>(UTF8String("REASON 1")); //Size 1 + 2 + 8 = 11
				constexpr std::uint8_t binaryArr[5]{ 1,2,3,4,5 };
				properties.tryAddProperty<PropertyType::AUTHENTICATION_DATA>(BinaryData(5, binaryArr)); //Size 1 + 2 + 5 = 8;
				properties.tryAddProperty<PropertyType::USER_PROPERTY>(UTF8StringPair("KEY_1", "VALUE_1")); //Size 1 + 2 + 5 + 2 + 7 = 17;
				properties.tryAddProperty<PropertyType::USER_PROPERTY>(UTF8StringPair("KEY_1", "VALUE_1")); //Size 1 + 2 + 5 + 2 + 7 = 17;
				properties.tryAddProperty<PropertyType::MAXIMUM_QOS>( 2); //Size 1 + 1 = 2;

				ConnectVariableHeader header{
					"MQTT",
					MqttVersion::MQTT_5_0,
					43000,
					std::move(flags),
					std::move(properties)
				};

				ByteBuffer buffer{ header.getEncodedBytesSize() };

				CHECK_NOTHROW(header.encode(buffer));

				CHECK(header.flags.getFlags() == 0b11010010);
				CHECK(header.keepAliveInSec == 43000);
				CHECK(header.properties.size() == 60);
				CHECK(static_cast<std::uint8_t>(header.protocolLevel) == 5);
				CHECK(header.protocolName.getString().compare("MQTT") == 0);

				CHECK(buffer.capacity() == buffer.size());
				CHECK(buffer.headroom() == 0);

				CHECK(buffer[0] == 0);			//Protocol Name UTF8 Length - Byte 0
				CHECK(buffer[1] == 4);			//Protocol Name UTF8 Length - Byte 1
				CHECK(buffer[2] == 'M');		//Protocol Name
				CHECK(buffer[3] == 'Q');		//Protocol Name
				CHECK(buffer[4] == 'T');		//Protocol Name
				CHECK(buffer[5] == 'T');		//Protocol Name
				CHECK(buffer[6] == 5);			//Protocol Level
				CHECK(buffer[7] == 0b11010010);	//Connect Flags
				CHECK(buffer[8] == 0b10100111);	//Keep alive in seconds - byte 0
				CHECK(buffer[9] == 0b11111000);	//Keep alive in seconds - byte 1
				CHECK(buffer[10] == 60);			//Properties Length Variable Byte Integer

				std::uint8_t nextIdByte = 11;
				bool successAuth{ false };
				bool successWill{ false };
				bool successReason{ false };
				bool successUser1{ false };
				bool successUser2{ false };
				bool successMaxQos{ false };
				for (size_t i = 0; i < 6; i++)
				{
					VariableByteInteger propertyIdVal;
					propertyIdVal.setValue(buffer[nextIdByte]);

					switch (static_cast<PropertyType>(propertyIdVal.uint32Value()))
					{
					case PropertyType::AUTHENTICATION_DATA:
						CHECK(buffer[++nextIdByte] == 0); //Binary data length - MSB
						CHECK(buffer[++nextIdByte] == 5); //Binary data length - LSB
						CHECK(buffer[++nextIdByte] == 1);
						CHECK(buffer[++nextIdByte] == 2);
						CHECK(buffer[++nextIdByte] == 3);
						CHECK(buffer[++nextIdByte] == 4);
						CHECK(buffer[++nextIdByte] == 5);
						successAuth = true;
						break;
					case PropertyType::WILL_DELAY_INTERVAL:
						CHECK(buffer[++nextIdByte] == 0);			//200 in uint32
						CHECK(buffer[++nextIdByte] == 0);			//...
						CHECK(buffer[++nextIdByte] == 0);			//...
						CHECK(buffer[++nextIdByte] == 200);		//...
						successWill = true;
						break;
					case PropertyType::REASON_STRING:
						CHECK(buffer[++nextIdByte] == 0); //String length uint16 - MSB = 0;
						CHECK(buffer[++nextIdByte] == 8); //String length uint16 - LSB = 8;
						CHECK(buffer[++nextIdByte] == 'R');
						CHECK(buffer[++nextIdByte] == 'E');
						CHECK(buffer[++nextIdByte] == 'A');
						CHECK(buffer[++nextIdByte] == 'S');
						CHECK(buffer[++nextIdByte] == 'O');
						CHECK(buffer[++nextIdByte] == 'N');
						CHECK(buffer[++nextIdByte] == ' ');
						CHECK(buffer[++nextIdByte] == '1');
						successReason = true;
						break;
					case PropertyType::USER_PROPERTY:
						CHECK(buffer[++nextIdByte] == 0);
						CHECK(buffer[++nextIdByte] == 5);
						CHECK(buffer[++nextIdByte] == 'K');
						CHECK(buffer[++nextIdByte] == 'E');
						CHECK(buffer[++nextIdByte] == 'Y');
						CHECK(buffer[++nextIdByte] == '_');
						CHECK(buffer[++nextIdByte] == '1');
						CHECK(buffer[++nextIdByte] == 0);
						CHECK(buffer[++nextIdByte] == 7);
						CHECK(buffer[++nextIdByte] == 'V');
						CHECK(buffer[++nextIdByte] == 'A');
						CHECK(buffer[++nextIdByte] == 'L');
						CHECK(buffer[++nextIdByte] == 'U');
						CHECK(buffer[++nextIdByte] == 'E');
						CHECK(buffer[++nextIdByte] == '_');
						CHECK(buffer[++nextIdByte] == '1');

						if (successUser1)
						{
							successUser2 = true;
						}
						successUser1 = true;
						break;
					case PropertyType::MAXIMUM_QOS:
						CHECK(buffer[++nextIdByte] == 2);
						successMaxQos = true;
						break;
					}

					nextIdByte += 1; //For property ID byte.
				}

				CHECK((successAuth&& successMaxQos&& successReason&& successUser1&& successUser2&& successWill) == true);
			}
		}
	}

	TEST_CASE("Connect Payload Header")
	{
		using namespace cleanMqtt::mqtt;

		SUBCASE("Encoding")
		{
			SUBCASE("Default")
			{
				ConnectPayloadHeader header;
				header.clientId = "Client_1";

				ByteBuffer buffer{ header.getEncodedBytesSize() };

				CHECK_NOTHROW(header.encode(buffer));
				CHECK(header.clientId.getString().compare("Client_1") == 0);
				CHECK(header.password.size() == 0);
				CHECK(header.willPayload.size() == 0);
				CHECK(header.willProperties.size() == 0);
				CHECK(header.willTopic.getString().empty());
				CHECK(header.userName.getString().empty());

				CHECK(buffer.capacity() == buffer.size());
				CHECK(buffer.headroom() == 0);

				CHECK(buffer[0] == 0);
				CHECK(buffer[1] == 8);
				CHECK(buffer[2] == 'C');
				CHECK(buffer[3] == 'l');
				CHECK(buffer[4] == 'i');
				CHECK(buffer[5] == 'e');
				CHECK(buffer[6] == 'n');
				CHECK(buffer[7] == 't');
				CHECK(buffer[8] == '_');
				CHECK(buffer[9] == '1');
			}

			SUBCASE("Params 1")
			{
				Properties properties;

				ConnectPayloadHeader header;
				header.clientId = "Client_1"; //10
				header.userName = "User"; //6
				header.willTopic = "Topic"; //7
				header.willProperties = std::move(properties); //0

				ByteBuffer buffer{ header.getEncodedBytesSize() };

				CHECK_NOTHROW(header.encode(buffer));

				CHECK(buffer.capacity() == buffer.size());
				CHECK(buffer.headroom() == 0);

				CHECK(buffer[0] == 0);
				CHECK(buffer[1] == 8);
				CHECK(buffer[2] == 'C');
				CHECK(buffer[3] == 'l');
				CHECK(buffer[4] == 'i');
				CHECK(buffer[5] == 'e');
				CHECK(buffer[6] == 'n');
				CHECK(buffer[7] == 't');
				CHECK(buffer[8] == '_');
				CHECK(buffer[9] == '1');
				CHECK(buffer[10] == 0);
				CHECK(buffer[11] == 5);
				CHECK(buffer[12] == 'T');
				CHECK(buffer[13] == 'o');
				CHECK(buffer[14] == 'p');
				CHECK(buffer[15] == 'i');
				CHECK(buffer[16] == 'c');
				CHECK(buffer[17] == 0);
				CHECK(buffer[18] == 4);
				CHECK(buffer[19] == 'U');
				CHECK(buffer[20] == 's');
				CHECK(buffer[21] == 'e');
				CHECK(buffer[22] == 'r');
			}

			SUBCASE("Params 2")
			{
				Properties properties;
				properties.tryAddProperty<PropertyType::WILL_DELAY_INTERVAL>(200);

				ConnectPayloadHeader header;
				header.clientId = "Client_1"; //10
				header.userName = "User"; //6
				header.willTopic = "Topic"; //7
				header.willProperties = std::move(properties); //

				ByteBuffer buffer{ header.getEncodedBytesSize() };

				CHECK_NOTHROW(header.encode(buffer));

				CHECK(buffer.capacity() == buffer.size());
				CHECK(buffer.headroom() == 0);

				CHECK(buffer[0] == 0);
				CHECK(buffer[1] == 8);
				CHECK(buffer[2] == 'C');
				CHECK(buffer[3] == 'l');
				CHECK(buffer[4] == 'i');
				CHECK(buffer[5] == 'e');
				CHECK(buffer[6] == 'n');
				CHECK(buffer[7] == 't');
				CHECK(buffer[8] == '_');
				CHECK(buffer[9] == '1');
				CHECK(buffer[10] == 5);
				CHECK(buffer[10+1] == static_cast<std::uint8_t>(PropertyType::WILL_DELAY_INTERVAL));
				CHECK(buffer[11+1] == 0);
				CHECK(buffer[12+1] == 0);
				CHECK(buffer[13+1] == 0);
				CHECK(buffer[14+1] == 200);
				CHECK(buffer[15+1] == 0);
				CHECK(buffer[16+1] == 5);
				CHECK(buffer[17+1] == 'T');
				CHECK(buffer[18+1] == 'o');
				CHECK(buffer[19+1] == 'p');
				CHECK(buffer[20+1] == 'i');
				CHECK(buffer[21+1] == 'c');
				CHECK(buffer[22+1] == 0);
				CHECK(buffer[23+1] == 4);
				CHECK(buffer[24+1] == 'U');
				CHECK(buffer[25+1] == 's');
				CHECK(buffer[26+1] == 'e');
				CHECK(buffer[27+1] == 'r');
			}
		}
	}


	TEST_CASE("Publish Payload Header")
	{
		using cleanMqtt::mqtt::PublishPayloadHeader;
		using cleanMqtt::mqtt::BinaryData;

		SUBCASE("Encoding Default")
		{
			PublishPayloadHeader header;
			CHECK(header.payload.size() == 0);

			ByteBuffer buffer{ header.getEncodedBytesSize() };
			CHECK_NOTHROW(header.encode(buffer));
			CHECK(buffer.size() == 0);
		}

		SUBCASE("Encoding With Payload")
		{
			const std::uint8_t data[] = { 0xDE, 0xAD, 0xBE, 0xEF };
			ByteBuffer payload{ 4 };
			payload.append(data, 4);
			PublishPayloadHeader header{ std::move(payload) };

			CHECK(header.payload.size() == 4);
			ByteBuffer buffer{ header.getEncodedBytesSize() };
			CHECK_NOTHROW(header.encode(buffer));
			CHECK(buffer.size() == 4); //4 bytes data
			CHECK(buffer[0] == 0xDE);
			CHECK(buffer[1] == 0xAD);
			CHECK(buffer[2] == 0xBE);
			CHECK(buffer[3] == 0xEF);
		}

		SUBCASE("Decoding")
		{
			const std::uint8_t data[] = { 0, 3, 0x11, 0x22, 0x33 };
			ByteBuffer buffer{ 5 };
			buffer.append(data, 5);

			PublishPayloadHeader header;
			auto result = header.decode(buffer);
			CHECK(result.isSuccess());
			CHECK(header.payload.size() == 5);
			CHECK(header.payload.bytes()[2] == 0x11);
			CHECK(header.payload.bytes()[3] == 0x22);
			CHECK(header.payload.bytes()[4] == 0x33);
		}
	}

	TEST_CASE("Publish Variable Header")
	{
		using namespace cleanMqtt::mqtt;
		using cleanMqtt::mqtt::PublishVariableHeader;

		SUBCASE("Encoding")
		{
			SUBCASE("Default QOS_0")
			{
				PublishVariableHeader header;
				header.topicName = "test/topic";
				header.qos = Qos::QOS_0;

				ByteBuffer buffer{ header.getEncodedBytesSize() };

				CHECK_NOTHROW(header.encode(buffer));
				CHECK(header.topicName.getString().compare("test/topic") == 0);
				CHECK(header.packetIdentifier == 0);
				CHECK(header.properties.size() == 0);
				CHECK(header.qos == Qos::QOS_0);

				CHECK(buffer.capacity() == buffer.size());
				CHECK(buffer.headroom() == 0);

				// Topic name: length (2 bytes) + "test/topic" (10 bytes) + Properties length (1 byte)
				CHECK(buffer[0] == 0);		//Topic name length MSB
				CHECK(buffer[1] == 10);		//Topic name length LSB
				CHECK(buffer[2] == 't');
				CHECK(buffer[3] == 'e');
				CHECK(buffer[4] == 's');
				CHECK(buffer[5] == 't');
				CHECK(buffer[6] == '/');
				CHECK(buffer[7] == 't');
				CHECK(buffer[8] == 'o');
				CHECK(buffer[9] == 'p');
				CHECK(buffer[10] == 'i');
				CHECK(buffer[11] == 'c');
				CHECK(buffer[12] == 0); //Properties length
			}

			SUBCASE("QOS_1 with Packet ID")
			{
				PublishVariableHeader header{
					"sensor/data",
					1234,
					Properties{},
					Qos::QOS_1
				};

				ByteBuffer buffer{ header.getEncodedBytesSize() };

				CHECK_NOTHROW(header.encode(buffer));
				CHECK(header.topicName.getString().compare("sensor/data") == 0);
				CHECK(header.packetIdentifier == 1234);
				CHECK(header.properties.size() == 0);
				CHECK(header.qos == Qos::QOS_1);

				CHECK(buffer.capacity() == buffer.size());
				CHECK(buffer.headroom() == 0);

				//Topic name: length (2 bytes) + "sensor/data" (11 bytes) + Packet ID (2 bytes) + Properties length (1 byte)
				CHECK(buffer[0] == 0);		//Topic name length MSB
				CHECK(buffer[1] == 11);		//Topic name length LSB
				CHECK(buffer[2] == 's');
				CHECK(buffer[3] == 'e');
				CHECK(buffer[4] == 'n');
				CHECK(buffer[5] == 's');
				CHECK(buffer[6] == 'o');
				CHECK(buffer[7] == 'r');
				CHECK(buffer[8] == '/');
				CHECK(buffer[9] == 'd');
				CHECK(buffer[10] == 'a');
				CHECK(buffer[11] == 't');
				CHECK(buffer[12] == 'a');
				CHECK(buffer[13] == 0x04);	//Packet ID MSB
				CHECK(buffer[14] == 0xD2);	//Packet ID LSB
				CHECK(buffer[15] == 0);
			}

			SUBCASE("QOS_2 with Properties")
			{
				Properties properties;
				properties.tryAddProperty<PropertyType::MESSAGE_EXPIRY_INTERVAL>(3600);
				properties.tryAddProperty<PropertyType::CONTENT_TYPE>(UTF8String("application/json"));

				PublishVariableHeader header{
					"home/temperature",
					5678,
					std::move(properties),
					Qos::QOS_2
				};

				ByteBuffer buffer{ header.getEncodedBytesSize() };

				CHECK_NOTHROW(header.encode(buffer));
				CHECK(header.topicName.getString().compare("home/temperature") == 0);
				CHECK(header.packetIdentifier == 5678);
				CHECK(header.properties.size() > 0);
				CHECK(header.qos == Qos::QOS_2);

				CHECK(buffer.capacity() == buffer.size());
				CHECK(buffer.headroom() == 0);

				// Topic name: length (2 bytes) + "home/temperature" (16 bytes)
				CHECK(buffer[0] == 0);		//Topic name length MSB
				CHECK(buffer[1] == 16);		//Topic name length LSB
				CHECK(buffer[2] == 'h');
				CHECK(buffer[3] == 'o');
				CHECK(buffer[4] == 'm');
				CHECK(buffer[5] == 'e');
				CHECK(buffer[6] == '/');
				CHECK(buffer[7] == 't');
				CHECK(buffer[8] == 'e');
				CHECK(buffer[9] == 'm');
				CHECK(buffer[10] == 'p');
				CHECK(buffer[11] == 'e');
				CHECK(buffer[12] == 'r');
				CHECK(buffer[13] == 'a');
				CHECK(buffer[14] == 't');
				CHECK(buffer[15] == 'u');
				CHECK(buffer[16] == 'r');
				CHECK(buffer[17] == 'e');
				CHECK(buffer[18] == 0x16);	//Packet ID MSB
				CHECK(buffer[19] == 0x2E);	//Packet ID LSB
			}
		}

		SUBCASE("Decoding")
		{
			SUBCASE("QOS_0 Topic Only")
			{
				const std::uint8_t data[] = {
					0x00, 0x04,				//Topic length = 4
					't', 'e', 's', 't',		//Topic name = "test"
					0x00					//Properties length = 0
				};
				ByteBuffer buffer{ 7 };
				buffer.append(data, 7);

				PublishVariableHeader header;
				header.qos = Qos::QOS_0;
				auto result = header.decode(buffer);

				CHECK(result.isSuccess());
				CHECK(header.topicName.getString().compare("test") == 0);
				CHECK(header.packetIdentifier == 0);	// Should remain 0 for QOS_0
			}

			SUBCASE("QOS_1 with Packet ID")
			{
				const std::uint8_t data[] = {
					0x00, 0x04,				//Topic length = 4
					'd', 'a', 't', 'a',		//Topic name = "data"
					0x03, 0xE8,				//Packet ID = 1000 (0x03E8)
					0x00					//Properties length = 0
				};
				ByteBuffer buffer{ 9 };
				buffer.append(data, 9);

				PublishVariableHeader header;
				header.qos = Qos::QOS_1;
				auto result = header.decode(buffer);

				CHECK(result.isSuccess());
				CHECK(header.topicName.getString().compare("data") == 0);
				CHECK(header.packetIdentifier == 1000);
				CHECK(header.properties.size() == 0);
			}

			SUBCASE("QOS_2 with Properties")
			{
				const std::uint8_t data[] = {
					0x00, 0x03,				//Topic length = 3
					'f', 'o', 'o',			//Topic name = "foo"
					0x07, 0xD0,				//Packet ID = 2000 (0x07D0)
					0x05,					//Properties length = 5
					0x02,					//Property: MESSAGE_EXPIRY_INTERVAL
					0x00, 0x00, 0x0E, 0x10	//Value: 3600 seconds
				};
				ByteBuffer buffer{ sizeof(data)};
				buffer.append(data, sizeof(data));

				PublishVariableHeader header;
				header.qos = Qos::QOS_2;
				auto result = header.decode(buffer);

				CHECK(result.isSuccess());
				CHECK(header.topicName.getString().compare("foo") == 0);
				CHECK(header.packetIdentifier == 2000);
				CHECK(header.properties.size() == 5);
				CHECK(header.properties.count() == 1);
			}
		}

		SUBCASE("Size Calculations")
		{
			SUBCASE("QOS_0 No Properties")
			{
				PublishVariableHeader header;
				header.topicName = "test";
				header.qos = Qos::QOS_0;

				// Topic name encoding: 2 bytes length + 4 bytes content + 1 properties size = 7 bytes total
				CHECK(header.getEncodedBytesSize() == 7);
			}

			SUBCASE("QOS_1 No Properties")
			{
				PublishVariableHeader header;
				header.topicName = "test";
				header.packetIdentifier = 1234;
				header.qos = Qos::QOS_1;

				CHECK(header.getEncodedBytesSize() == 9);
			}

			SUBCASE("QOS_1 with Properties")
			{
				Properties properties;
				properties.tryAddProperty<PropertyType::MESSAGE_EXPIRY_INTERVAL>(3600);

				PublishVariableHeader header;
				header.topicName = "test";
				header.packetIdentifier = 1234;
				header.properties = std::move(properties);
				header.qos = Qos::QOS_1;

				//Topic name (2 + 4) + Packet ID (2) + Properties length (1) + Property (1 + 4) = 14 bytes
				CHECK(header.getEncodedBytesSize() == 14);
			}
		}
	}

	TEST_CASE("PacketUtils Tests")
	{
		SUBCASE("Check Packet Type")
		{
			ByteBuffer buffer{ 2 };
			buffer += 0b00010000; //CONNECT packet type
			buffer += 0; //Remaining length
			CHECK(cleanMqtt::mqtt::checkPacketType(buffer.bytes(), buffer.size()) == PacketType::CONNECT);
		}

		SUBCASE("Check Packet Type RESERVED")
		{
			ByteBuffer buffer{ 2 };
			buffer += 0b00000000; //Invalid packet type
			buffer += 0; //Remaining length
			CHECK(cleanMqtt::mqtt::checkPacketType(buffer.bytes(), buffer.size()) == PacketType::RESERVED);
		}
	}
}