// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

// ---------------------------------------------------------------------------
// Integration tests — connect / disconnect lifecycle
//
// Smoke suite: WS and WSS connect, graceful and non-graceful disconnect,
//              basic connection-state API checks.
// ---------------------------------------------------------------------------

#include <doctest.h>

#include <kmMqtt/Mqtt/MqttClientEvents.h>
#include <kmMqtt/Mqtt/Params/ConnectAddress.h>
#include <kmMqtt/Mqtt/Params/ConnectArgs.h>
#include <kmMqtt/Mqtt/Params/DisconnectArgs.h>
#include <kmMqtt/MqttClient.h>

#include <atomic>
#include <string>

#include "BrokerConfig.h"
#include "Helpers.h"

using namespace kmMqtt;
using namespace kmMqtt::mqtt;
using namespace kmMqtt_it;

// ---------------------------------------------------------------------------
// Shared test bodies
// ---------------------------------------------------------------------------

namespace {

	void testConnect(const BrokerEndpoint& ep, const std::string& tag) {
		MqttClient client;

		std::atomic<bool> connectFired{ false };
		std::atomic<bool> connectOk{ false };

		client.onConnectEvent().add(
			[&](const ConnectEventDetails& d, const ConnectAck&) {
				connectOk.store(d.isSuccessful && d.hasReceivedAck);
				connectFired.store(true);
			});

		auto result = client.connect(makeConnectArgs(tag), makeAddress(ep));
		REQUIRE_MESSAGE(result.noError(),
			"connect() error before reaching broker ("
			<< ep.scheme << "://" << ep.host << ":" << ep.port
			<< ep.path
			<< "): " << static_cast<int>(result.errorCode()));

		const bool timedOut = !waitFor(connectFired, ep.timeoutSec);
		REQUIRE_MESSAGE(!timedOut,
			"Timed out waiting for CONNACK from "
			<< ep.scheme << "://" << ep.host << ":" << ep.port
			<< " (timeout=" << ep.timeoutSec
			<< "s). "
			"Check network connectivity and broker availability.");

		CHECK(connectOk.load());

		client.shutdown();
	}

	void testGracefulDisconnect(const BrokerEndpoint& ep, const std::string& tag) {
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

		auto connectResult = client.connect(makeConnectArgs(tag), makeAddress(ep));
		REQUIRE_MESSAGE(connectResult.noError(),
			"connect() error before reaching broker ("
			<< ep.scheme << "://" << ep.host << ":" << ep.port
			<< ep.path
			<< "): " << static_cast<int>(connectResult.errorCode()));

		REQUIRE_MESSAGE(waitFor(connectFired, ep.timeoutSec),
			"Timed out waiting for CONNACK from "
			<< ep.scheme << "://" << ep.host << ":" << ep.port
			<< " (timeout=" << ep.timeoutSec
			<< "s). "
			"Check network connectivity and broker availability.");
		REQUIRE(connectOk.load());

		auto disconnectResult = client.disconnect(DisconnectArgs{ true });
		CHECK_MESSAGE(
			disconnectResult.noError(),
			"disconnect() error: " << static_cast<int>(disconnectResult.errorCode()));

		CHECK_MESSAGE(waitFor(disconnectFired, ep.timeoutSec),
			"Timed out waiting for disconnect event (timeout="
			<< ep.timeoutSec << "s).");
		CHECK(disconnectGraceful.load());

		client.shutdown();
	}

