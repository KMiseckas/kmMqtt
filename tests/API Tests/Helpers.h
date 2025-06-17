#ifndef APITESTS_HELPERS_H
#define APITESTS_HELPERS_H

#include <doctest.h>
#include <memory>
#include "MockWebSocket.h"
#include <cleanMqtt/Mqtt/MqttClient.h>

// Helper to create a connected MqttClient with a mock socket
struct TestClientContext 
{
    TestClientContext(const cleanMqtt::Config& config = {}, bool socketConnectResult = true)
    {
        auto mockSocket{ std::make_unique<MockWebSocket>() };

		socketPtr = mockSocket.get();
        socketPtr->connectResult = socketConnectResult;

        client = new cleanMqtt::mqtt::MqttClient{ config, std::move(mockSocket) };
    }

    ~TestClientContext()
    {
        if (client)
        {
            delete client;
        }
	}

    void receiveResponse(const cleanMqtt::ByteBuffer& data)
    {
		socketPtr->queueMockResponse(data);
		client->tick(.0f); //Initial tick to process the response on socket layer and move mqtt layer
		client->tick(.0f); //Second tick to process queue of received messages in the mqtt layer
    }

    static cleanMqtt::mqtt::ConnectArgs getDefaultConnectArgs()
    {
        cleanMqtt::mqtt::ConnectArgs args{ "Default_ID" };
        args.protocolName = "MQTT";
        args.version = cleanMqtt::mqtt::MqttVersion::MQTT_5_0;
		args.cleanStart = true;
        args.keepAliveInSec = 60;

        return args;
    }

    static cleanMqtt::mqtt::ConnectAddress getDefaultConnectAddress()
    {
        cleanMqtt::mqtt::ConnectAddress address;
        address.primaryAddress = cleanMqtt::mqtt::Address::createURL("localhost", "1883");
        return address;
	}

    cleanMqtt::mqtt::ClientError tryConnect(cleanMqtt::mqtt::ClientErrorCode expectedErrorCode = cleanMqtt::mqtt::ClientErrorCode::No_Error,
        cleanMqtt::mqtt::ConnectArgs&& args = getDefaultConnectArgs(),
        cleanMqtt::mqtt::ConnectAddress&& address = getDefaultConnectAddress())
    {
        auto err{ client->connect(std::move(args), std::move(address)) };
        CHECK(err.errorCode == expectedErrorCode);

        return err;
    }

    MockWebSocket* socketPtr;
    cleanMqtt::mqtt::MqttClient* client;
};

#endif // APITESTS_HELPERS_H
