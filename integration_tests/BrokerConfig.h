// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef KMMQTT_INTEGRATION_TESTS_BROKERCONFIG_H
#define KMMQTT_INTEGRATION_TESTS_BROKERCONFIG_H

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

namespace kmMqtt_it
{
    struct WsBrokerConfig
    {
        static constexpr const char* scheme     = KMMQTT_IT_WS_SCHEME;
        static constexpr const char* host       = KMMQTT_IT_WS_HOST;
        static constexpr const char* port       = KMMQTT_IT_WS_PORT;
        static constexpr const char* path       = KMMQTT_IT_WS_PATH;
        static constexpr int         timeoutSec = KMMQTT_IT_TIMEOUT_SEC;
    };

    struct WssBrokerConfig
    {
        static constexpr const char* scheme     = KMMQTT_IT_WSS_SCHEME;
        static constexpr const char* host       = KMMQTT_IT_WSS_HOST;
        static constexpr const char* port       = KMMQTT_IT_WSS_PORT;
        static constexpr const char* path       = KMMQTT_IT_WSS_PATH;
        static constexpr int         timeoutSec = KMMQTT_IT_TIMEOUT_SEC;
    };
} // namespace kmMqtt_it

#endif // KMMQTT_INTEGRATION_TESTS_BROKERCONFIG_H
