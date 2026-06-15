// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <doctest.h>
#include <kmMqtt/Mqtt/PacketHelper.h>
#include <kmMqtt/Mqtt/MqttConnectionInfo.h>
#include <kmMqtt/Mqtt/Params/ConnectArgs.h>
#include <kmMqtt/Mqtt/Params/DisconnectArgs.h>
#include <kmMqtt/Mqtt/Params/PublishOptions.h>
#include <kmMqtt/Mqtt/Params/PubAckOptions.h>
#include <kmMqtt/Mqtt/Params/PubCompOptions.h>
#include <kmMqtt/Mqtt/Params/PubRecOptions.h>
#include <kmMqtt/Mqtt/Params/PubRelOptions.h>
#include <kmMqtt/Mqtt/Params/SubscribeOptions.h>
#include <kmMqtt/Mqtt/Params/UnSubscribeOptions.h>
#include <kmMqtt/Mqtt/Params/Topic.h>
#include <kmMqtt/ByteBuffer.h>
#include <kmMqtt/Mqtt/Packets/Subscribe/SubscribeAck.h>

TEST_SUITE("PacketHelper Tests")
{
	using namespace kmMqtt::mqtt;

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
		
		auto will = kmMqtt::kmStd::make_unique<Will>("will/topic");
		will->willQos = Qos::QOS_1;
		will->retainWillMessage = true;
		will->willDelayInterval = 10;
		std::uint8_t payloadBytes[] = {0x01, 0x02, 0x03};
		will->payload = kmMqtt::kmStd::make_unique<BinaryData>(3, payloadBytes);
		
		connectionInfo.connectArgs.will = std::move(will);

		Connect packet = createConnectPacket(connectionInfo);

		CHECK(packet.getPayloadHeader().willTopic.getString() == "will/topic");
		CHECK(packet.getPayloadHeader().willPayload.size() == 3);
	}

	TEST_CASE("createConnectPacket with will message and null payload")
	{
		MqttConnectionInfo connectionInfo;
		connectionInfo.connectArgs = ConnectArgs("TestClient");

		auto will = kmMqtt::kmStd::make_unique<Will>("will/topic");
		will->willQos = Qos::QOS_1;
		will->payload = nullptr;

		connectionInfo.connectArgs.will = std::move(will);

		Connect packet = createConnectPacket(connectionInfo);

		CHECK(packet.getPayloadHeader().willTopic.getString() == "will/topic");
		CHECK(packet.getPayloadHeader().willPayload.size() == 0);
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
		kmMqtt::ByteBuffer payload(5);
		PublishOptions options;
		options.qos = Qos::QOS_0;
		options.retain = false;

		Publish packet = createPublishPacket(connectionInfo, false, "test/topic", payload, options, 0);

		CHECK(packet.getVariableHeader().topicName.getString() == "test/topic");
		CHECK(packet.getVariableHeader().qos == Qos::QOS_0);
		CHECK(packet.getFixedHeader().flags.getFlagValue<PublishFlags, bool>(PublishFlags::IS_RETAINED) == false);
	}

	TEST_CASE("created packets encode after move")
	{
		MqttConnectionInfo connectionInfo;
		connectionInfo.connectArgs = ConnectArgs("TestClient");

		{
			Connect moved{ createConnectPacket(connectionInfo) };
			auto result = moved.encode();
			CHECK(result.isSuccess());
			CHECK(result.packetType == PacketType::CONNECT);
			CHECK(moved.getDataBuffer().size() > 0);
		}

		{
			DisconnectArgs args;
			Disconnect moved{ createDisconnectPacket(connectionInfo, args, DisconnectReasonCode::NORMAL_DISCONNECTION) };
			auto result = moved.encode();
			CHECK(result.isSuccess());
			CHECK(result.packetType == PacketType::DISCONNECT);
			CHECK(moved.getDataBuffer().size() >= 2);
		}

		{
			kmMqtt::ByteBuffer payload(3);
			payload += 0x01;
			payload += 0x02;
			payload += 0x03;
			PublishOptions options;
			options.qos = Qos::QOS_1;

			Publish moved{ createPublishPacket(connectionInfo, false, "test/topic", payload, options, 7) };
			auto result = moved.encode();
			CHECK(result.isSuccess());
			CHECK(result.packetType == PacketType::PUBLISH);
			CHECK(moved.getDataBuffer().size() > payload.size());
		}

		{
			PubAckOptions options;
			PublishAck moved{ createPubAckPacket(1, PubAckReasonCode::SUCCESS, options) };
			auto result = moved.encode();
			CHECK(result.isSuccess());
			CHECK(result.packetType == PacketType::PUBLISH_ACKNOWLEDGE);
			CHECK(moved.getDataBuffer().size() >= 4);
		}

		{
			PubRecOptions options;
			PublishRec moved{ createPubRecPacket(2, PubRecReasonCode::SUCCESS, options) };
			auto result = moved.encode();
			CHECK(result.isSuccess());
			CHECK(result.packetType == PacketType::PUBLISH_RECEIVED);
			CHECK(moved.getDataBuffer().size() >= 4);
		}

		{
			PubRelOptions options;
			PublishRel moved{ createPubRelPacket(3, PubRelReasonCode::SUCCESS, options) };
			auto result = moved.encode();
			CHECK(result.isSuccess());
			CHECK(result.packetType == PacketType::PUBLISH_RELEASED);
			CHECK(moved.getDataBuffer().size() >= 4);
		}

		{
			PubCompOptions options;
			PublishComp moved{ createPubCompPacket(4, PubCompReasonCode::SUCCESS, options) };
			auto result = moved.encode();
			CHECK(result.isSuccess());
			CHECK(result.packetType == PacketType::PUBLISH_COMPLETE);
			CHECK(moved.getDataBuffer().size() >= 4);
		}

		{
			std::vector<Topic> topics;
			topics.emplace_back("test/topic", TopicSubscriptionOptions(Qos::QOS_1));
			SubscribeOptions options;

			Subscribe moved{ createSubscribePacket(5, topics, options) };
			auto result = moved.encode();
			CHECK(result.isSuccess());
			CHECK(result.packetType == PacketType::SUBSCRIBE);
			CHECK(moved.getDataBuffer().size() > 0);
		}

		{
			std::vector<Topic> topics;
			topics.emplace_back("test/topic");
			UnSubscribeOptions options;

			UnSubscribe moved{ createUnSubscribePacket(6, topics, options) };
			auto result = moved.encode();
			CHECK(result.isSuccess());
			CHECK(result.packetType == PacketType::UNSUBSCRIBE);
			CHECK(moved.getDataBuffer().size() > 0);
		}
	}

	TEST_CASE("received packets decode after move")
	{
		{
			kmMqtt::ByteBuffer buffer(5);
			buffer += 0x20;
			buffer += 0x03;
			buffer += 0x00;
			buffer += 0x00;
			buffer += 0x00;

			ConnectAck moved{ ConnectAck{ std::move(buffer) } };
			auto result = moved.decode();
			CHECK(result.isSuccess());
			CHECK(moved.getVariableHeader().reasonCode == ConnectReasonCode::SUCCESS);
		}

		{
			const char topic[] = "test";
			const std::uint8_t payload[] = { 0x10, 0x20, 0x30 };

			kmMqtt::ByteBuffer buffer(2 + 2 + 4 + 1 + sizeof(payload));
			buffer += 0x30;
			buffer += static_cast<std::uint8_t>(2 + 4 + 1 + sizeof(payload));
			buffer.append(static_cast<std::uint16_t>(4));
			buffer.append(reinterpret_cast<const std::uint8_t*>(topic), 4);
			buffer += 0x00;
			buffer.append(payload, sizeof(payload));

			Publish moved{ Publish{ std::move(buffer) } };
			auto result = moved.decode();
			CHECK(result.isSuccess());
			CHECK(moved.getVariableHeader().topicName.getString() == "test");
			CHECK(moved.getPayloadHeader().payload.size() == sizeof(payload));
			CHECK(moved.getPayloadHeader().payload[0] == payload[0]);
			CHECK(moved.getPayloadHeader().payload[2] == payload[2]);
		}

		{
			kmMqtt::ByteBuffer buffer(4);
			buffer += 0x40;
			buffer += 0x02;
			buffer += 0x00;
			buffer += 0x07;

			PublishAck moved{ PublishAck{ std::move(buffer) } };
			auto result = moved.decode();
			CHECK(result.isSuccess());
			CHECK(moved.getVariableHeader().packetId == 7);
		}

		{
			kmMqtt::ByteBuffer buffer(6);
			buffer += 0x90;
			buffer += 0x04;
			buffer += 0x00;
			buffer += 0x05;
			buffer += 0x00;
			buffer += 0x00;

			SubscribeAck moved{ SubscribeAck{ std::move(buffer) } };
			auto result = moved.decode();
			CHECK(result.isSuccess());
			CHECK(moved.getVariableHeader().packetId == 5);
			CHECK(moved.getPayloadHeader().reasonCodes.size() == 1);
		}

		{
			kmMqtt::ByteBuffer buffer(6);
			buffer += 0xB0;
			buffer += 0x04;
			buffer += 0x00;
			buffer += 0x06;
			buffer += 0x00;
			buffer += 0x00;

			UnSubscribeAck moved{ UnSubscribeAck{ std::move(buffer) } };
			auto result = moved.decode();
			CHECK(result.isSuccess());
			CHECK(moved.getVariableHeader().packetId == 6);
			CHECK(moved.getPayloadHeader().reasonCodes.size() == 1);
		}
	}

	TEST_CASE("createPublishPacket with QoS 1 and packet ID")
	{
		MqttConnectionInfo connectionInfo;
		kmMqtt::ByteBuffer payload(10);
		PublishOptions options;
		options.qos = Qos::QOS_1;
		options.retain = true;

		Publish packet = createPublishPacket(connectionInfo, false, "sensor/data", payload, options, 42);

		CHECK(packet.getVariableHeader().topicName.getString() == "sensor/data");
		CHECK(packet.getVariableHeader().qos == Qos::QOS_1);
		CHECK(packet.getVariableHeader().packetIdentifier == 42);
		CHECK(packet.getFixedHeader().flags.getFlagValue<PublishFlags, bool>(PublishFlags::IS_RETAINED) == true);
	}

	TEST_CASE("createPublishPacket with topic alias")
	{
		MqttConnectionInfo connectionInfo;
		kmMqtt::ByteBuffer payload(5);
		PublishOptions options;
		options.topicAlias = 5;

		Publish packet = createPublishPacket(connectionInfo, false, "test/topic", payload, options, 0);

		CHECK(packet.getVariableHeader().properties.count() != 0);
	}

	TEST_CASE("createPublishPacket with response topic and correlation data")
	{
		MqttConnectionInfo connectionInfo;
		kmMqtt::ByteBuffer payload(5);
		PublishOptions options;
		options.responseTopic = "response/topic";
		std::uint8_t corrData[] = {0xAA, 0xBB};
		options.correlationData = kmMqtt::kmStd::make_unique<BinaryData>(2, corrData);

		Publish packet = createPublishPacket(connectionInfo, false, "request/topic", payload, options, 0);

		CHECK(packet.getVariableHeader().properties.count() != 0);
	}

	TEST_CASE("createPublishPacket with user properties")
	{
		MqttConnectionInfo connectionInfo;
		kmMqtt::ByteBuffer payload(5);
		PublishOptions options;
		options.userProperties["custom"] = "value";

		Publish packet = createPublishPacket(connectionInfo, false, "test/topic", payload, options, 0);

		CHECK(packet.getVariableHeader().properties.count() != 0);
	}

	TEST_CASE("createPublishPacket with message expiry interval")
	{
		MqttConnectionInfo connectionInfo;
		kmMqtt::ByteBuffer payload(5);
		PublishOptions options;
		options.messageExpiryInterval = 300;
		options.addMessageExpiryInterval = true;

		Publish packet = createPublishPacket(connectionInfo, false, "test/topic", payload, options, 0);

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
		kmMqtt::kmStd::vector<Topic> topics;
		topics.emplace_back("test/topic", TopicSubscriptionOptions(Qos::QOS_1));
		SubscribeOptions options;

		Subscribe packet = createSubscribePacket(100, topics, options);

		CHECK(packet.getVariableHeader().packetId == 100);
		CHECK(packet.getPayloadHeader().subscriptions.size() == 1);
		CHECK(packet.getPayloadHeader().subscriptions[0].topicFilter.getString() == "test/topic");
	}

	TEST_CASE("createSubscribePacket multiple topics")
	{
		kmMqtt::kmStd::vector<Topic> topics;
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
		kmMqtt::kmStd::vector<Topic> topics;
		TopicSubscriptionOptions topicOpts(Qos::QOS_1, true, true, RetainHandling::DoNotSend);
		topics.emplace_back("test/topic", topicOpts);
		SubscribeOptions options;

		Subscribe packet = createSubscribePacket(300, topics, options);

		CHECK(packet.getPayloadHeader().subscriptions.size() == 1);
	}

	TEST_CASE("createSubscribePacket with subscription identifier")
	{
		kmMqtt::kmStd::vector<Topic> topics;
		topics.emplace_back("test/topic", TopicSubscriptionOptions(Qos::QOS_0));
		SubscribeOptions options;
		options.subscribeIdentifier = VariableByteInteger::tryCreateFromValue(42);

		Subscribe packet = createSubscribePacket(400, topics, options);

		CHECK(packet.getVariableHeader().properties.count() != 0);
	}

	TEST_CASE("createSubscribePacket with user properties")
	{
		kmMqtt::kmStd::vector<Topic> topics;
		topics.emplace_back("test/topic", TopicSubscriptionOptions(Qos::QOS_0));
		SubscribeOptions options;
		options.userProperties["client"] = "test";

		Subscribe packet = createSubscribePacket(500, topics, options);

		CHECK(packet.getVariableHeader().properties.count() != 0);
	}

	TEST_CASE("createUnSubscribePacket single topic")
	{
		kmMqtt::kmStd::vector<Topic> topics;
		topics.emplace_back("test/topic");
		UnSubscribeOptions options;

		UnSubscribe packet = createUnSubscribePacket(600, topics, options);

		CHECK(packet.getVariableHeader().packetId == 600);
		CHECK(packet.getPayloadHeader().topics.size() == 1);
		CHECK(packet.getPayloadHeader().topics[0].getString() == "test/topic");
	}

	TEST_CASE("createUnSubscribePacket multiple topics")
	{
		kmMqtt::kmStd::vector<Topic> topics;
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
		kmMqtt::kmStd::vector<Topic> topics;
		topics.emplace_back("test/topic");
		UnSubscribeOptions options;
		options.userProperties["reason"] = "cleanup";

		UnSubscribe packet = createUnSubscribePacket(800, topics, options);

		CHECK(packet.getVariableHeader().properties.count() != 0);
	}

	TEST_CASE("createUnSubscribePacket empty topics list")
	{
		kmMqtt::kmStd::vector<Topic> topics;
		UnSubscribeOptions options;

		UnSubscribe packet = createUnSubscribePacket(900, topics, options);

		CHECK(packet.getVariableHeader().packetId == 900);
		CHECK(packet.getPayloadHeader().topics.empty());
	}
}
