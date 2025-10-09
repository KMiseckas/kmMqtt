#ifndef TESTS_ENVIRONMENTS_TESTENVIRONMENT_H
#define TESTS_ENVIRONMENTS_TESTENVIRONMENT_H

#include <cleanMqtt/Interfaces/IMqttEnvironment.h>
#include "../../API Tests/MockWebSocket.h"
#include <memory>

namespace cleanMqtt
{
    class TestEnvironment : public IMqttEnvironment
    {
    public:
        ~TestEnvironment() override = default;

        Config createConfig() const noexcept override;

        std::unique_ptr<IWebSocket> createWebSocket() const noexcept override;

        mutable MockWebSocket* socketPtr{ nullptr };
    };
}

#endif // TESTS_ENVIRONMENTS_TESTENVIRONMENT_H
