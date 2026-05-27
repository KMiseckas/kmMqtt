// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include "Helpers.h"

#include <doctest.h>

#include <kmMqtt/Mqtt/Params/DisconnectArgs.h>

#include <algorithm>
#include <sstream>
#include <string>

using namespace kmMqtt;
using namespace kmMqtt::mqtt;

namespace kmMqtt_it {

	EndpointSelection selectReachableEndpoint(const BrokerEndpoint* candidates,
		std::size_t candidateCount,
		const char* transportTag) {
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
			const auto& candidate = candidates[i];

			MqttClient probeClient;
			std::atomic<bool> connectFired{ false };
			std::atomic<bool> connectOk{ false };

			probeClient.onConnectEvent().add(
				[&](const ConnectEventDetails& details, const ConnectAck&) {
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

			probeClient.disconnect(DisconnectArgs{ false });
			probeClient.shutdown();

			selection.found = true;
			selection.endpoint = candidate;
			selection.diagnostics = diagnostics.str();
			return selection;
		}

		selection.diagnostics = diagnostics.str();
		return selection;
	}

	const EndpointSelection& getWsSelection() {
		static const EndpointSelection selection = selectReachableEndpoint(
			k_wsBrokerCandidates, k_wsBrokerCandidatesCount, "ws");
		return selection;
	}

	const EndpointSelection& getWssSelection() {
		static const EndpointSelection selection = selectReachableEndpoint(
			k_wssBrokerCandidates, k_wssBrokerCandidatesCount, "wss");
		return selection;
	}

	void requireReachableSelection(const EndpointSelection& selection,
		const char* transportName) {
		REQUIRE_MESSAGE(selection.found, "No reachable "
			<< transportName
			<< " public broker candidate. Details: "
			<< selection.diagnostics);
	}

} // namespace kmMqtt_it
