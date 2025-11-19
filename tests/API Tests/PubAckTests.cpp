#include <doctest.h>
#include <cleanMqtt/MqttClient.h>
#include <string>
#include "MockWebSocket.h"
#include "Helpers.h"

using namespace cleanMqtt;
using namespace cleanMqtt::mqtt;

TEST_SUITE("MqttClient PubAck")
{
    TEST_CASE("Successful PubAck after QOS 1 Publish")
    {
        TestClientContext testContext{  };
        CHECK(testContext.tryConnectWithResponse().noError());

        std::string topic = "test/puback";
        ByteBuffer payload(2);
        payload += 0xAA;
        payload += 0xBB;

        PublishOptions options;
        options.qos = Qos::QOS_1;

        bool pubAckEventFired = false;
        std::uint16_t ackedPacketId = 0;
        PubAckReasonCode receivedReasonCode = PubAckReasonCode::UNSPECIFIED_ERROR;

        testContext.client->onPublishAckEvent().add([&](const PublishAckEventDetails& details, const mqtt::PublishAck& packet)
        {
            pubAckEventFired = true;
            ackedPacketId = details.packetId;
            receivedReasonCode = packet.getVariableHeader().reasonCode;
            CHECK(packet.getPacketType() == PacketType::PUBLISH_ACKNOWLEDGE);
        });

        auto err = testContext.client->publish(topic.c_str(), std::move(payload), std::move(options));
        CHECK(err.noError());

		testContext.client->tick(); //Process the publish
        testContext.client->tick(); //Process the publish

        //Simulate PUBACK from broker with SUCCESS reason code
        ByteBuffer pubAckBuffer(6);
        pubAckBuffer += 0x40; //PUBACK type
        pubAckBuffer += 0x04; //Remaining length
        pubAckBuffer += 0x00; //Packet ID MSB
        pubAckBuffer += 0x01; //Packet ID LSB (assuming packetId = 1)
        pubAckBuffer += 0x00; //Reason code (SUCCESS)
		pubAckBuffer += 0x00; //Property Length

        testContext.receiveResponse(pubAckBuffer);

        CHECK(pubAckEventFired);
        CHECK(ackedPacketId == 1);
        CHECK(receivedReasonCode == PubAckReasonCode::SUCCESS);
    }

    TEST_CASE("PubAck with NO_MATCHING_SUBSCRIBERS reason code")
    {
        TestClientContext testContext;
        CHECK(testContext.tryConnectWithResponse().noError());

        std::string topic = "test/no_subscribers";
        ByteBuffer payload(1);
        payload += 0xFF;

        PublishOptions options;
        options.qos = Qos::QOS_1;

        bool pubAckEventFired = false;
        PubAckReasonCode receivedReasonCode = PubAckReasonCode::UNSPECIFIED_ERROR;

        testContext.client->onPublishAckEvent().add([&](const PublishAckEventDetails&, const mqtt::PublishAck& packet)
        {
            pubAckEventFired = true;
            receivedReasonCode = packet.getVariableHeader().reasonCode;
        });

        auto err = testContext.client->publish(topic.c_str(), std::move(payload), std::move(options));
        CHECK(err.noError());

        //Simulate PUBACK with NO_MATCHING_SUBSCRIBERS
        ByteBuffer pubAckBuffer(6);
        pubAckBuffer += 0x40; //PUBACK type
        pubAckBuffer += 0x04; //Remaining length
        pubAckBuffer += 0x00; //Packet ID MSB
        pubAckBuffer += 0x01; //Packet ID LSB
        pubAckBuffer += 0x10; //Reason code (NO_MATCHING_SUBSCRIBERS = 0x10)
        pubAckBuffer += 0x00; //Property Length

        testContext.receiveResponse(pubAckBuffer);

        CHECK(pubAckEventFired);
        CHECK(receivedReasonCode == PubAckReasonCode::NO_MATCHING_SUBSCRIBERS);
    }

    TEST_CASE("PubAck releases packet ID after acknowledgment")
    {
        TestClientContext testContext;
        CHECK(testContext.tryConnectWithResponse().noError());

        std::vector<std::uint16_t> receivedPacketIds;

        testContext.client->onPublishAckEvent().add([&](const PublishAckEventDetails& details, const mqtt::PublishAck&)
        {
            receivedPacketIds.push_back(details.packetId);
        });

        //First publish
        std::string topic1 = "test/first";
        ByteBuffer payload1(1);
        payload1 += 0x01;
        PublishOptions options1;
        options1.qos = Qos::QOS_1;

        auto err1 = testContext.client->publish(topic1.c_str(), std::move(payload1), std::move(options1));
        CHECK(err1.noError());

        //Simulate PUBACK for first message
        ByteBuffer pubAck1(6);
        pubAck1 += 0x40; //PUBACK type
        pubAck1 += 0x04; //Remaining length
        pubAck1 += 0x00; //Packet ID MSB
        pubAck1 += 0x01; //Packet ID LSB
        pubAck1 += 0x00; //Reason code (SUCCESS)
        pubAck1 += 0x00; //Property Length

        testContext.receiveResponse(pubAck1);

        //Second publish (should reuse packet ID 1 after it's released)
        std::string topic2 = "test/second";
        ByteBuffer payload2(1);
        payload2 += 0x02;
        PublishOptions options2;
        options2.qos = Qos::QOS_1;

        auto err2 = testContext.client->publish(topic2.c_str(), std::move(payload2), std::move(options2));
        CHECK(err2.noError());

        //Simulate PUBACK for second message
        ByteBuffer pubAck2(6);
        pubAck2 += 0x40; //PUBACK type
        pubAck2 += 0x04; //Remaining length
        pubAck2 += 0x00; //Packet ID MSB
        pubAck2 += 0x01; //Packet ID LSB (reused)
        pubAck2 += 0x00; //Reason code (SUCCESS)
        pubAck2 += 0x00; //Property Length

        testContext.receiveResponse(pubAck2);

        CHECK(receivedPacketIds.size() == 2);
        CHECK(receivedPacketIds[0] == 1);
        CHECK(receivedPacketIds[1] == 1); // Packet ID was reused
    }

    TEST_CASE("Multiple PubAck for different packet IDs")
    {
        TestClientContext testContext;
        CHECK(testContext.tryConnectWithResponse().noError());

        std::vector<std::uint16_t> ackedPacketIds;

        testContext.client->onPublishAckEvent().add([&](const PublishAckEventDetails& details, const mqtt::PublishAck&)
        {
            ackedPacketIds.push_back(details.packetId);
        });

        //Publish three messages
        for (int i = 0; i < 3; ++i)
        {
            std::string topic = "test/multi/" + std::to_string(i);
            ByteBuffer payload(1);
            payload += static_cast<std::uint8_t>(i);
            PublishOptions options;
            options.qos = Qos::QOS_1;

            auto err = testContext.client->publish(topic.c_str(), std::move(payload), std::move(options));
            CHECK(err.noError());
        }

        //Simulate PUBACK for all three messages
        for (int i = 1; i <= 3; ++i)
        {
            ByteBuffer pubAckBuffer(6);
            pubAckBuffer += 0x40; //PUBACK type
            pubAckBuffer += 0x04; //Remaining length
            pubAckBuffer += 0x00; //Packet ID MSB
            pubAckBuffer += static_cast<std::uint8_t>(i); //Packet ID LSB
            pubAckBuffer += 0x00; //Reason code (SUCCESS)
            pubAckBuffer += 0x00; //Property Length

            testContext.receiveResponse(pubAckBuffer);
        }

        CHECK(ackedPacketIds.size() == 3);
        CHECK(ackedPacketIds[0] == 1);
        CHECK(ackedPacketIds[1] == 2);
        CHECK(ackedPacketIds[2] == 3);
    }

    TEST_CASE("PubAck not fired for QOS 0 publish")
    {
        TestClientContext testContext;
        CHECK(testContext.tryConnectWithResponse().noError());

        bool pubAckEventFired = false;

        testContext.client->onPublishAckEvent().add([&](const PublishAckEventDetails&, const mqtt::PublishAck&)
        {
            pubAckEventFired = true;
        });

        std::string topic = "test/qos0";
        ByteBuffer payload(1);
        payload += 0xAA;
        PublishOptions options;
        options.qos = Qos::QOS_0;

        auto err = testContext.client->publish(topic.c_str(), std::move(payload), std::move(options));
        CHECK(err.noError());

        CHECK(testContext.client->tick().noError()); //Process send queue

        //No PUBACK should be expected for QOS 0
        CHECK(pubAckEventFired == false);
    }

    TEST_CASE("PubAck with QUOTA_EXCEEDED reason code")
    {
        TestClientContext testContext;
        CHECK(testContext.tryConnectWithResponse().noError());

        std::string topic = "test/quota";
        ByteBuffer payload(1);
        payload += 0x42;

        PublishOptions options;
        options.qos = Qos::QOS_1;

        bool pubAckEventFired = false;
        PubAckReasonCode receivedReasonCode = PubAckReasonCode::UNSPECIFIED_ERROR;

        testContext.client->onPublishAckEvent().add([&](const PublishAckEventDetails&, const mqtt::PublishAck& packet)
        {
            pubAckEventFired = true;
            receivedReasonCode = packet.getVariableHeader().reasonCode;
        });

        auto err = testContext.client->publish(topic.c_str(), std::move(payload), std::move(options));
        CHECK(err.noError());

        //Simulate PUBACK with QUOTA_EXCEEDED
        ByteBuffer pubAckBuffer(6);
        pubAckBuffer += 0x40; //PUBACK type
        pubAckBuffer += 0x04; //Remaining length
        pubAckBuffer += 0x00; //Packet ID MSB
        pubAckBuffer += 0x01; //Packet ID LSB
        pubAckBuffer += 0x97; //Reason code (QUOTA_EXCEEDED = 0x97)
		pubAckBuffer += 0x00; //Property Length

        testContext.receiveResponse(pubAckBuffer);

        CHECK(pubAckEventFired);
        CHECK(receivedReasonCode == PubAckReasonCode::QUOTA_EXCEEDED);
    }

    TEST_CASE("PubAck with PAYLOAD_FORMAT_INVALID reason code")
    {
        TestClientContext testContext;
        CHECK(testContext.tryConnectWithResponse().noError());

        std::string topic = "test/format";
        ByteBuffer payload(3);
        payload += 0x01;
        payload += 0x02;
        payload += 0x03;

        PublishOptions options;
        options.qos = Qos::QOS_1;

        bool pubAckEventFired = false;
        PubAckReasonCode receivedReasonCode = PubAckReasonCode::UNSPECIFIED_ERROR;

        testContext.client->onPublishAckEvent().add([&](const PublishAckEventDetails&, const mqtt::PublishAck& packet)
        {
            pubAckEventFired = true;
            receivedReasonCode = packet.getVariableHeader().reasonCode;
        });

        auto err = testContext.client->publish(topic.c_str(), std::move(payload), std::move(options));
        CHECK(err.noError());

        //Simulate PUBACK with PAYLOAD_FORMAT_INVALID
        ByteBuffer pubAckBuffer(6);
        pubAckBuffer += 0x40; //PUBACK type
        pubAckBuffer += 0x04; //Remaining length
        pubAckBuffer += 0x00; //Packet ID MSB
        pubAckBuffer += 0x01; //Packet ID LSB
        pubAckBuffer += 0x99; //Reason code (PAYLOAD_FORMAT_INVALID = 0x99)
		pubAckBuffer += 0x00; //Property Length

        testContext.receiveResponse(pubAckBuffer);

        CHECK(pubAckEventFired);
        CHECK(receivedReasonCode == PubAckReasonCode::PAYLOAD_FORMAT_INVALID);
    }

    TEST_CASE("PubAck removes message from session state")
    {
        TestClientContext testContext;
        CHECK(testContext.tryConnectWithResponse().noError());

        std::string topic = "test/session";
        ByteBuffer payload(2);
        payload += 0xDE;
        payload += 0xAD;

        PublishOptions options;
        options.qos = Qos::QOS_1;

        auto err = testContext.client->publish(topic.c_str(), std::move(payload), std::move(options));
        CHECK(err.noError());

        //Verify message is in session state (implementation detail, assumes access to session state)
        const auto& connectionInfo = testContext.client->getConnectionInfo();
        CHECK(connectionInfo.sessionState.messages().size() == 1);

        //Simulate PUBACK
        ByteBuffer pubAckBuffer(6);
        pubAckBuffer += 0x40; //PUBACK type
        pubAckBuffer += 0x04; //Remaining length
        pubAckBuffer += 0x00; //Packet ID MSB
        pubAckBuffer += 0x01; //Packet ID LSB
        pubAckBuffer += 0x00; //Reason code (SUCCESS)
		pubAckBuffer += 0x00; //Property Length

        testContext.receiveResponse(pubAckBuffer);

        //Verify message is removed from session state
        CHECK(connectionInfo.sessionState.messages().size() == 0);
    }

    TEST_CASE("PubAck with TOPIC_NAME_INVALID reason code")
    {
        TestClientContext testContext;
        CHECK(testContext.tryConnectWithResponse().noError());

        std::string topic = "test/invalid";
        ByteBuffer payload(1);
        payload += 0xFF;

        PublishOptions options;
        options.qos = Qos::QOS_1;

        bool pubAckEventFired = false;
        PubAckReasonCode receivedReasonCode = PubAckReasonCode::UNSPECIFIED_ERROR;

        testContext.client->onPublishAckEvent().add([&](const PublishAckEventDetails&, const mqtt::PublishAck& packet)
        {
            pubAckEventFired = true;
            receivedReasonCode = packet.getVariableHeader().reasonCode;
        });

        auto err = testContext.client->publish(topic.c_str(), std::move(payload), std::move(options));
        CHECK(err.noError());

        //Simulate PUBACK with TOPIC_NAME_INVALID
        ByteBuffer pubAckBuffer(6);
        pubAckBuffer += 0x40; //PUBACK type
        pubAckBuffer += 0x04; //Remaining length
        pubAckBuffer += 0x00; //Packet ID MSB
        pubAckBuffer += 0x01; //Packet ID LSB
        pubAckBuffer += 0x90; //Reason code (TOPIC_NAME_INVALID = 0x90)
		pubAckBuffer += 0x00; //Property Length

        testContext.receiveResponse(pubAckBuffer);

        CHECK(pubAckEventFired);
        CHECK(receivedReasonCode == PubAckReasonCode::TOPIC_NAME_INVALID);
    }

    TEST_CASE("PubAck with PACKET_IDENTIFIER_IN_USE reason code")
    {
        TestClientContext testContext;
        CHECK(testContext.tryConnectWithResponse().noError());

        std::string topic = "test/id_in_use";
        ByteBuffer payload(1);
        payload += 0x88;

        PublishOptions options;
        options.qos = Qos::QOS_1;

        bool pubAckEventFired = false;
        PubAckReasonCode receivedReasonCode = PubAckReasonCode::UNSPECIFIED_ERROR;

        testContext.client->onPublishAckEvent().add([&](const PublishAckEventDetails&, const mqtt::PublishAck& packet)
        {
            pubAckEventFired = true;
            receivedReasonCode = packet.getVariableHeader().reasonCode;
        });

        auto err = testContext.client->publish(topic.c_str(), std::move(payload), std::move(options));
        CHECK(err.noError());

        //Simulate PUBACK with PACKET_IDENTIFIER_IN_USE
        ByteBuffer pubAckBuffer(6);
        pubAckBuffer += 0x40; //PUBACK type
        pubAckBuffer += 0x04; //Remaining length
        pubAckBuffer += 0x00; //Packet ID MSB
        pubAckBuffer += 0x01; //Packet ID LSB
        pubAckBuffer += 0x91; //Reason code (PACKET_IDENTIFIER_IN_USE = 0x91)
		pubAckBuffer += 0x00; //Property Length

        testContext.receiveResponse(pubAckBuffer);

        CHECK(pubAckEventFired);
        CHECK(receivedReasonCode == PubAckReasonCode::PACKET_IDENTIFIER_IN_USE);
    }

    TEST_CASE("PubAck with IMPLEMENTATION_SPECIFIC_ERROR reason code")
    {
        TestClientContext testContext;
        CHECK(testContext.tryConnectWithResponse().noError());

        std::string topic = "test/impl_error";
        ByteBuffer payload(4);
        payload += 0x11;
        payload += 0x22;
        payload += 0x33;
        payload += 0x44;

        PublishOptions options;
        options.qos = Qos::QOS_1;

        bool pubAckEventFired = false;
        PubAckReasonCode receivedReasonCode = PubAckReasonCode::UNSPECIFIED_ERROR;

        testContext.client->onPublishAckEvent().add([&](const PublishAckEventDetails&, const mqtt::PublishAck& packet)
        {
            pubAckEventFired = true;
            receivedReasonCode = packet.getVariableHeader().reasonCode;
        });

        auto err = testContext.client->publish(topic.c_str(), std::move(payload), std::move(options));
        CHECK(err.noError());

        //Simulate PUBACK with IMPLEMENTATION_SPECIFIC_ERROR
        ByteBuffer pubAckBuffer(6);
        pubAckBuffer += 0x40; //PUBACK type
        pubAckBuffer += 0x04; //Remaining length
        pubAckBuffer += 0x00; //Packet ID MSB
        pubAckBuffer += 0x01; //Packet ID LSB
        pubAckBuffer += 0x83; //Reason code (IMPLEMENTATION_SPECIFIC_ERROR = 0x83)
		pubAckBuffer += 0x00; //Property Length

        testContext.receiveResponse(pubAckBuffer);

        CHECK(pubAckEventFired);
        CHECK(receivedReasonCode == PubAckReasonCode::IMPLEMENTATION_SPECIFIC_ERROR);
    }

    TEST_CASE("PubAck event fires in correct order for sequential publishes")
    {
        TestClientContext testContext;
        CHECK(testContext.tryConnectWithResponse().noError());

        std::vector<std::uint16_t> expectedOrder = { 1, 2, 3 };
        std::vector<std::uint16_t> actualOrder;

        testContext.client->onPublishAckEvent().add([&](const PublishAckEventDetails& details, const mqtt::PublishAck&)
        {
            actualOrder.push_back(details.packetId);
        });

        //Publish three messages
        for (int i = 0; i < 3; ++i)
        {
            std::string topic = "test/order/" + std::to_string(i);
            ByteBuffer payload(1);
            payload += static_cast<std::uint8_t>(i);
            PublishOptions options;
            options.qos = Qos::QOS_1;

            auto err = testContext.client->publish(topic.c_str(), std::move(payload), std::move(options));
            CHECK(err.noError());

            //Immediately acknowledge each message
            ByteBuffer pubAckBuffer(6);
            pubAckBuffer += 0x40; //PUBACK type
            pubAckBuffer += 0x04; //Remaining length
            pubAckBuffer += 0x00; //Packet ID MSB
            pubAckBuffer += static_cast<std::uint8_t>(i + 1); //Packet ID LSB
            pubAckBuffer += 0x00; //Reason code (SUCCESS)
			pubAckBuffer += 0x00; //Property Length

            testContext.receiveResponse(pubAckBuffer);
        }

        CHECK(actualOrder == expectedOrder);
    }
}