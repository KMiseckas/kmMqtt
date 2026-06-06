// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <doctest.h>
#include <kmMqtt/Logger/Log.h>
#include <kmMqtt/Logger/LoggerInstance.h>
#include <kmMqtt/Interfaces/ILogger.h>
#include <kmMqtt/Memory/AllocatorUtils.h>

#include <stdexcept>
#include <string>

namespace
{
	class CountingLogger : public kmMqtt::ILogger
	{
	public:
		void Log(const kmMqtt::LogLevel logLvl, const char* const msg) const noexcept override
		{
			lastLevel = logLvl;
			lastMessage = msg == nullptr ? "" : msg;
			simpleCalls++;
		}

		void Log(const kmMqtt::LogLevel logLvl, const char* const category, const char* const msg) const noexcept override
		{
			lastLevel = logLvl;
			lastCategory = category == nullptr ? "" : category;
			lastMessage = msg == nullptr ? "" : msg;
			categoryCalls++;
		}

		mutable int simpleCalls{ 0 };
		mutable int categoryCalls{ 0 };
		mutable kmMqtt::LogLevel lastLevel{ kmMqtt::LogLevel::Info };
		mutable std::string lastCategory{};
		mutable std::string lastMessage{};
	};
}

TEST_SUITE("Logger Tests")
{
	TEST_CASE("LogException routes fatal log and rethrows")
	{
		auto* previousLogger = kmMqtt::getLogger();
		auto* testLogger = kmNew(CountingLogger);
		kmMqtt::setLogger(testLogger, false);

		const std::runtime_error error("logger failure");
		CHECK_THROWS_AS(kmMqtt::LogException("logger-test", error), std::exception);
		CHECK(testLogger->categoryCalls == 1);
		CHECK(testLogger->lastLevel == kmMqtt::LogLevel::Fatal);
		CHECK(testLogger->lastCategory == "logger-test");

		kmMqtt::setLogger(previousLogger, false);
		kmDelete(testLogger);
	}

	TEST_CASE("Log APIs can be called with formatted arguments")
	{
		auto* previousLogger = kmMqtt::getLogger();
		auto* testLogger = kmNew(CountingLogger);
		kmMqtt::setLogger(testLogger, false);

		CHECK_NOTHROW(kmMqtt::LogInfo("logger-test", "value=%d", 42));
		CHECK_NOTHROW(kmMqtt::LogWarning("logger-test", "value=%d", 43));
		CHECK_NOTHROW(kmMqtt::LogError("logger-test", "value=%d", 44));

		kmMqtt::setLogger(previousLogger, false);
		kmDelete(testLogger);
	}
}