	void testNonGracefulDisconnect(const BrokerEndpoint& ep,
		const std::string& tag) {
		MqttClient client;

		std::atomic<bool> connectFired{ false };
		std::atomic<bool> connectOk{ false };
		std::atomic<bool> disconnectFired{ false };

		client.onConnectEvent().add(
			[&](const ConnectEventDetails& d, const ConnectAck&) {
				connectOk.store(d.isSuccessful && d.hasReceivedAck);
				connectFired.store(true);
			});

		client.onDisconnectEvent().add(
			[&](const DisconnectEventDetails&) { disconnectFired.store(true); });

		auto connectResult = client.connect(makeConnectArgs(tag), makeAddress(ep));
		REQUIRE_MESSAGE(connectResult.noError(),
			"connect() error before reaching broker ("
			<< ep.scheme << "://" << ep.host << ":" << ep.port
			<< ep.path
			<< "): " << static_cast<int>(connectResult.errorCode()));

		REQUIRE_MESSAGE(waitFor(connectFired, ep.timeoutSec),
			"Timed out waiting for CONNACK from "
			<< ep.scheme << "://" << ep.host << ":" << ep.port
			<< " (timeout=" << ep.timeoutSec
			<< "s). "
			"Check network connectivity and broker availability.");
		REQUIRE(connectOk.load());

		auto disconnectResult = client.disconnect(DisconnectArgs{ false });
		CHECK_MESSAGE(
			disconnectResult.noError(),
			"disconnect() error: " << static_cast<int>(disconnectResult.errorCode()));

		CHECK_MESSAGE(waitFor(disconnectFired, ep.timeoutSec),
			"Timed out waiting for disconnect event (timeout="
			<< ep.timeoutSec << "s).");

		client.shutdown();
	}

} // anonymous namespace

// ---------------------------------------------------------------------------
// Smoke suites
// ---------------------------------------------------------------------------

TEST_SUITE("Integration - Smoke - WS - Connect / Disconnect") {
	TEST_CASE("Connect to broker and receive CONNACK") {
		const auto& selection = getWsSelection();
		requireReachableSelection(selection, "WS");
		testConnect(selection.endpoint, "ws_connect");
	}

	TEST_CASE("Connect then disconnect gracefully") {
		const auto& selection = getWsSelection();
		requireReachableSelection(selection, "WS");
		testGracefulDisconnect(selection.endpoint, "ws_graceful");
	}

	TEST_CASE("Connect then disconnect non-gracefully") {
		const auto& selection = getWsSelection();
		requireReachableSelection(selection, "WS");
		testNonGracefulDisconnect(selection.endpoint, "ws_abort");
	}
}

TEST_SUITE("Integration - Smoke - WSS - Connect / Disconnect") {
	TEST_CASE("Connect to broker and receive CONNACK") {
		const auto& selection = getWssSelection();
		requireReachableSelection(selection, "WSS");
		testConnect(selection.endpoint, "wss_connect");
	}

	TEST_CASE("Connect then disconnect gracefully") {
		const auto& selection = getWssSelection();
		requireReachableSelection(selection, "WSS");
		testGracefulDisconnect(selection.endpoint, "wss_graceful");
	}

	TEST_CASE("Connect then disconnect non-gracefully") {
		const auto& selection = getWssSelection();
		requireReachableSelection(selection, "WSS");
		testNonGracefulDisconnect(selection.endpoint, "wss_abort");
	}
}

TEST_SUITE("Integration - Smoke - API") {
	TEST_CASE(
		"Default client is async and exposes connection info after connect") {
		const auto& selection = getWsSelection();
		requireReachableSelection(selection, "WS");
		const auto& endpoint = selection.endpoint;

		MqttClient client;
		CHECK(client.getIsTickAsync());
		CHECK(client.getConnectionStatus() == ConnectionStatus::DISCONNECTED);

		const std::string clientTag = "api_state";
		const std::string expectedClientIdPrefix = "kmMqtt_it_" + clientTag + "_";

		std::atomic<bool> connectFired{ false };
		std::atomic<bool> connectOk{ false };

		client.onConnectEvent().add(
			[&](const ConnectEventDetails& details, const ConnectAck&) {
				connectOk.store(details.isSuccessful && details.hasReceivedAck);
				connectFired.store(true);
			});

		auto connectResult =
			client.connect(makeConnectArgs(clientTag), makeAddress(endpoint));
		REQUIRE(connectResult.noError());
		REQUIRE(waitFor(connectFired, endpoint.timeoutSec));
		REQUIRE(connectOk.load());

		CHECK(client.getConnectionStatus() == ConnectionStatus::CONNECTED);
		const auto& clientId = client.getConnectionInfo().connectArgs.clientId;
		CHECK(clientId.find(expectedClientIdPrefix) == 0);
		CHECK(client.getConnectionInfo().connectAddress.primaryAddress.hostname() ==
			endpoint.host);

		auto disconnectResult = client.disconnect(DisconnectArgs{ true });
		CHECK(disconnectResult.noError());
		client.shutdown();
	}
}
