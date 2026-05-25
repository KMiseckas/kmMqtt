// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

// ---------------------------------------------------------------------------
// Integration tests — Connect / Disconnect
//
// These tests connect to a real MQTT broker (HiveMQ public broker by default)
// and validate end-to-end connect / disconnect behaviour.
//
// The broker address is configurable at CMake configuration time:
//   cmake -DKMMQTT_IT_BROKER_HOST=<host> -DKMMQTT_IT_BROKER_PORT=<port> ...
//
// To target a local Mosquitto or HiveMQ container:
//   docker run --rm -p 8000:8000 hivemq/hivemq4
//   cmake -DKMMQTT_IT_BROKER_HOST=localhost -DKMMQTT_IT_BROKER_PORT=8000 ...
// ---------------------------------------------------------------------------

#include <doctest.h>

#include <kmMqtt/MqttClient.h>
#include <kmMqtt/Mqtt/Params/ConnectArgs.h>
#include <kmMqtt/Mqtt/Params/ConnectAddress.h>
#include <kmMqtt/Mqtt/Params/DisconnectArgs.h>
#include <kmMqtt/Mqtt/MqttClientEvents.h>

#include <atomic>
#include <chrono>
#include <string>
#include <thread>

#include "BrokerConfig.h"

using namespace kmMqtt;
using namespace kmMqtt::mqtt;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

namespace
{
    /// Returns a ConnectAddress pointing at the configured test broker.
    ConnectAddress makeBrokerAddress()
    {
        ConnectAddress address;
        address.primaryAddress = Address::createURL(
            kmMqtt_it::BrokerConfig::scheme,
            kmMqtt_it::BrokerConfig::host,
            kmMqtt_it::BrokerConfig::port,
            kmMqtt_it::BrokerConfig::path
        );
        return address;
    }

    /// Builds minimal MQTT 5.0 connect args with a unique-enough client id.
    ConnectArgs makeConnectArgs(const std::string& clientIdSuffix = "")
    {
        ConnectArgs args{ "kmMqtt_it_" + clientIdSuffix };
        args.protocolName  = "MQTT";
        args.version       = MqttVersion::MQTT_5_0;
        args.cleanStart    = true;
        args.keepAliveInSec = 60;
        return args;
    }

