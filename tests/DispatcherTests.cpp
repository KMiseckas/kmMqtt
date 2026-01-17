#include <doctest.h>
#include <kmMqtt/Dispatchers/DefaultDispatcher.h>
#include <kmMqtt/Dispatchers/ImmediateDispatcher.h>
#include <kmMqtt/MqttClientOptions.h>
#include <kmMqtt/Interfaces/ICallbackDispatcher.h>
#include <atomic>
#include <thread>
#include <chrono>
#include <vector>
#include <cstring>
#include <map>

using namespace kmMqtt;

TEST_SUITE("Dispatcher Tests")
{
	using namespace kmMqtt;

	TEST_CASE("DefaultDispatcher - Does Not Execute Callback")
	{
		DefaultDispatcher dispatcher;
		bool callbackExecuted = false;

		UniqueFunction callback([&callbackExecuted]() {
			callbackExecuted = true;
		});

		dispatcher.dispatch(std::move(callback));

		//DefaultDispatcher should not execute the callback (it's deferred/ignored)
		CHECK(callbackExecuted == false);
	}

	TEST_CASE("DefaultDispatcher - Multiple Dispatches")
	{
		DefaultDispatcher dispatcher;
		int executionCount = 0;

		for (int i = 0; i < 5; ++i)
		{
			UniqueFunction callback([&executionCount]() {
				executionCount++;
			});

			dispatcher.dispatch(std::move(callback));
		}

		//None of the callbacks should be executed
		CHECK(executionCount == 0);
	}

	TEST_CASE("ImmediateDispatcher - Executes Callback Immediately")
	{
		ImmediateDispatcher dispatcher;
		bool callbackExecuted = false;

		UniqueFunction callback([&callbackExecuted]() {
			callbackExecuted = true;
		});

		dispatcher.dispatch(std::move(callback));

		//ImmediateDispatcher should execute the callback immediately
		CHECK(callbackExecuted == true);
	}

	TEST_CASE("ImmediateDispatcher - Multiple Dispatches")
	{
		ImmediateDispatcher dispatcher;
		int executionCount = 0;

		for (int i = 0; i < 5; ++i)
		{
			UniqueFunction callback([&executionCount]() {
				executionCount++;
			});

			dispatcher.dispatch(std::move(callback));
		}

		//All callbacks should be executed
		CHECK(executionCount == 5);
	}

	TEST_CASE("ImmediateDispatcher - Callback Execution Order")
	{
		ImmediateDispatcher dispatcher;
		std::vector<int> executionOrder;

		for (int i = 0; i < 10; ++i)
		{
			UniqueFunction callback([&executionOrder, i]() {
				executionOrder.push_back(i);
			});

			dispatcher.dispatch(std::move(callback));
		}

		// Callbacks should execute in order
		CHECK(executionOrder.size() == 10);
		for (int i = 0; i < 10; ++i)
		{
			CHECK(executionOrder[i] == i);
		}
	}

	TEST_CASE("ImmediateDispatcher - Callback with Captures")
	{
		ImmediateDispatcher dispatcher;
		
		std::string text = "Hello";
		int number = 42;
		bool flag = false;

		UniqueFunction callback([text, number, &flag]() {
			flag = (text == "Hello" && number == 42);
		});

		dispatcher.dispatch(std::move(callback));

		CHECK(flag == true);
	}

	TEST_CASE("ImmediateDispatcher - Nested Data Structure Modification")
	{
		ImmediateDispatcher dispatcher;
		
		std::map<std::string, std::vector<int>> data;
		
		UniqueFunction callback([&data]() {
			data["first"] = {1, 2, 3};
			data["second"] = {4, 5, 6};
		});

		dispatcher.dispatch(std::move(callback));

		CHECK(data.size() == 2);
		CHECK(data["first"].size() == 3);
		CHECK(data["first"][0] == 1);
		CHECK(data["second"][2] == 6);
	}
}