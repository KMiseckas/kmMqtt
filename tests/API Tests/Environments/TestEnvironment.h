// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef TESTS_ENVIRONMENTS_TESTENVIRONMENT_H
#define TESTS_ENVIRONMENTS_TESTENVIRONMENT_H

#include <kmMqtt/Interfaces/IMqttEnvironment.h>
#include "../../API Tests/MockWebSocket.h"
#include <memory>

namespace kmMqtt
{
    class TestEnvironment : public IMqttEnvironment
    {
    public:
        ~TestEnvironment() override = default;

        Config createConfig() const noexcept override;

        std::shared_ptr<IWebSocket> createWebSocket() const noexcept override;

        mutable MockWebSocket* socketPtr{ nullptr };
        Config config{};
    };
}

#endif // TESTS_ENVIRONMENTS_TESTENVIRONMENT_H
