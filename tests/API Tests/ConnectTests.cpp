#include <doctest.h>
#include <cleanMqtt/Mqtt/MqttClient.h>
#include <memory>
#include <string>
#include "MockWebSocket.h"
#include "Helpers.h"

using namespace cleanMqtt;
using namespace cleanMqtt::mqtt;

TEST_SUITE("MqttClient Connect")
{
    TEST_CASE("Successful Connect")
    {
        TestClientContext testContext;
        CHECK(testContext.tryConnect().noError());
        CHECK(testContext.socketPtr->connectCalled);
        CHECK(testContext.socketPtr->lastHost == "localhost");
        CHECK(testContext.socketPtr->lastPort == "1883");
    }

    TEST_CASE("Empty Hostname")
    {
        TestClientContext testContext;
		auto address = Address::createURL("", "1883");
        testContext.tryConnect(ClientErrorCode::Missing_Argument, testContext.getDefaultConnectArgs(), std::move(address));
    }

    TEST_CASE("Empty ClientId")
    {
        TestClientContext testContext;
        auto args{ testContext.getDefaultConnectArgs() };
        args.clientId.clear();
        testContext.tryConnect(ClientErrorCode::Missing_Argument, std::move(args));
    }

    TEST_CASE("Socket Connect Fail")
    {
        TestClientContext testContext{ {}, false };
        testContext.tryConnect(ClientErrorCode::Socket_Connect_Failed);
    }

    TEST_CASE("Connect Event Success and Failure")
    {
        TestClientContext testContext{ {}, true };

        bool connectEventFired = false;
        bool connectEventSuccess = false;
        int connectEventError = -1;

        testContext.client->onConnectEvent().add([&](bool success, int error, const mqtt::packets::ConnectAck&)
            {
                connectEventFired = true;
                connectEventSuccess = success;
                connectEventError = error;
            });

        testContext.tryConnect();

        ByteBuffer ackBuffer(5);
        ackBuffer += 32;    //Type
        ackBuffer += 3;     //Remaining Length
        ackBuffer += 0;     //Flag
        ackBuffer += 0;     //Reason
		ackBuffer += 0;     //Property Length

        testContext.receiveResponse(ackBuffer);

        CHECK(connectEventFired);
        CHECK(connectEventSuccess);
        CHECK(connectEventError == 0);
    }

    TEST_CASE("Connect Event Failure")
    {
        Config config;
        auto mockSocket = std::make_unique<MockWebSocket>();
        auto* socketPtr = mockSocket.get();
        socketPtr->connectResult = false;
        MqttClient client(config, std::move(mockSocket));

        bool connectEventFired = false;
        bool connectEventSuccess = true;
        int connectEventError = -1;
        client.onConnectEvent().add([&](bool success, int error, const mqtt::packets::ConnectAck&) {
            connectEventFired = true;
            connectEventSuccess = success;
            connectEventError = error;
            });

        ConnectArgs args{ "NAME" };
        args.protocolName = "MQTT";
        args.version = MqttVersion::MQTT_5_0;
        ConnectAddress address;
        address.primaryAddress = Address::createURL("localhost", "1883");

        auto err = client.connect(std::move(args), std::move(address));
        CHECK(err.errorCode == ClientErrorCode::Socket_Connect_Failed);
        // Simulate socket connect event (fail)
        socketPtr->onConnectCb(false);
        client.tick(0.0f);
        // The event should be fired with success == false
        CHECK_FALSE(connectEventSuccess);
    }
}