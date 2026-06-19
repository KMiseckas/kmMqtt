// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

// ---------------------------------------------------------------------------
// Integration tests — stability and edge-case session scenarios
//
// Full suite: connection cycling, QoS 2 flow, burst QoS 1, keepalive,
//             burst QoS 2, and SYNC tick mode.
// ---------------------------------------------------------------------------

#include <doctest.h>

#include <kmMqtt/ByteBuffer.h>
#include <kmMqtt/Environments/DefaultEnvironmentFactory.h>
#include <kmMqtt/Interfaces/IMqttEnvironment.h>
#include <kmMqtt/Mqtt/MqttClientEvents.h>
#include <kmMqtt/Mqtt/Params/ConnectArgs.h>
#include <kmMqtt/Mqtt/Params/DisconnectArgs.h>
#include <kmMqtt/Mqtt/Params/PublishOptions.h>
#include <kmMqtt/Mqtt/Params/SubscribeOptions.h>
#include <kmMqtt/Mqtt/Params/Topic.h>
#include <kmMqtt/Mqtt/Params/UnSubscribeOptions.h>
#include <kmMqtt/Mqtt/State/SubAckTopicReason.h>
#include <kmMqtt/MqttClient.h>
#include <kmMqtt/MqttClientOptions.h>

#include <atomic>
#include <chrono>
#include <cstring>
#include <memory>
#include <kmMqtt/STL/KmString.h>
#include <kmMqtt/STL/KmVector.h>

#include "BrokerConfig.h"
#include "Helpers.h"
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
// Helpers local to this file
// ---------------------------------------------------------------------------

namespace {

	void gracefulConnectDisconnect(const BrokerEndpoint& ep,
		const kmMqtt::kmStd::string& tag) {
		MqttClient client;

		std::atomic<bool> connectFired{ false };
		std::atomic<bool> connectOk{ false };
		std::atomic<bool> disconnectFired{ false };
		std::atomic<bool> disconnectGraceful{ false };

		client.onConnectEvent().add(
			[&](const ConnectEventDetails& d, const ConnectAck&) {
				connectOk.store(d.isSuccessful && d.hasReceivedAck);
				connectFired.store(true);
			});

		client.onDisconnectEvent().add([&](const DisconnectEventDetails& d) {
			disconnectGraceful.store(d.isGraceful);
			disconnectFired.store(true);
			});

		auto cr = client.connect(makeConnectArgs(tag), makeAddress(ep));
		REQUIRE_MESSAGE(cr.noError(),
			"connect() error: " << static_cast<int>(cr.errorCode()));
		REQUIRE_MESSAGE(waitFor(connectFired, ep.timeoutSec),
			"Timed out waiting for CONNACK (timeout=" << ep.timeoutSec
			<< "s).");
		REQUIRE(connectOk.load());

		auto dr = client.disconnect(DisconnectArgs{ true });
		CHECK_MESSAGE(dr.noError(),
			"disconnect() error: " << static_cast<int>(dr.errorCode()));
		CHECK_MESSAGE(waitFor(disconnectFired, ep.timeoutSec),
			"Timed out waiting for disconnect event.");
		CHECK(disconnectGraceful.load());

		client.shutdown();
	}

} // anonymous namespace

// ---------------------------------------------------------------------------
// Full suite
// ---------------------------------------------------------------------------

