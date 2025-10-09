
#include <doctest.h>
#include <cleanMqtt/MqttClient.h>
#include <memory>
#include <string>
#include "MockWebSocket.h"
#include "Helpers.h"

using namespace cleanMqtt;
using namespace cleanMqtt::mqtt;

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

        auto err{ testContext.client->publish(topic.c_str(), std::move(payload), std::move(options)) };
        CHECK(err.errorCode == ClientErrorCode::Not_Connected);
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

		testContext.client->onPublishAckEvent().add([&](const PublishAckEventDetails& details, const mqtt::packets::PublishAck&)
			{
				pubAckEventFired = true;
				ackedPacketId = details.packetId;
			});

        auto err = testContext.client->publish(topic.c_str(), std::move(payload), std::move(options));
        CHECK(err.noError());

        // Simulate PUBACK from broker
        ByteBuffer pubAckBuffer(4);
        pubAckBuffer += 0x40; // PUBACK type
        pubAckBuffer += 0x02; // Remaining length
        pubAckBuffer += 0x00; // Packet ID MSB
        pubAckBuffer += 0x01; // Packet ID LSB (assuming packetId = 1)

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
        options.topicAlias = 2; // Exceeds maxServerTopicAlias

        auto err{ testContext.client->publish(topic.c_str(), std::move(payload), std::move(options)) };
        CHECK(err.noError());
    }
}
