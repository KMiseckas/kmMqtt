// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

// ---------------------------------------------------------------------------
// Integration tests — publish / subscribe
//
// Smoke suite: QoS 0 and QoS 1 E2E flows, empty payload, error path.
// Full suite: multi-topic, wildcards, duplicate subscribe, unsubscribe unknown.
// ---------------------------------------------------------------------------

#include <doctest.h>

#include <kmMqtt/ByteBuffer.h>
#include <kmMqtt/Mqtt/MqttClientEvents.h>
#include <kmMqtt/Mqtt/Params/ConnectAddress.h>
#include <kmMqtt/Mqtt/Params/DisconnectArgs.h>
#include <kmMqtt/Mqtt/Params/PublishOptions.h>
#include <kmMqtt/Mqtt/Params/SubscribeOptions.h>
#include <kmMqtt/Mqtt/Params/Topic.h>
#include <kmMqtt/Mqtt/Params/UnSubscribeOptions.h>
#include <kmMqtt/Mqtt/State/SubAckTopicReason.h>
#include <kmMqtt/Mqtt/State/UnSubAckTopicReason.h>
#include <kmMqtt/MqttClient.h>

#include <atomic>
#include <cstring>
#include <string>
#include <vector>

#include "BrokerConfig.h"
#include "Helpers.h"
#include <chrono>
#include <cstdint>
#include <utility>
#include <kmMqtt/Mqtt/Enums/ConnectionStatus.h>
#include <kmMqtt/Mqtt/Enums/Qos.h>
#include <kmMqtt/Mqtt/Packets/Connection/ConnectAck.h>
#include <kmMqtt/Mqtt/Packets/PacketType.h>
#include <kmMqtt/Mqtt/Packets/Publish/Publish.h>
#include <kmMqtt/Mqtt/Packets/Subscribe/SubscribeAck.h>
#include <kmMqtt/Mqtt/Packets/UnSubscribe/UnSubscribeAck.h>

using namespace kmMqtt;
using namespace kmMqtt::mqtt;
using namespace kmMqtt_it;

// ---------------------------------------------------------------------------
// Smoke suite
// ---------------------------------------------------------------------------

