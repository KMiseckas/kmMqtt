// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <doctest.h>
#include <kmMqtt/Environments/DefaultEnvironmentFactory.h>
#include <kmMqtt/Environments/DefaultWinEnv.h>
#include <kmMqtt/Environments/DefaultLinuxEnv.h>
#include <kmMqtt/Interfaces/IMqttEnvironment.h>
#include <kmMqtt/Config.h>
#include <memory>

TEST_SUITE("Environment Tests")
{
	using namespace kmMqtt;

	TEST_CASE("DefaultEnvironmentFactory creates environment")
	{
		DefaultEnvironmentFactory factory;
		IMqttEnvironment* env = factory.createEnvironment();

		CHECK(env != nullptr);

		delete env;
	}

	TEST_CASE("DefaultEnvironmentFactory created environment is valid")
	{
		DefaultEnvironmentFactory factory;
		IMqttEnvironment* env = factory.createEnvironment();

		REQUIRE(env != nullptr);

		Config config = env->createConfig();
		CHECK(config.connectTimeOutMS == 15000U);
		CHECK(config.pingTimeOutMS == 10000U);
		CHECK(config.pingAlways == true);
		CHECK(config.defaultPingInterval == 15000U);

		std::shared_ptr<IWebSocket> socket = env->createWebSocket();
		CHECK(socket != nullptr);

		delete env;
	}

#if defined(_WIN32) || defined(_WIN64)
	TEST_CASE("DefaultWinEnv createConfig returns default config")
	{
		DefaultWinEnv env;
		Config config = env.createConfig();

		CHECK(config.connectTimeOutMS == 15000U);
		CHECK(config.pingTimeOutMS == 10000U);
		CHECK(config.pingAlways == true);
		CHECK(config.defaultPingInterval == 15000U);
		CHECK(config.retryPublishIntervalMS == 10000U);
		CHECK(config.tickAsyncWaitForMS == 50U);
	}

	TEST_CASE("DefaultWinEnv createWebSocket returns valid socket")
	{
		DefaultWinEnv env;
		std::shared_ptr<IWebSocket> socket = env.createWebSocket();

		CHECK(socket != nullptr);
		CHECK(socket->isConnected() == false);
	}

	TEST_CASE("DefaultWinEnv createWebSocket returns unique instances")
	{
		DefaultWinEnv env;
		std::shared_ptr<IWebSocket> socket1 = env.createWebSocket();
		std::shared_ptr<IWebSocket> socket2 = env.createWebSocket();

		CHECK(socket1 != nullptr);
		CHECK(socket2 != nullptr);
		CHECK(socket1 != socket2);
	}

	TEST_CASE("DefaultWinEnv multiple config creations are independent")
	{
		DefaultWinEnv env;
		Config config1 = env.createConfig();
		Config config2 = env.createConfig();

		config1.connectTimeOutMS = 20000U;
		CHECK(config2.connectTimeOutMS == 15000U);
	}

	TEST_CASE("DefaultWinEnv websocket has correct interface methods")
	{
		DefaultWinEnv env;
		std::shared_ptr<IWebSocket> socket = env.createWebSocket();

		REQUIRE(socket != nullptr);

		CHECK(socket->getLastError() >= 0);
		CHECK(socket->getLastCloseCode() == 0);
		CHECK(socket->getLastCloseReason() != nullptr);
	}
#endif

#if defined(__linux__)
	TEST_CASE("DefaultLinuxEnv createConfig returns default config")
	{
		DefaultLinuxEnv env;
		Config config = env.createConfig();

		CHECK(config.connectTimeOutMS == 15000U);
		CHECK(config.pingTimeOutMS == 10000U);
		CHECK(config.pingAlways == true);
		CHECK(config.defaultPingInterval == 15000U);
		CHECK(config.retryPublishIntervalMS == 10000U);
		CHECK(config.tickAsyncWaitForMS == 50U);
	}

	TEST_CASE("DefaultLinuxEnv createWebSocket returns valid socket")
	{
		DefaultLinuxEnv env;
		std::shared_ptr<IWebSocket> socket = env.createWebSocket();

		CHECK(socket != nullptr);
		CHECK(socket->isConnected() == false);
	}

	TEST_CASE("DefaultLinuxEnv createWebSocket returns unique instances")
	{
		DefaultLinuxEnv env;
		std::shared_ptr<IWebSocket> socket1 = env.createWebSocket();
		std::shared_ptr<IWebSocket> socket2 = env.createWebSocket();

		CHECK(socket1 != nullptr);
		CHECK(socket2 != nullptr);
		CHECK(socket1 != socket2);
	}

	TEST_CASE("DefaultLinuxEnv multiple config creations are independent")
	{
		DefaultLinuxEnv env;
		Config config1 = env.createConfig();
		Config config2 = env.createConfig();

		config1.connectTimeOutMS = 20000U;
		CHECK(config2.connectTimeOutMS == 15000U);
	}

	TEST_CASE("DefaultLinuxEnv websocket has correct interface methods")
	{
		DefaultLinuxEnv env;
		std::shared_ptr<IWebSocket> socket = env.createWebSocket();

		REQUIRE(socket != nullptr);

		CHECK(socket->getLastError() >= 0);
		CHECK(socket->getLastCloseCode() == 0);
		CHECK(socket->getLastCloseReason() != nullptr);
	}
#endif

	TEST_CASE("IMqttEnvironment polymorphism works correctly")
	{
		DefaultEnvironmentFactory factory;
		IMqttEnvironment* env = factory.createEnvironment();

		REQUIRE(env != nullptr);

		//Test polymorphic behavior
		Config config = env->createConfig();
		CHECK(config.connectTimeOutMS > 0);

		std::shared_ptr<IWebSocket> socket = env->createWebSocket();
		CHECK(socket != nullptr);

		delete env;
	}

	TEST_CASE("Environment can be deleted safely")
	{
		DefaultEnvironmentFactory factory;
		IMqttEnvironment* env = factory.createEnvironment();

		REQUIRE(env != nullptr);

		//Create some objects
		env->createConfig();
		env->createWebSocket();

		//Should not crash
		delete env;
		CHECK(true);
	}

	TEST_CASE("Multiple environments can coexist")
	{
		DefaultEnvironmentFactory factory;
		IMqttEnvironment* env1 = factory.createEnvironment();
		IMqttEnvironment* env2 = factory.createEnvironment();

		CHECK(env1 != nullptr);
		CHECK(env2 != nullptr);
		CHECK(env1 != env2);

		Config config1 = env1->createConfig();
		Config config2 = env2->createConfig();

		config1.connectTimeOutMS = 20000U;
		CHECK(config2.connectTimeOutMS == 15000U);

		delete env1;
		delete env2;
	}

	TEST_CASE("Config default values are correct")
	{
		DefaultEnvironmentFactory factory;
		IMqttEnvironment* env = factory.createEnvironment();

		Config config = env->createConfig();

		CHECK(config.connectTimeOutMS == 15000U);
		CHECK(config.pingTimeOutMS == 10000U);
		CHECK(config.pingAlways == true);
		CHECK(config.defaultPingInterval == 15000U);
		CHECK(config.retryPublishIntervalMS == 10000U);
		CHECK(config.tickAsyncWaitForMS == 50U);

		delete env;
	}

	TEST_CASE("WebSocket callbacks can be set")
	{
		DefaultEnvironmentFactory factory;
		IMqttEnvironment* env = factory.createEnvironment();

		std::shared_ptr<IWebSocket> socket = env->createWebSocket();
		REQUIRE(socket != nullptr);

		bool connectCalled = false;
		bool disconnectCalled = false;
		bool recvdCalled = false;
		bool errorCalled = false;

		socket->setOnConnectCallback([&](bool success) { connectCalled = true; });
		socket->setOnDisconnectCallback([&]() { disconnectCalled = true; });
		socket->setOnRecvdCallback([&](ByteBuffer&& data) { recvdCalled = true; });
		socket->setOnErrorCallback([&](std::uint16_t code) { errorCalled = true; });

		//Should not crash
		CHECK(true);

		delete env;
	}

	TEST_CASE("Config can be modified after creation")
	{
		DefaultEnvironmentFactory factory;
		IMqttEnvironment* env = factory.createEnvironment();

		Config config = env->createConfig();

		config.connectTimeOutMS = 30000U;
		config.pingTimeOutMS = 20000U;
		config.pingAlways = false;
		config.defaultPingInterval = 25000U;
		config.retryPublishIntervalMS = 15000U;
		config.tickAsyncWaitForMS = 100U;

		CHECK(config.connectTimeOutMS == 30000U);
		CHECK(config.pingTimeOutMS == 20000U);
		CHECK(config.pingAlways == false);
		CHECK(config.defaultPingInterval == 25000U);
		CHECK(config.retryPublishIntervalMS == 15000U);
		CHECK(config.tickAsyncWaitForMS == 100U);

		delete env;
	}
}