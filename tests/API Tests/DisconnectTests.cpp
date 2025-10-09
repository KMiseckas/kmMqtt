#include <doctest.h>
#include <cleanMqtt/MqttClient.h>
#include <memory>
#include <string>
#include "MockWebSocket.h"
#include "Helpers.h"

using namespace cleanMqtt;
using namespace cleanMqtt::mqtt;

TEST_SUITE("MqttClient Disconnect")
{
    TEST_CASE("Disconnect when not connected")
    {
        TestClientContext testContext;
        auto err{ testContext.client->disconnect() };
        CHECK(err.errorCode == ClientErrorCode::Not_Connected);
    }

    TEST_CASE("Disconnect when connected")
    {
        TestClientContext testContext{ {}, true };
        bool disconnectEventFired{ false };
        packets::DisconnectReasonCode disconnectReason;

        testContext.client->onDisconnectEvent().add([&](const mqtt::DisconnectEventDetails& details)
        {
            disconnectEventFired = true;
            disconnectReason = details.reasonCode;
            CHECK(details.error.errorCode == ClientErrorCode::No_Error);
            CHECK(!details.isBrokerInduced);
        });

        testContext.tryConnect();
        // Simulate successful connect
        ByteBuffer ackBuffer(5);
        ackBuffer += 32;    //Type
        ackBuffer += 3;     //Remaining Length
        ackBuffer += 0;     //Flag
        ackBuffer += 0;     //Reason
        ackBuffer += 0;     //Property Length
        testContext.receiveResponse(ackBuffer);

        auto err = testContext.client->disconnect();
        testContext.client->tick(.1f);
        CHECK(err.errorCode == ClientErrorCode::No_Error);

        CHECK(disconnectEventFired);
        CHECK(disconnectReason == packets::DisconnectReasonCode::NORMAL_DISCONNECTION);
    }

    TEST_CASE("Disconnect with willPublish triggers correct reason code")
    {
        TestClientContext testContext{ {}, true };
        bool disconnectEventFired = false;
        packets::DisconnectReasonCode disconnectReason;

        testContext.client->onDisconnectEvent().add([&](const mqtt::DisconnectEventDetails& details)
        {
            disconnectEventFired = true;
            disconnectReason = details.reasonCode;
        });

        testContext.tryConnect();
        // Simulate successful connect
        ByteBuffer ackBuffer(5);
        ackBuffer += 32;    //Type
        ackBuffer += 3;     //Remaining Length
        ackBuffer += 0;     //Flag
        ackBuffer += 0;     //Reason
        ackBuffer += 0;     //Property Length
        testContext.receiveResponse(ackBuffer);

        DisconnectArgs args;
        args.willPublish = true;
        auto err = testContext.client->disconnect(std::move(args));
        testContext.client->tick(.1f);
        CHECK(err.errorCode == ClientErrorCode::No_Error);

        CHECK(disconnectEventFired);
        CHECK(disconnectReason == packets::DisconnectReasonCode::DISCONNECT_WITH_WILL_MESSAGE);
    }

    TEST_CASE("Shutdown when not connected returns MQTT_Not_Active")
    {
        TestClientContext testContext;
        auto err = testContext.client->shutdown();
        CHECK(err.errorCode == ClientErrorCode::MQTT_Not_Active);
    }

    TEST_CASE("Shutdown when connected")
    {
        TestClientContext testContext{ {}, true };
        bool disconnectEventFired = false;

        testContext.client->onDisconnectEvent().add([&](const mqtt::DisconnectEventDetails& details)
        {
            disconnectEventFired = true;
            CHECK(details.reasonCode == packets::DisconnectReasonCode::NORMAL_DISCONNECTION);
        });

        testContext.tryConnect();
        // Simulate successful connect
        ByteBuffer ackBuffer(5);
        ackBuffer += 32;    //Type
        ackBuffer += 3;     //Remaining Length
        ackBuffer += 0;     //Flag
        ackBuffer += 0;     //Reason
        ackBuffer += 0;     //Property Length
        testContext.receiveResponse(ackBuffer);

        auto err = testContext.client->shutdown();
        CHECK(err.errorCode == ClientErrorCode::No_Error);

        CHECK(disconnectEventFired);
    }

    TEST_CASE("External disconnect (broker sends Disconnect packet)")
    {
        TestClientContext testContext{ {}, true };
        bool disconnectEventFired = false;
        packets::DisconnectReasonCode disconnectReason;

        testContext.client->onDisconnectEvent().add([&](const mqtt::DisconnectEventDetails& details)
        {
            disconnectEventFired = true;
            disconnectReason = details.reasonCode;
            CHECK(details.isBrokerInduced);
        });

        testContext.tryConnect();
        // Simulate successful connect
        ByteBuffer ackBuffer(5);
        ackBuffer += 32;    //Type
        ackBuffer += 3;     //Remaining Length
        ackBuffer += 0;     //Flag
        ackBuffer += 0;     //Reason
        ackBuffer += 0;     //Property Length
        testContext.receiveResponse(ackBuffer);

        // Simulate broker sending Disconnect packet
        ByteBuffer disconnectBuffer(5);
        disconnectBuffer += 224; //Type (0xE0)
        disconnectBuffer += 3;   //Remaining Length
        disconnectBuffer += 0;   //Flag
        disconnectBuffer += 0;   //Reason (NORMAL_DISCONNECTION)
        disconnectBuffer += 0;   //Property Length
        testContext.receiveResponse(disconnectBuffer);
        testContext.client->tick(.1f);

        CHECK(disconnectEventFired);
        CHECK(disconnectReason == packets::DisconnectReasonCode::NORMAL_DISCONNECTION);
    }

    TEST_CASE("External disconnect (socket error)")
    {
        TestClientContext testContext{ {}, true };
        bool disconnectEventFired = false;

        testContext.client->onDisconnectEvent().add([&](const mqtt::DisconnectEventDetails& details)
        {
            disconnectEventFired = true;
            CHECK(details.reasonCode == packets::DisconnectReasonCode::UNSPECIFIED_ERROR);
            CHECK(details.error.errorCode == ClientErrorCode::Socket_Error);
        });

        testContext.tryConnect();
        // Simulate successful connect
        ByteBuffer ackBuffer(5);
        ackBuffer += 32;    //Type
        ackBuffer += 3;     //Remaining Length
        ackBuffer += 0;     //Flag
        ackBuffer += 0;     //Reason
        ackBuffer += 0;     //Property Length
        testContext.receiveResponse(ackBuffer);

        // Simulate socket error
		testContext.socketPtr->lastCloseCode = 1001; // Normal closure
		testContext.socketPtr->lastCloseReason = "Socket closed";
        testContext.socketPtr->close();
        testContext.client->tick(.1f);

        CHECK(disconnectEventFired);
    }

    
}
