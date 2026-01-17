// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <doctest.h>
#include <kmMqtt/ByteBuffer.h>
#include <kmMqtt/Mqtt/Packets/DataTypes.h>
#include <kmMqtt/Mqtt/Packets/PropertyType.h>
#include <kmMqtt/Mqtt/Packets/Properties.h>
#include <kmMqtt/GlobalMacros.h>
#include <cstring>

TEST_SUITE("Properties Tests")
{
	using namespace kmMqtt::mqtt;

	TEST_CASE("Adding/Getting property")
	{
		Properties properties;
		CHECK(properties.tryAddProperty<PropertyType::ASSIGNED_CLIENT_IDENTIFIER>(UTF8String("Client_123")));
		CHECK(properties.tryAddProperty<PropertyType::AUTHENTICATION_DATA>(BinaryData{}));
		CHECK(properties.tryAddProperty<PropertyType::RETAIN_AVAILABLE>(true));
		CHECK(properties.tryAddProperty<PropertyType::CONTENT_TYPE>(UTF8String("JSON")));
		CHECK(properties.tryAddProperty<PropertyType::MAXIMUM_PACKET_SIZE>(2222222222));

		CHECK(properties.tryAddProperty<PropertyType::USER_PROPERTY>(UTF8StringPair("key", "value")));
		CHECK(properties.tryAddProperty<PropertyType::WILL_DELAY_INTERVAL>(32));
		CHECK(properties.tryAddProperty<PropertyType::WILDCARD_SUBSCRIPTION_AVAILABLE>(150));

		const UTF8String* valueUtf8String;
		const bool* valueBool;
		const std::uint32_t* valueUint32;
		std::vector<const BinaryData*> dataVec;

		CHECK(properties.tryGetProperty<UTF8String>(PropertyType::ASSIGNED_CLIENT_IDENTIFIER, valueUtf8String));
		CHECK(valueUtf8String->getString() == "Client_123");

		CHECK(properties.tryGetProperty<bool>(PropertyType::RETAIN_AVAILABLE, valueBool));
		CHECK(*valueBool == true);

		CHECK(properties.tryGetProperty<UTF8String>(PropertyType::CONTENT_TYPE, valueUtf8String));
		CHECK(valueUtf8String->getString() == "JSON");

		CHECK(properties.tryGetProperty<std::uint32_t>(PropertyType::MAXIMUM_PACKET_SIZE, valueUint32));
		CHECK(*valueUint32 == 2222222222);

		CHECK(properties.tryGetProperty<BinaryData>(PropertyType::USER_PROPERTY, dataVec));
		CHECK(dataVec.empty() == false);

		CHECK(properties.tryAddProperty<PropertyType::MAXIMUM_PACKET_SIZE>(30) == false);
		CHECK(properties.tryAddProperty<PropertyType::CONTENT_TYPE>(UTF8String("JSON_OVERRIDE")) == false);
	}

	TEST_CASE("Add/Get Duplicate Properties")
	{
		Properties properties;
		CHECK(properties.tryAddProperty<PropertyType::USER_PROPERTY>(UTF8StringPair("key1", "value1")));
		CHECK(properties.tryAddProperty<PropertyType::USER_PROPERTY>(UTF8StringPair("key2", "value2")));

		std::vector<const UTF8StringPair*> userProperties;
		CHECK(properties.tryGetProperty<UTF8StringPair>(PropertyType::USER_PROPERTY, userProperties));
		CHECK(userProperties.size() == 2);
		
		bool foundKey1Value1 = false;
		bool foundKey2Value2 = false;
		
		for (const auto* prop : userProperties)
		{
			if (prop->first().getString() == "key1" && prop->second().getString() == "value1")
			{
				foundKey1Value1 = true;
			}
			if (prop->first().getString() == "key2" && prop->second().getString() == "value2")
			{
				foundKey2Value2 = true;
			}
		}
		
		CHECK(foundKey1Value1);
		CHECK(foundKey2Value2);
	}

	TEST_CASE("PropertyType enum matches MQTT Spec")
	{
		CHECK(static_cast<std::uint8_t>(PropertyType::PAYLOAD_FORMAT_INDICATOR) == 1U);
		CHECK(static_cast<std::uint8_t>(PropertyType::MESSAGE_EXPIRY_INTERVAL) == 2U);
		CHECK(static_cast<std::uint8_t>(PropertyType::CONTENT_TYPE) == 3U);
		CHECK(static_cast<std::uint8_t>(PropertyType::RESPONSE_TOPIC) == 8U);
		CHECK(static_cast<std::uint8_t>(PropertyType::CORRELATION_DATA) == 9U);
		CHECK(static_cast<std::uint8_t>(PropertyType::SUBSCRIPTION_IDENTIFIER) == 11U);
		CHECK(static_cast<std::uint8_t>(PropertyType::SESSION_EXPIRY_INTERVAL) == 17U);
		CHECK(static_cast<std::uint8_t>(PropertyType::ASSIGNED_CLIENT_IDENTIFIER) == 18U);
		CHECK(static_cast<std::uint8_t>(PropertyType::SERVER_KEEP_ALIVE) == 19U);
		CHECK(static_cast<std::uint8_t>(PropertyType::AUTHENTICATION_METHOD) == 21U);
		CHECK(static_cast<std::uint8_t>(PropertyType::AUTHENTICATION_DATA) == 22U);
		CHECK(static_cast<std::uint8_t>(PropertyType::REQUEST_PROBLEM_INFORMATION) == 23U);
		CHECK(static_cast<std::uint8_t>(PropertyType::WILL_DELAY_INTERVAL) == 24U);
		CHECK(static_cast<std::uint8_t>(PropertyType::REQUEST_RESPONSE_INFORMATION) == 25U);
		CHECK(static_cast<std::uint8_t>(PropertyType::RESPONSE_INFORMATION) == 26U);
		CHECK(static_cast<std::uint8_t>(PropertyType::SERVER_REFERENCE) == 28U);
		CHECK(static_cast<std::uint8_t>(PropertyType::REASON_STRING) == 31U);
		CHECK(static_cast<std::uint8_t>(PropertyType::RECEIVE_MAXIMUM) == 33U);
		CHECK(static_cast<std::uint8_t>(PropertyType::TOPIC_ALIAS_MAXIMUM) == 34U);
		CHECK(static_cast<std::uint8_t>(PropertyType::TOPIC_ALIAS) == 35U);
		CHECK(static_cast<std::uint8_t>(PropertyType::MAXIMUM_QOS) == 36U);
		CHECK(static_cast<std::uint8_t>(PropertyType::RETAIN_AVAILABLE) == 37U);
		CHECK(static_cast<std::uint8_t>(PropertyType::USER_PROPERTY) == 38U);
		CHECK(static_cast<std::uint8_t>(PropertyType::MAXIMUM_PACKET_SIZE) == 39U);
		CHECK(static_cast<std::uint8_t>(PropertyType::WILDCARD_SUBSCRIPTION_AVAILABLE) == 40U);
		CHECK(static_cast<std::uint8_t>(PropertyType::SUBSCRIPTION_IDENTIFIER_AVAILABLE) == 41U);
		CHECK(static_cast<std::uint8_t>(PropertyType::SHARED_SUBSCRIPTION_AVAILABLE) == 42U);
	}

	TEST_CASE("Property Encoders - UInt8")
	{
		kmMqtt::ByteBuffer buffer(100);

		SUBCASE("Encode PAYLOAD_FORMAT_INDICATOR")
		{
			std::uint8_t value = 1;
			propertyEncodings::encode(buffer, PropertyType::PAYLOAD_FORMAT_INDICATOR, &value);
			
			CHECK(buffer.size() == 2);
			CHECK(buffer[0] == static_cast<std::uint8_t>(PropertyType::PAYLOAD_FORMAT_INDICATOR));
			CHECK(buffer[1] == value);
		}

		SUBCASE("Encode REQUEST_PROBLEM_INFORMATION")
		{
			std::uint8_t value = 0;
			propertyEncodings::encode(buffer, PropertyType::REQUEST_PROBLEM_INFORMATION, &value);
			
			CHECK(buffer.size() == 2);
			CHECK(buffer[0] == static_cast<std::uint8_t>(PropertyType::REQUEST_PROBLEM_INFORMATION));
			CHECK(buffer[1] == value);
		}

		SUBCASE("Encode MAXIMUM_QOS")
		{
			std::uint8_t value = 2;
			propertyEncodings::encode(buffer, PropertyType::MAXIMUM_QOS, &value);
			
			CHECK(buffer.size() == 2);
			CHECK(buffer[0] == static_cast<std::uint8_t>(PropertyType::MAXIMUM_QOS));
			CHECK(buffer[1] == value);
		}

		SUBCASE("Encode RETAIN_AVAILABLE")
		{
			std::uint8_t value = 1;
			propertyEncodings::encode(buffer, PropertyType::RETAIN_AVAILABLE, &value);
			
			CHECK(buffer.size() == 2);
			CHECK(buffer[0] == static_cast<std::uint8_t>(PropertyType::RETAIN_AVAILABLE));
			CHECK(buffer[1] == value);
		}
	}

	TEST_CASE("Property Encoders - UInt16")
	{
		kmMqtt::ByteBuffer buffer(100);

		SUBCASE("Encode SERVER_KEEP_ALIVE")
		{
			std::uint16_t value = 60;
			propertyEncodings::encode(buffer, PropertyType::SERVER_KEEP_ALIVE, &value);
			
			CHECK(buffer.size() == 3);
			CHECK(buffer[0] == static_cast<std::uint8_t>(PropertyType::SERVER_KEEP_ALIVE));
			CHECK(buffer[1] == 0x00);
			CHECK(buffer[2] == 0x3C);
		}

		SUBCASE("Encode RECEIVE_MAXIMUM")
		{
			std::uint16_t value = 0x1234;
			propertyEncodings::encode(buffer, PropertyType::RECEIVE_MAXIMUM, &value);
			
			CHECK(buffer.size() == 3);
			CHECK(buffer[0] == static_cast<std::uint8_t>(PropertyType::RECEIVE_MAXIMUM));
			CHECK(buffer[1] == 0x12);
			CHECK(buffer[2] == 0x34);
		}

		SUBCASE("Encode TOPIC_ALIAS")
		{
			std::uint16_t value = 0xABCD;
			propertyEncodings::encode(buffer, PropertyType::TOPIC_ALIAS, &value);
			
			CHECK(buffer.size() == 3);
			CHECK(buffer[0] == static_cast<std::uint8_t>(PropertyType::TOPIC_ALIAS));
			CHECK(buffer[1] == 0xAB);
			CHECK(buffer[2] == 0xCD);
		}
	}

	TEST_CASE("Property Encoders - UInt32")
	{
		kmMqtt::ByteBuffer buffer(100);

		SUBCASE("Encode MESSAGE_EXPIRY_INTERVAL")
		{
			std::uint32_t value = 3600;
			propertyEncodings::encode(buffer, PropertyType::MESSAGE_EXPIRY_INTERVAL, &value);
			
			CHECK(buffer.size() == 5);
			CHECK(buffer[0] == static_cast<std::uint8_t>(PropertyType::MESSAGE_EXPIRY_INTERVAL));
			CHECK(buffer[1] == 0x00);
			CHECK(buffer[2] == 0x00);
			CHECK(buffer[3] == 0x0E);
			CHECK(buffer[4] == 0x10);
		}

		SUBCASE("Encode SESSION_EXPIRY_INTERVAL")
		{
			std::uint32_t value = 0x12345678;
			propertyEncodings::encode(buffer, PropertyType::SESSION_EXPIRY_INTERVAL, &value);
			
			CHECK(buffer.size() == 5);
			CHECK(buffer[0] == static_cast<std::uint8_t>(PropertyType::SESSION_EXPIRY_INTERVAL));
			CHECK(buffer[1] == 0x12);
			CHECK(buffer[2] == 0x34);
			CHECK(buffer[3] == 0x56);
			CHECK(buffer[4] == 0x78);
		}

		SUBCASE("Encode MAXIMUM_PACKET_SIZE")
		{
			std::uint32_t value = 268435455;
			propertyEncodings::encode(buffer, PropertyType::MAXIMUM_PACKET_SIZE, &value);
			
			CHECK(buffer.size() == 5);
			CHECK(buffer[0] == static_cast<std::uint8_t>(PropertyType::MAXIMUM_PACKET_SIZE));
		}
	}

	TEST_CASE("Property Encoders - UTF8String")
	{
		kmMqtt::ByteBuffer buffer(100);

		SUBCASE("Encode CONTENT_TYPE")
		{
			UTF8String value("json");
			propertyEncodings::encode(buffer, PropertyType::CONTENT_TYPE, &value);
			
			CHECK(buffer.size() == 7); // 1 (property id) + 2 (length) + 4 (string)
			CHECK(buffer[0] == static_cast<std::uint8_t>(PropertyType::CONTENT_TYPE));
		}

		SUBCASE("Encode RESPONSE_TOPIC")
		{
			UTF8String value("test/topic");
			propertyEncodings::encode(buffer, PropertyType::RESPONSE_TOPIC, &value);
			
			CHECK(buffer.size() == 13); // 1 + 2 + 10
			CHECK(buffer[0] == static_cast<std::uint8_t>(PropertyType::RESPONSE_TOPIC));
		}

		SUBCASE("Encode ASSIGNED_CLIENT_IDENTIFIER")
		{
			UTF8String value("client123");
			propertyEncodings::encode(buffer, PropertyType::ASSIGNED_CLIENT_IDENTIFIER, &value);
			
			CHECK(buffer.size() == 12); // 1 + 2 + 9
			CHECK(buffer[0] == static_cast<std::uint8_t>(PropertyType::ASSIGNED_CLIENT_IDENTIFIER));
		}
	}

	TEST_CASE("Property Encoders - BinaryData")
	{
		kmMqtt::ByteBuffer buffer(100);

		SUBCASE("Encode CORRELATION_DATA")
		{
			const std::uint8_t data[] = {0xDE, 0xAD, 0xBE, 0xEF};
			BinaryData value(4, data);
			propertyEncodings::encode(buffer, PropertyType::CORRELATION_DATA, &value);
			
			CHECK(buffer.size() == 7); // 1 + 2 + 4
			CHECK(buffer[0] == static_cast<std::uint8_t>(PropertyType::CORRELATION_DATA));
		}

		SUBCASE("Encode AUTHENTICATION_DATA")
		{
			const std::uint8_t data[] = {0x01, 0x02, 0x03};
			BinaryData value(3, data);
			propertyEncodings::encode(buffer, PropertyType::AUTHENTICATION_DATA, &value);
			
			CHECK(buffer.size() == 6); // 1 + 2 + 3
			CHECK(buffer[0] == static_cast<std::uint8_t>(PropertyType::AUTHENTICATION_DATA));
		}
	}

	TEST_CASE("Property Encoders - UTF8StringPair")
	{
		kmMqtt::ByteBuffer buffer(100);

		SUBCASE("Encode USER_PROPERTY")
		{
			UTF8StringPair value("key", "value"); //5 + 7
			propertyEncodings::encode(buffer, PropertyType::USER_PROPERTY, &value);
			
			CHECK(buffer.size() == 13); // 1(Porperty) + 2(UTF8 Size) + 3(Key) + 2(UTF8 Size) + 5(Value)
			CHECK(buffer[0] == static_cast<std::uint8_t>(PropertyType::USER_PROPERTY));
		}
	}

	TEST_CASE("Property Encoders - VariableByteInteger")
	{
		kmMqtt::ByteBuffer buffer(100);

		SUBCASE("Encode SUBSCRIPTION_IDENTIFIER - small value")
		{
			bool success;
			VariableByteInteger value = VariableByteInteger::tryCreateFromValue(127, &success);
			CHECK(success);
			propertyEncodings::encode(buffer, PropertyType::SUBSCRIPTION_IDENTIFIER, &value);
			
			CHECK(buffer.size() == 2); // 1 + 1
			CHECK(buffer[0] == static_cast<std::uint8_t>(PropertyType::SUBSCRIPTION_IDENTIFIER));
		}

		SUBCASE("Encode SUBSCRIPTION_IDENTIFIER - large value")
		{
			bool success;
			VariableByteInteger value = VariableByteInteger::tryCreateFromValue(16383, &success);
			CHECK(success);
			propertyEncodings::encode(buffer, PropertyType::SUBSCRIPTION_IDENTIFIER, &value);
			
			CHECK(buffer.size() == 3); // 1 + 2
			CHECK(buffer[0] == static_cast<std::uint8_t>(PropertyType::SUBSCRIPTION_IDENTIFIER));
		}
	}

	TEST_CASE("Property Decoders - UInt8")
	{
		SUBCASE("Decode PAYLOAD_FORMAT_INDICATOR")
		{
			kmMqtt::ByteBuffer buffer(10);
			buffer += static_cast<std::uint8_t>(1);
			
			void* result = propertyDecodings::decode(buffer, PropertyType::PAYLOAD_FORMAT_INDICATOR);
			CHECK(result != nullptr);
			
			std::uint8_t* value = static_cast<std::uint8_t*>(result);
			CHECK(*value == 1);
			
			propertyDestructors::destruct(result, PropertyType::PAYLOAD_FORMAT_INDICATOR);
		}

		SUBCASE("Decode REQUEST_PROBLEM_INFORMATION")
		{
			kmMqtt::ByteBuffer buffer(10);
			buffer += static_cast<std::uint8_t>(0);
			
			void* result = propertyDecodings::decode(buffer, PropertyType::REQUEST_PROBLEM_INFORMATION);
			CHECK(result != nullptr);
			
			std::uint8_t* value = static_cast<std::uint8_t*>(result);
			CHECK(*value == 0);
			
			propertyDestructors::destruct(result, PropertyType::REQUEST_PROBLEM_INFORMATION);
		}

		SUBCASE("Decode WILDCARD_SUBSCRIPTION_AVAILABLE")
		{
			kmMqtt::ByteBuffer buffer(10);
			buffer += static_cast<std::uint8_t>(1);
			
			void* result = propertyDecodings::decode(buffer, PropertyType::WILDCARD_SUBSCRIPTION_AVAILABLE);
			CHECK(result != nullptr);
			
			std::uint8_t* value = static_cast<std::uint8_t*>(result);
			CHECK(*value == 1);
			
			propertyDestructors::destruct(result, PropertyType::WILDCARD_SUBSCRIPTION_AVAILABLE);
		}
	}

	TEST_CASE("Property Decoders - UInt16")
	{
		SUBCASE("Decode SERVER_KEEP_ALIVE")
		{
			kmMqtt::ByteBuffer buffer(10);
			buffer.append(static_cast<std::uint16_t>(60));
			
			void* result = propertyDecodings::decode(buffer, PropertyType::SERVER_KEEP_ALIVE);
			CHECK(result != nullptr);
			
			std::uint16_t* value = static_cast<std::uint16_t*>(result);
			CHECK(*value == 60);
			
			propertyDestructors::destruct(result, PropertyType::SERVER_KEEP_ALIVE);
		}

		SUBCASE("Decode RECEIVE_MAXIMUM")
		{
			kmMqtt::ByteBuffer buffer(10);
			buffer.append(static_cast<std::uint16_t>(0x1234));
			
			void* result = propertyDecodings::decode(buffer, PropertyType::RECEIVE_MAXIMUM);
			CHECK(result != nullptr);
			
			std::uint16_t* value = static_cast<std::uint16_t*>(result);
			CHECK(*value == 0x1234);
			
			propertyDestructors::destruct(result, PropertyType::RECEIVE_MAXIMUM);
		}

		SUBCASE("Decode TOPIC_ALIAS_MAXIMUM")
		{
			kmMqtt::ByteBuffer buffer(10);
			buffer.append(static_cast<std::uint16_t>(100));
			
			void* result = propertyDecodings::decode(buffer, PropertyType::TOPIC_ALIAS_MAXIMUM);
			CHECK(result != nullptr);
			
			std::uint16_t* value = static_cast<std::uint16_t*>(result);
			CHECK(*value == 100);
			
			propertyDestructors::destruct(result, PropertyType::TOPIC_ALIAS_MAXIMUM);
		}
	}

	TEST_CASE("Property Decoders - UInt32")
	{
		SUBCASE("Decode MESSAGE_EXPIRY_INTERVAL")
		{
			kmMqtt::ByteBuffer buffer(10);
			buffer.append(static_cast<std::uint32_t>(3600));
			
			void* result = propertyDecodings::decode(buffer, PropertyType::MESSAGE_EXPIRY_INTERVAL);
			CHECK(result != nullptr);
			
			std::uint32_t* value = static_cast<std::uint32_t*>(result);
			CHECK(*value == 3600);
			
			propertyDestructors::destruct(result, PropertyType::MESSAGE_EXPIRY_INTERVAL);
		}

		SUBCASE("Decode SESSION_EXPIRY_INTERVAL")
		{
			kmMqtt::ByteBuffer buffer(10);
			buffer.append(static_cast<std::uint32_t>(0x12345678));
			
			void* result = propertyDecodings::decode(buffer, PropertyType::SESSION_EXPIRY_INTERVAL);
			CHECK(result != nullptr);
			
			std::uint32_t* value = static_cast<std::uint32_t*>(result);
			CHECK(*value == 0x12345678);
			
			propertyDestructors::destruct(result, PropertyType::SESSION_EXPIRY_INTERVAL);
		}

		SUBCASE("Decode MAXIMUM_PACKET_SIZE")
		{
			kmMqtt::ByteBuffer buffer(10);
			buffer.append(static_cast<std::uint32_t>(268435455));
			
			void* result = propertyDecodings::decode(buffer, PropertyType::MAXIMUM_PACKET_SIZE);
			CHECK(result != nullptr);
			
			std::uint32_t* value = static_cast<std::uint32_t*>(result);
			CHECK(*value == 268435455);
			
			propertyDestructors::destruct(result, PropertyType::MAXIMUM_PACKET_SIZE);
		}
	}

	TEST_CASE("Property Decoders - UTF8String")
	{
		SUBCASE("Decode CONTENT_TYPE")
		{
			kmMqtt::ByteBuffer buffer(20);
			UTF8String original("json");
			original.encode(buffer);
			
			void* result = propertyDecodings::decode(buffer, PropertyType::CONTENT_TYPE);
			CHECK(result != nullptr);
			
			UTF8String* value = static_cast<UTF8String*>(result);
			CHECK(value->getString() == "json");
			
			propertyDestructors::destruct(result, PropertyType::CONTENT_TYPE);
		}

		SUBCASE("Decode RESPONSE_TOPIC")
		{
			kmMqtt::ByteBuffer buffer(30);
			UTF8String original("test/topic");
			original.encode(buffer);
			
			void* result = propertyDecodings::decode(buffer, PropertyType::RESPONSE_TOPIC);
			CHECK(result != nullptr);
			
			UTF8String* value = static_cast<UTF8String*>(result);
			CHECK(value->getString() == "test/topic");
			
			propertyDestructors::destruct(result, PropertyType::RESPONSE_TOPIC);
		}

		SUBCASE("Decode ASSIGNED_CLIENT_IDENTIFIER")
		{
			kmMqtt::ByteBuffer buffer(30);
			UTF8String original("client123");
			original.encode(buffer);
			
			void* result = propertyDecodings::decode(buffer, PropertyType::ASSIGNED_CLIENT_IDENTIFIER);
			CHECK(result != nullptr);
			
			UTF8String* value = static_cast<UTF8String*>(result);
			CHECK(value->getString() == "client123");
			
			propertyDestructors::destruct(result, PropertyType::ASSIGNED_CLIENT_IDENTIFIER);
		}

		SUBCASE("Decode REASON_STRING")
		{
			kmMqtt::ByteBuffer buffer(50);
			UTF8String original("Connection refused");
			original.encode(buffer);
			
			void* result = propertyDecodings::decode(buffer, PropertyType::REASON_STRING);
			CHECK(result != nullptr);
			
			UTF8String* value = static_cast<UTF8String*>(result);
			CHECK(value->getString() == "Connection refused");
			
			propertyDestructors::destruct(result, PropertyType::REASON_STRING);
		}
	}

	TEST_CASE("Property Decoders - BinaryData")
	{
		SUBCASE("Decode CORRELATION_DATA")
		{
			kmMqtt::ByteBuffer buffer(20);
			const std::uint8_t data[] = {0xDE, 0xAD, 0xBE, 0xEF};
			BinaryData original(4, data);
			original.encode(buffer);
			
			void* result = propertyDecodings::decode(buffer, PropertyType::CORRELATION_DATA);
			CHECK(result != nullptr);
			
			BinaryData* value = static_cast<BinaryData*>(result);
			CHECK(value->size() == 4);
			CHECK(value->bytes()[0] == 0xDE);
			CHECK(value->bytes()[1] == 0xAD);
			CHECK(value->bytes()[2] == 0xBE);
			CHECK(value->bytes()[3] == 0xEF);
			
			propertyDestructors::destruct(result, PropertyType::CORRELATION_DATA);
		}

		SUBCASE("Decode AUTHENTICATION_DATA")
		{
			kmMqtt::ByteBuffer buffer(20);
			const std::uint8_t data[] = {0x01, 0x02, 0x03};
			BinaryData original(3, data);
			original.encode(buffer);
			
			void* result = propertyDecodings::decode(buffer, PropertyType::AUTHENTICATION_DATA);
			CHECK(result != nullptr);
			
			BinaryData* value = static_cast<BinaryData*>(result);
			CHECK(value->size() == 3);
			
			propertyDestructors::destruct(result, PropertyType::AUTHENTICATION_DATA);
		}
	}

	TEST_CASE("Property Decoders - UTF8StringPair")
	{
		SUBCASE("Decode USER_PROPERTY")
		{
			kmMqtt::ByteBuffer buffer(50);
			UTF8StringPair original("key", "value");
			original.encode(buffer);
			
			void* result = propertyDecodings::decode(buffer, PropertyType::USER_PROPERTY);
			CHECK(result != nullptr);
			
			UTF8StringPair* value = static_cast<UTF8StringPair*>(result);
			CHECK(value->first().getString() == "key");
			CHECK(value->second().getString() == "value");
			
			propertyDestructors::destruct(result, PropertyType::USER_PROPERTY);
		}
	}

	TEST_CASE("Property Decoders - VariableByteInteger")
	{
		SUBCASE("Decode SUBSCRIPTION_IDENTIFIER - small value")
		{
			kmMqtt::ByteBuffer buffer(10);
			bool success;
			VariableByteInteger original = VariableByteInteger::tryCreateFromValue(127, &success);
			CHECK(success);
			original.encode(buffer);
			
			void* result = propertyDecodings::decode(buffer, PropertyType::SUBSCRIPTION_IDENTIFIER);
			CHECK(result != nullptr);
			
			VariableByteInteger* value = static_cast<VariableByteInteger*>(result);
			CHECK(value->uint32Value() == 127);
			
			propertyDestructors::destruct(result, PropertyType::SUBSCRIPTION_IDENTIFIER);
		}

		SUBCASE("Decode SUBSCRIPTION_IDENTIFIER - large value")
		{
			kmMqtt::ByteBuffer buffer(10);
			bool success;
			VariableByteInteger original = VariableByteInteger::tryCreateFromValue(16383, &success);
			CHECK(success);
			original.encode(buffer);
			
			void* result = propertyDecodings::decode(buffer, PropertyType::SUBSCRIPTION_IDENTIFIER);
			CHECK(result != nullptr);
			
			VariableByteInteger* value = static_cast<VariableByteInteger*>(result);
			CHECK(value->uint32Value() == 16383);
			
			propertyDestructors::destruct(result, PropertyType::SUBSCRIPTION_IDENTIFIER);
		}
	}

	TEST_CASE("Property Encode/Decode Round Trip")
	{
		SUBCASE("Round trip UInt8 properties")
		{
			kmMqtt::ByteBuffer buffer(100);
			std::uint8_t originalValue = 42;
			
			propertyEncodings::encode(buffer, PropertyType::MAXIMUM_QOS, &originalValue);
			buffer.incrementReadCursor(1); // Skip property ID
			
			void* decoded = propertyDecodings::decode(buffer, PropertyType::MAXIMUM_QOS);
			std::uint8_t* decodedValue = static_cast<std::uint8_t*>(decoded);
			
			CHECK(*decodedValue == originalValue);
			
			propertyDestructors::destruct(decoded, PropertyType::MAXIMUM_QOS);
		}

		SUBCASE("Round trip UInt16 properties")
		{
			kmMqtt::ByteBuffer buffer(100);
			std::uint16_t originalValue = 0xABCD;
			
			propertyEncodings::encode(buffer, PropertyType::TOPIC_ALIAS, &originalValue);
			buffer.incrementReadCursor(1);
			
			void* decoded = propertyDecodings::decode(buffer, PropertyType::TOPIC_ALIAS);
			std::uint16_t* decodedValue = static_cast<std::uint16_t*>(decoded);
			
			CHECK(*decodedValue == originalValue);
			
			propertyDestructors::destruct(decoded, PropertyType::TOPIC_ALIAS);
		}

		SUBCASE("Round trip UInt32 properties")
		{
			kmMqtt::ByteBuffer buffer(100);
			std::uint32_t originalValue = 0x12345678;
			
			propertyEncodings::encode(buffer, PropertyType::WILL_DELAY_INTERVAL, &originalValue);
			buffer.incrementReadCursor(1);
			
			void* decoded = propertyDecodings::decode(buffer, PropertyType::WILL_DELAY_INTERVAL);
			std::uint32_t* decodedValue = static_cast<std::uint32_t*>(decoded);
			
			CHECK(*decodedValue == originalValue);
			
			propertyDestructors::destruct(decoded, PropertyType::WILL_DELAY_INTERVAL);
		}

		SUBCASE("Round trip UTF8String properties")
		{
			kmMqtt::ByteBuffer buffer(100);
			UTF8String originalValue("test_string");
			
			propertyEncodings::encode(buffer, PropertyType::AUTHENTICATION_METHOD, &originalValue);
			buffer.incrementReadCursor(1);
			
			void* decoded = propertyDecodings::decode(buffer, PropertyType::AUTHENTICATION_METHOD);
			UTF8String* decodedValue = static_cast<UTF8String*>(decoded);
			
			CHECK(decodedValue->getString() == originalValue.getString());
			
			propertyDestructors::destruct(decoded, PropertyType::AUTHENTICATION_METHOD);
		}

		SUBCASE("Round trip BinaryData properties")
		{
			kmMqtt::ByteBuffer buffer(100);
			const std::uint8_t data[] = {0x11, 0x22, 0x33, 0x44};
			BinaryData originalValue(4, data);
			
			propertyEncodings::encode(buffer, PropertyType::CORRELATION_DATA, &originalValue);
			buffer.incrementReadCursor(1);
			
			void* decoded = propertyDecodings::decode(buffer, PropertyType::CORRELATION_DATA);
			BinaryData* decodedValue = static_cast<BinaryData*>(decoded);
			
			CHECK(decodedValue->size() == originalValue.size());
			CHECK(std::memcmp(decodedValue->bytes(), originalValue.bytes(), originalValue.size()) == 0);
			
			propertyDestructors::destruct(decoded, PropertyType::CORRELATION_DATA);
		}

		SUBCASE("Round trip UTF8StringPair properties")
		{
			kmMqtt::ByteBuffer buffer(100);
			UTF8StringPair originalValue("mykey", "myvalue");
			
			propertyEncodings::encode(buffer, PropertyType::USER_PROPERTY, &originalValue);
			buffer.incrementReadCursor(1);
			
			void* decoded = propertyDecodings::decode(buffer, PropertyType::USER_PROPERTY);
			UTF8StringPair* decodedValue = static_cast<UTF8StringPair*>(decoded);
			
			CHECK(decodedValue->first().getString() == originalValue.first().getString());
			CHECK(decodedValue->second().getString() == originalValue.second().getString());
			
			propertyDestructors::destruct(decoded, PropertyType::USER_PROPERTY);
		}
	}

	TEST_CASE("Property Destructors - Memory Safety")
	{
		SUBCASE("Destruct all UInt8 property types")
		{
			std::uint8_t* value = new std::uint8_t(5);
			CHECK_NOTHROW(propertyDestructors::destruct(value, PropertyType::PAYLOAD_FORMAT_INDICATOR));
		}

		SUBCASE("Destruct all UInt16 property types")
		{
			std::uint16_t* value = new std::uint16_t(1000);
			CHECK_NOTHROW(propertyDestructors::destruct(value, PropertyType::SERVER_KEEP_ALIVE));
		}

		SUBCASE("Destruct all UInt32 property types")
		{
			std::uint32_t* value = new std::uint32_t(123456);
			CHECK_NOTHROW(propertyDestructors::destruct(value, PropertyType::MESSAGE_EXPIRY_INTERVAL));
		}

		SUBCASE("Destruct UTF8String property types")
		{
			UTF8String* value = new UTF8String("test");
			CHECK_NOTHROW(propertyDestructors::destruct(value, PropertyType::CONTENT_TYPE));
		}

		SUBCASE("Destruct BinaryData property types")
		{
			const std::uint8_t data[] = {0x01};
			BinaryData* value = new BinaryData(1, data);
			CHECK_NOTHROW(propertyDestructors::destruct(value, PropertyType::CORRELATION_DATA));
		}

		SUBCASE("Destruct UTF8StringPair property types")
		{
			UTF8StringPair* value = new UTF8StringPair("k", "v");
			CHECK_NOTHROW(propertyDestructors::destruct(value, PropertyType::USER_PROPERTY));
		}

		SUBCASE("Destruct VariableByteInteger property types")
		{
			bool success;
			VariableByteInteger* value = new VariableByteInteger(VariableByteInteger::tryCreateFromValue(100, &success));
			CHECK(success);
			CHECK_NOTHROW(propertyDestructors::destruct(value, PropertyType::SUBSCRIPTION_IDENTIFIER));
		}
	}
}