    /// Blocks until \p flag becomes true or the configured timeout elapses.
    /// Returns true if the flag was set before the timeout.
    bool waitFor(std::atomic<bool>& flag)
    {
        const auto deadline = std::chrono::steady_clock::now()
            + std::chrono::seconds(kmMqtt_it::BrokerConfig::timeoutSec);

        while (!flag.load())
        {
            if (std::chrono::steady_clock::now() >= deadline)
            {
                return false;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        return true;
    }
} // anonymous namespace

// ---------------------------------------------------------------------------
// Test suite
// ---------------------------------------------------------------------------

TEST_SUITE("Integration - Connect / Disconnect")
{
    // -----------------------------------------------------------------------
    TEST_CASE("Connect to broker and receive CONNACK")
    // Verifies that a plain MQTT 5.0 connect over WebSocket results in a
    // successful CONNACK from the broker.
    // -----------------------------------------------------------------------
    {
        MqttClient client;

        std::atomic<bool> connectEventFired{ false };
        std::atomic<bool> connectSuccess{ false };

        client.onConnectEvent().add(
            [&](const ConnectEventDetails& details, const ConnectAck& /*ack*/)
            {
                connectSuccess.store(details.isSuccessful && details.hasReceivedAck);
                connectEventFired.store(true);
            });

        auto result = client.connect(makeConnectArgs("connect_test"), makeBrokerAddress());
        REQUIRE_MESSAGE(result.noError(),
            "connect() returned an error before reaching the broker: "
            << static_cast<int>(result.errorCode()));

        const bool timedOut = !waitFor(connectEventFired);
        REQUIRE_MESSAGE(!timedOut,
            "Timed out waiting for CONNACK from "
            << kmMqtt_it::BrokerConfig::host << ":" << kmMqtt_it::BrokerConfig::port
            << " (timeout = " << kmMqtt_it::BrokerConfig::timeoutSec << "s). "
            "Check network connectivity and broker availability.");

        CHECK(connectSuccess.load());

        client.shutdown();
    }

    // -----------------------------------------------------------------------
    TEST_CASE("Connect then disconnect gracefully")
    // Verifies that after a successful connect the client can send a DISCONNECT
    // packet and receives the corresponding local disconnect event.
    // -----------------------------------------------------------------------
    {
        MqttClient client;

        std::atomic<bool> connectEventFired{ false };
        std::atomic<bool> connectSuccess{ false };
        std::atomic<bool> disconnectEventFired{ false };
        std::atomic<bool> disconnectGraceful{ false };

        client.onConnectEvent().add(
            [&](const ConnectEventDetails& details, const ConnectAck& /*ack*/)
            {
                connectSuccess.store(details.isSuccessful && details.hasReceivedAck);
                connectEventFired.store(true);
            });

        client.onDisconnectEvent().add(
            [&](const DisconnectEventDetails& details)
            {
                disconnectGraceful.store(details.isGraceful);
                disconnectEventFired.store(true);
            });

        auto connectResult = client.connect(makeConnectArgs("disconnect_test"), makeBrokerAddress());
        REQUIRE_MESSAGE(connectResult.noError(),
            "connect() returned an error before reaching the broker: "
            << static_cast<int>(connectResult.errorCode()));

        // Wait for CONNACK
        {
            const bool timedOut = !waitFor(connectEventFired);
            REQUIRE_MESSAGE(!timedOut,
                "Timed out waiting for CONNACK from "
                << kmMqtt_it::BrokerConfig::host << ":" << kmMqtt_it::BrokerConfig::port
                << " (timeout = " << kmMqtt_it::BrokerConfig::timeoutSec << "s). "
                "Check network connectivity and broker availability.");
            REQUIRE(connectSuccess.load());
        }

        // Graceful disconnect
        DisconnectArgs disconnectArgs{ true /*graceful*/ };
        auto disconnectResult = client.disconnect(std::move(disconnectArgs));
        CHECK_MESSAGE(disconnectResult.noError(),
            "disconnect() returned an error: "
            << static_cast<int>(disconnectResult.errorCode()));

        // Wait for disconnect event
        {
            const bool timedOut = !waitFor(disconnectEventFired);
            CHECK_MESSAGE(!timedOut,
                "Timed out waiting for the disconnect event "
                "(timeout = " << kmMqtt_it::BrokerConfig::timeoutSec << "s).");
        }

        CHECK(disconnectGraceful.load());

        client.shutdown();
    }

    // -----------------------------------------------------------------------
    TEST_CASE("Connect then disconnect non-gracefully")
    // Verifies that a non-graceful (abortive) disconnect closes the socket
    // and fires the disconnect event without a DISCONNECT packet being sent.
    // -----------------------------------------------------------------------
    {
        MqttClient client;

        std::atomic<bool> connectEventFired{ false };
        std::atomic<bool> connectSuccess{ false };
        std::atomic<bool> disconnectEventFired{ false };

        client.onConnectEvent().add(
            [&](const ConnectEventDetails& details, const ConnectAck& /*ack*/)
            {
                connectSuccess.store(details.isSuccessful && details.hasReceivedAck);
                connectEventFired.store(true);
            });

        client.onDisconnectEvent().add(
            [&](const DisconnectEventDetails& /*details*/)
            {
                disconnectEventFired.store(true);
            });

        auto connectResult = client.connect(makeConnectArgs("abort_test"), makeBrokerAddress());
        REQUIRE_MESSAGE(connectResult.noError(),
            "connect() returned an error before reaching the broker: "
            << static_cast<int>(connectResult.errorCode()));

        {
            const bool timedOut = !waitFor(connectEventFired);
            REQUIRE_MESSAGE(!timedOut,
                "Timed out waiting for CONNACK from "
                << kmMqtt_it::BrokerConfig::host << ":" << kmMqtt_it::BrokerConfig::port
                << " (timeout = " << kmMqtt_it::BrokerConfig::timeoutSec << "s). "
                "Check network connectivity and broker availability.");
            REQUIRE(connectSuccess.load());
        }

        DisconnectArgs disconnectArgs{ false /*non-graceful*/ };
        auto disconnectResult = client.disconnect(std::move(disconnectArgs));
        CHECK_MESSAGE(disconnectResult.noError(),
            "disconnect() returned an error: "
            << static_cast<int>(disconnectResult.errorCode()));

        {
            const bool timedOut = !waitFor(disconnectEventFired);
            CHECK_MESSAGE(!timedOut,
                "Timed out waiting for the disconnect event "
                "(timeout = " << kmMqtt_it::BrokerConfig::timeoutSec << "s).");
        }

        client.shutdown();
    }
}