TEST_SUITE("Integration - Smoke - Publish / Subscribe") {
	TEST_CASE("QoS 1 publish subscribe unsubscribe flow emits expected events") {
		const auto& selection = getWsSelection();
		requireReachableSelection(selection, "WS");
		const auto& endpoint = selection.endpoint;

		MqttClient subscriber;
		MqttClient publisher;

		std::atomic<bool> subscriberConnected{ false };
		std::atomic<bool> publisherConnected{ false };
		std::atomic<int> subAckCount{ 0 };
		std::atomic<int> unSubAckCount{ 0 };
		std::atomic<int> publishReceivedCount{ 0 };
		std::atomic<int> publishCompletedCount{ 0 };
		std::atomic<bool> publishTopicMatches{ false };
		std::atomic<bool> publishPayloadMatches{ false };
		std::atomic<bool> publishCompleteIsAck{ false };
		std::atomic<bool> allSubscribedOk{ false };
		std::atomic<bool> allUnSubscribedOk{ false };

		subscriber.onConnectEvent().add(
			[&](const ConnectEventDetails& d, const ConnectAck&) {
				subscriberConnected.store(d.isSuccessful && d.hasReceivedAck);
			});

		publisher.onConnectEvent().add(
			[&](const ConnectEventDetails& d, const ConnectAck&) {
				publisherConnected.store(d.isSuccessful && d.hasReceivedAck);
			});

		const std::string topic = makeUniqueTopic("kmMqtt/it/smoke/pubsub");
		const std::string payloadText = "smoke_payload_qos1";

		subscriber.onSubscribeAckEvent().add(
			[&](const SubscribeAckEventDetails& details, const SubscribeAck&) {
				allSubscribedOk.store(details.results.allSubscribedSuccesfully());
				subAckCount.fetch_add(1);
			});

		subscriber.onUnSubscribeAckEvent().add(
			[&](const UnSubscribeAckEventDetails& details, const UnSubscribeAck&) {
				allUnSubscribedOk.store(details.results.allUnSubscribedSuccesfully());
				unSubAckCount.fetch_add(1);
			});

		subscriber.onPublishEvent().add(
			[&](const PublishEventDetails& details, const Publish&) {
				if (details.topic == topic)
					publishTopicMatches.store(true);

				if (details.payload != nullptr &&
					details.payload->size() == payloadText.size() &&
					std::memcmp(details.payload->bytes(), payloadText.data(),
						payloadText.size()) == 0)
					publishPayloadMatches.store(true);

				publishReceivedCount.fetch_add(1);
			});

		publisher.onPublishCompletedEvent().add(
			[&](const PublishCompleteEventDetails& details) {
				if (details.packetType == PacketType::PUBLISH_ACKNOWLEDGE &&
					details.isSuccess())
					publishCompleteIsAck.store(true);
				publishCompletedCount.fetch_add(1);
			});

		REQUIRE(
			subscriber
			.connect(makeConnectArgs("sub_smoke_ps1"), makeAddress(endpoint))
			.noError());
		REQUIRE(waitFor(subscriberConnected, endpoint.timeoutSec));

		REQUIRE(
			publisher
			.connect(makeConnectArgs("pub_smoke_ps1"), makeAddress(endpoint))
			.noError());
		REQUIRE(waitFor(publisherConnected, endpoint.timeoutSec));

		std::vector<Topic> topics{ Topic{topic} };
		REQUIRE(subscriber.subscribe(topics, SubscribeOptions{}).noError());
		REQUIRE(waitForAtLeast(subAckCount, 1, endpoint.timeoutSec));
		CHECK(allSubscribedOk.load());

		ByteBuffer payload(payloadText.size());
		payload.append(reinterpret_cast<const std::uint8_t*>(payloadText.data()),
			payloadText.size());

		PublishOptions options;
		options.qos = Qos::QOS_1;
		REQUIRE(
			publisher.publish(topic.c_str(), std::move(payload), std::move(options))
			.noError());
		REQUIRE(waitForAtLeast(publishCompletedCount, 1, endpoint.timeoutSec));
		REQUIRE(waitForAtLeast(publishReceivedCount, 1, endpoint.timeoutSec));

		CHECK(publishCompleteIsAck.load());
		CHECK(publishTopicMatches.load());
		CHECK(publishPayloadMatches.load());

		REQUIRE(subscriber.unSubscribe(topics, UnSubscribeOptions{}).noError());
		REQUIRE(waitForAtLeast(unSubAckCount, 1, endpoint.timeoutSec));
		CHECK(allUnSubscribedOk.load());

		CHECK(subscriber.disconnect(DisconnectArgs{ true }).noError());
		CHECK(publisher.disconnect(DisconnectArgs{ true }).noError());
		subscriber.shutdown();
		publisher.shutdown();
	}

	TEST_CASE("QoS 0 publish subscribe delivers message without publish "
		"completed event") {
		const auto& selection = getWsSelection();
		requireReachableSelection(selection, "WS");
		const auto& endpoint = selection.endpoint;

		MqttClient subscriber;
		MqttClient publisher;

		std::atomic<bool> subscriberConnected{ false };
		std::atomic<bool> publisherConnected{ false };
		std::atomic<int> subAckCount{ 0 };
		std::atomic<int> publishReceivedCount{ 0 };
		std::atomic<int> publishCompletedCount{ 0 };

		subscriber.onConnectEvent().add(
			[&](const ConnectEventDetails& d, const ConnectAck&) {
				subscriberConnected.store(d.isSuccessful && d.hasReceivedAck);
			});

		publisher.onConnectEvent().add(
			[&](const ConnectEventDetails& d, const ConnectAck&) {
				publisherConnected.store(d.isSuccessful && d.hasReceivedAck);
			});

		const std::string topic = makeUniqueTopic("kmMqtt/it/smoke/qos0");

		subscriber.onSubscribeAckEvent().add(
			[&](const SubscribeAckEventDetails&, const SubscribeAck&) {
				subAckCount.fetch_add(1);
			});

		subscriber.onPublishEvent().add(
			[&](const PublishEventDetails&, const Publish&) {
				publishReceivedCount.fetch_add(1);
			});

		publisher.onPublishCompletedEvent().add(
			[&](const PublishCompleteEventDetails&) {
				publishCompletedCount.fetch_add(1);
			});

		REQUIRE(
			subscriber
			.connect(makeConnectArgs("sub_smoke_qos0"), makeAddress(endpoint))
			.noError());
		REQUIRE(waitFor(subscriberConnected, endpoint.timeoutSec));

		REQUIRE(
			publisher
			.connect(makeConnectArgs("pub_smoke_qos0"), makeAddress(endpoint))
			.noError());
		REQUIRE(waitFor(publisherConnected, endpoint.timeoutSec));

		std::vector<Topic> topics{
			Topic{topic, TopicSubscriptionOptions{Qos::QOS_0}} };
		REQUIRE(subscriber.subscribe(topics, SubscribeOptions{}).noError());
		REQUIRE(waitForAtLeast(subAckCount, 1, endpoint.timeoutSec));

		const std::string payloadText = "qos0_payload";
		ByteBuffer payload(payloadText.size());
		payload.append(reinterpret_cast<const std::uint8_t*>(payloadText.data()),
			payloadText.size());

		PublishOptions options;
		options.qos = Qos::QOS_0;
		REQUIRE(
			publisher.publish(topic.c_str(), std::move(payload), std::move(options))
			.noError());

		REQUIRE(waitForAtLeast(publishReceivedCount, 1, endpoint.timeoutSec));
		// QoS 0 has no ACK handshake — publishCompletedEvent must NOT have
		// fired.
		CHECK(publishCompletedCount.load() == 0);

		CHECK(subscriber.disconnect(DisconnectArgs{ true }).noError());
		CHECK(publisher.disconnect(DisconnectArgs{ true }).noError());
		subscriber.shutdown();
		publisher.shutdown();
	}

	TEST_CASE("Empty payload publish is received by subscriber") {
		const auto& selection = getWsSelection();
		requireReachableSelection(selection, "WS");
		const auto& endpoint = selection.endpoint;

		MqttClient subscriber;
		MqttClient publisher;

		std::atomic<bool> subscriberConnected{ false };
		std::atomic<bool> publisherConnected{ false };
		std::atomic<int> subAckCount{ 0 };
		std::atomic<int> publishReceivedCount{ 0 };
		std::atomic<bool> emptyPayloadReceived{ false };

		subscriber.onConnectEvent().add(
			[&](const ConnectEventDetails& d, const ConnectAck&) {
				subscriberConnected.store(d.isSuccessful && d.hasReceivedAck);
			});

		publisher.onConnectEvent().add(
			[&](const ConnectEventDetails& d, const ConnectAck&) {
				publisherConnected.store(d.isSuccessful && d.hasReceivedAck);
			});

		const std::string topic = makeUniqueTopic("kmMqtt/it/smoke/empty");

		subscriber.onSubscribeAckEvent().add(
			[&](const SubscribeAckEventDetails&, const SubscribeAck&) {
				subAckCount.fetch_add(1);
			});

		subscriber.onPublishEvent().add(
			[&](const PublishEventDetails& details, const Publish&) {
				if (details.topic == topic) {
					// Empty payload means null or zero-size buffer.
					const bool isEmpty =
						(details.payload == nullptr || details.payload->size() == 0);
					emptyPayloadReceived.store(isEmpty);
					publishReceivedCount.fetch_add(1);
				}
			});

		REQUIRE(
			subscriber
			.connect(makeConnectArgs("sub_smoke_empty"), makeAddress(endpoint))
			.noError());
		REQUIRE(waitFor(subscriberConnected, endpoint.timeoutSec));

		REQUIRE(
			publisher
			.connect(makeConnectArgs("pub_smoke_empty"), makeAddress(endpoint))
			.noError());
		REQUIRE(waitFor(publisherConnected, endpoint.timeoutSec));

		std::vector<Topic> topics{ Topic{topic} };
		REQUIRE(subscriber.subscribe(topics, SubscribeOptions{}).noError());
		REQUIRE(waitForAtLeast(subAckCount, 1, endpoint.timeoutSec));

		// Publish a zero-byte buffer.
		ByteBuffer emptyPayload(0);
		PublishOptions options;
		options.qos = Qos::QOS_0;
		REQUIRE(
			publisher
			.publish(topic.c_str(), std::move(emptyPayload), std::move(options))
			.noError());

		REQUIRE(waitForAtLeast(publishReceivedCount, 1, endpoint.timeoutSec));
		CHECK(emptyPayloadReceived.load());

		CHECK(subscriber.disconnect(DisconnectArgs{ true }).noError());
		CHECK(publisher.disconnect(DisconnectArgs{ true }).noError());
		subscriber.shutdown();
		publisher.shutdown();
	}

	TEST_CASE("Connect to invalid endpoint surfaces error through API") {
		// Port 19487 is an arbitrary high port that should have nothing
		// listening on it, triggering an immediate TCP connection refused.
		MqttClient client;

		std::atomic<bool> connectFired{ false };
		std::atomic<bool> connectOk{ true }; // default true; set false on fail

		client.onConnectEvent().add(
			[&](const ConnectEventDetails& d, const ConnectAck&) {
				connectOk.store(d.isSuccessful);
				connectFired.store(true);
			});

		ConnectAddress badAddress;
		badAddress.primaryAddress =
			Address::createURL("ws", "127.0.0.1", "19487", "/mqtt");

		const auto result = client.connect(makeConnectArgs("smoke_err_path"),
			std::move(badAddress));

		if (!result.noError()) {

			// Error surfaced synchronously — acceptable outcome.
			CHECK(!result.noError());
		}
		else {
			// Async connect attempt started; SDK should surface failure via
			// ConnectEvent with isSuccessful=false.
			const bool connectEventFired = waitFor(connectFired, 30);
			CHECK_MESSAGE(connectEventFired,
				"Expected failed ConnectEvent for invalid endpoint "
				"within timeout");
			CHECK_MESSAGE(!connectOk.load(),
				"Expected isSuccessful=false for connection "
				"to ws://127.0.0.1:19487/mqtt");
			CHECK(client.getConnectionStatus() == ConnectionStatus::DISCONNECTED);
		}

		client.shutdown();
	}
}

