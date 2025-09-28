#include <doctest.h>
#include <cleanMqtt/Mqtt/MqttClient.h>
#include <memory>
#include <string>
#include "MockWebSocket.h"
#include "Helpers.h"

using namespace cleanMqtt;
using namespace cleanMqtt::mqtt;

TEST_SUITE("MqttClient Subscribe")
{
    TEST_CASE("Successful Subscribe QOS 0")
    {
        TestClientContext testContext;
        CHECK(testContext.tryConnectWithResponse().noError());

        std::vector<Topic> topics;
        topics.push_back({ "test/topic"});

        SubscribeOptions options{};

        bool subAckEventFired{ false };
        std::uint16_t ackedPacketId{ 0 };
        SubAckResults ackResults{};

        testContext.client->onSubscribeAckEvent().add([&](const SubscribeAckEventDetails& details, const mqtt::packets::SubscribeAck& packet)
        {
            subAckEventFired = true;
            ackedPacketId = details.packetId;
            ackResults = details.results;
            CHECK(packet.getPacketType() == packets::PacketType::SUBSCRIBE_ACKNOWLEDGE);
        });

        auto err = testContext.client->subscribe(topics, std::move(options));
        CHECK(err.noError());

        // Simulate SUBACK from broker
        ByteBuffer subAckBuffer{ 6 };
        subAckBuffer += 0x90; // SUBACK type
        subAckBuffer += 0x04; // Remaining length
        subAckBuffer += 0x00; // Packet ID MSB
        subAckBuffer += 0x01; // Packet ID LSB (assuming packetId = 1)
        subAckBuffer += 0x00; // Properties Length
        subAckBuffer += 0x00; // Reason code (QOS 0)

        testContext.receiveResponse(subAckBuffer);

        CHECK(subAckEventFired);
        CHECK(ackedPacketId == 1);
        CHECK(ackResults.allSubscribedSuccesfully() == true);
        CHECK(ackResults.getTopicReasons().size() == 1);
        CHECK(ackResults.getTopicReasons()[0].reasonCode == packets::SubAckReasonCode::GRANTED_QOS_0); // QOS 0
    }

    TEST_CASE("Subscribe fails when not connected")
    {
        TestClientContext testContext;

        std::vector<Topic> topics;
        topics.push_back({ "test/topic" });
        SubscribeOptions options{};

        auto err = testContext.client->subscribe(topics, std::move(options));
        CHECK(err.errorCode == ClientErrorCode::Not_Connected);
    }

    TEST_CASE("Subscribe with multiple topics and QOS 1/2")
    {
        TestClientContext testContext;
        CHECK(testContext.tryConnectWithResponse().noError());

        std::vector<Topic> topics;
        topics.push_back({ "topic/1", Qos::QOS_1 });
        topics.push_back({ "topic/2", Qos::QOS_2 });

        SubscribeOptions options{};
        options.subscribeIdentifier = packets::VariableByteInteger::tryCreateFromValue(5);

        bool subAckEventFired{ false };
        SubAckResults ackResults{};

        testContext.client->onSubscribeAckEvent().add([&](const SubscribeAckEventDetails& details, const mqtt::packets::SubscribeAck& ackPacket)
        {
            subAckEventFired = true;
            ackResults = details.results;

			CHECK(ackPacket.getPacketType() == packets::PacketType::SUBSCRIBE_ACKNOWLEDGE);
        });

        auto err = testContext.client->subscribe(topics, std::move(options));
        CHECK(err.noError());

        // Simulate SUBACK from broker with QOS 1 and QOS 2
        ByteBuffer subAckBuffer{ 7 };
        subAckBuffer += 0x90; // SUBACK type
        subAckBuffer += 0x05; // Remaining length
        subAckBuffer += 0x00; // Packet ID MSB
        subAckBuffer += 0x01; // Packet ID LSB
		subAckBuffer += 0x00; // Properties Length
        subAckBuffer += 0x01; // Reason code (QOS 1)
        subAckBuffer += 0x02; // Reason code (QOS 2)

        testContext.receiveResponse(subAckBuffer);

        CHECK(subAckEventFired);
        CHECK(ackResults.allSubscribedSuccesfully() == true);
        CHECK(ackResults.getTopicReasons().size() == 2);

        for (auto topicReason : ackResults.getTopicReasons())
        {
            if (topicReason.topic.topicFilter == "topic/1")
            {
				CHECK(topicReason.topic.options.qos == Qos::QOS_1);

                //Check API defaults
                CHECK(topicReason.topic.options.noLocal == false);
                CHECK(topicReason.topic.options.retainHandling == RetainHandling::SendAtSubscribe);
                CHECK(topicReason.topic.options.retainAsPublished == false);

                //Check response
                CHECK(topicReason.reasonCode == packets::SubAckReasonCode::GRANTED_QOS_1);
            }
            else if (topicReason.topic.topicFilter == "topic/2")
            {
                CHECK(topicReason.topic.options.qos == Qos::QOS_2);

                //Check API defaults
                CHECK(topicReason.topic.options.noLocal == false);
                CHECK(topicReason.topic.options.retainHandling == RetainHandling::SendAtSubscribe);
                CHECK(topicReason.topic.options.retainAsPublished == false);

                //Check response
				CHECK(topicReason.reasonCode == packets::SubAckReasonCode::GRANTED_QOS_2);
            }
        }
    }

    TEST_CASE("Subscribe with empty topic list")
    {
        TestClientContext testContext;
        CHECK(testContext.tryConnectWithResponse().noError());

        std::vector<Topic> topics{};
        SubscribeOptions options{};

        auto err = testContext.client->subscribe(topics, std::move(options));
        CHECK(err.errorCode == ClientErrorCode::Invalid_Argument);
    }

    TEST_CASE("SubscribeAck with unknown packetId releases id and does not fire event")
    {
        TestClientContext testContext;
        CHECK(testContext.tryConnectWithResponse().noError());

        bool subAckEventFired{ false };
        testContext.client->onSubscribeAckEvent().add([&](const SubscribeAckEventDetails&, const mqtt::packets::SubscribeAck&)
        {
            subAckEventFired = true;
        });

        // Simulate SUBACK with unknown packetId
        ByteBuffer subAckBuffer{ 5 };
        subAckBuffer += 0x90; // SUBACK type
        subAckBuffer += 0x03; // Remaining length
        subAckBuffer += 0x00; // Packet ID MSB
        subAckBuffer += 0xFF; // Packet ID LSB (unknown)
        subAckBuffer += 0x00; // Reason code

        testContext.receiveResponse(subAckBuffer);

        CHECK(subAckEventFired == false);
    }
}
