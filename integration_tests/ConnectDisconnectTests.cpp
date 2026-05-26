// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

// ---------------------------------------------------------------------------
// Integration tests — Connect / Disconnect
//
// Exercises connect and disconnect over both plain WebSocket (WS) and TLS
// WebSocket (WSS) using the production DefaultWebsocket (IXWebSocket) path.
//
// Broker address is configured at CMake time (see CMakeLists.txt):
//   WS  — default: ws://broker.hivemq.com:8000/mqtt
//   WSS — default: wss://broker.hivemq.com:8884/mqtt
//
// To target a local broker (e.g. Mosquitto in Docker):
//   cmake -DKMMQTT_IT_WS_HOST=localhost  -DKMMQTT_IT_WS_PORT=8000  \
//         -DKMMQTT_IT_WSS_HOST=localhost -DKMMQTT_IT_WSS_PORT=8884 ...
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
// Shared helpers
// ---------------------------------------------------------------------------

namespace
{
    /// Lightweight description of a broker endpoint passed into each shared test.
    struct BrokerEndpoint
    {
        const char* scheme;
        const char* host;
        const char* port;
        const char* path;
        int         timeoutSec;
    };

    ConnectAddress makeAddress(const BrokerEndpoint& ep)
    {
        ConnectAddress address;
        address.primaryAddress = Address::createURL(ep.scheme, ep.host, ep.port, ep.path);
        return address;
    }

    /// Builds minimal MQTT 5.0 connect args with a transport-tagged client id.
    ConnectArgs makeConnectArgs(const std::string& tag)
    {
        ConnectArgs args{ "kmMqtt_it_" + tag };
        args.protocolName   = "MQTT";
        args.version        = MqttVersion::MQTT_5_0;
        args.cleanStart     = true;
        args.keepAliveInSec = 60;
        return args;
    }

