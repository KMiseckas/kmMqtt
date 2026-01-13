#include <doctest.h>
#include <cleanMqtt/MqttClient.h>
#include <memory>
#include <string>
#include "MockWebSocket.h"
#include "Helpers.h"

using namespace cleanMqtt;
using namespace cleanMqtt::mqtt;

TEST_SUITE("MqttClient Receive Maximum")
{
    TEST_CASE("Receive Maximum - Client side - Disconnect when exceeded")
    {
        Config config;
        config.pingAlways = false;

        TestClientContext testContext{ config };
        
        ConnectArgs args{ "Id" };
        args.receiveMaximum = 2;
        args.keepAliveInSec = 60;

        ConnectAddress address{ Address::createURL("", "localhost", "1883", "") };
        testContext.client->connect(std::move(args), std::move(address));
        CHECK(testContext.client->tick().noError());

        ByteBuffer connAckBuffer(5);
        connAckBuffer += 0x20; // CONNACK type
        connAckBuffer += 0x03; // Remaining length
        connAckBuffer += 0x00; // Session present = false
        connAckBuffer += 0x00; // Reason code = success
        connAckBuffer += 0x00; // No properties

        testContext.receiveResponse(connAckBuffer);
        CHECK(testContext.client->getConnectionStatus() == ConnectionStatus::CONNECTED);

        bool disconnectEventFired = false;
        DisconnectReasonCode disconnectReason = DisconnectReasonCode::NORMAL_DISCONNECTION;

        testContext.client->onDisconnectEvent().add([&](const DisconnectEventDetails& details) {
            disconnectEventFired = true;
            disconnectReason = details.reasonCode;
        });

		//Queue up byte buffer of three PUBLISH QOS 1 packets to exceed receive maximum of 2
        ByteBuffer pub(51);
        pub += 0x32; // PUBLISH QOS 1, DUP=0, RETAIN=0
        pub += 0x0F; // Remaining length
        pub += 0x00; // Topic length MSB
        pub += 0x04; // Topic length LSB
        pub += 't';
        pub += 'e';
        pub += 's';
        pub += 't';
        pub += 0x00; // Packet ID MSB
        pub += 0x01; // Packet ID LSB
        pub += 0x00; // No properties
        pub += 0x01; // Payload
        pub += 0x02;
        pub += 0x03;
        pub += 0x04;
        pub += 0x05;
        pub += 0x06;

        pub += 0x32; // PUBLISH QOS 1
        pub += 0x0F; // Remaining length
        pub += 0x00; // Topic length MSB
        pub += 0x04; // Topic length LSB
        pub += 't';
        pub += 'e';
        pub += 's';
        pub += 't';
        pub += 0x00; // Packet ID MSB
        pub += 0x02; // Packet ID LSB
        pub += 0x00; // No properties
        pub += 0x01;
        pub += 0x02;
        pub += 0x03;
        pub += 0x04;
        pub += 0x05;
        pub += 0x06;

        pub += 0x32; // PUBLISH QOS 1
        pub += 0x0F; // Remaining length
        pub += 0x00; // Topic length MSB
        pub += 0x04; // Topic length LSB
        pub += 't';
        pub += 'e';
        pub += 's';
        pub += 't';
        pub += 0x00; // Packet ID MSB
        pub += 0x03; // Packet ID LSB
        pub += 0x00; // No properties
        pub += 0x01;
        pub += 0x02;
        pub += 0x03;
        pub += 0x04;
        pub += 0x05;
        pub += 0x06;

        std::vector<ByteBuffer> packets;
        std::size_t leftOver;
        CHECK(separateMqttPacketByteBuffers(pub, packets, leftOver) == true);
        REQUIRE(leftOver == 0);
		REQUIRE(packets.size() == 3);

        testContext.receiveResponse(pub);
        CHECK(testContext.client->tick().noError());
        CHECK(disconnectEventFired == true);

        // Should disconnect with RECEIVE_MAXIMUM_EXCEEDED
        CHECK(disconnectEventFired);
        CHECK(disconnectReason == DisconnectReasonCode::RECEIVE_MAXIMUM_EXCEEDED);
        CHECK(testContext.client->getConnectionStatus() == ConnectionStatus::DISCONNECTED);
    }

    TEST_CASE("Receive Maximum - Server side - Respect broker's receive maximum")
    {
        Config config;
        config.pingAlways = false;

        TestClientContext testContext{ config };
        
        ConnectArgs args{ "Id" };
        args.keepAliveInSec = 60;

        ConnectAddress address{ Address::createURL("", "localhost", "1883", "") };
        testContext.client->connect(std::move(args), std::move(address));
        CHECK(testContext.client->tick().noError());

        // Simulate CONNACK with broker receive maximum = 2
        ByteBuffer connAckBuffer(8);
        connAckBuffer += 0x20; // CONNACK type
        connAckBuffer += 0x06; // Remaining length
        connAckBuffer += 0x00; // Session present = false
        connAckBuffer += 0x00; // Reason code = success
        connAckBuffer += 0x03; // Properties length
        connAckBuffer += 0x21; // Receive Maximum property ID
        connAckBuffer += 0x00; // Value MSB
        connAckBuffer += 0x02; // Value LSB (receive maximum = 2)

        testContext.receiveResponse(connAckBuffer);
        CHECK(testContext.client->getConnectionStatus() == ConnectionStatus::CONNECTED);

        const MqttConnectionInfo& info = testContext.client->getConnectionInfo();
        CHECK(info.receiveMaximumAsServer == 2);

        // Publish first message QOS 1
        std::string topic1 = "test/1";
        ByteBuffer payload1(2);
        payload1 += 0x01; payload1 += 0x02;
        PublishOptions options1;
        options1.qos = Qos::QOS_1;

        auto err1 = testContext.client->publish(topic1.c_str(), std::move(payload1), std::move(options1));
        CHECK(err1.noError());

        testContext.socketPtr->sentPackets.clear();
        CHECK(testContext.client->tick().noError());
        CHECK(testContext.socketPtr->sentPackets.size() == 1);

        // Publish second message QOS 1
        std::string topic2 = "test/2";
        ByteBuffer payload2(2);
        payload2 += 0x03; payload2 += 0x04;
        PublishOptions options2;
        options2.qos = Qos::QOS_1;

        auto err2 = testContext.client->publish(topic2.c_str(), std::move(payload2), std::move(options2));
        CHECK(err2.noError());

        testContext.socketPtr->sentPackets.clear();
        CHECK(testContext.client->tick().noError());
        CHECK(testContext.socketPtr->sentPackets.size() == 1);

        testContext.socketPtr->sentPackets.clear();

        // Publish third message QOS 1 - Should be queued but not sent yet
        std::string topic3 = "test/3";
        ByteBuffer payload3(2);
        payload3 += 0x05; payload3 += 0x06;
        PublishOptions options3;
        options3.qos = Qos::QOS_1;

        auto err3 = testContext.client->publish(topic3.c_str(), std::move(payload3), std::move(options3));
        CHECK(err3.noError());
        testContext.socketPtr->sentPackets.clear();
        CHECK(testContext.client->tick().noError());
        CHECK(testContext.socketPtr->sentPackets.size() == 0);

        // Simulate PUBACK for first message (packet ID 1)
        ByteBuffer pubAck1(4);
        pubAck1 += 0x40;
        pubAck1 += 0x02;
        pubAck1 += 0x00;
        pubAck1 += 0x01;

        testContext.receiveResponse(pubAck1);
		CHECK(testContext.client->tick().noError());
        CHECK(testContext.socketPtr->sentPackets.size() == 1);
    }

    TEST_CASE("Receive Maximum - Default value is 65535")
    {
        Config config;
        TestClientContext testContext{ config };
        
        ConnectArgs args{ "Id" };

        ConnectAddress address{ Address::createURL("", "localhost", "1883", "") };
        testContext.client->connect(std::move(args), std::move(address));
        CHECK(testContext.client->tick().noError());

        ByteBuffer connAckBuffer(5);
        connAckBuffer += 0x20;
        connAckBuffer += 0x03;
        connAckBuffer += 0x00;
        connAckBuffer += 0x00;
        connAckBuffer += 0x00;

        testContext.receiveResponse(connAckBuffer);
        
        const MqttConnectionInfo& info = testContext.client->getConnectionInfo();
        CHECK(info.receiveMaximumAsClient == 65535);
        CHECK(info.receiveMaximumAsServer == 65535);
    }
}