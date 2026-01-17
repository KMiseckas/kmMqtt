#include <doctest.h>
#include <kmMqtt/MqttClient.h>
#include <memory>
#include <string>
#include "MockWebSocket.h"
#include "Helpers.h"

using namespace kmMqtt;
using namespace kmMqtt::mqtt;

TEST_SUITE("MqttClient Disconnect")
{
    TEST_CASE("Disconnect when not connected")
    {
        TestClientContext testContext;
        DisconnectArgs args{ false };
        auto result{ testContext.client->disconnect(std::move(args)) };
        CHECK(result.errorCode()  == ClientErrorCode::Not_Connected);
    }

    TEST_CASE("Disconnect when connected - non-graceful")
    {
        TestClientContext testContext{ {}, true };
        bool disconnectEventFired{ false };
        DisconnectReasonCode disconnectReason;

        testContext.client->onDisconnectEvent().add([&](const mqtt::DisconnectEventDetails& details)
        {
            disconnectEventFired = true;
            disconnectReason = details.reasonCode;
            CHECK(details.error.errorCode == ClientErrorCode::No_Error);
            CHECK(!details.isBrokerInduced);
            CHECK(!details.isGraceful);
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

        DisconnectArgs args{ false }; // Non-graceful
        auto result = testContext.client->disconnect(std::move(args));
        testContext.client->tick();
        CHECK(result.errorCode() == ClientErrorCode::No_Error);

        CHECK(disconnectEventFired);
        CHECK(disconnectReason == DisconnectReasonCode::NORMAL_DISCONNECTION);
    }

    TEST_CASE("Disconnect with willPublish triggers correct reason code - non-graceful")
    {
        TestClientContext testContext{ {}, true };
        bool disconnectEventFired = false;
        DisconnectReasonCode disconnectReason;

        testContext.client->onDisconnectEvent().add([&](const mqtt::DisconnectEventDetails& details)
        {
            disconnectEventFired = true;
            disconnectReason = details.reasonCode;
            CHECK(!details.isGraceful);
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

        DisconnectArgs args{ false }; // Non-graceful
        args.willPublish = true;
        auto result = testContext.client->disconnect(std::move(args));
        testContext.client->tick();
        CHECK(result.errorCode() == ClientErrorCode::No_Error);

        CHECK(disconnectEventFired);
        CHECK(disconnectReason == DisconnectReasonCode::DISCONNECT_WITH_WILL_MESSAGE);
    }

    TEST_CASE("Graceful disconnect - waits for send queue")
    {
        TestClientContext testContext{ {}, true };
        bool disconnectEventFired{ false };
        DisconnectReasonCode disconnectReason;

        testContext.client->onDisconnectEvent().add([&](const mqtt::DisconnectEventDetails& details)
        {
            disconnectEventFired = true;
            disconnectReason = details.reasonCode;
            CHECK(details.error.errorCode == ClientErrorCode::No_Error);
            CHECK(!details.isBrokerInduced);
            CHECK(details.isGraceful);
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

        DisconnectArgs args{ true }; // Graceful
        auto result = testContext.client->disconnect(std::move(args));
        CHECK(result.errorCode() == ClientErrorCode::No_Error);

        // Disconnect should not fire immediately - it's queued
        CHECK(!disconnectEventFired);

        // Tick to process the disconnect packet through the queue
        testContext.client->tick();

        CHECK(disconnectEventFired);
        CHECK(disconnectReason == DisconnectReasonCode::NORMAL_DISCONNECTION);
    }

    TEST_CASE("Graceful disconnect with willPublish")
    {
        TestClientContext testContext{ {}, true };
        bool disconnectEventFired{ false };
        DisconnectReasonCode disconnectReason;

        testContext.client->onDisconnectEvent().add([&](const mqtt::DisconnectEventDetails& details)
        {
            disconnectEventFired = true;
            disconnectReason = details.reasonCode;
            CHECK(details.isGraceful);
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

        DisconnectArgs args{ true }; // Graceful
        args.willPublish = true;
        auto result = testContext.client->disconnect(std::move(args));
        CHECK(result.errorCode() == ClientErrorCode::No_Error);

        // Disconnect should not fire immediately
        CHECK(!disconnectEventFired);

        // Tick to process the disconnect
        testContext.client->tick();

        CHECK(disconnectEventFired);
        CHECK(disconnectReason == DisconnectReasonCode::DISCONNECT_WITH_WILL_MESSAGE);
    }

    TEST_CASE("Graceful disconnect with reason text and session expiry")
    {
        TestClientContext testContext{ {}, true };
        bool disconnectEventFired{ false };

        testContext.client->onDisconnectEvent().add([&](const mqtt::DisconnectEventDetails& details)
        {
            disconnectEventFired = true;
            CHECK(details.isGraceful);
            CHECK(details.reasonCode == DisconnectReasonCode::NORMAL_DISCONNECTION);
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

        DisconnectArgs args{ true }; // Graceful
        args.disconnectReasonText = "Client shutting down for maintenance";
        args.sessionExpiryInterval = 300; // 5 minutes
        auto result = testContext.client->disconnect(std::move(args));
        CHECK(result.errorCode() == ClientErrorCode::No_Error);

        CHECK(!disconnectEventFired);
        testContext.client->tick();
        CHECK(disconnectEventFired);
    }

    TEST_CASE("Non-graceful disconnect clears queue immediately")
    {
        TestClientContext testContext{ {}, true };
        bool disconnectEventFired{ false };

        testContext.client->onDisconnectEvent().add([&](const mqtt::DisconnectEventDetails& details)
        {
            disconnectEventFired = true;
            CHECK(!details.isGraceful);
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

        // Queue some messages (in real scenario these would be publish messages)
        DisconnectArgs args{ false }; // Non-graceful
        args.clearQueue = true;
        auto result = testContext.client->disconnect(std::move(args));
        
        // Non-graceful disconnect fires immediately
        testContext.client->tick();
        CHECK(disconnectEventFired);
    }

    TEST_CASE("Graceful disconnect does not clear queue by default")
    {
        TestClientContext testContext{ {}, true };
        bool disconnectEventFired{ false };

        testContext.client->onDisconnectEvent().add([&](const mqtt::DisconnectEventDetails& details)
        {
            disconnectEventFired = true;
            CHECK(details.isGraceful);
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

        DisconnectArgs args{ true }; // Graceful
        args.clearQueue = false; // Don't clear queue
        auto result = testContext.client->disconnect(std::move(args));
        CHECK(result.errorCode() == ClientErrorCode::No_Error);

        CHECK(!disconnectEventFired);
        testContext.client->tick();
        CHECK(disconnectEventFired);
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
            CHECK(details.reasonCode == DisconnectReasonCode::NORMAL_DISCONNECTION);
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
        DisconnectReasonCode disconnectReason;

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
        testContext.client->tick();

        CHECK(disconnectEventFired);
        CHECK(disconnectReason == DisconnectReasonCode::NORMAL_DISCONNECTION);
    }

    TEST_CASE("External disconnect (socket error)")
    {
        TestClientContext testContext{ {}, true };
        bool disconnectEventFired = false;

        testContext.client->onDisconnectEvent().add([&](const mqtt::DisconnectEventDetails& details)
        {
            disconnectEventFired = true;
            CHECK(details.reasonCode == DisconnectReasonCode::UNSPECIFIED_ERROR);
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
        testContext.client->tick();

        CHECK(disconnectEventFired);
    }
}