    /// Spin-waits until \p flag becomes true or the endpoint timeout elapses.
    bool waitFor(std::atomic<bool>& flag, int timeoutSec)
    {
        const auto deadline = std::chrono::steady_clock::now()
            + std::chrono::seconds(timeoutSec);

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

    // -----------------------------------------------------------------------
    // Shared test bodies — called from both WS and WSS suites
    // -----------------------------------------------------------------------

    void testConnect(const BrokerEndpoint& ep, const std::string& tag)
    {
        MqttClient client;

        std::atomic<bool> connectFired{ false };
        std::atomic<bool> connectOk{ false };

        client.onConnectEvent().add(
            [&](const ConnectEventDetails& d, const ConnectAck&)
            {
                connectOk.store(d.isSuccessful && d.hasReceivedAck);
                connectFired.store(true);
            });

        auto result = client.connect(makeConnectArgs(tag), makeAddress(ep));
        REQUIRE_MESSAGE(result.noError(),
            "connect() error before reaching broker ("
            << ep.scheme << "://" << ep.host << ":" << ep.port << ep.path
            << "): " << static_cast<int>(result.errorCode()));

        const bool timedOut = !waitFor(connectFired, ep.timeoutSec);
        REQUIRE_MESSAGE(!timedOut,
            "Timed out waiting for CONNACK from "
            << ep.scheme << "://" << ep.host << ":" << ep.port
            << " (timeout=" << ep.timeoutSec << "s). "
            "Check network connectivity and broker availability.");

        CHECK(connectOk.load());

        client.shutdown();
    }

    void testGracefulDisconnect(const BrokerEndpoint& ep, const std::string& tag)
    {
        MqttClient client;

        std::atomic<bool> connectFired{ false };
        std::atomic<bool> connectOk{ false };
        std::atomic<bool> disconnectFired{ false };
        std::atomic<bool> disconnectGraceful{ false };

        client.onConnectEvent().add(
            [&](const ConnectEventDetails& d, const ConnectAck&)
            {
                connectOk.store(d.isSuccessful && d.hasReceivedAck);
                connectFired.store(true);
            });

        client.onDisconnectEvent().add(
            [&](const DisconnectEventDetails& d)
            {
                disconnectGraceful.store(d.isGraceful);
                disconnectFired.store(true);
            });

        auto connectResult = client.connect(makeConnectArgs(tag), makeAddress(ep));
        REQUIRE_MESSAGE(connectResult.noError(),
            "connect() error before reaching broker ("
            << ep.scheme << "://" << ep.host << ":" << ep.port << ep.path
            << "): " << static_cast<int>(connectResult.errorCode()));

        REQUIRE_MESSAGE(waitFor(connectFired, ep.timeoutSec),
            "Timed out waiting for CONNACK from "
            << ep.scheme << "://" << ep.host << ":" << ep.port
            << " (timeout=" << ep.timeoutSec << "s). "
            "Check network connectivity and broker availability.");
        REQUIRE(connectOk.load());

        auto disconnectResult = client.disconnect(DisconnectArgs{ true });
        CHECK_MESSAGE(disconnectResult.noError(),
            "disconnect() error: " << static_cast<int>(disconnectResult.errorCode()));

        CHECK_MESSAGE(waitFor(disconnectFired, ep.timeoutSec),
            "Timed out waiting for disconnect event (timeout=" << ep.timeoutSec << "s).");
        CHECK(disconnectGraceful.load());

        client.shutdown();
    }

    void testNonGracefulDisconnect(const BrokerEndpoint& ep, const std::string& tag)
    {
        MqttClient client;

        std::atomic<bool> connectFired{ false };
        std::atomic<bool> connectOk{ false };
        std::atomic<bool> disconnectFired{ false };

        client.onConnectEvent().add(
            [&](const ConnectEventDetails& d, const ConnectAck&)
            {
                connectOk.store(d.isSuccessful && d.hasReceivedAck);
                connectFired.store(true);
            });

        client.onDisconnectEvent().add(
            [&](const DisconnectEventDetails&)
            {
                disconnectFired.store(true);
            });

        auto connectResult = client.connect(makeConnectArgs(tag), makeAddress(ep));
        REQUIRE_MESSAGE(connectResult.noError(),
            "connect() error before reaching broker ("
            << ep.scheme << "://" << ep.host << ":" << ep.port << ep.path
            << "): " << static_cast<int>(connectResult.errorCode()));

        REQUIRE_MESSAGE(waitFor(connectFired, ep.timeoutSec),
            "Timed out waiting for CONNACK from "
            << ep.scheme << "://" << ep.host << ":" << ep.port
            << " (timeout=" << ep.timeoutSec << "s). "
            "Check network connectivity and broker availability.");
        REQUIRE(connectOk.load());

        auto disconnectResult = client.disconnect(DisconnectArgs{ false });
        CHECK_MESSAGE(disconnectResult.noError(),
            "disconnect() error: " << static_cast<int>(disconnectResult.errorCode()));

        CHECK_MESSAGE(waitFor(disconnectFired, ep.timeoutSec),
            "Timed out waiting for disconnect event (timeout=" << ep.timeoutSec << "s).");

        client.shutdown();
    }

    // Endpoint singletons built from compile-time BrokerConfig constants
    const BrokerEndpoint k_wsEndpoint{
        kmMqtt_it::WsBrokerConfig::scheme,
        kmMqtt_it::WsBrokerConfig::host,
        kmMqtt_it::WsBrokerConfig::port,
        kmMqtt_it::WsBrokerConfig::path,
        kmMqtt_it::WsBrokerConfig::timeoutSec
    };

    const BrokerEndpoint k_wssEndpoint{
        kmMqtt_it::WssBrokerConfig::scheme,
        kmMqtt_it::WssBrokerConfig::host,
        kmMqtt_it::WssBrokerConfig::port,
        kmMqtt_it::WssBrokerConfig::path,
        kmMqtt_it::WssBrokerConfig::timeoutSec
    };

} // anonymous namespace

// ---------------------------------------------------------------------------
// WS test suite
// ---------------------------------------------------------------------------

TEST_SUITE("Integration - WS - Connect / Disconnect")
{
    TEST_CASE("Connect to broker and receive CONNACK")
    {
        testConnect(k_wsEndpoint, "ws_connect");
    }

    TEST_CASE("Connect then disconnect gracefully")
    {
        testGracefulDisconnect(k_wsEndpoint, "ws_graceful");
    }

    TEST_CASE("Connect then disconnect non-gracefully")
    {
        testNonGracefulDisconnect(k_wsEndpoint, "ws_abort");
    }
}

// ---------------------------------------------------------------------------
// WSS test suite
// ---------------------------------------------------------------------------

TEST_SUITE("Integration - WSS - Connect / Disconnect")
{
    TEST_CASE("Connect to broker and receive CONNACK")
    {
        testConnect(k_wssEndpoint, "wss_connect");
    }

    TEST_CASE("Connect then disconnect gracefully")
    {
        testGracefulDisconnect(k_wssEndpoint, "wss_graceful");
    }

    TEST_CASE("Connect then disconnect non-gracefully")
    {
        testNonGracefulDisconnect(k_wssEndpoint, "wss_abort");
    }
}

