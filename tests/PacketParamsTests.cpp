#include <doctest.h>
#include <kmMqtt/Mqtt/Params/PubAckOptions.h>
#include <kmMqtt/Mqtt/Params/ConnectArgs.h>
#include <kmMqtt/Mqtt/Params/DisconnectArgs.h>
#include <kmMqtt/Mqtt/Params/PublishOptions.h>
#include <kmMqtt/Mqtt/Params/SubscribeOptions.h>
#include <kmMqtt/Mqtt/Params/UnSubscribeOptions.h>

TEST_SUITE("Packet Params Tests")
{
	using namespace kmMqtt::mqtt;

	TEST_CASE("PubAckOptions - Default Constructor")
	{
		PubAckOptions options;
		
		CHECK(options.reasonString == "");
		CHECK(options.userProperties.empty());
	}

	TEST_CASE("PubAckOptions - Copy Constructor")
	{
		PubAckOptions original;
		original.reasonString = "Test reason";
		original.userProperties["key1"] = "value1";
		original.userProperties["key2"] = "value2";

		PubAckOptions copy(original);

		CHECK(copy.reasonString == "Test reason");
		CHECK(copy.userProperties.size() == 2);
		CHECK(copy.userProperties["key1"] == "value1");
		CHECK(copy.userProperties["key2"] == "value2");

		// Ensure deep copy
		original.reasonString = "Modified";
		original.userProperties["key3"] = "value3";
		CHECK(copy.reasonString == "Test reason");
		CHECK(copy.userProperties.size() == 2);
	}

	TEST_CASE("PubAckOptions - Copy Assignment")
	{
		PubAckOptions original;
		original.reasonString = "Assignment test";
		original.userProperties["prop1"] = "val1";

		PubAckOptions target;
		target = original;

		CHECK(target.reasonString == "Assignment test");
		CHECK(target.userProperties["prop1"] == "val1");

		SUBCASE("Self assignment")
		{
			target = target;
			CHECK(target.reasonString == "Assignment test");
		}
	}

	TEST_CASE("PubAckOptions - Move Constructor")
	{
		PubAckOptions original;
		original.reasonString = "Move test";
		original.userProperties["key"] = "value";

		PubAckOptions moved(std::move(original));

		CHECK(moved.reasonString == "Move test");
		CHECK(moved.userProperties["key"] == "value");
	}

	TEST_CASE("PubAckOptions - Move Assignment")
	{
		PubAckOptions original;
		original.reasonString = "Move assignment";
		original.userProperties["a"] = "b";

		PubAckOptions target;
		target = std::move(original);

		CHECK(target.reasonString == "Move assignment");
		CHECK(target.userProperties["a"] == "b");
		CHECK(original.userProperties.empty());

		SUBCASE("Self move assignment")
		{
			target = std::move(target);
			CHECK(target.reasonString == "Move assignment");
		}
	}

	TEST_CASE("Will - Constructor")
	{
		Will will("test/topic");

		CHECK(will.willTopic == "test/topic");
		CHECK(will.willQos == Qos::QOS_0);
		CHECK(will.retainWillMessage == false);
		CHECK(will.willDelayInterval == 0U);
		CHECK(will.messageExpiryInterval == 0U);
		CHECK(will.contentType == "");
		CHECK(will.responseTopic == "");
		CHECK(will.correlationData == nullptr);
		CHECK(will.payloadFormat == PayloadFormatIndicator::BINARY);
		CHECK(will.payload == nullptr);
		CHECK(will.userProperties.empty());
	}

	TEST_CASE("Will - Copy Constructor")
	{
		Will original("original/topic");
		original.willQos = Qos::QOS_1;
		original.retainWillMessage = true;
		original.willDelayInterval = 100;
		original.messageExpiryInterval = 3600;
		original.contentType = "application/json";
		original.responseTopic = "response/topic";
		
		const std::uint8_t corrData[] = {0x01, 0x02, 0x03};
		original.correlationData = std::make_unique<BinaryData>(3, corrData);
		
		const std::uint8_t payloadData[] = {0xAA, 0xBB};
		original.payload = std::make_unique<BinaryData>(2, payloadData);
		
		original.userProperties["key"] = "value";

		Will copy(original);

		CHECK(copy.willTopic == "original/topic");
		CHECK(copy.willQos == Qos::QOS_1);
		CHECK(copy.retainWillMessage == true);
		CHECK(copy.willDelayInterval == 100);
		CHECK(copy.messageExpiryInterval == 3600);
		CHECK(copy.contentType == "application/json");
		CHECK(copy.responseTopic == "response/topic");
		CHECK(copy.correlationData != nullptr);
		CHECK(copy.correlationData->size() == 3);
		CHECK(copy.payload != nullptr);
		CHECK(copy.payload->size() == 2);
		CHECK(copy.userProperties["key"] == "value");
		
		// Ensure deep copy
		CHECK(copy.correlationData.get() != original.correlationData.get());
		CHECK(copy.payload.get() != original.payload.get());
	}

	TEST_CASE("Will - Move Constructor")
	{
		Will original("move/topic");
		original.willQos = Qos::QOS_2;
		
		const std::uint8_t data[] = {0xFF};
		original.payload = std::make_unique<BinaryData>(1, data);

		Will moved(std::move(original));

		CHECK(moved.willTopic == "move/topic");
		CHECK(moved.willQos == Qos::QOS_2);
		CHECK(moved.payload != nullptr);
		CHECK(original.payload == nullptr);
		CHECK(original.correlationData == nullptr);
	}

	TEST_CASE("Will - Copy Assignment")
	{
		Will original("assign/topic");
		const std::uint8_t data[] = {0x11};
		original.correlationData = std::make_unique<BinaryData>(1, data);

		Will target("target/topic");
		target = original;

		CHECK(target.willTopic == "assign/topic");
		CHECK(target.correlationData != nullptr);
		CHECK(target.correlationData.get() != original.correlationData.get());
	}

	TEST_CASE("Will - Move Assignment")
	{
		Will original("moveassign/topic");
		const std::uint8_t data[] = {0x22};
		original.payload = std::make_unique<BinaryData>(1, data);

		Will target("target/topic");
		target = std::move(original);

		CHECK(target.willTopic == "moveassign/topic");
		CHECK(target.payload != nullptr);
		CHECK(original.payload == nullptr);
	}

	TEST_CASE("ConnectArgs - Constructor")
	{
		ConnectArgs args("testClient");

		CHECK(args.clientId == "testClient");
		CHECK(args.cleanStart == true);
		CHECK(args.will == nullptr);
		CHECK(args.username == "");
		CHECK(args.password == "");
		CHECK(args.extendedAuthenticationMethod == "");
		CHECK(args.extendedAuthenticationData == nullptr);
		CHECK(args.version == MqttVersion::MQTT_5_0);
		CHECK(args.protocolName == "MQTT");
		CHECK(args.keepAliveInSec == 60U);
		CHECK(args.sessionExpiryInterval == 0U);
		CHECK(args.receiveMaximum == 0U);
		CHECK(args.maximumPacketSize == 0U);
		CHECK(args.maximumTopicAliases == 0U);
		CHECK(args.requestResponseInformation == false);
		CHECK(args.requestProblemInformation == true);
		CHECK(args.userProperties.empty());
	}

	TEST_CASE("ConnectArgs - Copy Constructor")
	{
		ConnectArgs original("client123");
		original.cleanStart = false;
		original.username = "user";
		original.password = "pass";
		original.keepAliveInSec = 120;
		original.sessionExpiryInterval = 3600;
		original.userProperties["prop"] = "value";
		
		original.will = std::make_unique<Will>("will/topic");
		original.will->willQos = Qos::QOS_1;
		
		const std::uint8_t authData[] = {0x01, 0x02};
		original.extendedAuthenticationData = std::make_unique<BinaryData>(2, authData);

		ConnectArgs copy(original);

		CHECK(copy.clientId == "client123");
		CHECK(copy.cleanStart == false);
		CHECK(copy.username == "user");
		CHECK(copy.password == "pass");
		CHECK(copy.keepAliveInSec == 120);
		CHECK(copy.sessionExpiryInterval == 3600);
		CHECK(copy.userProperties["prop"] == "value");
		CHECK(copy.will != nullptr);
		CHECK(copy.will->willTopic == "will/topic");
		CHECK(copy.will->willQos == Qos::QOS_1);
		CHECK(copy.extendedAuthenticationData != nullptr);
		CHECK(copy.extendedAuthenticationData->size() == 2);
		
		// Deep copy verification
		CHECK(copy.will.get() != original.will.get());
		CHECK(copy.extendedAuthenticationData.get() != original.extendedAuthenticationData.get());
	}

	TEST_CASE("ConnectArgs - Move Constructor")
	{
		ConnectArgs original("moveClient");
		original.will = std::make_unique<Will>("will/move");
		original.extendedAuthenticationMethod = "SCRAM-SHA-256";

		ConnectArgs moved(std::move(original));

		CHECK(moved.clientId == "moveClient");
		CHECK(moved.will != nullptr);
		CHECK(moved.will->willTopic == "will/move");
		CHECK(moved.extendedAuthenticationMethod == "SCRAM-SHA-256");
		CHECK(original.will == nullptr);
		CHECK(original.extendedAuthenticationData == nullptr);
	}

	TEST_CASE("ConnectArgs - Copy Assignment")
	{
		ConnectArgs original("assignClient");
		original.receiveMaximum = 100;
		original.will = std::make_unique<Will>("assign/will");

		ConnectArgs target("target");
		target = original;

		CHECK(target.clientId == "assignClient");
		CHECK(target.receiveMaximum == 100);
		CHECK(target.will != nullptr);
		CHECK(target.will.get() != original.will.get());

		SUBCASE("Self assignment")
		{
			target = target;
			CHECK(target.clientId == "assignClient");
		}
	}

	TEST_CASE("ConnectArgs - Move Assignment")
	{
		ConnectArgs original("moveAssignClient");
		original.maximumTopicAliases = 50;
		original.will = std::make_unique<Will>("moveassign/will");

		ConnectArgs target("target");
		target = std::move(original);

		CHECK(target.clientId == "moveAssignClient");
		CHECK(target.maximumTopicAliases == 50);
		CHECK(target.will != nullptr);
		CHECK(original.will == nullptr);

		SUBCASE("Self move assignment")
		{
			target = std::move(target);
			CHECK(target.clientId == "moveAssignClient");
		}
	}

	TEST_CASE("DisconnectArgs - Constructor")
	{
		SUBCASE("Default parameters")
		{
			DisconnectArgs args;
			
			CHECK(args.willPublish == false);
			CHECK(args.clearQueue == true);
			CHECK(args.sessionExpiryInterval == 0U);
			CHECK(args.disconnectReasonText == "");
			CHECK(args.userProperties.empty());
		}

		SUBCASE("With will publish")
		{
			DisconnectArgs args(true, true);
			
			CHECK(args.willPublish == true);
			CHECK(args.clearQueue == true);
		}

		SUBCASE("With custom parameters")
		{
			DisconnectArgs args(true, true, false);
			
			CHECK(args.willPublish == true);
			CHECK(args.clearQueue == false);
		}
	}

	TEST_CASE("DisconnectArgs - Move Constructor")
	{
		DisconnectArgs original(true, true, false);
		original.sessionExpiryInterval = 7200;
		original.disconnectReasonText = "Server shutdown";
		original.userProperties["reason"] = "maintenance";

		DisconnectArgs moved(std::move(original));

		CHECK(moved.gracefulDisconnect == true);
		CHECK(moved.willPublish == true);
		CHECK(moved.clearQueue == false);
		CHECK(moved.sessionExpiryInterval == 7200);
		CHECK(moved.disconnectReasonText == "Server shutdown");
		CHECK(moved.userProperties["reason"] == "maintenance");
	}

	TEST_CASE("DisconnectArgs - Move Assignment")
	{
		DisconnectArgs original(false, false, true);
		original.disconnectReasonText = "Client disconnect";
		original.userProperties["status"] = "normal";

		DisconnectArgs target;
		target = std::move(original);

		CHECK(target.gracefulDisconnect == false);
		CHECK(target.willPublish == false);
		CHECK(target.clearQueue == true);
		CHECK(target.disconnectReasonText == "Client disconnect");
		CHECK(target.userProperties["status"] == "normal");

		SUBCASE("Self move assignment")
		{
			target = std::move(target);
			CHECK(target.disconnectReasonText == "Client disconnect");
		}
	}

	TEST_CASE("PublishOptions - Default Constructor")
	{
		PublishOptions options;

		CHECK(options.correlationData == nullptr);
		CHECK(options.responseTopic == "");
		CHECK(options.topicAlias == 0U);
		CHECK(options.messageExpiryInterval == 0U);
		CHECK(options.addMessageExpiryInterval == false);
		CHECK(options.payloadFormatIndicator == PayloadFormatIndicator::UTF8);
		CHECK(options.retain == false);
		CHECK(options.qos == Qos::QOS_0);
		CHECK(options.userProperties.empty());
	}

	TEST_CASE("PublishOptions - Copy Constructor")
	{
		PublishOptions original;
		original.responseTopic = "response/topic";
		original.topicAlias = 5;
		original.messageExpiryInterval = 1800;
		original.addMessageExpiryInterval = true;
		original.payloadFormatIndicator = PayloadFormatIndicator::BINARY;
		original.retain = true;
		original.qos = Qos::QOS_2;
		original.userProperties["type"] = "telemetry";
		
		const std::uint8_t corrData[] = {0xDE, 0xAD, 0xBE, 0xEF};
		original.correlationData = std::make_unique<BinaryData>(4, corrData);

		PublishOptions copy(original);

		CHECK(copy.responseTopic == "response/topic");
		CHECK(copy.topicAlias == 5);
		CHECK(copy.messageExpiryInterval == 1800);
		CHECK(copy.addMessageExpiryInterval == true);
		CHECK(copy.payloadFormatIndicator == PayloadFormatIndicator::BINARY);
		CHECK(copy.retain == true);
		CHECK(copy.qos == Qos::QOS_2);
		CHECK(copy.userProperties["type"] == "telemetry");
		CHECK(copy.correlationData != nullptr);
		CHECK(copy.correlationData->size() == 4);
		
		// Deep copy verification
		CHECK(copy.correlationData.get() != original.correlationData.get());
	}

	TEST_CASE("PublishOptions - Copy Assignment")
	{
		PublishOptions original;
		original.topicAlias = 10;
		original.qos = Qos::QOS_1;
		
		const std::uint8_t data[] = {0x11, 0x22};
		original.correlationData = std::make_unique<BinaryData>(2, data);

		PublishOptions target;
		target = original;

		CHECK(target.topicAlias == 10);
		CHECK(target.qos == Qos::QOS_1);
		CHECK(target.correlationData != nullptr);
		CHECK(target.correlationData.get() != original.correlationData.get());

		SUBCASE("Self assignment")
		{
			target = target;
			CHECK(target.topicAlias == 10);
		}
	}

	TEST_CASE("PublishOptions - Move Constructor")
	{
		PublishOptions original;
		original.responseTopic = "move/response";
		original.retain = true;
		
		const std::uint8_t data[] = {0xAA, 0xBB, 0xCC};
		original.correlationData = std::make_unique<BinaryData>(3, data);
		original.userProperties["key"] = "value";

		PublishOptions moved(std::move(original));

		CHECK(moved.responseTopic == "move/response");
		CHECK(moved.retain == true);
		CHECK(moved.correlationData != nullptr);
		CHECK(moved.correlationData->size() == 3);
		CHECK(moved.userProperties["key"] == "value");
		CHECK(original.correlationData == nullptr);
	}

	TEST_CASE("PublishOptions - Move Assignment")
	{
		PublishOptions original;
		original.messageExpiryInterval = 600;
		original.addMessageExpiryInterval = true;
		
		const std::uint8_t data[] = {0xFF};
		original.correlationData = std::make_unique<BinaryData>(1, data);
		original.userProperties["priority"] = "high";

		PublishOptions target;
		target = std::move(original);

		CHECK(target.messageExpiryInterval == 600);
		CHECK(target.addMessageExpiryInterval == true);
		CHECK(target.correlationData != nullptr);
		CHECK(target.userProperties["priority"] == "high");
		CHECK(original.correlationData == nullptr);
		CHECK(original.userProperties.empty());

		SUBCASE("Self move assignment")
		{
			target = std::move(target);
			CHECK(target.messageExpiryInterval == 600);
		}
	}

	TEST_CASE("SubscribeOptions - Default Constructor")
	{
		SubscribeOptions options;

		CHECK(options.userProperties.empty());
	}

	TEST_CASE("SubscribeOptions - Copy Constructor")
	{
		SubscribeOptions original;
		bool success;
		original.subscribeIdentifier = VariableByteInteger::tryCreateFromValue(100, &success);
		CHECK(success);
		original.userProperties["filter"] = "topic/+";

		SubscribeOptions copy(original);

		CHECK(copy.subscribeIdentifier.uint32Value() == 100);
		CHECK(copy.userProperties["filter"] == "topic/+");

		// Ensure deep copy
		original.userProperties["new"] = "value";
		CHECK(copy.userProperties.size() == 1);
	}

	TEST_CASE("SubscribeOptions - Copy Assignment")
	{
		SubscribeOptions original;
		bool success;
		original.subscribeIdentifier = VariableByteInteger::tryCreateFromValue(200, &success);
		CHECK(success);
		original.userProperties["qos"] = "1";

		SubscribeOptions target;
		target = original;

		CHECK(target.subscribeIdentifier.uint32Value() == 200);
		CHECK(target.userProperties["qos"] == "1");

		SUBCASE("Self assignment")
		{
			target = target;
			CHECK(target.subscribeIdentifier.uint32Value() == 200);
		}
	}

	TEST_CASE("SubscribeOptions - Move Constructor")
	{
		SubscribeOptions original;
		bool success;
		original.subscribeIdentifier = VariableByteInteger::tryCreateFromValue(300, &success);
		CHECK(success);
		original.userProperties["pattern"] = "sensor/#";

		SubscribeOptions moved(std::move(original));

		CHECK(moved.subscribeIdentifier.uint32Value() == 300);
		CHECK(moved.userProperties["pattern"] == "sensor/#");
	}

	TEST_CASE("SubscribeOptions - Move Assignment")
	{
		SubscribeOptions original;
		bool success;
		original.subscribeIdentifier = VariableByteInteger::tryCreateFromValue(400, &success);
		CHECK(success);
		original.userProperties["retain"] = "true";

		SubscribeOptions target;
		target = std::move(original);

		CHECK(target.subscribeIdentifier.uint32Value() == 400);
		CHECK(target.userProperties["retain"] == "true");
		CHECK(original.userProperties.empty());

		SUBCASE("Self move assignment")
		{
			target = std::move(target);
			CHECK(target.subscribeIdentifier.uint32Value() == 400);
		}
	}

	TEST_CASE("UnSubscribeOptions - Default Constructor")
	{
		UnSubscribeOptions options;

		CHECK(options.userProperties.empty());
	}

	TEST_CASE("UnSubscribeOptions - Copy Constructor")
	{
		UnSubscribeOptions original;
		original.userProperties["reason"] = "cleanup";
		original.userProperties["timestamp"] = "12345";

		UnSubscribeOptions copy(original);

		CHECK(copy.userProperties.size() == 2);
		CHECK(copy.userProperties["reason"] == "cleanup");
		CHECK(copy.userProperties["timestamp"] == "12345");

		// Ensure deep copy
		original.userProperties["new"] = "value";
		CHECK(copy.userProperties.size() == 2);
	}

	TEST_CASE("UnSubscribeOptions - Copy Assignment")
	{
		UnSubscribeOptions original;
		original.userProperties["action"] = "unsubscribe";

		UnSubscribeOptions target;
		target = original;

		CHECK(target.userProperties["action"] == "unsubscribe");

		SUBCASE("Self assignment")
		{
			target = target;
			CHECK(target.userProperties["action"] == "unsubscribe");
		}
	}

	TEST_CASE("UnSubscribeOptions - Move Constructor")
	{
		UnSubscribeOptions original;
		original.userProperties["topic"] = "test/topic";
		original.userProperties["id"] = "123";

		UnSubscribeOptions moved(std::move(original));

		CHECK(moved.userProperties["topic"] == "test/topic");
		CHECK(moved.userProperties["id"] == "123");
	}

	TEST_CASE("UnSubscribeOptions - Move Assignment")
	{
		UnSubscribeOptions original;
		original.userProperties["status"] = "active";

		UnSubscribeOptions target;
		target = std::move(original);

		CHECK(target.userProperties["status"] == "active");
		CHECK(original.userProperties.empty());

		SUBCASE("Self move assignment")
		{
			target = std::move(target);
			CHECK(target.userProperties["status"] == "active");
		}
	}

	TEST_CASE("Integration - Complex ConnectArgs with Will")
	{
		ConnectArgs args("integrationClient");
		args.cleanStart = false;
		args.username = "testUser";
		args.password = "securePassword";
		args.keepAliveInSec = 300;
		args.sessionExpiryInterval = 7200;
		args.receiveMaximum = 100;
		args.maximumPacketSize = 1048576;
		args.maximumTopicAliases = 10;
		args.requestResponseInformation = true;
		args.requestProblemInformation = false;
		args.userProperties["client_type"] = "iot_device";
		args.userProperties["version"] = "1.0";

		args.will = std::make_unique<Will>("device/status/offline");
		args.will->willQos = Qos::QOS_1;
		args.will->retainWillMessage = true;
		args.will->willDelayInterval = 60;
		args.will->messageExpiryInterval = 3600;
		args.will->contentType = "application/json";
		args.will->responseTopic = "device/will/response";
		args.will->payloadFormat = PayloadFormatIndicator::UTF8;
		
		const std::uint8_t willPayload[] = {'{', '"', 's', 't', 'a', 't', 'u', 's', '"', ':', '"', 'o', 'f', 'f', 'l', 'i', 'n', 'e', '"', '}'};
		args.will->payload = std::make_unique<BinaryData>(20, willPayload);
		
		const std::uint8_t corrData[] = {0x12, 0x34, 0x56, 0x78};
		args.will->correlationData = std::make_unique<BinaryData>(4, corrData);
		
		args.will->userProperties["offline_reason"] = "unexpected";

		// Test copy
		ConnectArgs copy(args);
		CHECK(copy.clientId == "integrationClient");
		CHECK(copy.keepAliveInSec == 300);
		CHECK(copy.userProperties.size() == 2);
		CHECK(copy.will != nullptr);
		CHECK(copy.will->willTopic == "device/status/offline");
		CHECK(copy.will->willQos == Qos::QOS_1);
		CHECK(copy.will->payload != nullptr);
		CHECK(copy.will->payload->size() == 20);
		CHECK(copy.will->correlationData != nullptr);
		CHECK(copy.will->correlationData->size() == 4);
		CHECK(copy.will->userProperties["offline_reason"] == "unexpected");

		// Verify deep copy
		CHECK(copy.will.get() != args.will.get());
		CHECK(copy.will->payload.get() != args.will->payload.get());
		CHECK(copy.will->correlationData.get() != args.will->correlationData.get());
	}

	TEST_CASE("Integration - Complex PublishOptions")
	{
		PublishOptions options;
		options.responseTopic = "device/command/response";
		options.topicAlias = 42;
		options.messageExpiryInterval = 300;
		options.addMessageExpiryInterval = true;
		options.payloadFormatIndicator = PayloadFormatIndicator::UTF8;
		options.retain = true;
		options.qos = Qos::QOS_2;
		
		const std::uint8_t corrData[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
		options.correlationData = std::make_unique<BinaryData>(6, corrData);
		
		options.userProperties["content_type"] = "application/json";
		options.userProperties["encoding"] = "utf-8";
		options.userProperties["priority"] = "high";

		// Test move
		PublishOptions moved(std::move(options));
		CHECK(moved.responseTopic == "device/command/response");
		CHECK(moved.topicAlias == 42);
		CHECK(moved.messageExpiryInterval == 300);
		CHECK(moved.qos == Qos::QOS_2);
		CHECK(moved.correlationData != nullptr);
		CHECK(moved.correlationData->size() == 6);
		CHECK(moved.userProperties.size() == 3);
		CHECK(options.correlationData == nullptr);
	}
}