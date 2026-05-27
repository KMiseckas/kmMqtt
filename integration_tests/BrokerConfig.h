// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef KMMQTT_INTEGRATION_TESTS_BROKERCONFIG_H
#define KMMQTT_INTEGRATION_TESTS_BROKERCONFIG_H

#include <cstddef>

// ---------------------------------------------------------------------------
// Broker connection defaults — injected by CMake at compile time.
// Override via CMake cache variables (see integration_tests/CMakeLists.txt).
//
// WS  defaults: ws://broker.hivemq.com:8000/mqtt
// WSS defaults: wss://broker.hivemq.com:8884/mqtt
// ---------------------------------------------------------------------------

// WS defaults
#ifndef KMMQTT_IT_WS_SCHEME
#define KMMQTT_IT_WS_SCHEME "ws"
#endif
#ifndef KMMQTT_IT_WS_HOST
#define KMMQTT_IT_WS_HOST "broker.hivemq.com"
#endif
#ifndef KMMQTT_IT_WS_PORT
#define KMMQTT_IT_WS_PORT "8000"
#endif
#ifndef KMMQTT_IT_WS_PATH
#define KMMQTT_IT_WS_PATH "/mqtt"
#endif

// WSS defaults
#ifndef KMMQTT_IT_WSS_SCHEME
#define KMMQTT_IT_WSS_SCHEME "wss"
#endif
#ifndef KMMQTT_IT_WSS_HOST
#define KMMQTT_IT_WSS_HOST "broker.hivemq.com"
#endif
#ifndef KMMQTT_IT_WSS_PORT
#define KMMQTT_IT_WSS_PORT "8884"
#endif
#ifndef KMMQTT_IT_WSS_PATH
#define KMMQTT_IT_WSS_PATH "/mqtt"
#endif

#ifndef KMMQTT_IT_TIMEOUT_SEC
#define KMMQTT_IT_TIMEOUT_SEC 15
#endif

// Additional public-broker fallback defaults.
// These are normally supplied from integration_tests/CMakeLists.txt via
// compile definitions, with header defaults kept as defensive fallback.
#ifndef KMMQTT_IT_WS2_HOST
#define KMMQTT_IT_WS2_HOST "broker.emqx.io"
#endif
#ifndef KMMQTT_IT_WS2_PORT
#define KMMQTT_IT_WS2_PORT "8083"
#endif
#ifndef KMMQTT_IT_WS2_PATH
#define KMMQTT_IT_WS2_PATH "/mqtt"
#endif

#ifndef KMMQTT_IT_WSS2_HOST
#define KMMQTT_IT_WSS2_HOST "broker.emqx.io"
#endif
#ifndef KMMQTT_IT_WSS2_PORT
#define KMMQTT_IT_WSS2_PORT "8084"
#endif
#ifndef KMMQTT_IT_WSS2_PATH
#define KMMQTT_IT_WSS2_PATH "/mqtt"
#endif

#ifndef KMMQTT_IT_WS3_HOST
#define KMMQTT_IT_WS3_HOST "test.mosquitto.org"
#endif
#ifndef KMMQTT_IT_WS3_PORT
#define KMMQTT_IT_WS3_PORT "8080"
#endif
#ifndef KMMQTT_IT_WS3_PATH
#define KMMQTT_IT_WS3_PATH "/mqtt"
#endif

#ifndef KMMQTT_IT_WSS3_HOST
#define KMMQTT_IT_WSS3_HOST "test.mosquitto.org"
#endif
#ifndef KMMQTT_IT_WSS3_PORT
#define KMMQTT_IT_WSS3_PORT "8081"
#endif
#ifndef KMMQTT_IT_WSS3_PATH
#define KMMQTT_IT_WSS3_PATH "/mqtt"
#endif

namespace kmMqtt_it {
	struct BrokerEndpoint {
		const char* scheme;
		const char* host;
		const char* port;
		const char* path;
		int timeoutSec;
	};

	struct WsBrokerConfig {
		static constexpr const char* scheme = KMMQTT_IT_WS_SCHEME;
		static constexpr const char* host = KMMQTT_IT_WS_HOST;
		static constexpr const char* port = KMMQTT_IT_WS_PORT;
		static constexpr const char* path = KMMQTT_IT_WS_PATH;
		static constexpr int timeoutSec = KMMQTT_IT_TIMEOUT_SEC;
	};

	struct WssBrokerConfig {
		static constexpr const char* scheme = KMMQTT_IT_WSS_SCHEME;
		static constexpr const char* host = KMMQTT_IT_WSS_HOST;
		static constexpr const char* port = KMMQTT_IT_WSS_PORT;
		static constexpr const char* path = KMMQTT_IT_WSS_PATH;
		static constexpr int timeoutSec = KMMQTT_IT_TIMEOUT_SEC;
	};

	static constexpr BrokerEndpoint k_wsBrokerCandidates[] = {
		{WsBrokerConfig::scheme, WsBrokerConfig::host, WsBrokerConfig::port,
		 WsBrokerConfig::path, WsBrokerConfig::timeoutSec},
		{WsBrokerConfig::scheme, KMMQTT_IT_WS2_HOST, KMMQTT_IT_WS2_PORT,
		 KMMQTT_IT_WS2_PATH, KMMQTT_IT_TIMEOUT_SEC},
		{WsBrokerConfig::scheme, KMMQTT_IT_WS3_HOST, KMMQTT_IT_WS3_PORT,
		 KMMQTT_IT_WS3_PATH, KMMQTT_IT_TIMEOUT_SEC} };

	static constexpr BrokerEndpoint k_wssBrokerCandidates[] = {
		{WssBrokerConfig::scheme, WssBrokerConfig::host, WssBrokerConfig::port,
		 WssBrokerConfig::path, WssBrokerConfig::timeoutSec},
		{WssBrokerConfig::scheme, KMMQTT_IT_WSS2_HOST, KMMQTT_IT_WSS2_PORT,
		 KMMQTT_IT_WSS2_PATH, KMMQTT_IT_TIMEOUT_SEC},
		{WssBrokerConfig::scheme, KMMQTT_IT_WSS3_HOST, KMMQTT_IT_WSS3_PORT,
		 KMMQTT_IT_WSS3_PATH, KMMQTT_IT_TIMEOUT_SEC} };

	static constexpr std::size_t k_wsBrokerCandidatesCount =
		sizeof(k_wsBrokerCandidates) / sizeof(k_wsBrokerCandidates[0]);
	static constexpr std::size_t k_wssBrokerCandidatesCount =
		sizeof(k_wssBrokerCandidates) / sizeof(k_wssBrokerCandidates[0]);
} // namespace kmMqtt_it

#endif // KMMQTT_INTEGRATION_TESTS_BROKERCONFIG_H
