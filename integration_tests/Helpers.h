// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

// ---------------------------------------------------------------------------
// Shared helpers for all integration test files.
// ---------------------------------------------------------------------------

#pragma once

#include <kmMqtt/Mqtt/Params/ConnectAddress.h>
#include <kmMqtt/Mqtt/Params/ConnectArgs.h>

#include <atomic>
#include <chrono>
#include <sstream>
#include <string>
#include <thread>

#include "BrokerConfig.h"
#include <kmMqtt/Mqtt/Enums/MqttVersion.h>
#include <doctest.h>

namespace kmMqtt_it {

	// ---------------------------------------------------------------------------
	// Data types
	// ---------------------------------------------------------------------------

	struct EndpointSelection {
		bool found{ false };
		BrokerEndpoint endpoint{ "", "", "", "", 1 };
		std::string diagnostics;
	};

	// ---------------------------------------------------------------------------
	// Address and connect arg factories
	// ---------------------------------------------------------------------------

	inline kmMqtt::mqtt::ConnectAddress makeAddress(const BrokerEndpoint& ep) {
		kmMqtt::mqtt::ConnectAddress addr;
		addr.primaryAddress =
			kmMqtt::mqtt::Address::createURL(ep.scheme, ep.host, ep.port, ep.path);
		return addr;
	}

	/// Builds minimal MQTT 5.0 connect args with a transport-tagged client id.
	inline kmMqtt::mqtt::ConnectArgs makeConnectArgs(const std::string& tag) {
		kmMqtt::mqtt::ConnectArgs args{ "kmMqtt_it_" + tag };
		args.protocolName = "MQTT";
		args.version = kmMqtt::mqtt::MqttVersion::MQTT_5_0;
		args.cleanStart = true;
		args.keepAliveInSec = 30;
		return args;
	}

	// ---------------------------------------------------------------------------
	// Wait utilities
	// ---------------------------------------------------------------------------

	/// Spin-waits until \p flag becomes true or the timeout elapses.
	inline bool waitFor(std::atomic<bool>& flag, int timeoutSec) {
		const auto deadline =
			std::chrono::steady_clock::now() + std::chrono::seconds(timeoutSec);
		while (!flag.load()) {
			if (std::chrono::steady_clock::now() >= deadline)
				return false;
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
		return true;
	}

	inline bool waitForAtLeast(std::atomic<int>& value, int expectedCount,
		int timeoutSec) {
		const auto deadline =
			std::chrono::steady_clock::now() + std::chrono::seconds(timeoutSec);
		while (value.load() < expectedCount) {
			if (std::chrono::steady_clock::now() >= deadline)
				return false;
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
		return true;
	}

	// ---------------------------------------------------------------------------
	// Topic utilities
	// ---------------------------------------------------------------------------

	inline std::string makeUniqueTopic(const char* base) {
		// Static local in an inline function has a single shared instance across
		// all translation units (C++14 [dcl.inline]).
		static std::atomic<unsigned long> counter{ 0UL };
		const auto nowMs = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch())
			.count();
		std::ostringstream stream;
		stream << base << "/" << nowMs << "_" << counter.fetch_add(1UL);
		return stream.str();
	}

	// ---------------------------------------------------------------------------
	// Broker pool / selection (defined in Helpers.cpp)
	// ---------------------------------------------------------------------------

	EndpointSelection selectReachableEndpoint(const BrokerEndpoint* candidates,
		std::size_t candidateCount,
		const char* transportTag);

	const EndpointSelection& getWsSelection();
	const EndpointSelection& getWssSelection();

	void requireReachableSelection(const EndpointSelection& selection,
		const char* transportName);

} // namespace kmMqtt_it
