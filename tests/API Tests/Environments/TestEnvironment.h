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
