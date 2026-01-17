// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef APITESTS_HELPERS_H
#define APITESTS_HELPERS_H

#include <doctest.h>
#include <memory>
#include "MockWebSocket.h"
#include <kmMqtt/MqttClient.h>
#include "Environments/TestEnvironment.h"

// Helper to create a connected MqttClient with a mock socket
struct TestClientContext 
{
    TestClientContext(const kmMqtt::Config& config = {}, bool socketConnectResult = true)
    {
        MqttClientOptions options{ kmMqtt::TickMode::SYNC };

        auto env{ TestEnvironment() };
        env.config = config;

        client = new kmMqtt::mqtt::MqttClient(&env, options);

        socketPtr = env.socketPtr;
        socketPtr->connectResult = socketConnectResult;
    }

    ~TestClientContext()
    {
        if (client)
        {
            delete client;
        }
	}

    void receiveResponse(const kmMqtt::ByteBuffer& data)
    {
		socketPtr->queueMockResponse(data);
		client->tick(); //Initial tick to process the response on socket layer and move mqtt layer
		client->tick(); //Second tick to process queue of received messages in the mqtt layer
    }

    static kmMqtt::mqtt::ConnectArgs getDefaultConnectArgs()
    {
        kmMqtt::mqtt::ConnectArgs args{ "Default_ID" };
        args.protocolName = "MQTT";
        args.version = kmMqtt::mqtt::MqttVersion::MQTT_5_0;
		args.cleanStart = true;
        args.keepAliveInSec = 60;

        return args;
    }

    static kmMqtt::mqtt::ConnectAddress getDefaultConnectAddress()
    {
        kmMqtt::mqtt::ConnectAddress address;
        address.primaryAddress = kmMqtt::mqtt::Address::createURL("", "localhost", "1883", "");
        return address;
	}

    /**
     * Try connect client with different args and user expected outcome, does not automatically try to receive the response unless `includeSuccessResponse` is set to true and `No_Error`
     * is received from the `connect()` result.
     */
    kmMqtt::mqtt::ClientError tryConnect(kmMqtt::mqtt::ClientErrorCode expectedErrorCode = kmMqtt::mqtt::ClientErrorCode::No_Error,
        kmMqtt::mqtt::ConnectArgs&& args = getDefaultConnectArgs(),
        kmMqtt::mqtt::ConnectAddress&& address = getDefaultConnectAddress(),
        bool includeSuccessResponse = false)
    {
        auto result{ client->connect(std::move(args), std::move(address)) };
        CHECK(result.errorCode() == expectedErrorCode);

        if (result.errorCode() == mqtt::ClientErrorCode::No_Error)
        {
            if (includeSuccessResponse)
            {
                ByteBuffer ackBuffer(5);
                ackBuffer += 32;    //Type
                ackBuffer += 3;     //Remaining Length
                ackBuffer += 0;     //Flag
                ackBuffer += 0;     //Reason
                ackBuffer += 0;     //Property Length

                receiveResponse(ackBuffer);
            }
        }

        return result.error;
    }

    /**
     * Try connect the client and if no error then also try get a connect ack response to put client in a fully connected state.
     */
    kmMqtt::mqtt::ClientError tryConnectWithResponse(kmMqtt::mqtt::ConnectArgs&& args = getDefaultConnectArgs(),
        kmMqtt::mqtt::ConnectAddress&& address = getDefaultConnectAddress())
    {
        auto result{ client->connect(std::move(args), std::move(address)) };
        CHECK(result.errorCode() == mqtt::ClientErrorCode::No_Error);

        if (result.errorCode() == mqtt::ClientErrorCode::No_Error)
        {
			ByteBuffer ackBuffer(5);
			ackBuffer += 32;    //Type
			ackBuffer += 3;     //Remaining Length
			ackBuffer += 0;     //Flag
			ackBuffer += 0;     //Reason
			ackBuffer += 0;     //Property Length

			receiveResponse(ackBuffer);
        }

        return result.error;
    }

    MockWebSocket* socketPtr;
    kmMqtt::mqtt::MqttClient* client;
};

#endif // APITESTS_HELPERS_H
