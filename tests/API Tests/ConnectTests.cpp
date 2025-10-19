#include <doctest.h>
#include <cleanMqtt/MqttClient.h>
#include <memory>
#include <string>
#include "MockWebSocket.h"
#include "Helpers.h"
#include <cleanMqtt/Mqtt/Transport/SendResultData.h>

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

    TEST_CASE("Connect Event Success")
    {
        TestClientContext testContext{ {}, true };

        bool connectEventFired = false;
        bool connectEventSuccess = false;
        mqtt::ClientError connectEventError = { mqtt::ClientErrorCode::No_Error };
		bool hasReceivedConnectAck = false;

        testContext.client->onConnectEvent().add([&](const mqtt::ConnectEventDetails& details, const mqtt::ConnectAck&)
            {
                connectEventFired = true;
                connectEventSuccess = details.isSuccessful;
                connectEventError = details.error;
                hasReceivedConnectAck = details.hasReceivedAck;
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
        CHECK(connectEventError.noError());
        CHECK(hasReceivedConnectAck);
    }

    TEST_CASE("Connect Event  - Missing property length")
    {
        TestClientContext testContext{ {}, true };

        bool connectEventFired = false;
        bool connectEventSuccess = false;
        mqtt::ClientError connectEventError = { mqtt::ClientErrorCode::No_Error };
        bool hasReceivedConnectAck = false;

        testContext.client->onConnectEvent().add([&](const mqtt::ConnectEventDetails& details, const mqtt::ConnectAck&)
            {
                connectEventFired = true;
                connectEventSuccess = details.isSuccessful;
                connectEventError = details.error;
                hasReceivedConnectAck = details.hasReceivedAck;
            });


        ClientError cError;
        mqtt::SendResultData rData;
        testContext.client->onErrorEvent().add([&](ClientError error, mqtt::SendResultData data)
            {
                cError = error;
                rData = data;
            });

        testContext.tryConnect();

        ByteBuffer ackBuffer(4);
        ackBuffer += 32;    //Type
        ackBuffer += 2;     //Remaining Length
        ackBuffer += 0;     //Flag
        ackBuffer += 0;     //Reason
        //ackBuffer += 0;     //Property Length

        testContext.receiveResponse(ackBuffer);

        CHECK(connectEventFired == true);
        CHECK(connectEventSuccess == false);
        CHECK(connectEventError.errorCode == cError.errorCode);
        CHECK(cError.errorCode == ClientErrorCode::Failed_Decoding_Packet);
    }

    TEST_CASE("Connect Event  - Missing property length")
    {
        TestClientContext testContext{ {}, true };

        bool connectEventFired = false;
        bool connectEventSuccess = false;
        mqtt::ClientError connectEventError = { mqtt::ClientErrorCode::No_Error };
        bool hasReceivedConnectAck = false;

        testContext.client->onConnectEvent().add([&](const mqtt::ConnectEventDetails& details, const mqtt::ConnectAck&)
            {
                connectEventFired = true;
                connectEventSuccess = details.isSuccessful;
                connectEventError = details.error;
                hasReceivedConnectAck = details.hasReceivedAck;
            });

        ClientError cError;
        mqtt::SendResultData rData;
        testContext.client->onErrorEvent().add([&](ClientError error, mqtt::SendResultData data)
            {
                cError = error;
                rData = data;
            });

        testContext.tryConnect();

        ByteBuffer ackBuffer(4);
        ackBuffer += 32;    //Type
        ackBuffer += 2;     //Remaining Length
        ackBuffer += 0;     //Flag
        ackBuffer += 0;     //Reason
        //ackBuffer += 0;     //Property Length

        testContext.receiveResponse(ackBuffer);

        CHECK(connectEventFired == true);
        CHECK(connectEventSuccess == false);
        CHECK(connectEventError.errorCode == cError.errorCode);
        CHECK(cError.errorCode == ClientErrorCode::Failed_Decoding_Packet);
    }

    TEST_CASE("Connect Event - ConnectAck with error reason code triggers connect event with failure")
    {
        TestClientContext testContext{ {}, true };

        bool connectEventFired = false;
        bool connectEventSuccess = false;
        mqtt::ClientError connectEventError = { mqtt::ClientErrorCode::No_Error };
        bool hasReceivedConnectAck = false;

		testContext.client->onConnectEvent().add([&](const mqtt::ConnectEventDetails& details, const mqtt::ConnectAck& packet)
			{
				connectEventFired = true;
				connectEventSuccess = details.isSuccessful;
				connectEventError = details.error;
				hasReceivedConnectAck = details.hasReceivedAck;

				CHECK(packet.getVariableHeader().reasonCode == ConnectReasonCode::UNSPECIFIED_ERROR);
			});

        testContext.tryConnect();

        // Simulate ConnectAck with error reason code (e.g. 0x80 = UNSPECIFIED_ERROR)
        ByteBuffer ackBuffer(5);
        ackBuffer += 32;    //Type
        ackBuffer += 3;     //Remaining Length
        ackBuffer += 0;     //Flag
        ackBuffer += 0x80;  //Reason (error)
        ackBuffer += 0;     //Property Length

        testContext.receiveResponse(ackBuffer);

        CHECK(connectEventFired);
        CHECK(connectEventSuccess == false);
        CHECK(connectEventError.noError());
		CHECK(hasReceivedConnectAck);
    }

    TEST_CASE("Connect Event - ConnectAck with missing bytes after remaining length")
    {
        TestClientContext testContext{ {}, true };

        testContext.client->onConnectEvent().add([&](const mqtt::ConnectEventDetails& details, const mqtt::ConnectAck&)
            {
                CHECK(false); // Should not onConnectEvent
            });

        testContext.client->onErrorEvent().add([&](ClientError error, mqtt::SendResultData data)
            {
                CHECK(false); // Should not onErrorEvent
            });

        testContext.tryConnect();

        // Simulate incomplete ConnectAck (missing reason code and property length)
        ByteBuffer ackBuffer(2);
        ackBuffer += 32;    //Type
        ackBuffer += 1;     //Remaining Length

        testContext.receiveResponse(ackBuffer);
    }

    TEST_CASE("Connect Event - ConnectAck with missing bytes but receive remaining after")
    {
        TestClientContext testContext{ {}, true };

        testContext.client->onConnectEvent().add([&](const mqtt::ConnectEventDetails& details, const mqtt::ConnectAck&)
            {
               
            });

        testContext.client->onErrorEvent().add([&](ClientError error, mqtt::SendResultData data)
            {
                CHECK(false); // Should not fire onErrorEvent
            });

        testContext.tryConnect();

        // Simulate incomplete ConnectAck (missing reason code and property length)
        ByteBuffer ackBuffer(3);
        ackBuffer += 32;    //Type
        ackBuffer += 3;     //Remaining Length
        ackBuffer += 0;     //Flag

        testContext.receiveResponse(ackBuffer);

        ByteBuffer ackBuffer2(2);
        ackBuffer2 += 0;  //Reason (error)
        ackBuffer2 += 0;     //Property Length

        testContext.receiveResponse(ackBuffer2);
    }

    TEST_CASE("Connect Event - ConnectAck with property length but no properties")
    {
        TestClientContext testContext{ {}, true };

        bool connectEventFired = false;
        bool connectEventSuccess = false;
        mqtt::ClientError connectEventError = { mqtt::ClientErrorCode::No_Error };
        bool hasReceivedConnectAck = false;

        testContext.client->onConnectEvent().add([&](const mqtt::ConnectEventDetails& details, const mqtt::ConnectAck&)
            {
                connectEventFired = true;
                connectEventSuccess = details.isSuccessful;
                connectEventError = details.error;
                hasReceivedConnectAck = details.hasReceivedAck;
            });

        testContext.tryConnect();

        // Simulate ConnectAck with property length set but no properties
        ByteBuffer ackBuffer(6);
        ackBuffer += 32;    //Type
        ackBuffer += 4;     //Remaining Length
        ackBuffer += 0;     //Flag
        ackBuffer += 0;     //Reason
        ackBuffer += 1;     //Property Length (should be 1, but no property bytes follow)
        // No property bytes

        testContext.receiveResponse(ackBuffer);

        // Should trigger error event, not connect event
        CHECK(connectEventFired == false);
    }

    TEST_CASE("Connect Event - Multiple Connect attempts")
    {
        TestClientContext testContext{};

        CHECK(testContext.tryConnect().noError());

        auto err = testContext.tryConnect(ClientErrorCode::Already_Connecting);
        CHECK(err.errorCode == ClientErrorCode::Already_Connecting);
    }

    TEST_CASE("Connect with custom keepAlive, cleanStart false, and MQTT 3.1.1")
    {
        TestClientContext testContext;
        auto args = TestClientContext::getDefaultConnectArgs();
        args.keepAliveInSec = 120;
        args.cleanStart = false;
        args.version = cleanMqtt::mqtt::MqttVersion::MQTT_3_1_1;
        args.protocolName = "MQTT";
        args.clientId = "CustomClientId";
        testContext.tryConnect(cleanMqtt::mqtt::ClientErrorCode::No_Error, std::move(args));
        CHECK(testContext.socketPtr->connectCalled);
    }

    TEST_CASE("Connect with username and password")
    {
        TestClientContext testContext;
        auto args = TestClientContext::getDefaultConnectArgs();
        args.username = "user";
        args.password = "pass";
        testContext.tryConnect(cleanMqtt::mqtt::ClientErrorCode::No_Error, std::move(args));
        CHECK(testContext.socketPtr->connectCalled);
    }

    TEST_CASE("Connect with Will message")
    {
        TestClientContext testContext;
        auto args = TestClientContext::getDefaultConnectArgs();
        args.will = std::make_unique<cleanMqtt::mqtt::Will>("will/topic");
        static constexpr uint8_t data[] = { 1,2,3 };
        args.will->payload = std::make_unique<cleanMqtt::mqtt::BinaryData>(sizeof(data), data);
        args.will->willQos = cleanMqtt::mqtt::Qos::QOS_1;
        args.will->retainWillMessage = true;
        testContext.tryConnect(cleanMqtt::mqtt::ClientErrorCode::No_Error, std::move(args));
        CHECK(testContext.socketPtr->connectCalled);
    }

    TEST_CASE("Connect with user properties")
    {
        TestClientContext testContext;
        auto args = TestClientContext::getDefaultConnectArgs();
        args.userProperties["key1"] = "value1";
        args.userProperties["key2"] = "value2";
        testContext.tryConnect(cleanMqtt::mqtt::ClientErrorCode::No_Error, std::move(args));
        CHECK(testContext.socketPtr->connectCalled);
    }

    TEST_CASE("ConnectAck - Success with properties")
    {
        TestClientContext testContext{ {}, true };
        bool connectEventFired = false;
        testContext.client->onConnectEvent().add([&](const mqtt::ConnectEventDetails& details, const mqtt::ConnectAck& ack)
            {
                connectEventFired = true;
                CHECK(details.isSuccessful);
                CHECK(details.hasReceivedAck);
                const uint32_t* interval;
                CHECK(ack.getVariableHeader().properties.tryGetProperty(mqtt::PropertyType::SESSION_EXPIRY_INTERVAL, interval));
				CHECK(*interval == 1);
            });

        testContext.tryConnect();

        // Simulate ConnectAck with property length and a dummy property (property id 0x11, value 0x01)
        ByteBuffer ackBuffer(10);
        ackBuffer += 32;    //Type
        ackBuffer += 8;     //Remaining Length
        ackBuffer += 0;     //Flag
        ackBuffer += 0;     //Reason
        ackBuffer += 5;     //Property Length
        ackBuffer += 0x11;  //Property identifier (Session Expiry Interval for example)
		ackBuffer.append((uint32_t)1); // Session Expiry Interval value

        testContext.receiveResponse(ackBuffer);

        CHECK(connectEventFired);
    }

    TEST_CASE("ConnectAck - Failure due to protocol error - disconnect called internally")
    {
        TestClientContext testContext{ {}, true };
        bool connectEventFired = false;
		bool disconnectCalled = false;
        testContext.client->onConnectEvent().add([&](const mqtt::ConnectEventDetails& details, const mqtt::ConnectAck& ack)
            {
                CHECK(!disconnectCalled); //Connect event trigger first.
                connectEventFired = true;
                CHECK(!details.isSuccessful);
                CHECK(details.hasReceivedAck);
            });

        testContext.client->onDisconnectEvent().add([&](const mqtt::DisconnectEventDetails& details)
            {
                disconnectCalled = true;
                CHECK(details.error.errorCode == mqtt::ClientErrorCode::No_Error);
				CHECK(details.reasonCode == mqtt::DisconnectReasonCode::MALFORMED_PACKET);
                CHECK(!details.isBrokerInduced);
            });

        testContext.tryConnect();

        //Simulate ConnectAck with property length and a dummy property (property id 0x11, value 0x01) but malformed packet
        ByteBuffer ackBuffer(10);
        ackBuffer += 32;    //Type
        ackBuffer += 5;     //Remaining Length (Protocol error here because 5 (Remaining Length) + 1(Fixed Header) does not equal 10(Total size) in the ackBuffer)
        ackBuffer += 0;     //Flag
        ackBuffer += 0;     //Reason
        ackBuffer += 2;     //Property Length
        ackBuffer += 0x11;  //Property identifier (Session Expiry Interval for example)
        ackBuffer.append((uint32_t)1); // Session Expiry Interval value

        testContext.receiveResponse(ackBuffer);

        CHECK(connectEventFired);
        CHECK(disconnectCalled);
    }

    TEST_CASE("ConnectAck - Not authorized reason code")
    {
        TestClientContext testContext{ {}, true };
        bool connectEventFired = false;
        bool connectEventSuccess = false;
        mqtt::ClientError connectEventError = { mqtt::ClientErrorCode::No_Error };
        bool hasReceivedConnectAck = false;

        testContext.client->onConnectEvent().add([&](const mqtt::ConnectEventDetails& details, const mqtt::ConnectAck& packet)
            {
                connectEventFired = true;
                connectEventSuccess = details.isSuccessful;
                connectEventError = details.error;
                hasReceivedConnectAck = details.hasReceivedAck;

                CHECK(packet.getVariableHeader().reasonCode == mqtt::ConnectReasonCode::NOT_AUTHORIZED);
            });

        testContext.tryConnect();

        //Simulate ConnectAck with NOT_AUTHORIZED reason code (0x87)
        ByteBuffer ackBuffer(5);
        ackBuffer += 32;    //Type
        ackBuffer += 3;     //Remaining Length
        ackBuffer += 0;     //Flag
        ackBuffer += 0x87;  //Reason (NOT_AUTHORIZED)
        ackBuffer += 0;     //Property Length

        testContext.receiveResponse(ackBuffer);

        CHECK(connectEventFired);
        CHECK(connectEventSuccess == false);
        CHECK(connectEventError.noError());
    }

    TEST_CASE("ConnectAck - Protocol error reason code")
    {
        TestClientContext testContext{ {}, true };
        bool connectEventFired = false;

        testContext.client->onConnectEvent().add([&](const mqtt::ConnectEventDetails& details, const mqtt::ConnectAck& packet)
            {
                connectEventFired = true;
                CHECK(!details.isSuccessful);
                CHECK(packet.getVariableHeader().reasonCode == mqtt::ConnectReasonCode::PROTOCOL_ERROR);
            });

        testContext.tryConnect();

        //Simulate ConnectAck with PROTOCOL_ERROR reason code (0x82)
        ByteBuffer ackBuffer(5);
        ackBuffer += 32;    //Type
        ackBuffer += 3;     //Remaining Length
        ackBuffer += 0;     //Flag
        ackBuffer += 0x82;  //Reason (PROTOCOL_ERROR)
        ackBuffer += 0;     //Property Length

        testContext.receiveResponse(ackBuffer);

        CHECK(connectEventFired);
    }
}