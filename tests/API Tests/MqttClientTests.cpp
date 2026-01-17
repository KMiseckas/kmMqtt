#include <doctest.h>
#include <kmMqtt/MqttClient.h>
#include <memory>
#include <string>
#include <thread>
#include <chrono>
#include "MockWebSocket.h"
#include "Helpers.h"

using namespace kmMqtt;
using namespace kmMqtt::mqtt;

TEST_SUITE("MqttClient API Tests")
{
	TEST_CASE("Constructor - Default")
	{
		// Default constructor should not throw and create valid client
		CHECK_NOTHROW(MqttClient());
	}

	TEST_CASE("Constructor - Custom Environment and Options")
	{
		TestEnvironment env;
		MqttClientOptions options(TickMode::SYNC);
		
		CHECK_NOTHROW(MqttClient(&env, options));
	}

	TEST_CASE("Constructor - ASYNC Mode Starts Ticking")
	{
		TestEnvironment env;
		MqttClientOptions options(TickMode::ASYNC);
		
		MqttClient client(&env, options);
		CHECK(client.getIsTickAsync() == true);
		
		// Cleanup properly
		client.shutdown();
	}

	TEST_CASE("getConnectionStatus - Initial State")
	{
		TestClientContext testContext;
		CHECK(testContext.client->getConnectionStatus() == ConnectionStatus::DISCONNECTED);
	}

	TEST_CASE("getConnectionStatus - After Connect Call")
	{
		TestClientContext testContext;
		testContext.tryConnect();
		
		// Should be CONNECTING before receiving ConnectAck
		CHECK(testContext.client->getConnectionStatus() == ConnectionStatus::CONNECTING);
	}

	TEST_CASE("getConnectionStatus - After Successful Connect")
	{
		TestClientContext testContext;
		testContext.tryConnectWithResponse();
		
		CHECK(testContext.client->getConnectionStatus() == ConnectionStatus::CONNECTED);
	}

	TEST_CASE("getConnectionInfo - ClientId Preserved")
	{
		TestClientContext testContext;
		auto args = testContext.getDefaultConnectArgs();
		args.clientId = "CustomTestClient123";
		
		testContext.tryConnect(ClientErrorCode::No_Error, std::move(args));
		
		const auto& info = testContext.client->getConnectionInfo();
		CHECK(info.connectArgs.clientId == "CustomTestClient123");
	}

	TEST_CASE("getConnectionInfo - Address Preserved")
	{
		TestClientContext testContext;
		auto address = mqtt::ConnectAddress();
		address.primaryAddress = mqtt::Address::createURL("", "test.mqtt.broker", "8883", "");
		
		testContext.tryConnect(ClientErrorCode::No_Error, testContext.getDefaultConnectArgs(), std::move(address));
		
		const auto& info = testContext.client->getConnectionInfo();
		CHECK(info.connectAddress.primaryAddress.hostname() == "test.mqtt.broker");
		CHECK(info.connectAddress.primaryAddress.port() == "8883");
	}

	TEST_CASE("getIsTickAsync - SYNC Mode")
	{
		TestClientContext testContext;
		CHECK(testContext.client->getIsTickAsync() == false);
	}

	TEST_CASE("getIsTickAsync - ASYNC Mode")
	{
		TestEnvironment env;
		MqttClientOptions options(TickMode::ASYNC);
		MqttClient client(&env, options);
		
		CHECK(client.getIsTickAsync() == true);
		
		client.shutdown();
	}

	TEST_CASE("Event Accessors - All Events Available")
	{
		TestClientContext testContext;
		
		CHECK_NOTHROW(testContext.client->onErrorEvent());
		CHECK_NOTHROW(testContext.client->onConnectEvent());
		CHECK_NOTHROW(testContext.client->onDisconnectEvent());
		CHECK_NOTHROW(testContext.client->onReconnectEvent());
		CHECK_NOTHROW(testContext.client->onPublishEvent());
		CHECK_NOTHROW(testContext.client->onPublishCompletedEvent());
		CHECK_NOTHROW(testContext.client->onSubscribeAckEvent());
		CHECK_NOTHROW(testContext.client->onUnSubscribeAckEvent());
	}

	TEST_CASE("Event - Error Event Fires")
	{
		TestClientContext testContext;
		bool errorEventFired = false;
		ClientErrorCode errorCode = ClientErrorCode::No_Error;
		
		testContext.client->onErrorEvent().add([&](ClientError error, SendResultData)
		{
			errorEventFired = true;
			errorCode = error.errorCode;
		});
		
		testContext.tryConnect();
		
		// Send malformed ConnectAck to trigger error
		ByteBuffer malformed(3);
		malformed += 32;  // Type
		malformed += 1;   // Remaining Length
		malformed += 0;   // Flag only, missing reason code and property length
		
		testContext.receiveResponse(malformed);
		
		CHECK(errorEventFired);
		CHECK(errorCode == ClientErrorCode::Failed_Decoding_Packet);
	}

	TEST_CASE("Event - Multiple Subscribers to Same Event")
	{
		TestClientContext testContext;
		int callbackCount = 0;
		
		testContext.client->onConnectEvent().add([&](const ConnectEventDetails&, const ConnectAck&)
		{
			callbackCount++;
		});
		
		testContext.client->onConnectEvent().add([&](const ConnectEventDetails&, const ConnectAck&)
		{
			callbackCount++;
		});
		
		testContext.client->onConnectEvent().add([&](const ConnectEventDetails&, const ConnectAck&)
		{
			callbackCount++;
		});
		
		testContext.tryConnectWithResponse();
		
		CHECK(callbackCount == 3);
	}

	TEST_CASE("Publish - Not Connected Returns Error")
	{
		TestClientContext testContext;
		ByteBuffer payload(10);
		PublishOptions options;
		
		auto result = testContext.client->publish("test/topic", std::move(payload), std::move(options));
		CHECK(result.errorCode() == ClientErrorCode::Not_Connected);
	}

	TEST_CASE("Publish - QOS_0 Success")
	{
		TestClientContext testContext;
		testContext.tryConnectWithResponse();
		
		ByteBuffer payload(10);
		const std::uint8_t data[] = {0x01, 0x02, 0x03};
		payload.append(data, 3);
		
		PublishOptions options;
		options.qos = Qos::QOS_0;
		
		auto err = testContext.client->publish("test/qos0", std::move(payload), std::move(options));
		CHECK(err.noError());
		
		testContext.client->tick();
		
		// Verify publish packet was sent
		CHECK(testContext.socketPtr->sentPackets.size() >= 1);
	}

	TEST_CASE("Publish - QOS_1 Success")
	{
		TestClientContext testContext;
		testContext.tryConnectWithResponse();
		
		ByteBuffer payload(10);
		const std::uint8_t data[] = {0xAA, 0xBB};
		payload.append(data, 2);
		
		PublishOptions options;
		options.qos = Qos::QOS_1;
		
		auto err = testContext.client->publish("test/qos1", std::move(payload), std::move(options));
		CHECK(err.noError());
		
		testContext.client->tick();
		
		CHECK(testContext.socketPtr->sentPackets.size() >= 1);
	}

	TEST_CASE("Publish - With Retain Flag")
	{
		TestClientContext testContext;
		testContext.tryConnectWithResponse();
		
		ByteBuffer payload(10);
		PublishOptions options;
		options.retain = true;
		
		auto err = testContext.client->publish("test/retain", std::move(payload), std::move(options));
		CHECK(err.noError());
	}

	TEST_CASE("Publish - With Response Topic")
	{
		TestClientContext testContext;
		testContext.tryConnectWithResponse();
		
		ByteBuffer payload(10);
		PublishOptions options;
		options.responseTopic = "response/topic";
		
		auto err = testContext.client->publish("test/response", std::move(payload), std::move(options));
		CHECK(err.noError());
	}

	TEST_CASE("Publish - With User Properties")
	{
		TestClientContext testContext;
		testContext.tryConnectWithResponse();
		
		ByteBuffer payload(10);
		PublishOptions options;
		options.userProperties["key1"] = "value1";
		options.userProperties["key2"] = "value2";
		
		auto err = testContext.client->publish("test/props", std::move(payload), std::move(options));
		CHECK(err.noError());
	}

	TEST_CASE("Subscribe - Not Connected Returns Error")
	{
		TestClientContext testContext;
		std::vector<Topic> topics;
		topics.push_back(Topic("test/topic", Qos::QOS_0));
		
		SubscribeOptions options;
		auto result = testContext.client->subscribe(topics, std::move(options));
		CHECK(result.errorCode() == ClientErrorCode::Not_Connected);
	}

	TEST_CASE("Subscribe - Empty Topics Returns Error")
	{
		TestClientContext testContext;
		testContext.tryConnectWithResponse();
		
		std::vector<Topic> emptyTopics;
		SubscribeOptions options;
		
		auto result = testContext.client->subscribe(emptyTopics, std::move(options));
		CHECK(result.errorCode() == ClientErrorCode::Invalid_Argument);
	}

	TEST_CASE("Subscribe - Single Topic Success")
	{
		TestClientContext testContext;
		testContext.tryConnectWithResponse();
		
		std::vector<Topic> topics;
		topics.push_back(Topic("test/topic", Qos::QOS_1));
		
		SubscribeOptions options;
		auto err = testContext.client->subscribe(topics, std::move(options));
		CHECK(err.noError());
		
		testContext.client->tick();
		CHECK(testContext.socketPtr->sentPackets.size() >= 1);
	}

	TEST_CASE("Subscribe - Multiple Topics Success")
	{
		TestClientContext testContext;
		testContext.tryConnectWithResponse();
		
		std::vector<Topic> topics;
		topics.push_back(Topic("topic/1", Qos::QOS_0));
		topics.push_back(Topic("topic/2", Qos::QOS_1));
		topics.push_back(Topic("topic/3", Qos::QOS_2));
		
		SubscribeOptions options;
		auto err = testContext.client->subscribe(topics, std::move(options));
		CHECK(err.noError());
	}

	TEST_CASE("Subscribe - With User Properties")
	{
		TestClientContext testContext;
		testContext.tryConnectWithResponse();
		
		std::vector<Topic> topics;
		topics.push_back(Topic("test/sub", Qos::QOS_1));
		
		SubscribeOptions options;
		options.userProperties["subscription"] = "priority";
		
		auto err = testContext.client->subscribe(topics, std::move(options));
		CHECK(err.noError());
	}

	TEST_CASE("UnSubscribe - Not Connected Returns Error")
	{
		TestClientContext testContext;
		std::vector<Topic> topics;
		topics.push_back(Topic("test/topic", Qos::QOS_0));
		
		UnSubscribeOptions options;
		auto result = testContext.client->unSubscribe(topics, std::move(options));
		CHECK(result.errorCode() == ClientErrorCode::Not_Connected);
	}

	TEST_CASE("UnSubscribe - Empty Topics Returns Error")
	{
		TestClientContext testContext;
		testContext.tryConnectWithResponse();
		
		std::vector<Topic> emptyTopics;
		UnSubscribeOptions options;
		
		auto result = testContext.client->unSubscribe(emptyTopics, std::move(options));
		CHECK(result.errorCode() == ClientErrorCode::Invalid_Argument);
	}

	TEST_CASE("UnSubscribe - Single Topic Success")
	{
		TestClientContext testContext;
		testContext.tryConnectWithResponse();
		
		std::vector<Topic> topics;
		topics.push_back(Topic("test/unsub", Qos::QOS_0));
		
		UnSubscribeOptions options;
		auto err = testContext.client->unSubscribe(topics, std::move(options));
		CHECK(err.noError());
		
		testContext.client->tick();
		CHECK(testContext.socketPtr->sentPackets.size() >= 1);
	}

	TEST_CASE("UnSubscribe - Multiple Topics Success")
	{
		TestClientContext testContext;
		testContext.tryConnectWithResponse();
		
		std::vector<Topic> topics;
		topics.push_back(Topic("unsub/1", Qos::QOS_0));
		topics.push_back(Topic("unsub/2", Qos::QOS_1));
		
		UnSubscribeOptions options;
		auto err = testContext.client->unSubscribe(topics, std::move(options));
		CHECK(err.noError());
	}

	TEST_CASE("Disconnect - Not Connected Returns Error")
	{
		TestClientContext testContext;
		auto result = testContext.client->disconnect();
		CHECK(result.errorCode() == ClientErrorCode::Not_Connected);
	}

	TEST_CASE("Disconnect - When Connected Success")
	{
		TestClientContext testContext;
		testContext.tryConnectWithResponse();
		
		CHECK(testContext.client->getConnectionStatus() == ConnectionStatus::CONNECTED);
		
		auto err = testContext.client->disconnect();
		CHECK(err.noError());
		
		testContext.client->tick();
		CHECK(testContext.client->getConnectionStatus() == ConnectionStatus::DISCONNECTED);
	}

	TEST_CASE("Disconnect - With Reason Text")
	{
		TestClientContext testContext;
		testContext.tryConnectWithResponse();
		
		DisconnectArgs args;
		args.disconnectReasonText = "User initiated disconnect";
		
		auto err = testContext.client->disconnect(std::move(args));
		CHECK(err.noError());
	}

	TEST_CASE("Disconnect - With Will Publish")
	{
		TestClientContext testContext;
		testContext.tryConnectWithResponse();
		
		DisconnectArgs args(true); //requestWillPublish = true
		
		auto err = testContext.client->disconnect(std::move(args));
		CHECK(err.noError());
	}

	TEST_CASE("Disconnect - Without Clearing Send Queue")
	{
		TestClientContext testContext;
		testContext.tryConnectWithResponse();
		
		DisconnectArgs args(false, false); //Don't clear queue
		
		auto err = testContext.client->disconnect(std::move(args));
		CHECK(err.noError());
	}

	TEST_CASE("Shutdown - When Not Connected")
	{
		TestClientContext testContext;
		auto err = testContext.client->shutdown();
		CHECK(err.errorCode == ClientErrorCode::MQTT_Not_Active);
	}

	TEST_CASE("Shutdown - When Connected")
	{
		TestClientContext testContext;
		testContext.tryConnectWithResponse();
		
		auto err = testContext.client->shutdown();
		CHECK(err.noError());
		CHECK(testContext.client->getConnectionStatus() == ConnectionStatus::DISCONNECTED);
	}

	TEST_CASE("Shutdown - Cleans Up Resources")
	{
		TestClientContext testContext;
		testContext.tryConnectWithResponse();
		
		//Add some pending operations
		ByteBuffer payload(10);
		PublishOptions pubOptions;
		testContext.client->publish("test/cleanup", std::move(payload), std::move(pubOptions));
		
		auto err = testContext.client->shutdown();
		CHECK(err.noError());
		
		//Socket should be closed
		CHECK(testContext.socketPtr->closeCalled);
	}

	TEST_CASE("Full Lifecycle - Connect, Publish, Subscribe, Disconnect")
	{
		TestClientContext testContext;
		
		testContext.tryConnectWithResponse();
		CHECK(testContext.client->getConnectionStatus() == ConnectionStatus::CONNECTED);
		
		ByteBuffer payload(20);
		const std::uint8_t data[] = {0x01, 0x02, 0x03};
		payload.append(data, 3);
		PublishOptions pubOptions;
		pubOptions.qos = Qos::QOS_1;
		auto pubErr = testContext.client->publish("test/lifecycle", std::move(payload), std::move(pubOptions));
		CHECK(pubErr.noError());
		
		std::vector<Topic> topics;
		topics.push_back(Topic("test/sub/lifecycle", Qos::QOS_1));
		SubscribeOptions subOptions;
		auto subErr = testContext.client->subscribe(topics, std::move(subOptions));
		CHECK(subErr.noError());
		
		auto disconnectErr = testContext.client->disconnect();
		CHECK(disconnectErr.noError());
	}

	TEST_CASE("Event - SubscribeAckEvent Fires")
	{
		TestClientContext testContext;
		testContext.tryConnectWithResponse();
		
		bool subAckFired = false;
		std::uint16_t receivedPacketId = 0;
		
		testContext.client->onSubscribeAckEvent().add([&](const SubscribeAckEventDetails& details, const SubscribeAck&)
		{
			subAckFired = true;
			receivedPacketId = details.packetId;
		});
		
		std::vector<Topic> topics;
		topics.push_back(Topic("test/suback", Qos::QOS_1));
		SubscribeOptions options;
		testContext.client->subscribe(topics, std::move(options));
		testContext.client->tick();
		
		ByteBuffer suback(10);
		suback += 144;  //Type (SUBACK)
		suback += 4;    //Remaining Length
		suback += 0;    //Packet ID MSB
		suback += 1;    //Packet ID LSB
		suback += 0;    //Property Length
		suback += 0;    //Reason Code (Granted QoS 0)
		
		testContext.receiveResponse(suback);
		
		CHECK(subAckFired);
	}

	TEST_CASE("Event - UnSubscribeAckEvent Fires")
	{
		TestClientContext testContext;
		testContext.tryConnectWithResponse();
		
		bool unsubAckFired = false;
		
		testContext.client->onUnSubscribeAckEvent().add([&](const UnSubscribeAckEventDetails&, const UnSubscribeAck&)
		{
			unsubAckFired = true;
		});
		
		std::vector<Topic> topics;
		topics.push_back(Topic("test/unsuback", Qos::QOS_0));
		UnSubscribeOptions options;
		testContext.client->unSubscribe(topics, std::move(options));
		testContext.client->tick();
		
		ByteBuffer unsuback(10);
		unsuback += 176;
		unsuback += 4;  
		unsuback += 0;  
		unsuback += 1;  
		unsuback += 0;  
		unsuback += 0;  
		
		testContext.receiveResponse(unsuback);
		
		CHECK(unsubAckFired);
	}

	TEST_CASE("Event - PublishEvent Fires on Received Publish")
	{
		TestClientContext testContext;
		testContext.tryConnectWithResponse();
		
		bool publishEventFired = false;
		std::string receivedTopic;
		
		testContext.client->onPublishEvent().add([&](const PublishEventDetails& details, const Publish&)
		{
			publishEventFired = true;
			receivedTopic = details.topic;
		});
		
		ByteBuffer publish(20);
		publish += 48; 
		publish += 12;
		publish += 0; 
		publish += 9;  
		publish += 't';
		publish += 'e';
		publish += 's';
		publish += 't';
		publish += '/';
		publish += 'r';
		publish += 'e';
		publish += 'c';
		publish += 'v';
		publish += 0;
		
		testContext.receiveResponse(publish);
		
		CHECK(publishEventFired);
		CHECK(receivedTopic == "test/recv");
	}

	TEST_CASE("Multiple Operations - Interleaved")
	{
		TestClientContext testContext{ };
		testContext.tryConnectWithResponse();
		
		ByteBuffer payload1(10);
		PublishOptions pubOpts1;
		CHECK(testContext.client->publish("test/1", std::move(payload1), std::move(pubOpts1)).noError());
		
		std::vector<Topic> topics1;
		topics1.push_back(Topic("sub/1", Qos::QOS_0));
		SubscribeOptions subOpts1;
		CHECK(testContext.client->subscribe(topics1, std::move(subOpts1)).noError());
		
		ByteBuffer payload2(10);
		PublishOptions pubOpts2;
		CHECK(testContext.client->publish("test/2", std::move(payload2), std::move(pubOpts2)).noError());
		
		std::vector<Topic> topics2;
		topics2.push_back(Topic("unsub/1", Qos::QOS_0));
		UnSubscribeOptions unsubOpts;
		CHECK(testContext.client->unSubscribe(topics2, std::move(unsubOpts)).noError());
		
		CHECK(testContext.client->tick().noError());
	
	}

	TEST_CASE("Destructor - Cleans Up Properly")
	{
		TestEnvironment env;
		MqttClientOptions options(TickMode::SYNC);
		
		{
			MqttClient client(&env, options);
			
			ConnectArgs args("DestructorTest");
			ConnectAddress address;
			address.primaryAddress = Address::createURL("", "localhost", "1883", "");
			
			CHECK(client.connect(std::move(args), std::move(address)).noError());
			CHECK(client.tick().noError());
		}
		
		CHECK(true);
	}

	TEST_CASE("Error Handling - Invalid Will Payload Size")
	{
		TestClientContext testContext;
		
		ConnectArgs args("TestClient");
		args.will = std::make_unique<Will>("will/topic");
		args.will->payload = std::make_unique<BinaryData>();
		
		ConnectAddress address;
		address.primaryAddress = Address::createURL("", "localhost", "1883", "");
		
		auto result = testContext.client->connect(std::move(args), std::move(address));
		CHECK(result.errorCode() == ClientErrorCode::Invalid_Argument);
	}

	TEST_CASE("Error Handling - Invalid Will Correlation Data Size")
	{
		TestClientContext testContext;
		
		ConnectArgs args("TestClient");
		args.will = std::make_unique<Will>("will/topic");
		args.will->correlationData = std::make_unique<BinaryData>();
		
		ConnectAddress address;
		address.primaryAddress = Address::createURL("", "localhost", "1883", "");
		
		auto result = testContext.client->connect(std::move(args), std::move(address));
		CHECK(result.errorCode() == ClientErrorCode::Invalid_Argument);
	}

	TEST_CASE("Error Handling - Will Without Topic")
	{
		TestClientContext testContext;
		
		ConnectArgs args("TestClient");
		args.will = std::make_unique<Will>("");
		const std::uint8_t data[] = {0x01};
		args.will->payload = std::make_unique<BinaryData>(1, data);
		
		ConnectAddress address;
		address.primaryAddress = Address::createURL("", "localhost", "1883", "");
		
		auto result = testContext.client->connect(std::move(args), std::move(address));
		CHECK(result.errorCode() == ClientErrorCode::Missing_Argument);
	}

	TEST_CASE("Connection Status Transitions")
	{
		TestClientContext testContext;
		
		CHECK(testContext.client->getConnectionStatus() == ConnectionStatus::DISCONNECTED);
		
		testContext.tryConnect();
		CHECK(testContext.client->getConnectionStatus() == ConnectionStatus::CONNECTING);
		
		ByteBuffer ack(5);
		ack += 32; ack += 3; ack += 0; ack += 0; ack += 0;
		testContext.receiveResponse(ack);
		CHECK(testContext.client->getConnectionStatus() == ConnectionStatus::CONNECTED);
		
		testContext.client->disconnect();
		testContext.client->tick();
		CHECK(testContext.client->getConnectionStatus() == ConnectionStatus::DISCONNECTED);
	}
}