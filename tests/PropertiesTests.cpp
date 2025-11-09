#include <doctest.h>
#include <cleanMqtt/ByteBuffer.h>
#include <cleanMqtt/Mqtt/Packets/DataTypes.h>
#include <cleanMqtt/Mqtt/Packets/PropertyType.h>
#include <cleanMqtt/Mqtt/Packets/Properties.h>
#include <cleanMqtt/GlobalMacros.h>
#include <cstring>

TEST_SUITE("Properties Tests")
{
	using namespace cleanMqtt::mqtt;

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

		CHECK(properties.tryAddProperty<PropertyType::MAXIMUM_PACKET_SIZE>(30.5) == false);
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
		CHECK(userProperties[0]->first().getString() == "key1");
		CHECK(userProperties[0]->second().getString() == "value1");
		CHECK(userProperties[1]->first().getString() == "key2");
		CHECK(userProperties[1]->second().getString() == "value2");
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