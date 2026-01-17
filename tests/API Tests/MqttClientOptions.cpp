// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <doctest.h>
#include <kmMqtt/Dispatchers/DefaultDispatcher.h>
#include <kmMqtt/Dispatchers/ImmediateDispatcher.h>
#include <kmMqtt/MqttClientOptions.h>
#include <kmMqtt/Interfaces/ICallbackDispatcher.h>
#include <atomic>
#include <thread>
#include <chrono>

using namespace kmMqtt;

TEST_SUITE("MqttClientOptions Tests")
{
	TEST_CASE("MqttClientOptions - Default Constructor Uses ASYNC")
	{
		MqttClientOptions options;

		CHECK(options.getTickMode() == TickMode::ASYNC);
		CHECK(options.getCallbackDispatcher() != nullptr);

		//In ASYNC mode with default dispatcher, should use ImmediateDispatcher
		auto immediateDispatcher = std::dynamic_pointer_cast<ImmediateDispatcher>(options.getCallbackDispatcher());
		CHECK(immediateDispatcher != nullptr);
		CHECK(options.isUsingInternalCallbackDeferrer() == false);
	}

	TEST_CASE("MqttClientOptions - SYNC Mode Constructor")
	{
		MqttClientOptions options(TickMode::SYNC);

		CHECK(options.getTickMode() == TickMode::SYNC);
		CHECK(options.getCallbackDispatcher() != nullptr);

		//In SYNC mode with default dispatcher, should use DefaultDispatcher
		auto defaultDispatcher = std::dynamic_pointer_cast<DefaultDispatcher>(options.getCallbackDispatcher());
		CHECK(defaultDispatcher != nullptr);
		CHECK(options.isUsingInternalCallbackDeferrer() == true);
	}

	TEST_CASE("MqttClientOptions - Change TickMode to ASYNC")
	{
		MqttClientOptions options(TickMode::SYNC);
		options.tickMode(TickMode::ASYNC);

		CHECK(options.getTickMode() == TickMode::ASYNC);

		//Does change to ImmediateDispatcher automatically after changing to ASYNC from SYNC (ASYNC cannot use DefaultDispatcher)
		auto immediateDispatcher = std::dynamic_pointer_cast<ImmediateDispatcher>(options.getCallbackDispatcher());
		CHECK(immediateDispatcher != nullptr);
		CHECK(options.isUsingInternalCallbackDeferrer() == false);
	}

	TEST_CASE("MqttClientOptions - Change TickMode to SYNC")
	{
		MqttClientOptions options(TickMode::ASYNC);
		options.tickMode(TickMode::SYNC);

		CHECK(options.getTickMode() == TickMode::SYNC);

		//Doesnt change to DefaultDispatcher automatically after changing to SYNC after ASYNC (SYNC can still use ImmediateDispatcher)
		auto defaultDispatcher = std::dynamic_pointer_cast<DefaultDispatcher>(options.getCallbackDispatcher());
		CHECK(defaultDispatcher == nullptr);
		CHECK(options.isUsingInternalCallbackDeferrer() == false);
	}

	TEST_CASE("MqttClientOptions - Custom ImmediateDispatcher in ASYNC Mode")
	{
		MqttClientOptions options(TickMode::ASYNC);
		auto customDispatcher = std::make_shared<ImmediateDispatcher>();

		options.callbackDispatcher(customDispatcher);

		CHECK(options.getCallbackDispatcher() == customDispatcher);
		CHECK(options.isUsingInternalCallbackDeferrer() == false);
	}

	TEST_CASE("MqttClientOptions - Custom ImmediateDispatcher in SYNC Mode")
	{
		MqttClientOptions options(TickMode::SYNC);
		auto customDispatcher = std::make_shared<ImmediateDispatcher>();

		options.callbackDispatcher(customDispatcher);

		CHECK(options.getCallbackDispatcher() == customDispatcher);
		CHECK(options.isUsingInternalCallbackDeferrer() == false);
	}

	TEST_CASE("MqttClientOptions - Null Dispatcher in ASYNC Mode")
	{
		MqttClientOptions options(TickMode::ASYNC);
		options.callbackDispatcher(nullptr);

		//Should create ImmediateDispatcher automatically
		CHECK(options.getCallbackDispatcher() != nullptr);
		auto immediateDispatcher = std::dynamic_pointer_cast<ImmediateDispatcher>(options.getCallbackDispatcher());
		CHECK(immediateDispatcher != nullptr);
		CHECK(options.isUsingInternalCallbackDeferrer() == false);
	}

	TEST_CASE("MqttClientOptions - Null Dispatcher in SYNC Mode")
	{
		MqttClientOptions options(TickMode::SYNC);
		options.callbackDispatcher(nullptr);

		//Should create DefaultDispatcher automatically
		CHECK(options.getCallbackDispatcher() != nullptr);
		auto defaultDispatcher = std::dynamic_pointer_cast<DefaultDispatcher>(options.getCallbackDispatcher());
		CHECK(defaultDispatcher != nullptr);
		CHECK(options.isUsingInternalCallbackDeferrer() == true);
	}

	TEST_CASE("MqttClientOptions - DefaultDispatcher in ASYNC Mode Gets Replaced")
	{
		MqttClientOptions options(TickMode::ASYNC);
		auto defaultDispatcher = std::make_shared<DefaultDispatcher>();

		options.callbackDispatcher(defaultDispatcher);

		//In ASYNC mode, DefaultDispatcher should be replaced with ImmediateDispatcher
		CHECK(options.getCallbackDispatcher() != defaultDispatcher);
		auto immediateDispatcher = std::dynamic_pointer_cast<ImmediateDispatcher>(options.getCallbackDispatcher());
		CHECK(immediateDispatcher != nullptr);
		CHECK(options.isUsingInternalCallbackDeferrer() == false);
	}

	TEST_CASE("MqttClientOptions - DefaultDispatcher in SYNC Mode")
	{
		MqttClientOptions options(TickMode::SYNC);
		auto defaultDispatcher = std::make_shared<DefaultDispatcher>();

		options.callbackDispatcher(defaultDispatcher);

		//In SYNC mode, DefaultDispatcher should be kept
		CHECK(options.getCallbackDispatcher() == defaultDispatcher);
		CHECK(options.isUsingInternalCallbackDeferrer() == true);
	}

	TEST_CASE("MqttClientOptions - Chaining Methods")
	{
		MqttClientOptions options;
		auto customDispatcher = std::make_shared<ImmediateDispatcher>();

		options.tickMode(TickMode::SYNC).callbackDispatcher(customDispatcher);

		CHECK(options.getTickMode() == TickMode::SYNC);
		CHECK(options.getCallbackDispatcher() == customDispatcher);
		CHECK(options.isUsingInternalCallbackDeferrer() == false);
	}

	TEST_CASE("MqttClientOptions - Switching Between Modes Preserves Custom Dispatcher")
	{
		MqttClientOptions options(TickMode::ASYNC);
		auto customDispatcher = std::make_shared<ImmediateDispatcher>();

		options.callbackDispatcher(customDispatcher);
		options.tickMode(TickMode::SYNC);

		// Custom dispatcher should be preserved
		CHECK(options.getCallbackDispatcher() == customDispatcher);
		CHECK(options.isUsingInternalCallbackDeferrer() == false);
	}
}