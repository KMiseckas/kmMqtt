#include <doctest.h>
#include <cleanMqtt/Mqtt/PacketHelper.h>
#include <cleanMqtt/Mqtt/MqttConnectionInfo.h>
#include <cleanMqtt/Mqtt/Params/ConnectArgs.h>
#include <cleanMqtt/Mqtt/Params/DisconnectArgs.h>
#include <cleanMqtt/Mqtt/Params/PublishOptions.h>
#include <cleanMqtt/Mqtt/Params/PubAckOptions.h>
#include <cleanMqtt/Mqtt/Params/SubscribeOptions.h>
#include <cleanMqtt/Mqtt/Params/UnSubscribeOptions.h>
#include <cleanMqtt/Mqtt/Params/Topic.h>
#include <cleanMqtt/ByteBuffer.h>

TEST_SUITE("PacketHelper Tests")
{
	using namespace cleanMqtt::mqtt;

	TEST_CASE("createConnectPacket basic connection")
	{
		MqttConnectionInfo connectionInfo;
		connectionInfo.connectArgs = ConnectArgs("TestClient");
		connectionInfo.connectArgs.cleanStart = true;
		connectionInfo.connectArgs.keepAliveInSec = 60;

		Connect packet = createConnectPacket(connectionInfo);

		CHECK(packet.getVariableHeader().protocolName.getString() == "MQTT");
		CHECK(packet.getVariableHeader().protocolLevel == MqttVersion::MQTT_5_0);
		CHECK(packet.getVariableHeader().keepAliveInSec == 60);
		CHECK(packet.getPayloadHeader().clientId.getString() == "TestClient");
	}

	TEST_CASE("createConnectPacket with username and password")
	{
		MqttConnectionInfo connectionInfo;
		connectionInfo.connectArgs = ConnectArgs("TestClient");
		connectionInfo.connectArgs.username = "user";
		connectionInfo.connectArgs.password = "pass";

		Connect packet = createConnectPacket(connectionInfo);

		CHECK(packet.getPayloadHeader().userName.getString() == "user");
		CHECK(packet.getPayloadHeader().password.size() == 4);
	}

	TEST_CASE("createConnectPacket with will message")
	{
		MqttConnectionInfo connectionInfo;
		connectionInfo.connectArgs = ConnectArgs("TestClient");
		
		auto will = std::make_unique<Will>("will/topic");
		will->willQos = Qos::QOS_1;
		will->retainWillMessage = true;
		will->willDelayInterval = 10;
		std::uint8_t payloadBytes[] = {0x01, 0x02, 0x03};
		will->payload = std::make_unique<BinaryData>(3, payloadBytes);
		
		connectionInfo.connectArgs.will = std::move(will);

		Connect packet = createConnectPacket(connectionInfo);

		CHECK(packet.getPayloadHeader().willTopic.getString() == "will/topic");
		CHECK(packet.getPayloadHeader().willPayload.size() == 3);
	}

	TEST_CASE("createConnectPacket with user properties")
	{
		MqttConnectionInfo connectionInfo;
		connectionInfo.connectArgs = ConnectArgs("TestClient");
		connectionInfo.connectArgs.userProperties["key1"] = "value1";
		connectionInfo.connectArgs.userProperties["key2"] = "value2";

		Connect packet = createConnectPacket(connectionInfo);

		CHECK(packet.getVariableHeader().properties.count() >= 2);
	}

	TEST_CASE("createConnectPacket with session expiry interval")
	{
		MqttConnectionInfo connectionInfo;
		connectionInfo.connectArgs = ConnectArgs("TestClient");
		connectionInfo.connectArgs.sessionExpiryInterval = 3600;

		Connect packet = createConnectPacket(connectionInfo);

		CHECK(packet.getVariableHeader().properties.count() != 0);
	}

	TEST_CASE("createDisconnectPacket normal disconnect")
	{
		MqttConnectionInfo connectionInfo;
		connectionInfo.connectArgs = ConnectArgs("TestClient");
		DisconnectArgs args;
		args.disconnectReasonText = "Normal disconnect";

		Disconnect packet = createDisconnectPacket(connectionInfo, args, DisconnectReasonCode::NORMAL_DISCONNECTION);

		CHECK(packet.getVariableHeader().reasonCode == DisconnectReasonCode::NORMAL_DISCONNECTION);
	}

	TEST_CASE("createDisconnectPacket with user properties")
	{
		MqttConnectionInfo connectionInfo;
		connectionInfo.connectArgs = ConnectArgs("TestClient");
		DisconnectArgs args;
		args.userProperties["reason"] = "test";

		Disconnect packet = createDisconnectPacket(connectionInfo, args, DisconnectReasonCode::NORMAL_DISCONNECTION);

		CHECK(packet.getVariableHeader().properties.count() != 0);
	}

	TEST_CASE("createDisconnectPacket with session expiry")
	{
		MqttConnectionInfo connectionInfo;
		connectionInfo.connectArgs = ConnectArgs("TestClient");
		connectionInfo.connectArgs.sessionExpiryInterval = 3600;
		DisconnectArgs args;
		args.sessionExpiryInterval = 7200;

		Disconnect packet = createDisconnectPacket(connectionInfo, args, DisconnectReasonCode::NORMAL_DISCONNECTION);

		CHECK(packet.getVariableHeader().reasonCode == DisconnectReasonCode::NORMAL_DISCONNECTION);
	}

	TEST_CASE("createPingRequestPacket")
	{
		PingReq packet = createPingRequestPacket();
		//Should not crash
		CHECK(true);
	}

	TEST_CASE("createPingResponsePacket")
	{
		PingResp packet = createPingResponsePacket();
		//Should not crash
		CHECK(true);
	}

	TEST_CASE("createPublishPacket basic publish")
	{
		MqttConnectionInfo connectionInfo;
		cleanMqtt::ByteBuffer payload(5);
		PublishOptions options;
		options.qos = Qos::QOS_0;
		options.retain = false;

		Publish packet = createPublishPacket(connectionInfo, "test/topic", payload, options, 0);

		CHECK(packet.getVariableHeader().topicName.getString() == "test/topic");
		CHECK(packet.getVariableHeader().qos == Qos::QOS_0);
		CHECK(packet.getFixedHeader().flags.getFlagValue<PublishFlags, bool>(PublishFlags::IS_RETAINED) == false);
	}

	TEST_CASE("createPublishPacket with QoS 1 and packet ID")
	{
		MqttConnectionInfo connectionInfo;
		cleanMqtt::ByteBuffer payload(10);
		PublishOptions options;
		options.qos = Qos::QOS_1;
		options.retain = true;

		Publish packet = createPublishPacket(connectionInfo, "sensor/data", payload, options, 42);

		CHECK(packet.getVariableHeader().topicName.getString() == "sensor/data");
		CHECK(packet.getVariableHeader().qos == Qos::QOS_1);
		CHECK(packet.getVariableHeader().packetIdentifier == 42);
		CHECK(packet.getFixedHeader().flags.getFlagValue<PublishFlags, bool>(PublishFlags::IS_RETAINED) == true);
	}

	TEST_CASE("createPublishPacket with topic alias")
	{
		MqttConnectionInfo connectionInfo;
		cleanMqtt::ByteBuffer payload(5);
		PublishOptions options;
		options.topicAlias = 5;

		Publish packet = createPublishPacket(connectionInfo, "test/topic", payload, options, 0);

		CHECK(packet.getVariableHeader().properties.count() != 0);
	}

	TEST_CASE("createPublishPacket with response topic and correlation data")
	{
		MqttConnectionInfo connectionInfo;
		cleanMqtt::ByteBuffer payload(5);
		PublishOptions options;
		options.responseTopic = "response/topic";
		std::uint8_t corrData[] = {0xAA, 0xBB};
		options.correlationData = std::make_unique<BinaryData>(2, corrData);

		Publish packet = createPublishPacket(connectionInfo, "request/topic", payload, options, 0);

		CHECK(packet.getVariableHeader().properties.count() != 0);
	}

	TEST_CASE("createPublishPacket with user properties")
	{
		MqttConnectionInfo connectionInfo;
		cleanMqtt::ByteBuffer payload(5);
		PublishOptions options;
		options.userProperties["custom"] = "value";

		Publish packet = createPublishPacket(connectionInfo, "test/topic", payload, options, 0);

		CHECK(packet.getVariableHeader().properties.count() != 0);
	}

	TEST_CASE("createPublishPacket with message expiry interval")
	{
		MqttConnectionInfo connectionInfo;
		cleanMqtt::ByteBuffer payload(5);
		PublishOptions options;
		options.messageExpiryInterval = 300;
		options.addMessageExpiryInterval = true;

		Publish packet = createPublishPacket(connectionInfo, "test/topic", payload, options, 0);

		CHECK(packet.getVariableHeader().properties.count() != 0);
	}

	TEST_CASE("createPubAckPacket success")
	{
		PubAckOptions options;
		
		PublishAck packet = createPubAckPacket(123, PubAckReasonCode::SUCCESS, options);

		CHECK(packet.getVariableHeader().packetId == 123);
		CHECK(packet.getVariableHeader().reasonCode == PubAckReasonCode::SUCCESS);
	}

	TEST_CASE("createPubAckPacket with reason string")
	{
		PubAckOptions options;
		options.reasonString = "Acknowledged";

		PublishAck packet = createPubAckPacket(456, PubAckReasonCode::SUCCESS, options);

		CHECK(packet.getVariableHeader().packetId == 456);
		CHECK(packet.getVariableHeader().properties.count() != 0);
	}

	TEST_CASE("createPubAckPacket with user properties")
	{
		PubAckOptions options;
		options.userProperties["status"] = "ok";

		PublishAck packet = createPubAckPacket(789, PubAckReasonCode::SUCCESS, options);

		CHECK(packet.getVariableHeader().packetId == 789);
		CHECK(packet.getVariableHeader().properties.count() != 0);
	}

	TEST_CASE("createSubscribePacket single topic")
	{
		std::vector<Topic> topics;
		topics.emplace_back("test/topic", TopicSubscriptionOptions(Qos::QOS_1));
		SubscribeOptions options;

		Subscribe packet = createSubscribePacket(100, topics, options);

		CHECK(packet.getVariableHeader().packetId == 100);
		CHECK(packet.getPayloadHeader().subscriptions.size() == 1);
		CHECK(packet.getPayloadHeader().subscriptions[0].topicFilter.getString() == "test/topic");
	}

	TEST_CASE("createSubscribePacket multiple topics")
	{
		std::vector<Topic> topics;
		topics.emplace_back("topic/1", TopicSubscriptionOptions(Qos::QOS_0));
		topics.emplace_back("topic/2", TopicSubscriptionOptions(Qos::QOS_1));
		topics.emplace_back("topic/3", TopicSubscriptionOptions(Qos::QOS_2));
		SubscribeOptions options;

		Subscribe packet = createSubscribePacket(200, topics, options);

		CHECK(packet.getVariableHeader().packetId == 200);
		CHECK(packet.getPayloadHeader().subscriptions.size() == 3);
	}

	TEST_CASE("createSubscribePacket with subscription options")
	{
		std::vector<Topic> topics;
		TopicSubscriptionOptions topicOpts(Qos::QOS_1, true, true, RetainHandling::DoNotSend);
		topics.emplace_back("test/topic", topicOpts);
		SubscribeOptions options;

		Subscribe packet = createSubscribePacket(300, topics, options);

		CHECK(packet.getPayloadHeader().subscriptions.size() == 1);
	}

	TEST_CASE("createSubscribePacket with subscription identifier")
	{
		std::vector<Topic> topics;
		topics.emplace_back("test/topic", TopicSubscriptionOptions(Qos::QOS_0));
		SubscribeOptions options;
		options.subscribeIdentifier = VariableByteInteger::tryCreateFromValue(42);

		Subscribe packet = createSubscribePacket(400, topics, options);

		CHECK(packet.getVariableHeader().properties.count() != 0);
	}

	TEST_CASE("createSubscribePacket with user properties")
	{
		std::vector<Topic> topics;
		topics.emplace_back("test/topic", TopicSubscriptionOptions(Qos::QOS_0));
		SubscribeOptions options;
		options.userProperties["client"] = "test";

		Subscribe packet = createSubscribePacket(500, topics, options);

		CHECK(packet.getVariableHeader().properties.count() != 0);
	}

	TEST_CASE("createUnSubscribePacket single topic")
	{
		std::vector<Topic> topics;
		topics.emplace_back("test/topic");
		UnSubscribeOptions options;

		UnSubscribe packet = createUnSubscribePacket(600, topics, options);

		CHECK(packet.getVariableHeader().packetId == 600);
		CHECK(packet.getPayloadHeader().topics.size() == 1);
		CHECK(packet.getPayloadHeader().topics[0].getString() == "test/topic");
	}

	TEST_CASE("createUnSubscribePacket multiple topics")
	{
		std::vector<Topic> topics;
		topics.emplace_back("topic/1");
		topics.emplace_back("topic/2");
		topics.emplace_back("topic/3");
		UnSubscribeOptions options;

		UnSubscribe packet = createUnSubscribePacket(700, topics, options);

		CHECK(packet.getVariableHeader().packetId == 700);
		CHECK(packet.getPayloadHeader().topics.size() == 3);
	}

	TEST_CASE("createUnSubscribePacket with user properties")
	{
		std::vector<Topic> topics;
		topics.emplace_back("test/topic");
		UnSubscribeOptions options;
		options.userProperties["reason"] = "cleanup";

		UnSubscribe packet = createUnSubscribePacket(800, topics, options);

		CHECK(packet.getVariableHeader().properties.count() != 0);
	}

	TEST_CASE("createUnSubscribePacket empty topics list")
	{
		std::vector<Topic> topics;
		UnSubscribeOptions options;

		UnSubscribe packet = createUnSubscribePacket(900, topics, options);

		CHECK(packet.getVariableHeader().packetId == 900);
		CHECK(packet.getPayloadHeader().topics.empty());
	}
}