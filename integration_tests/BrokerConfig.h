// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef KMMQTT_INTEGRATION_TESTS_BROKERCONFIG_H
#define KMMQTT_INTEGRATION_TESTS_BROKERCONFIG_H

#ifndef KMMQTT_IT_BROKER_SCHEME
#define KMMQTT_IT_BROKER_SCHEME "ws"
#endif

#ifndef KMMQTT_IT_BROKER_HOST
#define KMMQTT_IT_BROKER_HOST "broker.hivemq.com"
#endif

#ifndef KMMQTT_IT_BROKER_PORT
#define KMMQTT_IT_BROKER_PORT "8000"
#endif

#ifndef KMMQTT_IT_BROKER_PATH
#define KMMQTT_IT_BROKER_PATH "/mqtt"
#endif

#ifndef KMMQTT_IT_TIMEOUT_SEC
#define KMMQTT_IT_TIMEOUT_SEC 15
#endif

namespace kmMqtt_it
{
    struct BrokerConfig
    {
        static constexpr const char* scheme = KMMQTT_IT_BROKER_SCHEME;
        static constexpr const char* host = KMMQTT_IT_BROKER_HOST;
        static constexpr const char* port = KMMQTT_IT_BROKER_PORT;
        static constexpr const char* path = KMMQTT_IT_BROKER_PATH;
        static constexpr int timeoutSec = KMMQTT_IT_TIMEOUT_SEC;
    };
} // namespace kmMqtt_it

#endif // KMMQTT_INTEGRATION_TESTS_BROKERCONFIG_H