// ---------------------------------------------------------------------------
// Full suite
// ---------------------------------------------------------------------------

TEST_SUITE("Integration - Full - Publish / Subscribe") {
	TEST_CASE(
		"Multi-topic single subscribe receives from all subscribed topics") {
		const auto& selection = getWsSelection();
		requireReachableSelection(selection, "WS");
		const auto& endpoint = selection.endpoint;

		MqttClient subscriber;
		MqttClient publisher;

		std::atomic<bool> subscriberConnected{ false };
		std::atomic<bool> publisherConnected{ false };
		std::atomic<int> subAckCount{ 0 };
		std::atomic<int> receivedOnTopic1{ 0 };
		std::atomic<int> receivedOnTopic2{ 0 };
		std::atomic<bool> allSubscribedOk{ false };

		subscriber.onConnectEvent().add(
			[&](const ConnectEventDetails& d, const ConnectAck&) {
				subscriberConnected.store(d.isSuccessful && d.hasReceivedAck);
			});

		publisher.onConnectEvent().add(
			[&](const ConnectEventDetails& d, const ConnectAck&) {
				publisherConnected.store(d.isSuccessful && d.hasReceivedAck);
			});

		const std::string topic1 = makeUniqueTopic("kmMqtt/it/full/multi/a");
		const std::string topic2 = makeUniqueTopic("kmMqtt/it/full/multi/b");

		subscriber.onSubscribeAckEvent().add(
			[&](const SubscribeAckEventDetails& details, const SubscribeAck&) {
				allSubscribedOk.store(details.results.allSubscribedSuccesfully());
				subAckCount.fetch_add(1);
			});

		subscriber.onPublishEvent().add(
			[&](const PublishEventDetails& details, const Publish&) {
				if (details.topic == topic1)
					receivedOnTopic1.fetch_add(1);
				if (details.topic == topic2)
					receivedOnTopic2.fetch_add(1);
			});

		REQUIRE(
			subscriber
			.connect(makeConnectArgs("sub_full_multi"), makeAddress(endpoint))
			.noError());
		REQUIRE(waitFor(subscriberConnected, endpoint.timeoutSec));

		REQUIRE(
			publisher
			.connect(makeConnectArgs("pub_full_multi"), makeAddress(endpoint))
			.noError());
		REQUIRE(waitFor(publisherConnected, endpoint.timeoutSec));

		std::vector<Topic> topics{ Topic{topic1}, Topic{topic2} };
		REQUIRE(subscriber.subscribe(topics, SubscribeOptions{}).noError());
		REQUIRE(waitForAtLeast(subAckCount, 1, endpoint.timeoutSec));
		CHECK(allSubscribedOk.load());

		const std::string payloadText = "multi_payload";
		auto buildPayload = [&]() {
			ByteBuffer p(payloadText.size());
			p.append(reinterpret_cast<const std::uint8_t*>(payloadText.data()),
				payloadText.size());
			return p;
			};

		PublishOptions options;
		options.qos = Qos::QOS_0;
		REQUIRE(publisher.publish(topic1.c_str(), buildPayload(), PublishOptions{})
			.noError());
		REQUIRE(publisher.publish(topic2.c_str(), buildPayload(), PublishOptions{})
			.noError());

		REQUIRE(waitForAtLeast(receivedOnTopic1, 1, endpoint.timeoutSec));
		REQUIRE(waitForAtLeast(receivedOnTopic2, 1, endpoint.timeoutSec));

		CHECK(subscriber.disconnect(DisconnectArgs{ true }).noError());
		CHECK(publisher.disconnect(DisconnectArgs{ true }).noError());
		subscriber.shutdown();
		publisher.shutdown();
	}

	TEST_CASE("Wildcard + subscribe delivers from matching single-level topic") {
		const auto& selection = getWsSelection();
		requireReachableSelection(selection, "WS");
		const auto& endpoint = selection.endpoint;

		MqttClient subscriber;
		MqttClient publisher;

		std::atomic<bool> subscriberConnected{ false };
		std::atomic<bool> publisherConnected{ false };
		std::atomic<int> subAckCount{ 0 };
		std::atomic<int> publishReceivedCount{ 0 };
		std::atomic<bool> allSubscribedOk{ false };

		subscriber.onConnectEvent().add(
			[&](const ConnectEventDetails& d, const ConnectAck&) {
				subscriberConnected.store(d.isSuccessful && d.hasReceivedAck);
			});

		publisher.onConnectEvent().add(
			[&](const ConnectEventDetails& d, const ConnectAck&) {
				publisherConnected.store(d.isSuccessful && d.hasReceivedAck);
			});

		// Unique base path to avoid cross-test interference.
		const auto nowMs = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch())
			.count();
		const std::string base =
			"kmMqtt/it/full/wild_plus/" + std::to_string(nowMs);
		const std::string pattern = base + "/+/test";
		const std::string pubTopic = base + "/foo/test";

		subscriber.onSubscribeAckEvent().add(
			[&](const SubscribeAckEventDetails& details, const SubscribeAck&) {
				allSubscribedOk.store(details.results.allSubscribedSuccesfully());
				subAckCount.fetch_add(1);
			});

		subscriber.onPublishEvent().add(
			[&](const PublishEventDetails&, const Publish&) {
				publishReceivedCount.fetch_add(1);
			});

		REQUIRE(
			subscriber
			.connect(makeConnectArgs("sub_full_wplus"), makeAddress(endpoint))
			.noError());
		REQUIRE(waitFor(subscriberConnected, endpoint.timeoutSec));

		REQUIRE(
			publisher
			.connect(makeConnectArgs("pub_full_wplus"), makeAddress(endpoint))
			.noError());
		REQUIRE(waitFor(publisherConnected, endpoint.timeoutSec));

		std::vector<Topic> topics{ Topic{pattern} };
		REQUIRE(subscriber.subscribe(topics, SubscribeOptions{}).noError());
		REQUIRE(waitForAtLeast(subAckCount, 1, endpoint.timeoutSec));
		CHECK(allSubscribedOk.load());

		const std::string payloadText = "wildcard_plus";
		ByteBuffer payload(payloadText.size());
		payload.append(reinterpret_cast<const std::uint8_t*>(payloadText.data()),
			payloadText.size());
		REQUIRE(publisher
			.publish(pubTopic.c_str(), std::move(payload), PublishOptions{})
			.noError());

		REQUIRE(waitForAtLeast(publishReceivedCount, 1, endpoint.timeoutSec));

		CHECK(subscriber.disconnect(DisconnectArgs{ true }).noError());
		CHECK(publisher.disconnect(DisconnectArgs{ true }).noError());
		subscriber.shutdown();
		publisher.shutdown();
	}

	TEST_CASE("Wildcard # subscribe delivers from matching multi-level topic") {
		const auto& selection = getWsSelection();
		requireReachableSelection(selection, "WS");
		const auto& endpoint = selection.endpoint;

		MqttClient subscriber;
		MqttClient publisher;

		std::atomic<bool> subscriberConnected{ false };
		std::atomic<bool> publisherConnected{ false };
		std::atomic<int> subAckCount{ 0 };
		std::atomic<int> publishReceivedCount{ 0 };
		std::atomic<bool> allSubscribedOk{ false };

		subscriber.onConnectEvent().add(
			[&](const ConnectEventDetails& d, const ConnectAck&) {
				subscriberConnected.store(d.isSuccessful && d.hasReceivedAck);
			});

		publisher.onConnectEvent().add(
			[&](const ConnectEventDetails& d, const ConnectAck&) {
				publisherConnected.store(d.isSuccessful && d.hasReceivedAck);
			});

		const auto nowMs = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch())
			.count();
		const std::string base =
			"kmMqtt/it/full/wild_hash/" + std::to_string(nowMs);
		const std::string pattern = base + "/#";
		const std::string pubTopic = base + "/a/b/c";

		subscriber.onSubscribeAckEvent().add(
			[&](const SubscribeAckEventDetails& details, const SubscribeAck&) {
				allSubscribedOk.store(details.results.allSubscribedSuccesfully());
				subAckCount.fetch_add(1);
			});

		subscriber.onPublishEvent().add(
			[&](const PublishEventDetails&, const Publish&) {
				publishReceivedCount.fetch_add(1);
			});

		REQUIRE(
			subscriber
			.connect(makeConnectArgs("sub_full_whash"), makeAddress(endpoint))
			.noError());
		REQUIRE(waitFor(subscriberConnected, endpoint.timeoutSec));

		REQUIRE(
			publisher
			.connect(makeConnectArgs("pub_full_whash"), makeAddress(endpoint))
			.noError());
		REQUIRE(waitFor(publisherConnected, endpoint.timeoutSec));

		std::vector<Topic> topics{ Topic{pattern} };
		REQUIRE(subscriber.subscribe(topics, SubscribeOptions{}).noError());
		REQUIRE(waitForAtLeast(subAckCount, 1, endpoint.timeoutSec));
		CHECK(allSubscribedOk.load());

		const std::string payloadText = "wildcard_hash";
		ByteBuffer payload(payloadText.size());
		payload.append(reinterpret_cast<const std::uint8_t*>(payloadText.data()),
			payloadText.size());
		REQUIRE(publisher
			.publish(pubTopic.c_str(), std::move(payload), PublishOptions{})
			.noError());

		REQUIRE(waitForAtLeast(publishReceivedCount, 1, endpoint.timeoutSec));

		CHECK(subscriber.disconnect(DisconnectArgs{ true }).noError());
		CHECK(publisher.disconnect(DisconnectArgs{ true }).noError());
		subscriber.shutdown();
		publisher.shutdown();
	}

	TEST_CASE("Duplicate subscribe to same topic returns additional SUBACK") {
		const auto& selection = getWsSelection();
		requireReachableSelection(selection, "WS");
		const auto& endpoint = selection.endpoint;

		MqttClient client;

		std::atomic<bool> connected{ false };
		std::atomic<int> subAckCount{ 0 };

		client.onConnectEvent().add(
			[&](const ConnectEventDetails& d, const ConnectAck&) {
				connected.store(d.isSuccessful && d.hasReceivedAck);
			});

		client.onSubscribeAckEvent().add(
			[&](const SubscribeAckEventDetails&, const SubscribeAck&) {
				subAckCount.fetch_add(1);
			});

		REQUIRE(
			client.connect(makeConnectArgs("full_dup_sub"), makeAddress(endpoint))
			.noError());
		REQUIRE(waitFor(connected, endpoint.timeoutSec));
		REQUIRE(connected.load());

		const std::string topic = makeUniqueTopic("kmMqtt/it/full/dup");
		std::vector<Topic> topics{ Topic{topic} };

		REQUIRE(client.subscribe(topics, SubscribeOptions{}).noError());
		REQUIRE(waitForAtLeast(subAckCount, 1, endpoint.timeoutSec));

		// Second subscribe to the same topic.
		REQUIRE(client.subscribe(topics, SubscribeOptions{}).noError());
		REQUIRE(waitForAtLeast(subAckCount, 2, endpoint.timeoutSec));

		CHECK(client.disconnect(DisconnectArgs{ true }).noError());
		client.shutdown();
	}

	TEST_CASE("Unsubscribe from unknown topic returns UNSUBACK without error") {
		const auto& selection = getWsSelection();
		requireReachableSelection(selection, "WS");
		const auto& endpoint = selection.endpoint;

		MqttClient client;

		std::atomic<bool> connected{ false };
		std::atomic<int> unSubAckCount{ 0 };
		std::atomic<bool> unSubAckHasNoError{ false };

		client.onConnectEvent().add(
			[&](const ConnectEventDetails& d, const ConnectAck&) {
				connected.store(d.isSuccessful && d.hasReceivedAck);
			});

		client.onUnSubscribeAckEvent().add(
			[&](const UnSubscribeAckEventDetails& details, const UnSubscribeAck&) {
				// SUCCESS or NO_SUBSCRIPTION_EXISTED are both acceptable.
				const bool ok = details.results.allUnSubscribedSuccesfully();
				unSubAckHasNoError.store(ok);
				unSubAckCount.fetch_add(1);
			});

		REQUIRE(client
			.connect(makeConnectArgs("full_unsub_unknown"),
				makeAddress(endpoint))
			.noError());
		REQUIRE(waitFor(connected, endpoint.timeoutSec));
		REQUIRE(connected.load());

		const std::string topic = makeUniqueTopic("kmMqtt/it/full/unsub_unk");
		std::vector<Topic> topics{ Topic{topic} };

		// Unsubscribe from a topic that was never subscribed to.
		REQUIRE(client.unSubscribe(topics, UnSubscribeOptions{}).noError());
		REQUIRE(waitForAtLeast(unSubAckCount, 1, endpoint.timeoutSec));

		// Some brokers return SUCCESS (0x00), others NO_SUBSCRIPTION_EXISTED
		// (0x11). Either is fine — the API should surface the event.
		// allUnSubscribedSuccesfully() may return false for
		// NO_SUBSCRIPTION_EXISTED; just check the event fired.
		CHECK(unSubAckCount.load() >= 1);

		CHECK(client.disconnect(DisconnectArgs{ true }).noError());
		client.shutdown();
	}
}
