// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

// ---------------------------------------------------------------------------
// Integration tests — Public broker smoke/full suites
//
// Smoke suite: quick API checks intended for PR validation.
// Full suite: slower repetitive checks for broader branch-gate validation.
//
// Uses a public broker candidate pool configured in BrokerConfig.h and
// automatically falls back to the next candidate if one endpoint is down.
// ---------------------------------------------------------------------------

#include <doctest.h>

#include <kmMqtt/Mqtt/MqttClientEvents.h>
#include <kmMqtt/Mqtt/Params/ConnectAddress.h>
#include <kmMqtt/Mqtt/Params/ConnectArgs.h>
#include <kmMqtt/Mqtt/Params/DisconnectArgs.h>
#include <kmMqtt/Mqtt/Params/PublishOptions.h>
#include <kmMqtt/Mqtt/Params/SubscribeOptions.h>
#include <kmMqtt/Mqtt/Params/Topic.h>
#include <kmMqtt/Mqtt/Params/UnSubscribeOptions.h>
#include <kmMqtt/MqttClient.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstring>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "BrokerConfig.h"

using namespace kmMqtt;
using namespace kmMqtt::mqtt;

// ---------------------------------------------------------------------------
// Shared helpers
// ---------------------------------------------------------------------------

namespace {
struct EndpointSelection {
  bool found{false};
  kmMqtt_it::BrokerEndpoint endpoint{"", "", "", "", 1};
  std::string diagnostics;
};

ConnectAddress makeAddress(const kmMqtt_it::BrokerEndpoint &ep) {
  ConnectAddress address;
  address.primaryAddress =
      Address::createURL(ep.scheme, ep.host, ep.port, ep.path);
  return address;
}

/// Builds minimal MQTT 5.0 connect args with a transport-tagged client id.
ConnectArgs makeConnectArgs(const std::string &tag) {
  ConnectArgs args{"kmMqtt_it_" + tag};
  args.protocolName = "MQTT";
  args.version = MqttVersion::MQTT_5_0;
  args.cleanStart = true;
  args.keepAliveInSec = 30;
  return args;
}

/// Spin-waits until \p flag becomes true or the endpoint timeout elapses.
bool waitFor(std::atomic<bool> &flag, int timeoutSec) {
  const auto deadline =
      std::chrono::steady_clock::now() + std::chrono::seconds(timeoutSec);

  while (!flag.load()) {
    if (std::chrono::steady_clock::now() >= deadline) {
      return false;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
  return true;
}

bool waitForAtLeast(std::atomic<int> &value, int expectedCount,
                    int timeoutSec) {
  const auto deadline =
      std::chrono::steady_clock::now() + std::chrono::seconds(timeoutSec);
  while (value.load() < expectedCount) {
    if (std::chrono::steady_clock::now() >= deadline) {
      return false;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
  return true;
}

std::string makeUniqueTopic(const char *base) {
  static std::atomic<unsigned long> counter{0UL};
  const auto nowMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                         std::chrono::system_clock::now().time_since_epoch())
                         .count();

  std::ostringstream stream;
  stream << base << "/" << nowMs << "_" << counter.fetch_add(1UL);
  return stream.str();
}

EndpointSelection
selectReachableEndpoint(const kmMqtt_it::BrokerEndpoint *candidates,
                        std::size_t candidateCount, const char *transportTag) {
  EndpointSelection selection;
  if (candidateCount == 0) {
    selection.diagnostics = "No broker candidates configured.";
    return selection;
  }

  selection.endpoint = candidates[0];

  std::ostringstream diagnostics;
  diagnostics << "Tried " << candidateCount << " " << transportTag
              << " broker candidates:";

  for (std::size_t i = 0; i < candidateCount; ++i) {
    const auto &candidate = candidates[i];

    MqttClient probeClient;
    std::atomic<bool> connectFired{false};
    std::atomic<bool> connectOk{false};

    probeClient.onConnectEvent().add(
        [&](const ConnectEventDetails &details, const ConnectAck &) {
          connectOk.store(details.isSuccessful && details.hasReceivedAck);
          connectFired.store(true);
        });

    const auto connectResult = probeClient.connect(
        makeConnectArgs(std::string("probe_") + transportTag + "_" +
                        std::to_string(i)),
        makeAddress(candidate));

    if (!connectResult.noError()) {
      diagnostics << "\n  - " << candidate.scheme << "://" << candidate.host
                  << ":" << candidate.port << candidate.path
                  << " => connect() failed with error code "
                  << static_cast<int>(connectResult.errorCode());
      probeClient.shutdown();
      continue;
    }

    const int probeTimeoutSec = std::min(candidate.timeoutSec, 6);
    if (!waitFor(connectFired, probeTimeoutSec)) {
      diagnostics << "\n  - " << candidate.scheme << "://" << candidate.host
                  << ":" << candidate.port << candidate.path
                  << " => timed out waiting for CONNACK.";
      probeClient.shutdown();
      continue;
    }

    if (!connectOk.load()) {
      diagnostics << "\n  - " << candidate.scheme << "://" << candidate.host
                  << ":" << candidate.port << candidate.path
                  << " => connect event reported failure.";
      probeClient.shutdown();
      continue;
    }

    probeClient.disconnect(DisconnectArgs{false});
    probeClient.shutdown();

    selection.found = true;
    selection.endpoint = candidate;
    selection.diagnostics = diagnostics.str();
    return selection;
  }

  selection.diagnostics = diagnostics.str();
  return selection;
}

const EndpointSelection &getWsSelection() {
  static const EndpointSelection selection =
      selectReachableEndpoint(kmMqtt_it::k_wsBrokerCandidates,
                              kmMqtt_it::k_wsBrokerCandidatesCount, "ws");
  return selection;
}

const EndpointSelection &getWssSelection() {
  static const EndpointSelection selection =
      selectReachableEndpoint(kmMqtt_it::k_wssBrokerCandidates,
                              kmMqtt_it::k_wssBrokerCandidatesCount, "wss");
  return selection;
}

void requireReachableSelection(const EndpointSelection &selection,
                               const char *transportName) {
  REQUIRE_MESSAGE(selection.found, "No reachable "
                                       << transportName
                                       << " public broker candidate. "
                                       << "Details: " << selection.diagnostics);
}

// -----------------------------------------------------------------------
// Shared test bodies — called from both WS and WSS suites
// -----------------------------------------------------------------------

void testConnect(const kmMqtt_it::BrokerEndpoint &ep, const std::string &tag) {
  MqttClient client;

  std::atomic<bool> connectFired{false};
  std::atomic<bool> connectOk{false};

  client.onConnectEvent().add(
      [&](const ConnectEventDetails &d, const ConnectAck &) {
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

void testGracefulDisconnect(const kmMqtt_it::BrokerEndpoint &ep,
                            const std::string &tag) {
  MqttClient client;

  std::atomic<bool> connectFired{false};
  std::atomic<bool> connectOk{false};
  std::atomic<bool> disconnectFired{false};
  std::atomic<bool> disconnectGraceful{false};

  client.onConnectEvent().add(
      [&](const ConnectEventDetails &d, const ConnectAck &) {
        connectOk.store(d.isSuccessful && d.hasReceivedAck);
        connectFired.store(true);
      });

  client.onDisconnectEvent().add([&](const DisconnectEventDetails &d) {
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

  auto disconnectResult = client.disconnect(DisconnectArgs{true});
  CHECK_MESSAGE(
      disconnectResult.noError(),
      "disconnect() error: " << static_cast<int>(disconnectResult.errorCode()));

  CHECK_MESSAGE(waitFor(disconnectFired, ep.timeoutSec),
                "Timed out waiting for disconnect event (timeout="
                    << ep.timeoutSec << "s).");
  CHECK(disconnectGraceful.load());

  client.shutdown();
}

void testNonGracefulDisconnect(const kmMqtt_it::BrokerEndpoint &ep,
                               const std::string &tag) {
  MqttClient client;

  std::atomic<bool> connectFired{false};
  std::atomic<bool> connectOk{false};
  std::atomic<bool> disconnectFired{false};

  client.onConnectEvent().add(
      [&](const ConnectEventDetails &d, const ConnectAck &) {
        connectOk.store(d.isSuccessful && d.hasReceivedAck);
        connectFired.store(true);
      });

  client.onDisconnectEvent().add(
      [&](const DisconnectEventDetails &) { disconnectFired.store(true); });

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

  auto disconnectResult = client.disconnect(DisconnectArgs{false});
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
    const auto &selection = getWsSelection();
    requireReachableSelection(selection, "WS");
    testConnect(selection.endpoint, "ws_connect");
  }

  TEST_CASE("Connect then disconnect gracefully") {
    const auto &selection = getWsSelection();
    requireReachableSelection(selection, "WS");
    testGracefulDisconnect(selection.endpoint, "ws_graceful");
  }

  TEST_CASE("Connect then disconnect non-gracefully") {
    const auto &selection = getWsSelection();
    requireReachableSelection(selection, "WS");
    testNonGracefulDisconnect(selection.endpoint, "ws_abort");
  }
}

TEST_SUITE("Integration - Smoke - WSS - Connect / Disconnect") {
  TEST_CASE("Connect to broker and receive CONNACK") {
    const auto &selection = getWssSelection();
    requireReachableSelection(selection, "WSS");
    testConnect(selection.endpoint, "wss_connect");
  }

  TEST_CASE("Connect then disconnect gracefully") {
    const auto &selection = getWssSelection();
    requireReachableSelection(selection, "WSS");
    testGracefulDisconnect(selection.endpoint, "wss_graceful");
  }
}

TEST_SUITE("Integration - Smoke - API") {
  TEST_CASE(
      "Default client is async and exposes connection info after connect") {
    const auto &selection = getWsSelection();
    requireReachableSelection(selection, "WS");
    const auto &endpoint = selection.endpoint;

    MqttClient client;
    CHECK(client.getIsTickAsync());
    CHECK(client.getConnectionStatus() == ConnectionStatus::DISCONNECTED);

    const std::string clientTag = "api_state";
    const std::string expectedClientId = "kmMqtt_it_" + clientTag;

    std::atomic<bool> connectFired{false};
    std::atomic<bool> connectOk{false};

    client.onConnectEvent().add(
        [&](const ConnectEventDetails &details, const ConnectAck &) {
          connectOk.store(details.isSuccessful && details.hasReceivedAck);
          connectFired.store(true);
        });

    auto connectResult =
        client.connect(makeConnectArgs(clientTag), makeAddress(endpoint));
    REQUIRE(connectResult.noError());
    REQUIRE(waitFor(connectFired, endpoint.timeoutSec));
    REQUIRE(connectOk.load());

    CHECK(client.getConnectionStatus() == ConnectionStatus::CONNECTED);
    CHECK(client.getConnectionInfo().connectArgs.clientId == expectedClientId);
    CHECK(client.getConnectionInfo().connectAddress.primaryAddress.hostname() ==
          endpoint.host);

    auto disconnectResult = client.disconnect(DisconnectArgs{true});
    CHECK(disconnectResult.noError());
    client.shutdown();
  }

  TEST_CASE(
      "Publish subscribe unsubscribe flow emits expected quick API events") {
    const auto &selection = getWsSelection();
    requireReachableSelection(selection, "WS");
    const auto &endpoint = selection.endpoint;

    MqttClient subscriber;
    MqttClient publisher;

    std::atomic<bool> subscriberConnected{false};
    std::atomic<bool> subscriberConnectOk{false};
    std::atomic<bool> publisherConnected{false};
    std::atomic<bool> publisherConnectOk{false};
    std::atomic<int> subAckCount{0};
    std::atomic<int> unSubAckCount{0};
    std::atomic<int> publishReceivedCount{0};
    std::atomic<int> publishCompletedCount{0};
    std::atomic<bool> publishTopicMatches{false};
    std::atomic<bool> publishPayloadMatches{false};
    std::atomic<bool> publishCompleteIsAck{false};

    subscriber.onConnectEvent().add([&](const ConnectEventDetails &details,
                                        const ConnectAck &) {
      subscriberConnectOk.store(details.isSuccessful && details.hasReceivedAck);
      subscriberConnected.store(true);
    });

    publisher.onConnectEvent().add([&](const ConnectEventDetails &details,
                                       const ConnectAck &) {
      publisherConnectOk.store(details.isSuccessful && details.hasReceivedAck);
      publisherConnected.store(true);
    });

    std::string topic = makeUniqueTopic("kmMqtt/it/smoke/api");
    const std::string payloadText = "smoke_payload_qos1";

    subscriber.onSubscribeAckEvent().add(
        [&](const SubscribeAckEventDetails &, const SubscribeAck &) {
          subAckCount.fetch_add(1);
        });

    subscriber.onUnSubscribeAckEvent().add(
        [&](const UnSubscribeAckEventDetails &, const UnSubscribeAck &) {
          unSubAckCount.fetch_add(1);
        });

    subscriber.onPublishEvent().add(
        [&](const PublishEventDetails &details, const Publish &) {
          if (details.topic == topic) {
            publishTopicMatches.store(true);
          }

          if (details.payload != nullptr &&
              details.payload->size() == payloadText.size() &&
              std::memcmp(details.payload->bytes(), payloadText.data(),
                          payloadText.size()) == 0) {
            publishPayloadMatches.store(true);
          }

          publishReceivedCount.fetch_add(1);
        });

    publisher.onPublishCompletedEvent().add(
        [&](const PublishCompleteEventDetails &details) {
          if (details.packetType == PacketType::PUBLISH_ACKNOWLEDGE &&
              details.isSuccess()) {
            publishCompleteIsAck.store(true);
          }

          publishCompletedCount.fetch_add(1);
        });

    auto subscriberConnect = subscriber.connect(
        makeConnectArgs("sub_smoke_api"), makeAddress(endpoint));
    REQUIRE(subscriberConnect.noError());
    REQUIRE(waitFor(subscriberConnected, endpoint.timeoutSec));
    REQUIRE(subscriberConnectOk.load());

    auto publisherConnect = publisher.connect(makeConnectArgs("pub_smoke_api"),
                                              makeAddress(endpoint));
    REQUIRE(publisherConnect.noError());
    REQUIRE(waitFor(publisherConnected, endpoint.timeoutSec));
    REQUIRE(publisherConnectOk.load());

    std::vector<Topic> topics{Topic{topic}};

    auto subscribeResult = subscriber.subscribe(topics, SubscribeOptions{});
    REQUIRE(subscribeResult.noError());
    REQUIRE(waitForAtLeast(subAckCount, 1, endpoint.timeoutSec));

    ByteBuffer payload(payloadText.size());
    payload.append(reinterpret_cast<const std::uint8_t *>(payloadText.data()),
                   payloadText.size());

    PublishOptions options;
    options.qos = Qos::QOS_1;
    auto publishResult = publisher.publish(topic.c_str(), std::move(payload),
                                           std::move(options));
    REQUIRE(publishResult.noError());
    REQUIRE(waitForAtLeast(publishCompletedCount, 1, endpoint.timeoutSec));
    REQUIRE(waitForAtLeast(publishReceivedCount, 1, endpoint.timeoutSec));

    CHECK(publishCompleteIsAck.load());
    CHECK(publishTopicMatches.load());
    CHECK(publishPayloadMatches.load());

    auto unSubResult = subscriber.unSubscribe(topics, UnSubscribeOptions{});
    REQUIRE(unSubResult.noError());
    REQUIRE(waitForAtLeast(unSubAckCount, 1, endpoint.timeoutSec));

    CHECK(subscriber.disconnect(DisconnectArgs{true}).noError());
    CHECK(publisher.disconnect(DisconnectArgs{true}).noError());

    subscriber.shutdown();
    publisher.shutdown();
  }
}

// ---------------------------------------------------------------------------
// Full suite
// ---------------------------------------------------------------------------

TEST_SUITE("Integration - Full - Stability") {
  TEST_CASE("Repeated WS connect and graceful disconnect remains stable") {
    const auto &selection = getWsSelection();
    requireReachableSelection(selection, "WS");

    for (int i = 0; i < 8; ++i) {
      testGracefulDisconnect(selection.endpoint,
                             "ws_full_loop_" + std::to_string(i));
    }
  }

  TEST_CASE("QoS2 publish completes PUBREC/PUBCOMP flow and subscriber "
            "receives payload") {
    const auto &selection = getWsSelection();
    requireReachableSelection(selection, "WS");
    const auto &endpoint = selection.endpoint;

    MqttClient subscriber;
    MqttClient publisher;

    std::atomic<bool> subscriberConnected{false};
    std::atomic<bool> publisherConnected{false};
    std::atomic<int> subAckCount{0};
    std::atomic<int> unSubAckCount{0};
    std::atomic<int> publishReceivedCount{0};
    std::atomic<bool> sawPubRec{false};
    std::atomic<bool> sawPubComp{false};
    std::atomic<bool> payloadMatches{false};

    subscriber.onConnectEvent().add([&](const ConnectEventDetails &details,
                                        const ConnectAck &) {
      subscriberConnected.store(details.isSuccessful && details.hasReceivedAck);
    });

    publisher.onConnectEvent().add([&](const ConnectEventDetails &details,
                                       const ConnectAck &) {
      publisherConnected.store(details.isSuccessful && details.hasReceivedAck);
    });

    std::string topic = makeUniqueTopic("kmMqtt/it/full/qos2");
    const std::string payloadText = "full_qos2_payload";

    subscriber.onSubscribeAckEvent().add(
        [&](const SubscribeAckEventDetails &, const SubscribeAck &) {
          subAckCount.fetch_add(1);
        });

    subscriber.onUnSubscribeAckEvent().add(
        [&](const UnSubscribeAckEventDetails &, const UnSubscribeAck &) {
          unSubAckCount.fetch_add(1);
        });

    subscriber.onPublishEvent().add(
        [&](const PublishEventDetails &details, const Publish &) {
          if (details.payload != nullptr &&
              details.payload->size() == payloadText.size() &&
              std::memcmp(details.payload->bytes(), payloadText.data(),
                          payloadText.size()) == 0) {
            payloadMatches.store(true);
          }

          if (details.topic == topic) {
            publishReceivedCount.fetch_add(1);
          }
        });

    publisher.onPublishCompletedEvent().add(
        [&](const PublishCompleteEventDetails &details) {
          if (details.packetType == PacketType::PUBLISH_RECEIVED &&
              details.isSuccess()) {
            sawPubRec.store(true);
          }

          if (details.packetType == PacketType::PUBLISH_COMPLETE &&
              details.isSuccess()) {
            sawPubComp.store(true);
          }
        });

    auto subscriberConnect = subscriber.connect(
        makeConnectArgs("sub_full_qos2"), makeAddress(endpoint));
    REQUIRE(subscriberConnect.noError());
    REQUIRE(waitFor(subscriberConnected, endpoint.timeoutSec));

    auto publisherConnect = publisher.connect(makeConnectArgs("pub_full_qos2"),
                                              makeAddress(endpoint));
    REQUIRE(publisherConnect.noError());
    REQUIRE(waitFor(publisherConnected, endpoint.timeoutSec));

    std::vector<Topic> topics{Topic{topic}};
    auto subscribeResult = subscriber.subscribe(topics, SubscribeOptions{});
    REQUIRE(subscribeResult.noError());
    REQUIRE(waitForAtLeast(subAckCount, 1, endpoint.timeoutSec));

    ByteBuffer payload(payloadText.size());
    payload.append(reinterpret_cast<const std::uint8_t *>(payloadText.data()),
                   payloadText.size());

    PublishOptions options;
    options.qos = Qos::QOS_2;
    auto publishResult = publisher.publish(topic.c_str(), std::move(payload),
                                           std::move(options));
    REQUIRE(publishResult.noError());

    REQUIRE(waitFor(sawPubRec, endpoint.timeoutSec));
    REQUIRE(waitFor(sawPubComp, endpoint.timeoutSec));
    REQUIRE(waitForAtLeast(publishReceivedCount, 1, endpoint.timeoutSec));
    CHECK(payloadMatches.load());

    auto unSubResult = subscriber.unSubscribe(topics, UnSubscribeOptions{});
    REQUIRE(unSubResult.noError());
    REQUIRE(waitForAtLeast(unSubAckCount, 1, endpoint.timeoutSec));

    CHECK(subscriber.disconnect(DisconnectArgs{true}).noError());
    CHECK(publisher.disconnect(DisconnectArgs{true}).noError());

    subscriber.shutdown();
    publisher.shutdown();
  }

  TEST_CASE(
      "Burst QoS1 publish load completes acknowledgements and deliveries") {
    const auto &selection = getWsSelection();
    requireReachableSelection(selection, "WS");
    const auto &endpoint = selection.endpoint;

    static constexpr int kBurstCount = 20;

    MqttClient subscriber;
    MqttClient publisher;

    std::atomic<bool> subscriberConnected{false};
    std::atomic<bool> publisherConnected{false};
    std::atomic<int> subAckCount{0};
    std::atomic<int> unSubAckCount{0};
    std::atomic<int> publishAckCount{0};
    std::atomic<int> publishReceivedCount{0};

    subscriber.onConnectEvent().add([&](const ConnectEventDetails &details,
                                        const ConnectAck &) {
      subscriberConnected.store(details.isSuccessful && details.hasReceivedAck);
    });

    publisher.onConnectEvent().add([&](const ConnectEventDetails &details,
                                       const ConnectAck &) {
      publisherConnected.store(details.isSuccessful && details.hasReceivedAck);
    });

    subscriber.onSubscribeAckEvent().add(
        [&](const SubscribeAckEventDetails &, const SubscribeAck &) {
          subAckCount.fetch_add(1);
        });

    subscriber.onUnSubscribeAckEvent().add(
        [&](const UnSubscribeAckEventDetails &, const UnSubscribeAck &) {
          unSubAckCount.fetch_add(1);
        });

    subscriber.onPublishEvent().add(
        [&](const PublishEventDetails &, const Publish &) {
          publishReceivedCount.fetch_add(1);
        });

    publisher.onPublishCompletedEvent().add(
        [&](const PublishCompleteEventDetails &details) {
          if (details.packetType == PacketType::PUBLISH_ACKNOWLEDGE &&
              details.isSuccess()) {
            publishAckCount.fetch_add(1);
          }
        });

    std::string topic = makeUniqueTopic("kmMqtt/it/full/burst");

    auto subscriberConnect = subscriber.connect(
        makeConnectArgs("sub_full_burst"), makeAddress(endpoint));
    REQUIRE(subscriberConnect.noError());
    REQUIRE(waitFor(subscriberConnected, endpoint.timeoutSec));

    auto publisherConnect = publisher.connect(makeConnectArgs("pub_full_burst"),
                                              makeAddress(endpoint));
    REQUIRE(publisherConnect.noError());
    REQUIRE(waitFor(publisherConnected, endpoint.timeoutSec));

    std::vector<Topic> topics{Topic{topic}};
    auto subscribeResult = subscriber.subscribe(topics, SubscribeOptions{});
    REQUIRE(subscribeResult.noError());
    REQUIRE(waitForAtLeast(subAckCount, 1, endpoint.timeoutSec));

    for (int i = 0; i < kBurstCount; ++i) {
      const std::string payloadText = "burst_qos1_" + std::to_string(i);
      ByteBuffer payload(payloadText.size());
      payload.append(reinterpret_cast<const std::uint8_t *>(payloadText.data()),
                     payloadText.size());

      PublishOptions options;
      options.qos = Qos::QOS_1;

      auto publishResult = publisher.publish(topic.c_str(), std::move(payload),
                                             std::move(options));
      REQUIRE(publishResult.noError());
    }

    const int extendedTimeoutSec = endpoint.timeoutSec * 2;
    REQUIRE(waitForAtLeast(publishAckCount, kBurstCount, extendedTimeoutSec));
    REQUIRE(
        waitForAtLeast(publishReceivedCount, kBurstCount, extendedTimeoutSec));

    auto unSubResult = subscriber.unSubscribe(topics, UnSubscribeOptions{});
    REQUIRE(unSubResult.noError());
    REQUIRE(waitForAtLeast(unSubAckCount, 1, endpoint.timeoutSec));

    CHECK(subscriber.disconnect(DisconnectArgs{true}).noError());
    CHECK(publisher.disconnect(DisconnectArgs{true}).noError());

    subscriber.shutdown();
    publisher.shutdown();
  }

  TEST_CASE("Keepalive idle period remains connected") {
    const auto &selection = getWsSelection();
    requireReachableSelection(selection, "WS");
    const auto &endpoint = selection.endpoint;

    MqttClient client;
    std::atomic<bool> connectFired{false};
    std::atomic<bool> connectOk{false};
    std::atomic<bool> disconnectFired{false};

    client.onConnectEvent().add(
        [&](const ConnectEventDetails &details, const ConnectAck &) {
          connectOk.store(details.isSuccessful && details.hasReceivedAck);
          connectFired.store(true);
        });

    client.onDisconnectEvent().add(
        [&](const DisconnectEventDetails &) { disconnectFired.store(true); });

    auto args = makeConnectArgs("full_keepalive_idle");
    args.keepAliveInSec = 3;

    auto connectResult = client.connect(std::move(args), makeAddress(endpoint));
    REQUIRE(connectResult.noError());
    REQUIRE(waitFor(connectFired, endpoint.timeoutSec));
    REQUIRE(connectOk.load());

    std::this_thread::sleep_for(std::chrono::seconds(8));
    CHECK(client.getConnectionStatus() == ConnectionStatus::CONNECTED);
    CHECK(!disconnectFired.load());

    CHECK(client.disconnect(DisconnectArgs{true}).noError());
    client.shutdown();
  }
}