TEST_SUITE("Integration - Full - Stability") {
	TEST_CASE("Repeated WS connect and graceful disconnect remains stable") {
		const auto& selection = getWsSelection();
		requireReachableSelection(selection, "WS");

		for (int i = 0; i < 8; ++i) {
			gracefulConnectDisconnect(selection.endpoint,
				"ws_full_loop_" + kmMqtt::kmStd::to_string(i));
		}
	}

	TEST_CASE("QoS 2 publish completes PUBREC PUBCOMP flow and subscriber "
		"receives payload") {
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
		std::atomic<bool> sawPubRec{ false };
		std::atomic<bool> sawPubComp{ false };
		std::atomic<bool> payloadMatches{ false };
		std::atomic<bool> allSubscribedOk{ false };

		subscriber.onConnectEvent().add(
			[&](const ConnectEventDetails& d, const ConnectAck&) {
				subscriberConnected.store(d.isSuccessful && d.hasReceivedAck);
			});

		publisher.onConnectEvent().add(
			[&](const ConnectEventDetails& d, const ConnectAck&) {
				publisherConnected.store(d.isSuccessful && d.hasReceivedAck);
			});

		const kmMqtt::kmStd::string topic = makeUniqueTopic("kmMqtt/it/full/qos2");
		const kmMqtt::kmStd::string payloadText = "full_qos2_payload";

		subscriber.onSubscribeAckEvent().add(
			[&](const SubscribeAckEventDetails& details, const SubscribeAck&) {
				allSubscribedOk.store(details.results.allSubscribedSuccesfully());
				subAckCount.fetch_add(1);
			});

		subscriber.onUnSubscribeAckEvent().add(
			[&](const UnSubscribeAckEventDetails&, const UnSubscribeAck&) {
				unSubAckCount.fetch_add(1);
			});

		subscriber.onPublishEvent().add(
			[&](const PublishEventDetails& details, const Publish&) {
				if (details.payload != nullptr &&
					details.payload->size() == payloadText.size() &&
					std::memcmp(details.payload->bytes(), payloadText.data(),
						payloadText.size()) == 0)
					payloadMatches.store(true);

				if (details.topic == topic)
					publishReceivedCount.fetch_add(1);
			});

		publisher.onPublishCompletedEvent().add(
			[&](const PublishCompleteEventDetails& details) {
				if (details.packetType == PacketType::PUBLISH_RECEIVED &&
					details.isSuccess())
					sawPubRec.store(true);

				if (details.packetType == PacketType::PUBLISH_COMPLETE &&
					details.isSuccess())
					sawPubComp.store(true);
			});

		REQUIRE(
			subscriber
			.connect(makeConnectArgs("sub_full_qos2"), makeAddress(endpoint))
			.noError());
		REQUIRE(waitFor(subscriberConnected, endpoint.timeoutSec));

		REQUIRE(
			publisher
			.connect(makeConnectArgs("pub_full_qos2"), makeAddress(endpoint))
			.noError());
		REQUIRE(waitFor(publisherConnected, endpoint.timeoutSec));

		kmMqtt::kmStd::vector<Topic> topics{
			Topic{topic, TopicSubscriptionOptions{Qos::QOS_2}} };
		REQUIRE(subscriber.subscribe(topics, SubscribeOptions{}).noError());
		REQUIRE(waitForAtLeast(subAckCount, 1, endpoint.timeoutSec));
		CHECK(allSubscribedOk.load());

		ByteBuffer payload(payloadText.size());
		payload.append(reinterpret_cast<const std::uint8_t*>(payloadText.data()),
			payloadText.size());

		PublishOptions options;
		options.qos = Qos::QOS_2;
		REQUIRE(
			publisher.publish(topic.c_str(), std::move(payload), std::move(options))
			.noError());

		REQUIRE(waitFor(sawPubRec, endpoint.timeoutSec));
		REQUIRE(waitFor(sawPubComp, endpoint.timeoutSec));
		REQUIRE(waitForAtLeast(publishReceivedCount, 1, endpoint.timeoutSec));
		CHECK(payloadMatches.load());

		REQUIRE(subscriber.unSubscribe(topics, UnSubscribeOptions{}).noError());
		REQUIRE(waitForAtLeast(unSubAckCount, 1, endpoint.timeoutSec));

		CHECK(subscriber.disconnect(DisconnectArgs{ true }).noError());
		CHECK(publisher.disconnect(DisconnectArgs{ true }).noError());
		subscriber.shutdown();
		publisher.shutdown();
	}

	TEST_CASE("Burst QoS 1 publish load completes acknowledgements and "
		"deliveries") {
		const auto& selection = getWsSelection();
		requireReachableSelection(selection, "WS");
		const auto& endpoint = selection.endpoint;

		static constexpr int kBurstCount = 20;

		MqttClient subscriber;
		MqttClient publisher;

		std::atomic<bool> subscriberConnected{ false };
		std::atomic<bool> publisherConnected{ false };
		std::atomic<int> subAckCount{ 0 };
		std::atomic<int> unSubAckCount{ 0 };
		std::atomic<int> publishAckCount{ 0 };
		std::atomic<int> publishReceivedCount{ 0 };

		subscriber.onConnectEvent().add(
			[&](const ConnectEventDetails& d, const ConnectAck&) {
				subscriberConnected.store(d.isSuccessful && d.hasReceivedAck);
			});

		publisher.onConnectEvent().add(
			[&](const ConnectEventDetails& d, const ConnectAck&) {
				publisherConnected.store(d.isSuccessful && d.hasReceivedAck);
			});

		subscriber.onSubscribeAckEvent().add(
			[&](const SubscribeAckEventDetails&, const SubscribeAck&) {
				subAckCount.fetch_add(1);
			});

		subscriber.onUnSubscribeAckEvent().add(
			[&](const UnSubscribeAckEventDetails&, const UnSubscribeAck&) {
				unSubAckCount.fetch_add(1);
			});

		subscriber.onPublishEvent().add(
			[&](const PublishEventDetails&, const Publish&) {
				publishReceivedCount.fetch_add(1);
			});

		publisher.onPublishCompletedEvent().add(
			[&](const PublishCompleteEventDetails& details) {
				if (details.packetType == PacketType::PUBLISH_ACKNOWLEDGE &&
					details.isSuccess())
					publishAckCount.fetch_add(1);
			});

		const kmMqtt::kmStd::string topic = makeUniqueTopic("kmMqtt/it/full/burst_qos1");

		REQUIRE(
			subscriber
			.connect(makeConnectArgs("sub_full_burst1"), makeAddress(endpoint))
			.noError());
		REQUIRE(waitFor(subscriberConnected, endpoint.timeoutSec));

		REQUIRE(
			publisher
			.connect(makeConnectArgs("pub_full_burst1"), makeAddress(endpoint))
			.noError());
		REQUIRE(waitFor(publisherConnected, endpoint.timeoutSec));

		kmMqtt::kmStd::vector<Topic> topics{ Topic{topic} };
		REQUIRE(subscriber.subscribe(topics, SubscribeOptions{}).noError());
		REQUIRE(waitForAtLeast(subAckCount, 1, endpoint.timeoutSec));

		for (int i = 0; i < kBurstCount; ++i) {
			const kmMqtt::kmStd::string payloadText = "burst_qos1_" + kmMqtt::kmStd::to_string(i);
			ByteBuffer payload(payloadText.size());
			payload.append(reinterpret_cast<const std::uint8_t*>(payloadText.data()),
				payloadText.size());

			PublishOptions options;
			options.qos = Qos::QOS_1;
			REQUIRE(
				publisher
				.publish(topic.c_str(), std::move(payload), std::move(options))
				.noError());
		}

		const int ext = endpoint.timeoutSec * 2;
		REQUIRE(waitForAtLeast(publishAckCount, kBurstCount, ext));
		REQUIRE(waitForAtLeast(publishReceivedCount, kBurstCount, ext));

		REQUIRE(subscriber.unSubscribe(topics, UnSubscribeOptions{}).noError());
		REQUIRE(waitForAtLeast(unSubAckCount, 1, endpoint.timeoutSec));

		CHECK(subscriber.disconnect(DisconnectArgs{ true }).noError());
		CHECK(publisher.disconnect(DisconnectArgs{ true }).noError());
		subscriber.shutdown();
		publisher.shutdown();
	}

	TEST_CASE("Keepalive idle period remains connected") {
		const auto& selection = getWsSelection();
		requireReachableSelection(selection, "WS");
		const auto& endpoint = selection.endpoint;

		MqttClient client;

		std::atomic<bool> connectFired{ false };
		std::atomic<bool> connectOk{ false };
		std::atomic<bool> disconnectFired{ false };

		client.onConnectEvent().add(
			[&](const ConnectEventDetails& details, const ConnectAck&) {
				connectOk.store(details.isSuccessful && details.hasReceivedAck);
				connectFired.store(true);
			});

		client.onDisconnectEvent().add(
			[&](const DisconnectEventDetails&) { disconnectFired.store(true); });

		auto args = makeConnectArgs("full_keepalive_idle");
		args.keepAliveInSec = 3;

		REQUIRE(client.connect(std::move(args), makeAddress(endpoint)).noError());
		REQUIRE(waitFor(connectFired, endpoint.timeoutSec));
		REQUIRE(connectOk.load());

		// After the negotiated keepalive period the client must send PINGREQ
		// proactively. Wait 2.5x the keepalive (7.5s) to allow at least two
		// ping cycles and confirm the broker has not closed the session.
		kmStd::this_thread::sleep_for(std::chrono::seconds(8));

		CHECK(client.getConnectionStatus() == ConnectionStatus::CONNECTED);
		CHECK(!disconnectFired.load());

		// pingInterval should be set from the negotiated keepalive.
		CHECK(client.getConnectionInfo().pingInterval.count() > 0);

		CHECK(client.disconnect(DisconnectArgs{ true }).noError());
		client.shutdown();
	}

	TEST_CASE("Burst QoS 2 publish load completes all PUBREC PUBCOMP handshakes "
		"and deliveries") {
		const auto& selection = getWsSelection();
		requireReachableSelection(selection, "WS");
		const auto& endpoint = selection.endpoint;

		// Conservative count for public broker rate limits.
		static constexpr int kBurstCount = 5;

		MqttClient subscriber;
		MqttClient publisher;

		std::atomic<bool> subscriberConnected{ false };
		std::atomic<bool> publisherConnected{ false };
		std::atomic<int> subAckCount{ 0 };
		std::atomic<int> unSubAckCount{ 0 };
		std::atomic<int> pubCompCount{ 0 };
		std::atomic<int> publishReceivedCount{ 0 };

		subscriber.onConnectEvent().add(
			[&](const ConnectEventDetails& d, const ConnectAck&) {
				subscriberConnected.store(d.isSuccessful && d.hasReceivedAck);
			});

		publisher.onConnectEvent().add(
			[&](const ConnectEventDetails& d, const ConnectAck&) {
				publisherConnected.store(d.isSuccessful && d.hasReceivedAck);
			});

		subscriber.onSubscribeAckEvent().add(
			[&](const SubscribeAckEventDetails&, const SubscribeAck&) {
				subAckCount.fetch_add(1);
			});

		subscriber.onUnSubscribeAckEvent().add(
			[&](const UnSubscribeAckEventDetails&, const UnSubscribeAck&) {
				unSubAckCount.fetch_add(1);
			});

		subscriber.onPublishEvent().add(
			[&](const PublishEventDetails&, const Publish&) {
				publishReceivedCount.fetch_add(1);
			});

		publisher.onPublishCompletedEvent().add(
			[&](const PublishCompleteEventDetails& details) {
				if (details.packetType == PacketType::PUBLISH_COMPLETE &&
					details.isSuccess())
					pubCompCount.fetch_add(1);
			});

		const kmMqtt::kmStd::string topic = makeUniqueTopic("kmMqtt/it/full/burst_qos2");

		REQUIRE(
			subscriber
			.connect(makeConnectArgs("sub_full_burst2"), makeAddress(endpoint))
			.noError());
		REQUIRE(waitFor(subscriberConnected, endpoint.timeoutSec));

		REQUIRE(
			publisher
			.connect(makeConnectArgs("pub_full_burst2"), makeAddress(endpoint))
			.noError());
		REQUIRE(waitFor(publisherConnected, endpoint.timeoutSec));

		kmMqtt::kmStd::vector<Topic> topics{
			Topic{topic, TopicSubscriptionOptions{Qos::QOS_2}} };
		REQUIRE(subscriber.subscribe(topics, SubscribeOptions{}).noError());
		REQUIRE(waitForAtLeast(subAckCount, 1, endpoint.timeoutSec));

		for (int i = 0; i < kBurstCount; ++i) {
			const kmMqtt::kmStd::string payloadText = "burst_qos2_" + kmMqtt::kmStd::to_string(i);
			ByteBuffer payload(payloadText.size());
			payload.append(reinterpret_cast<const std::uint8_t*>(payloadText.data()),
				payloadText.size());

			PublishOptions options;
			options.qos = Qos::QOS_2;
			REQUIRE(
				publisher
				.publish(topic.c_str(), std::move(payload), std::move(options))
				.noError());
		}

		const int ext = endpoint.timeoutSec * 3;
		REQUIRE(waitForAtLeast(pubCompCount, kBurstCount, ext));
		REQUIRE(waitForAtLeast(publishReceivedCount, kBurstCount, ext));

		REQUIRE(subscriber.unSubscribe(topics, UnSubscribeOptions{}).noError());
		REQUIRE(waitForAtLeast(unSubAckCount, 1, endpoint.timeoutSec));

		CHECK(subscriber.disconnect(DisconnectArgs{ true }).noError());
		CHECK(publisher.disconnect(DisconnectArgs{ true }).noError());
		subscriber.shutdown();
		publisher.shutdown();
	}

	TEST_CASE("SYNC tick mode connect publish and disconnect works end to end") {
		const auto& selection = getWsSelection();
		requireReachableSelection(selection, "WS");
		const auto& endpoint = selection.endpoint;

		DefaultEnvironmentFactory envFactory;
		auto deleter = [&](IMqttEnvironment* env) { envFactory.deleteEnvironment(env); };
		kmMqtt::kmStd::unique_ptr<IMqttEnvironment, decltype(deleter)> env(envFactory.createEnvironment(), std::move(deleter));

		MqttClientOptions opts;
		opts.tickMode(TickMode::SYNC);

		MqttClient client{ env.get(), opts };

		CHECK(!client.getIsTickAsync());
		CHECK(client.getConnectionStatus() == ConnectionStatus::DISCONNECTED);

		std::atomic<bool> connectFired{ false };
		std::atomic<bool> connectOk{ false };
		std::atomic<bool> disconnectFired{ false };
		std::atomic<bool> disconnectGraceful{ false };

		client.onConnectEvent().add(
			[&](const ConnectEventDetails& d, const ConnectAck&) {
				connectOk.store(d.isSuccessful && d.hasReceivedAck);
				connectFired.store(true);
			});

		client.onDisconnectEvent().add([&](const DisconnectEventDetails& d) {
			disconnectGraceful.store(d.isGraceful);
			disconnectFired.store(true);
			});

		const auto connectResult = client.connect(makeConnectArgs("full_sync_tick"),
			makeAddress(endpoint));
		REQUIRE(connectResult.noError());

		// Tick until CONNACK received.
		const auto connectDeadline = std::chrono::steady_clock::now() +
			std::chrono::seconds(endpoint.timeoutSec);
		while (!connectFired.load() &&
			std::chrono::steady_clock::now() < connectDeadline) {
			client.tick();
			kmStd::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		REQUIRE_MESSAGE(connectFired.load(),
			"Timed out waiting for CONNACK in SYNC tick mode "
			"(timeout="
			<< endpoint.timeoutSec << "s).");
		REQUIRE(connectOk.load());
		CHECK(client.getConnectionStatus() == ConnectionStatus::CONNECTED);

		// QoS 0 publish — fire and forget; just verify no error.
		const kmMqtt::kmStd::string topic = makeUniqueTopic("kmMqtt/it/full/sync");
		const kmMqtt::kmStd::string payloadText = "sync_tick_payload";
		ByteBuffer payload(payloadText.size());
		payload.append(reinterpret_cast<const std::uint8_t*>(payloadText.data()),
			payloadText.size());

		PublishOptions pubOpts;
		pubOpts.qos = Qos::QOS_0;
		REQUIRE(
			client.publish(topic.c_str(), std::move(payload), std::move(pubOpts))
			.noError());

		// Tick briefly to give the library a chance to send the packet.
		for (int i = 0; i < 20; ++i) {
			client.tick();
			kmStd::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		// Graceful disconnect.
		REQUIRE(client.disconnect(DisconnectArgs{ true }).noError());

		const auto disconnectDeadline = std::chrono::steady_clock::now() +
			std::chrono::seconds(endpoint.timeoutSec);
		while (!disconnectFired.load() &&
			std::chrono::steady_clock::now() < disconnectDeadline) {
			client.tick();
			kmStd::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		REQUIRE_MESSAGE(disconnectFired.load(),
			"Timed out waiting for disconnect event in SYNC tick "
			"mode (timeout="
			<< endpoint.timeoutSec << "s).");
		CHECK(disconnectGraceful.load());

		client.shutdown();
	}
}
