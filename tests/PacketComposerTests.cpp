#include <doctest.h>
#include <cleanMqtt/Mqtt/Transport/Jobs/ConnectComposer.h>
#include <cleanMqtt/Mqtt/Transport/Jobs/PingComposer.h>
#include <cleanMqtt/Mqtt/Transport/Jobs/PubAckComposer.h>
#include <cleanMqtt/Mqtt/Transport/Jobs/PublishComposer.h>
#include <cleanMqtt/Mqtt/Transport/Jobs/SubscribeComposer.h>
#include <cleanMqtt/Mqtt/Transport/Jobs/UnSubscribeComposer.h>
#include <cleanMqtt/Mqtt/MqttConnectionInfo.h>
#include <cleanMqtt/Utils/PacketIdPool.h>

TEST_SUITE("PacketComposer Tests")
{
	using namespace cleanMqtt::mqtt;
	using cleanMqtt::PacketIdPool;

	TEST_CASE("ConnectComposer compose creates valid packet")
	{
		MqttConnectionInfo connectionInfo;
		connectionInfo.connectArgs = ConnectArgs("TestClient");
		connectionInfo.connectArgs.keepAliveInSec = 60;

		ConnectComposer composer(&connectionInfo);
		ComposeResult result = composer.compose();

		CHECK(result.encodeResult.isSuccess());
		CHECK(result.encodeResult.packetType == PacketType::CONNECT);
		CHECK(result.encodedData.size() > 0);
	}

	TEST_CASE("ConnectComposer cancel does nothing")
	{
		MqttConnectionInfo connectionInfo;
		connectionInfo.connectArgs = ConnectArgs("TestClient");

		ConnectComposer composer(&connectionInfo);
		//Should not crash
		composer.cancel();
		CHECK(true);
	}

	TEST_CASE("ConnectComposer with username and password")
	{
		MqttConnectionInfo connectionInfo;
		connectionInfo.connectArgs = ConnectArgs("TestClient");
		connectionInfo.connectArgs.username = "user";
		connectionInfo.connectArgs.password = "pass";

		ConnectComposer composer(&connectionInfo);
		ComposeResult result = composer.compose();

		CHECK(result.encodeResult.isSuccess());
		CHECK(result.encodedData.size() > 0);
	}

	TEST_CASE("PingComposer compose creates valid packet")
	{
		MqttConnectionInfo connectionInfo;

		PingComposer composer(&connectionInfo);
		ComposeResult result = composer.compose();

		CHECK(result.encodeResult.isSuccess());
		CHECK(result.encodeResult.packetType == PacketType::PING_REQUQEST);
		CHECK(result.encodedData.size() > 0);
	}

	TEST_CASE("PingComposer cancel does nothing")
	{
		MqttConnectionInfo connectionInfo;

		PingComposer composer(&connectionInfo);
		//Should not crash
		composer.cancel();
		CHECK(true);
	}

	TEST_CASE("PubAckComposer compose creates valid packet")
	{
		MqttConnectionInfo connectionInfo;
		PubAckOptions options;

		PubAckComposer composer(&connectionInfo, 123, PubAckReasonCode::SUCCESS, std::move(options));
		ComposeResult result = composer.compose();

		CHECK(result.encodeResult.isSuccess());
		CHECK(result.encodeResult.packetType == PacketType::PUBLISH_ACKNOWLEDGE);
		CHECK(result.encodedData.size() > 0);
	}

	TEST_CASE("PubAckComposer with reason string")
	{
		MqttConnectionInfo connectionInfo;
		PubAckOptions options;
		options.reasonString = "Acknowledged";

		PubAckComposer composer(&connectionInfo, 456, PubAckReasonCode::SUCCESS, std::move(options));
		ComposeResult result = composer.compose();

		CHECK(result.encodeResult.isSuccess());
		CHECK(result.encodedData.size() > 0);
	}

	TEST_CASE("PubAckComposer cancel does nothing")
	{
		MqttConnectionInfo connectionInfo;
		PubAckOptions options;

		PubAckComposer composer(&connectionInfo, 789, PubAckReasonCode::SUCCESS, std::move(options));
		//Should not crash
		composer.cancel();
		CHECK(true);
	}

	TEST_CASE("PublishComposer compose creates valid packet")
	{
		MqttConnectionInfo connectionInfo;
		PacketIdPool packetIdPool;
		std::uint16_t packetId = packetIdPool.getId();
		cleanMqtt::ByteBuffer payload(10);
		PublishOptions options;
		options.qos = Qos::QOS_1;

		PublishComposer composer(&connectionInfo, &packetIdPool, packetId, "test/topic", std::move(payload), std::move(options));
		ComposeResult result = composer.compose();

		CHECK(result.encodeResult.isSuccess());
		CHECK(result.encodeResult.packetType == PacketType::PUBLISH);
		CHECK(result.encodedData.size() > 0);
	}

	TEST_CASE("PublishComposer with QoS 0")
	{
		MqttConnectionInfo connectionInfo;
		PacketIdPool packetIdPool;
		cleanMqtt::ByteBuffer payload(5);
		PublishOptions options;
		options.qos = Qos::QOS_0;

		PublishComposer composer(&connectionInfo, &packetIdPool, 0, "sensor/data", std::move(payload), std::move(options));
		ComposeResult result = composer.compose();

		CHECK(result.encodeResult.isSuccess());
		CHECK(result.encodedData.size() > 0);
	}

	TEST_CASE("PublishComposer cancel releases packet ID")
	{
		MqttConnectionInfo connectionInfo;
		PacketIdPool packetIdPool;
		std::uint16_t packetId = packetIdPool.getId();
		cleanMqtt::ByteBuffer payload(10);
		PublishOptions options;

		CHECK(packetId == 1);

		PublishComposer composer(&connectionInfo, &packetIdPool, packetId, "test/topic", std::move(payload), std::move(options));
		composer.cancel();

		std::uint16_t reusedId = packetIdPool.getId();
		CHECK(reusedId == packetId);
	}

	TEST_CASE("PublishComposer with retain flag")
	{
		MqttConnectionInfo connectionInfo;
		PacketIdPool packetIdPool;
		std::uint16_t packetId = packetIdPool.getId();
		cleanMqtt::ByteBuffer payload(5);
		PublishOptions options;
		options.qos = Qos::QOS_1;
		options.retain = true;

		PublishComposer composer(&connectionInfo, &packetIdPool, packetId, "status/topic", std::move(payload), std::move(options));
		ComposeResult result = composer.compose();

		CHECK(result.encodeResult.isSuccess());
		CHECK(result.encodedData.size() > 0);
	}

	TEST_CASE("SubscribeComposer compose creates valid packet")
	{
		MqttConnectionInfo connectionInfo;
		PacketIdPool packetIdPool;
		std::uint16_t packetId = packetIdPool.getId();
		std::vector<Topic> topics;
		topics.emplace_back("test/topic", TopicSubscriptionOptions(Qos::QOS_1));
		SubscribeOptions options;

		SubscribeComposer composer(&connectionInfo, &packetIdPool, packetId, std::move(topics), std::move(options));
		ComposeResult result = composer.compose();

		CHECK(result.encodeResult.isSuccess());
		CHECK(result.encodeResult.packetType == PacketType::SUBSCRIBE);
		CHECK(result.encodedData.size() > 0);
	}

	TEST_CASE("SubscribeComposer with multiple topics")
	{
		MqttConnectionInfo connectionInfo;
		PacketIdPool packetIdPool;
		std::uint16_t packetId = packetIdPool.getId();
		std::vector<Topic> topics;
		topics.emplace_back("topic/1", TopicSubscriptionOptions(Qos::QOS_0));
		topics.emplace_back("topic/2", TopicSubscriptionOptions(Qos::QOS_1));
		topics.emplace_back("topic/3", TopicSubscriptionOptions(Qos::QOS_2));
		SubscribeOptions options;

		SubscribeComposer composer(&connectionInfo, &packetIdPool, packetId, std::move(topics), std::move(options));
		ComposeResult result = composer.compose();

		CHECK(result.encodeResult.isSuccess());
		CHECK(result.encodedData.size() > 0);
	}

	TEST_CASE("SubscribeComposer cancel releases packet ID")
	{
		MqttConnectionInfo connectionInfo;
		PacketIdPool packetIdPool;
		std::uint16_t packetId = packetIdPool.getId();
		std::vector<Topic> topics;
		topics.emplace_back("test/topic", TopicSubscriptionOptions(Qos::QOS_1));
		SubscribeOptions options;

		CHECK(packetId == 1);

		SubscribeComposer composer(&connectionInfo, &packetIdPool, packetId, std::move(topics), std::move(options));
		composer.cancel();

		std::uint16_t reusedId = packetIdPool.getId();
		CHECK(reusedId == packetId);
	}

	TEST_CASE("SubscribeComposer with subscription identifier")
	{
		MqttConnectionInfo connectionInfo;
		PacketIdPool packetIdPool;
		std::uint16_t packetId = packetIdPool.getId();
		std::vector<Topic> topics;
		topics.emplace_back("test/topic", TopicSubscriptionOptions(Qos::QOS_0));
		SubscribeOptions options;
		options.subscribeIdentifier = VariableByteInteger::tryCreateFromValue(42);

		SubscribeComposer composer(&connectionInfo, &packetIdPool, packetId, std::move(topics), std::move(options));
		ComposeResult result = composer.compose();

		CHECK(result.encodeResult.isSuccess());
		CHECK(result.encodedData.size() > 0);
	}

	TEST_CASE("UnSubscribeComposer compose creates valid packet")
	{
		MqttConnectionInfo connectionInfo;
		PacketIdPool packetIdPool;
		std::uint16_t packetId = packetIdPool.getId();
		std::vector<Topic> topics;
		topics.emplace_back("test/topic");
		UnSubscribeOptions options;

		UnSubscribeComposer composer(&connectionInfo, &packetIdPool, packetId, std::move(topics), std::move(options));
		ComposeResult result = composer.compose();

		CHECK(result.encodeResult.isSuccess());
		CHECK(result.encodeResult.packetType == PacketType::UNSUBSCRIBE);
		CHECK(result.encodedData.size() > 0);
	}

	TEST_CASE("UnSubscribeComposer with multiple topics")
	{
		MqttConnectionInfo connectionInfo;
		PacketIdPool packetIdPool;
		std::uint16_t packetId = packetIdPool.getId();
		std::vector<Topic> topics;
		topics.emplace_back("topic/1");
		topics.emplace_back("topic/2");
		topics.emplace_back("topic/3");
		UnSubscribeOptions options;

		UnSubscribeComposer composer(&connectionInfo, &packetIdPool, packetId, std::move(topics), std::move(options));
		ComposeResult result = composer.compose();

		CHECK(result.encodeResult.isSuccess());
		CHECK(result.encodedData.size() > 0);
	}

	TEST_CASE("UnSubscribeComposer cancel releases packet ID")
	{
		MqttConnectionInfo connectionInfo;
		PacketIdPool packetIdPool;
		std::uint16_t packetId = packetIdPool.getId();
		std::vector<Topic> topics;
		topics.emplace_back("test/topic");
		UnSubscribeOptions options;

		CHECK(packetId == 1);

		UnSubscribeComposer composer(&connectionInfo, &packetIdPool, packetId, std::move(topics), std::move(options));
		composer.cancel();

		std::uint16_t reusedId = packetIdPool.getId();
		CHECK(reusedId == packetId);
	}

	TEST_CASE("UnSubscribeComposer with empty topics list")
	{
		MqttConnectionInfo connectionInfo;
		PacketIdPool packetIdPool;
		std::uint16_t packetId = packetIdPool.getId();
		std::vector<Topic> topics;
		UnSubscribeOptions options;

		UnSubscribeComposer composer(&connectionInfo, &packetIdPool, packetId, std::move(topics), std::move(options));
		ComposeResult result = composer.compose();

		CHECK(result.encodeResult.isSuccess());
		CHECK(result.encodedData.size() > 0);
	}

	TEST_CASE("Multiple composers can be created and used")
	{
		MqttConnectionInfo connectionInfo;
		connectionInfo.connectArgs = ConnectArgs("TestClient");
		PacketIdPool packetIdPool;

		ConnectComposer connectComposer(&connectionInfo);
		ComposeResult connectResult = connectComposer.compose();

		PingComposer pingComposer(&connectionInfo);
		ComposeResult pingResult = pingComposer.compose();

		std::uint16_t pubId = packetIdPool.getId();
		cleanMqtt::ByteBuffer payload(10);
		PublishOptions pubOptions;
		pubOptions.qos = Qos::QOS_1;
		PublishComposer publishComposer(&connectionInfo, &packetIdPool, pubId, "test/topic", std::move(payload), std::move(pubOptions));
		ComposeResult publishResult = publishComposer.compose();

		CHECK(connectResult.encodeResult.isSuccess());
		CHECK(pingResult.encodeResult.isSuccess());
		CHECK(publishResult.encodeResult.isSuccess());
	}
}