#ifndef APITESTS_HELPERS_H
#define APITESTS_HELPERS_H

#include <doctest.h>
#include <memory>
#include "MockWebSocket.h"
#include <cleanMqtt/MqttClient.h>
#include "Environments/TestEnvironment.h"

// Helper to create a connected MqttClient with a mock socket
struct TestClientContext 
{
    TestClientContext(const cleanMqtt::Config& config = {}, bool socketConnectResult = true)
    {
        MqttClientOptions options;
        options.setTickAsync(false, nullptr);
        auto env{ TestEnvironment() };
        client = new cleanMqtt::mqtt::MqttClient(&env, options);

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

    void receiveResponse(const cleanMqtt::ByteBuffer& data)
    {
		socketPtr->queueMockResponse(data);
		client->tick(); //Initial tick to process the response on socket layer and move mqtt layer
		client->tick(); //Second tick to process queue of received messages in the mqtt layer
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

    /**
     * Try connect client with different args and user expected outcome, does not automatically try to receive the response unless `includeSuccessResponse` is set to true and `No_Error`
     * is received from the `connect()` result.
     */
    cleanMqtt::mqtt::ClientError tryConnect(cleanMqtt::mqtt::ClientErrorCode expectedErrorCode = cleanMqtt::mqtt::ClientErrorCode::No_Error,
        cleanMqtt::mqtt::ConnectArgs&& args = getDefaultConnectArgs(),
        cleanMqtt::mqtt::ConnectAddress&& address = getDefaultConnectAddress(),
        bool includeSuccessResponse = false)
    {
        auto err{ client->connect(std::move(args), std::move(address)) };
        CHECK(err.errorCode == expectedErrorCode);

        if (err.errorCode == mqtt::ClientErrorCode::No_Error)
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

        return err;
    }

    /**
     * Try connect the client and if no error then also try get a connect ack response to put client in a fully connected state.
     */
    cleanMqtt::mqtt::ClientError tryConnectWithResponse(cleanMqtt::mqtt::ConnectArgs&& args = getDefaultConnectArgs(),
        cleanMqtt::mqtt::ConnectAddress&& address = getDefaultConnectAddress())
    {
        auto err{ client->connect(std::move(args), std::move(address)) };
        CHECK(err.errorCode == mqtt::ClientErrorCode::No_Error);

        if (err.errorCode == mqtt::ClientErrorCode::No_Error)
        {
			ByteBuffer ackBuffer(5);
			ackBuffer += 32;    //Type
			ackBuffer += 3;     //Remaining Length
			ackBuffer += 0;     //Flag
			ackBuffer += 0;     //Reason
			ackBuffer += 0;     //Property Length

			receiveResponse(ackBuffer);
        }

        return err;
    }

    MockWebSocket* socketPtr;
    cleanMqtt::mqtt::MqttClient* client;
};

#endif // APITESTS_HELPERS_H
