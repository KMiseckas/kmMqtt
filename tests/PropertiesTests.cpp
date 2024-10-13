#include <doctest.h>
#include <cleanMqtt/ByteBuffer.h>
#include <cleanMqtt/Mqtt/Packets/DataTypes.h>
#include <cleanMqtt/Mqtt/Packets/PropertyType.h>
#include <cleanMqtt/Mqtt/Packets/Properties.h>
#include <cleanMqtt/GlobalMacros.h>
#include <cstring>

TEST_SUITE("Properties Tests")
{
	using namespace cleanMqtt::mqtt::packets;

	TEST_CASE("Adding/Getting property")
	{
		Properties properties;
		CHECK(properties.tryAddProperty<std::uint8_t>(PropertyType::ASSIGNED_CLIENT_IDENTIFIER, 10));
		CHECK(properties.tryAddProperty<std::uint8_t>(PropertyType::AUTHENTICATION_DATA, 20));
		CHECK(properties.tryAddProperty<bool>(PropertyType::RETAIN_AVAILABLE, true));
		CHECK(properties.tryAddProperty<const char*>(PropertyType::CONTENT_TYPE, "JSON"));
		CHECK(properties.tryAddProperty<double>(PropertyType::MAXIMUM_PACKET_SIZE, 22.5));

		CHECK(properties.tryAddProperty<BinaryData>(PropertyType::USER_PROPERTY, BinaryData()));
		CHECK(properties.tryAddProperty<UTF8String>(PropertyType::WILL_DELAY_INTERVAL, UTF8String("Hello World")));
		CHECK(properties.tryAddProperty<VariableByteInteger>(PropertyType::WILDCARD_SUBSCRIPTION_AVAILABLE, VariableByteInteger{ 150 }));

		const std::uint8_t* valueInt;
		const bool* valueBool;
		const char* const* valueChar;
		const double* valueDouble;
		const UTF8String* utf8String;

		CHECK(properties.tryGetProperty<std::uint8_t>(PropertyType::ASSIGNED_CLIENT_IDENTIFIER, valueInt));
		CHECK(*valueInt == 10);

		CHECK(properties.tryGetProperty<std::uint8_t>(PropertyType::AUTHENTICATION_DATA, valueInt));
		CHECK(*valueInt == 20);

		CHECK(properties.tryGetProperty<bool>(PropertyType::RETAIN_AVAILABLE, valueBool));
		CHECK(*valueBool == true);

		CHECK(properties.tryGetProperty<const char*>(PropertyType::CONTENT_TYPE, valueChar));
		CHECK(std::strcmp(*valueChar, "JSON") == 0);

		CHECK(properties.tryGetProperty<double>(PropertyType::MAXIMUM_PACKET_SIZE, valueDouble));
		CHECK(*valueDouble == 22.5);

		std::vector<const BinaryData*> dataVec;
		CHECK(properties.tryGetProperty<BinaryData>(PropertyType::USER_PROPERTY, dataVec));
		CHECK(dataVec[0]->bytes() == nullptr);
		CHECK(dataVec[0]->size() == 0);

		CHECK(properties.tryGetProperty<UTF8String>(PropertyType::WILL_DELAY_INTERVAL, utf8String));
		CHECK(utf8String->stringBytes() != nullptr);

		auto d = utf8String->getString();

		CHECK(utf8String->getString().compare("Hello World") == 0);
		CHECK(utf8String->stringSize() == utf8String->getString().size());

		CHECK(properties.tryGetProperty<double>(PropertyType::CONTENT_TYPE, valueDouble));
		CHECK(*valueDouble != 22.5);
		CHECK(properties.tryGetProperty<bool>(PropertyType::PAYLOAD_FORMAT_INDICATOR, valueBool) == false);

		CHECK(properties.tryAddProperty<double>(PropertyType::MAXIMUM_PACKET_SIZE, 30.5) == false);
		CHECK(properties.tryAddProperty<const char*>(PropertyType::CONTENT_TYPE, "JSON_OVERRIDE") == false);

		CHECK(properties.tryGetProperty<const char*>(PropertyType::CONTENT_TYPE, valueChar));
		CHECK(std::strcmp(*valueChar, "JSON") == 0);
		CHECK(properties.tryGetProperty<double>(PropertyType::MAXIMUM_PACKET_SIZE, valueDouble));
		CHECK(*valueDouble == 22.5);
	}

	TEST_CASE("Add/Get Duplicate Properties")
	{
		Properties properties;
		CHECK(properties.tryAddProperty<std::uint8_t>(PropertyType::USER_PROPERTY, 10));
		CHECK(properties.tryAddProperty<std::uint8_t>(PropertyType::AUTHENTICATION_DATA, 20));

		const std::uint8_t* valueInt;
		std::vector<const std::uint8_t*> valueIntVec;

		CHECK(properties.tryGetProperty<std::uint8_t>(PropertyType::USER_PROPERTY, valueIntVec));
		CHECK(*(valueIntVec[0]) == 10);

		CHECK(properties.tryGetProperty<std::uint8_t>(PropertyType::AUTHENTICATION_DATA, valueInt));
		CHECK(*valueInt == 20);

		CHECK(properties.tryAddProperty<std::uint8_t>(PropertyType::USER_PROPERTY, 20) == true); //USER_PROPERTY is allowed for duplication.
		CHECK(properties.tryAddProperty<std::uint8_t>(PropertyType::AUTHENTICATION_DATA, 30) == false); //AUTHENTICATION_DATA is not allowed for duplication.

		valueIntVec.clear();
		CHECK(properties.tryGetProperty<std::uint8_t>(PropertyType::USER_PROPERTY, valueIntVec));
		CHECK(*(valueIntVec[0]) == 10);
		CHECK(*(valueIntVec[1]) == 20);

		CHECK(properties.tryGetProperty<std::uint8_t>(PropertyType::AUTHENTICATION_DATA, valueInt));
		CHECK(*valueInt == 20);
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
}