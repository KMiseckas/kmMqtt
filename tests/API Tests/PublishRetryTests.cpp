#include "Helpers.h"
#include "MockWebSocket.h"
#include <chrono>
#include <kmMqtt/MqttClient.h>
#include <doctest.h>
#include <string>
#include <thread>

using namespace kmMqtt;
using namespace kmMqtt::mqtt;

TEST_SUITE("MqttClient Publish Retry and Session State")
{
    TEST_CASE("QOS 1 message retry after no PUBACK")
    {
        Config config;
        config.retryPublishIntervalMS = 100;
        config.pingAlways = false;
        
        TestClientContext testContext{ config };
        CHECK(testContext.tryConnectWithResponse().noError());

        testContext.socketPtr->sentPackets.clear();

        std::string topic = "test/retry";
        ByteBuffer payload(2);
        payload += 0x11;
        payload += 0x22;

        PublishOptions options;
        options.qos = Qos::QOS_1;

        auto err = testContext.client->publish(topic.c_str(), std::move(payload), std::move(options));
        CHECK(err.noError());

        testContext.client->tick();

        CHECK(testContext.socketPtr->sentPackets.size() == 1);

        std::this_thread::sleep_for(std::chrono::milliseconds(150));

        testContext.client->tick(); //Process pending retries into queue for sending
        testContext.client->tick(); //Send pending retries from queue

        CHECK(testContext.socketPtr->sentPackets.size() == 2);

        std::vector<ByteBuffer> packets;
        std::size_t leftOver;
        CHECK(separateMqttPacketByteBuffers(testContext.socketPtr->sentPackets[1], packets, leftOver) == true);
        REQUIRE(leftOver == 0);
        REQUIRE(packets.size() == 1);
    }

    TEST_CASE("Retry publish has duplicate flag set")
    {
        Config config;
        config.retryPublishIntervalMS = 50;
        config.pingAlways = false;

        TestClientContext testContext{ config };
        CHECK(testContext.tryConnectWithResponse().noError());

        testContext.socketPtr->sentPackets.clear();

        std::string topic = "test/dup";
        ByteBuffer payload(1);
        payload += 0xAA;

        PublishOptions options;
        options.qos = Qos::QOS_1;

        testContext.client->publish(topic.c_str(), std::move(payload), std::move(options));
        testContext.client->tick();

        ByteBuffer firstPacket = testContext.socketPtr->sentPackets[0];
        std::uint8_t firstByte = firstPacket.readUint8();
        std::uint8_t firstFlags = firstByte & 0x0F;
        bool firstDupFlag = (firstFlags & 0x08) != 0;

        CHECK(firstDupFlag == false);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        testContext.client->tick(); //Process pending retries into queue for sending
        testContext.client->tick(); //Send pending retries from queue

        CHECK(testContext.socketPtr->sentPackets.size() == 2);

        ByteBuffer retryPacket = testContext.socketPtr->sentPackets[1];
        std::uint8_t retryByte = retryPacket.readUint8();
        std::uint8_t retryFlags = retryByte & 0x0F;
        bool retryDupFlag = (retryFlags & 0x08) != 0;

        CHECK(retryDupFlag == true);

        std::vector<ByteBuffer> packets;
        std::size_t leftOver;
        CHECK(separateMqttPacketByteBuffers(retryPacket, packets, leftOver) == true);
        REQUIRE(leftOver == 0);
        REQUIRE(packets.size() == 1);
    }

    TEST_CASE("QOS 2 PUBREC retry after timeout")
    {
        Config config;
        config.retryPublishIntervalMS = 100;
        config.pingAlways = false;

        TestClientContext testContext{ config };
        CHECK(testContext.tryConnectWithResponse().noError());

        std::string topic = "test/qos2retry";
        ByteBuffer payload(3);
        payload += 0x01;
        payload += 0x02;
        payload += 0x03;

        PublishOptions options;
        options.qos = Qos::QOS_2;

        testContext.client->publish(topic.c_str(), std::move(payload), std::move(options));
        testContext.client->tick();

        ByteBuffer pubRecBuffer(4);
        pubRecBuffer += 0x50;
        pubRecBuffer += 0x02;
        pubRecBuffer += 0x00;
        pubRecBuffer += 0x01;

        testContext.receiveResponse(pubRecBuffer);

        testContext.socketPtr->sentPackets.clear();

        std::this_thread::sleep_for(std::chrono::milliseconds(150));
        testContext.client->tick(); //Process pending retries into queue for sending
        testContext.client->tick(); //Send pending retries from queue

        CHECK(testContext.socketPtr->sentPackets.size() >= 1);

        std::vector<ByteBuffer> packets;
        std::size_t leftOver;
        CHECK(separateMqttPacketByteBuffers(testContext.socketPtr->sentPackets[0], packets, leftOver) == true);
        REQUIRE(leftOver == 0);
        REQUIRE(packets.size() == 1);
    }

    TEST_CASE("Multiple messages retry in order")
    {
        Config config;
        config.retryPublishIntervalMS = 80;
        config.pingAlways = false;

        TestClientContext testContext{ config };
        CHECK(testContext.tryConnectWithResponse().noError());

        testContext.socketPtr->sentPackets.clear();

        for (int i = 0; i < 3; ++i)
        {
            std::string topic = "test/multi/" + std::to_string(i);
            ByteBuffer payload(1);
            payload += static_cast<std::uint8_t>(i);

            PublishOptions options;
            options.qos = Qos::QOS_1;

            testContext.client->publish(topic.c_str(), std::move(payload), std::move(options));
        }

        testContext.client->tick();
        CHECK(testContext.socketPtr->sentPackets.size() == 1);

        testContext.socketPtr->sentPackets.clear();

        std::this_thread::sleep_for(std::chrono::milliseconds(120));
        testContext.client->tick(); //Process pending retries into queue for sending
        testContext.client->tick(); //Send pending retries from queue

        CHECK(testContext.socketPtr->sentPackets.size() == 1);
        std::vector<ByteBuffer> packets;
        std::size_t leftOver;
        CHECK(separateMqttPacketByteBuffers(testContext.socketPtr->sentPackets[0], packets, leftOver) == true);
        REQUIRE(leftOver == 0);
        REQUIRE(packets.size() == 3);
    }

    TEST_CASE("Retry stops after PUBACK received")
    {
        Config config;
        config.retryPublishIntervalMS = 50;
        config.pingAlways = false;

        TestClientContext testContext{ config };
        CHECK(testContext.tryConnectWithResponse().noError());

        testContext.socketPtr->sentPackets.clear();

        std::string topic = "test/stopretry";
        ByteBuffer payload(1);
        payload += 0xFF;

        PublishOptions options;
        options.qos = Qos::QOS_1;

        testContext.client->publish(topic.c_str(), std::move(payload), std::move(options));
        testContext.client->tick();

        CHECK(testContext.socketPtr->sentPackets.size() == 1);

        ByteBuffer pubAckBuffer(4);
        pubAckBuffer += 0x40;
        pubAckBuffer += 0x02;
        pubAckBuffer += 0x00;
        pubAckBuffer += 0x01;

        testContext.receiveResponse(pubAckBuffer);

        testContext.socketPtr->sentPackets.clear();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        testContext.client->tick(); //Process pending retries into queue for sending
        testContext.client->tick(); //Send pending retries from queue

        CHECK(testContext.socketPtr->sentPackets.size() == 0);
    }

    TEST_CASE("Session state preserves message after retry")
    {
        Config config;
        config.retryPublishIntervalMS = 60;
        config.pingAlways = false;

        TestClientContext testContext{ config };
        CHECK(testContext.tryConnectWithResponse().noError());

        std::string topic = "test/session";
        ByteBuffer payload(2);
        payload += 0xDE;
        payload += 0xAD;

        PublishOptions options;
        options.qos = Qos::QOS_1;

        testContext.client->publish(topic.c_str(), std::move(payload), std::move(options));
        testContext.client->tick();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        testContext.client->tick(); //Process pending retries into queue for sending
        testContext.client->tick(); //Send pending retries from queue

        ByteBuffer pubAckBuffer(4);
        pubAckBuffer += 0x40;
        pubAckBuffer += 0x02;
        pubAckBuffer += 0x00;
        pubAckBuffer += 0x01;

        testContext.receiveResponse(pubAckBuffer);

        std::vector<ByteBuffer> packets;
        std::size_t leftOver;
        CHECK(separateMqttPacketByteBuffers(testContext.socketPtr->sentPackets[0], packets, leftOver) == true);
        REQUIRE(leftOver == 0);
        REQUIRE(packets.size() == 1);
    }

    TEST_CASE("QOS 2 full flow with retries")
    {
        Config config;
        config.retryPublishIntervalMS = 70;
        config.pingAlways = false;

        TestClientContext testContext{ config };
        CHECK(testContext.tryConnectWithResponse().noError());

        testContext.socketPtr->sentPackets.clear();

        std::string topic = "test/qos2full";
        ByteBuffer payload(1);
        payload += 0x42;

        PublishOptions options;
        options.qos = Qos::QOS_2;

        testContext.client->publish(topic.c_str(), std::move(payload), std::move(options));
        testContext.client->tick();

        CHECK(testContext.socketPtr->sentPackets.size() == 1);

        ByteBuffer pubRecBuffer(4);
        pubRecBuffer += 0x50;
        pubRecBuffer += 0x02;
        pubRecBuffer += 0x00;
        pubRecBuffer += 0x01;

        testContext.receiveResponse(pubRecBuffer);
        testContext.client->tick();

        testContext.socketPtr->sentPackets.clear();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        testContext.client->tick(); //Process pending retries into queue for sending
        testContext.client->tick(); //Send pending retries from queue

        CHECK(testContext.socketPtr->sentPackets.size() >= 1);

        ByteBuffer pubCompBuffer(4);
        pubCompBuffer += 0x70;
        pubCompBuffer += 0x02;
        pubCompBuffer += 0x00;
        pubCompBuffer += 0x01;

        testContext.receiveResponse(pubCompBuffer);

        testContext.socketPtr->sentPackets.clear();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        testContext.client->tick();

        CHECK(testContext.socketPtr->sentPackets.size() == 0);

        std::vector<ByteBuffer> packets;
        std::size_t leftOver;
        CHECK(separateMqttPacketByteBuffers(pubRecBuffer, packets, leftOver) == true);
        REQUIRE(leftOver == 0);
        REQUIRE(packets.size() == 1);
    }

    TEST_CASE("No retry for QOS 0 messages")
    {
        Config config;
        config.retryPublishIntervalMS = 50;
        config.pingAlways = false;

        TestClientContext testContext{ config };
        CHECK(testContext.tryConnectWithResponse().noError());

        testContext.socketPtr->sentPackets.clear();

        std::string topic = "test/qos0noretry";
        ByteBuffer payload(1);
        payload += 0x01;

        PublishOptions options;
        options.qos = Qos::QOS_0;

        testContext.client->publish(topic.c_str(), std::move(payload), std::move(options));
        testContext.client->tick();

        CHECK(testContext.socketPtr->sentPackets.size() == 1);

        testContext.socketPtr->sentPackets.clear();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        testContext.client->tick(); //Process pending retries into queue for sending
        testContext.client->tick(); //Send pending retries from queue

        CHECK(testContext.socketPtr->sentPackets.size() == 0);
    }

    TEST_CASE("Retry interval respected between attempts")
    {
        Config config;
        config.retryPublishIntervalMS = 100;
        config.pingAlways = false;

        TestClientContext testContext{ config };
        CHECK(testContext.tryConnectWithResponse().noError());

        testContext.socketPtr->sentPackets.clear();

        std::string topic = "test/interval";
        ByteBuffer payload(1);
        payload += 0xBB;

        PublishOptions options;
        options.qos = Qos::QOS_1;

        testContext.client->publish(topic.c_str(), std::move(payload), std::move(options));
        testContext.client->tick();

        std::size_t initialCount = testContext.socketPtr->sentPackets.size();

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        testContext.client->tick(); //Process pending retries into queue for sending
        testContext.client->tick(); //Send pending retries from queue

        CHECK(testContext.socketPtr->sentPackets.size() == initialCount);

        std::this_thread::sleep_for(std::chrono::milliseconds(80));
        testContext.client->tick(); //Process pending retries into queue for sending
        testContext.client->tick(); //Send pending retries from queue

        CHECK(testContext.socketPtr->sentPackets.size() > initialCount);

        std::vector<ByteBuffer> packets;
        std::size_t leftOver;
        CHECK(separateMqttPacketByteBuffers(testContext.socketPtr->sentPackets[0], packets, leftOver) == true);
        REQUIRE(leftOver == 0);
        REQUIRE(packets.size() == 1);
    }
}