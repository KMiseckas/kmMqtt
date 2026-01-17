#include <doctest.h>
#include <kmMqtt/MqttClient.h>
#include <memory>
#include <string>
#include "MockWebSocket.h"
#include "Helpers.h"

using namespace kmMqtt;
using namespace kmMqtt::mqtt;

TEST_SUITE("MqttClient Publish")
{
    TEST_CASE("Successful Publish QOS 0")
    {
        TestClientContext testContext;
        CHECK(testContext.tryConnectWithResponse().noError());

        std::string topic{ "test/topic" };
        ByteBuffer payload(3);
        payload += 0x01;
        payload += 0x02;
        payload += 0x03;

        PublishOptions options;
        options.qos = Qos::QOS_0;

        auto err{ testContext.client->publish(topic.c_str(), std::move(payload), std::move(options)) };
        CHECK(err.noError());
    }

    TEST_CASE("Publish fails when not connected")
    {
        TestClientContext testContext;
        std::string topic{ "test/topic" };
        ByteBuffer payload(1);
        payload += 0x01;
        PublishOptions options;
        options.qos = Qos::QOS_0;

        auto result{ testContext.client->publish(topic.c_str(), std::move(payload), std::move(options)) };
        CHECK(result.errorCode() == ClientErrorCode::Not_Connected);
    }

    TEST_CASE("Successful Publish QOS 1 triggers PubAck event")
    {
        TestClientContext testContext;
        CHECK(testContext.tryConnectWithResponse().noError());

        std::string topic = "test/qos1";
        ByteBuffer payload(2);
        payload += 0xAA;
        payload += 0xBB;

        PublishOptions options;
        options.qos = Qos::QOS_1;

        bool pubAckEventFired = false;
        std::uint16_t ackedPacketId = 0;

		testContext.client->onPublishCompletedEvent().add([&](const PublishCompleteEventDetails& details)
			{
				pubAckEventFired = true;
				ackedPacketId = details.packetId;
			});

        auto err = testContext.client->publish(topic.c_str(), std::move(payload), std::move(options));
        CHECK(err.noError());

        // Simulate PUBACK from broker
        ByteBuffer pubAckBuffer(4);
        pubAckBuffer += 0x40; //PUBACK type
        pubAckBuffer += 0x02; //Remaining length
        pubAckBuffer += 0x00; //Packet ID MSB
        pubAckBuffer += 0x01; //Packet ID LSB (assuming packetId = 1)

        testContext.receiveResponse(pubAckBuffer);

        CHECK(pubAckEventFired);
        CHECK(ackedPacketId == 1);
    }

    TEST_CASE("Publish exceeding max server topic alias")
    {
        TestClientContext testContext;
        CHECK(testContext.tryConnectWithResponse().noError());

        MqttConnectionInfo& info = const_cast<MqttConnectionInfo&>(testContext.client->getConnectionInfo());
        info.maxServerTopicAlias = 1;

        std::string topic{ "test/alias" };
        ByteBuffer payload(1);
        payload += 0x01;
        PublishOptions options;
        options.qos = Qos::QOS_0;
        options.topicAlias = 2; //Exceeds maxServerTopicAlias

        auto err{ testContext.client->publish(topic.c_str(), std::move(payload), std::move(options)) };
        CHECK(err.noError());

        CHECK(testContext.client->tick().noError()); //Process send queue
    }

    TEST_CASE("Publish QOS 2 message")
    {
        TestClientContext testContext;
        CHECK(testContext.tryConnectWithResponse().noError());

        std::string topic = "test/qos2";
        ByteBuffer payload(4);
        payload += 0x01;
        payload += 0x02;
        payload += 0x03;
        payload += 0x04;

        PublishOptions options;
        options.qos = Qos::QOS_2;

        auto err = testContext.client->publish(topic.c_str(), std::move(payload), std::move(options));
        CHECK(err.noError());

        CHECK(testContext.client->tick().noError()); //Process send queue
    }

    TEST_CASE("Publish with retain flag")
    {
        TestClientContext testContext;
        CHECK(testContext.tryConnectWithResponse().noError());

        std::string topic = "test/retain";
        ByteBuffer payload(2);
        payload += 0xFF;
        payload += 0xFE;

        PublishOptions options;
        options.qos = Qos::QOS_0;
        options.retain = true;

        auto err = testContext.client->publish(topic.c_str(), std::move(payload), std::move(options));
        CHECK(err.noError());

        CHECK(testContext.client->tick().noError()); //Process send queue
    }

    TEST_CASE("Publish empty payload")
    {
        TestClientContext testContext;
        CHECK(testContext.tryConnectWithResponse().noError());

        std::string topic = "test/empty";
        ByteBuffer payload(0);

        PublishOptions options;
        options.qos = Qos::QOS_0;

        auto err = testContext.client->publish(topic.c_str(), std::move(payload), std::move(options));
        CHECK(err.noError());

        CHECK(testContext.client->tick().noError()); //Process send queue
    }

    TEST_CASE("Publish large payload")
    {
        TestClientContext testContext;
        CHECK(testContext.tryConnectWithResponse().noError());

        std::string topic = "test/large";
        ByteBuffer payload(1024);
        for (int i = 0; i < 1024; ++i) 
        {
            payload += static_cast<std::uint8_t>(i % 256);
        }

        PublishOptions options;
        options.qos = Qos::QOS_0;

        auto err = testContext.client->publish(topic.c_str(), std::move(payload), std::move(options));
        CHECK(err.noError());

        CHECK(testContext.client->tick().noError()); //Process send queue
    }

    TEST_CASE("Publish with topic alias")
    {
        TestClientContext testContext;
        CHECK(testContext.tryConnectWithResponse().noError());

        std::string topic = "test/alias/topic";
        ByteBuffer payload(3);
        payload += 0x11;
        payload += 0x22;
        payload += 0x33;

        PublishOptions options;
        options.qos = Qos::QOS_0;
        options.topicAlias = 1;

        auto err = testContext.client->publish(topic.c_str(), std::move(payload), std::move(options));
        CHECK(err.noError());

        CHECK(testContext.client->tick().noError()); //Process send queue
    }

    TEST_CASE("Multiple QOS 1 publishes generate different packet IDs")
    {
        TestClientContext testContext;
        CHECK(testContext.tryConnectWithResponse().noError());

        std::vector<std::uint16_t> packetIds;
        
        testContext.client->onPublishCompletedEvent().add([&](const PublishCompleteEventDetails& details)
        {
            packetIds.push_back(details.packetId);
        });

        //Publish first message
        std::string topic1 = "test/qos1/first";
        ByteBuffer payload1(1);
        payload1 += 0x01;
        PublishOptions options1;
        options1.qos = Qos::QOS_1;

        auto err1 = testContext.client->publish(topic1.c_str(), std::move(payload1), std::move(options1));
        CHECK(err1.noError());

        //Publish second message
        std::string topic2 = "test/qos1/second";
        ByteBuffer payload2(1);
        payload2 += 0x02;
        PublishOptions options2;
        options2.qos = Qos::QOS_1;

        auto err2 = testContext.client->publish(topic2.c_str(), std::move(payload2), std::move(options2));
        CHECK(err2.noError());

        //Simulate PUBACK for first message (packet ID 1)
        ByteBuffer pubAck1(4);
        pubAck1 += 0x40; //PUBACK type
        pubAck1 += 0x02; //Remaining length
        pubAck1 += 0x00; //Packet ID MSB
        pubAck1 += 0x01; //Packet ID LSB

        testContext.receiveResponse(pubAck1);

        //Simulate PUBACK for second message (packet ID 2)
        ByteBuffer pubAck2(4);
        pubAck2 += 0x40; //PUBACK type
        pubAck2 += 0x02; //Remaining length
        pubAck2 += 0x00; //Packet ID MSB
        pubAck2 += 0x02; //Packet ID LSB

        testContext.receiveResponse(pubAck2);

        CHECK(packetIds.size() == 2);
        CHECK(packetIds[0] == 1);
        CHECK(packetIds[1] == 2);

        CHECK(testContext.client->tick().noError()); //Process send queue
    }

    TEST_CASE("Publish with message expiry interval")
    {
        TestClientContext testContext;
        CHECK(testContext.tryConnectWithResponse().noError());

        std::string topic = "test/expiry";
        ByteBuffer payload(2);
        payload += 0xAB;
        payload += 0xCD;

        PublishOptions options;
        options.qos = Qos::QOS_0;
        options.messageExpiryInterval = 300; // 5 minutes

        auto err = testContext.client->publish(topic.c_str(), std::move(payload), std::move(options));
        CHECK(err.noError());

        CHECK(testContext.client->tick().noError()); //Process send queue
    }

    TEST_CASE("Publish with payload format BINARY")
    {
        TestClientContext testContext;
        CHECK(testContext.tryConnectWithResponse().noError());

        std::string topic = "test/content-type";
        ByteBuffer payload(10);
        for (int i = 0; i < 10; ++i) 
        {
            payload += static_cast<std::uint8_t>(0x30 + i); // ASCII numbers
        }

        PublishOptions options;
        options.qos = Qos::QOS_0;
        options.payloadFormatIndicator = PayloadFormatIndicator::BINARY;

        auto err = testContext.client->publish(topic.c_str(), std::move(payload), std::move(options));
        CHECK(err.noError());

        CHECK(testContext.client->tick().noError()); //Process send queue
    }

    TEST_CASE("Publish with response topic")
    {
        TestClientContext testContext;
        CHECK(testContext.tryConnectWithResponse().noError());

        std::string topic = "test/request";
        ByteBuffer payload(7);
        payload += 0x52; // 'R'
        payload += 0x45; // 'E'
        payload += 0x51; // 'Q'
        payload += 0x55; // 'U'
        payload += 0x45; // 'E'
        payload += 0x53; // 'S'
        payload += 0x54; // 'T'

        PublishOptions options;
        options.qos = Qos::QOS_1;
        options.responseTopic = "test/response";

        auto err = testContext.client->publish(topic.c_str(), std::move(payload), std::move(options));
        CHECK(err.noError());

        CHECK(testContext.client->tick().noError()); //Process send queue
    }

    TEST_CASE("Publish with correlation data")
    {
        TestClientContext testContext;
        CHECK(testContext.tryConnectWithResponse().noError());

        std::string topic = "test/correlation";
        ByteBuffer payload(3);
        payload += 0x01;
        payload += 0x02;
        payload += 0x03;

        ByteBuffer correlationData(8);
        correlationData += 0x12;
        correlationData += 0x34;
        correlationData += 0x56;
        correlationData += 0x78;

		BinaryData data{ correlationData };

        PublishOptions options;
        options.qos = Qos::QOS_1;
        options.correlationData = std::make_unique<BinaryData>(std::move(data));

        auto err = testContext.client->publish(topic.c_str(), std::move(payload), std::move(options));
        CHECK(err.noError());

        CHECK(testContext.client->tick().noError()); //Process send queue
    }

    TEST_CASE("Publish multiple messages rapidly")
    {
        TestClientContext testContext;
        CHECK(testContext.tryConnectWithResponse().noError());

        const int messageCount = 10;
        std::vector<ClientError> results;

        testContext.socketPtr->sentPackets.clear(); //Clear any previous packets from connect

        for (int i = 0; i < messageCount; ++i) 
        {
            std::string topic = "test/rapid/" + std::to_string(i);
            ByteBuffer payload(4);
            payload += static_cast<std::uint8_t>((i >> 24) & 0xFF);
            payload += static_cast<std::uint8_t>((i >> 16) & 0xFF);
            payload += static_cast<std::uint8_t>((i >> 8) & 0xFF);
            payload += static_cast<std::uint8_t>(i & 0xFF);

            PublishOptions options;
            options.qos = Qos::QOS_0;

            auto result = testContext.client->publish(topic.c_str(), std::move(payload), std::move(options));
            results.push_back(result.error);
        }

        // All publishes should succeed
        for (const auto& result : results) 
        {
            CHECK(result.noError());
        }

		CHECK(testContext.client->tick().noError()); //Process send queue
    }

    TEST_CASE("Publish with user properties")
    {
        TestClientContext testContext;
        CHECK(testContext.tryConnectWithResponse().noError());

        std::string topic = "test/user-props";
        ByteBuffer payload(1);
        payload += 0x42;

        PublishOptions options;
        options.qos = Qos::QOS_0;
        options.userProperties["key1"] = "value1";
        options.userProperties["key2"] = "value2";

        auto err = testContext.client->publish(topic.c_str(), std::move(payload), std::move(options));
        CHECK(err.noError());

        CHECK(testContext.client->tick().noError()); //Process send queue
    }

    TEST_CASE("Publish fails with disconnected socket")
    {
        TestClientContext testContext(kmMqtt::Config{}, false); //Socket connect will fail
        
        std::string topic = "test/disconnected";
        ByteBuffer payload(1);
        payload += 0x01;
        PublishOptions options;
        options.qos = Qos::QOS_0;

        auto result = testContext.client->publish(topic.c_str(), std::move(payload), std::move(options));
        CHECK(result.errorCode() == ClientErrorCode::Not_Connected);

        CHECK(testContext.client->tick().noError()); //Process send queue
    }

    TEST_CASE("Publish packet structure verification")
    {
        Config config;
        config.pingAlways = false;

        TestClientContext testContext{ config };
        CHECK(testContext.tryConnectWithResponse().noError());

        testContext.socketPtr->sentPackets.clear();

        std::string topic = "test";
        ByteBuffer payload(2);
        payload += 0x12;
        payload += 0x34;

        PublishOptions options;
        options.qos = Qos::QOS_0;
        options.retain = true;

        testContext.socketPtr->sentPackets.clear(); //Clear any previous packets

        auto err = testContext.client->publish(topic.c_str(), std::move(payload), std::move(options));
        CHECK(err.noError());

        //Trigger tick to process send queue
        testContext.client->tick();

        //Verify a packet was sent
        CHECK(testContext.socketPtr->sentPackets.size() >= 1);
        
        if (!testContext.socketPtr->sentPackets.empty()) 
        {
            const auto& sentPacket = testContext.socketPtr->sentPackets.back();
            CHECK(sentPacket.size() > 0);
            
            //Basic MQTT packet structure check
            //First byte should be PUBLISH packet type (0x30) with retain flag (0x01)
            std::uint8_t firstByte = sentPacket.readUint8();
            std::uint8_t packetType = (firstByte >> 4) & 0x0F;
            std::uint8_t flags = firstByte & 0x0F;
            
            CHECK(packetType == static_cast<std::uint8_t>(PacketType::PUBLISH)); //PUBLISH packet type
            CHECK((flags & 0x01) == 0x01); //Retain flag should be set
        }
    }
}
