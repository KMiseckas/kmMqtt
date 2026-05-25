// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <doctest.h>
#include <kmMqtt/Mqtt/ClientError.h>

#include <array>
#include <set>

using namespace kmMqtt::mqtt;

TEST_SUITE("ClientError Tests")
{
	TEST_CASE("Default constructor initializes with No_Error")
	{
		ClientError error;
		
		CHECK(error.errorCode == ClientErrorCode::No_Error);
		CHECK(error.errorMsg == std::string(""));
		CHECK(error.noError());
	}

	TEST_CASE("Constructor with error code only")
	{
		ClientError error(ClientErrorCode::Invalid_Argument);
		
		CHECK(error.errorCode == ClientErrorCode::Invalid_Argument);
		CHECK(error.errorMsg == std::string(""));
		CHECK_FALSE(error.noError());
	}

	TEST_CASE("Constructor with error code and message")
	{
		const char* message = "Test error message";
		ClientError error(ClientErrorCode::Not_Connected, message);
		
		CHECK(error.errorCode == ClientErrorCode::Not_Connected);
		CHECK(error.errorMsg == message);
		CHECK_FALSE(error.noError());
	}

	TEST_CASE("noError() returns correct value")
	{
		SUBCASE("Returns true for No_Error")
		{
			ClientError error(ClientErrorCode::No_Error);
			CHECK(error.noError());
		}

		SUBCASE("Returns false for any other error code")
		{
			ClientError error(ClientErrorCode::Invalid_Argument);
			CHECK_FALSE(error.noError());
		}
	}

	TEST_CASE("operator== with ClientErrorCode")
	{
		ClientError error(ClientErrorCode::Invalid_Argument);
		
		SUBCASE("Returns true when codes match")
		{
			CHECK(error == ClientErrorCode::Invalid_Argument);
		}

		SUBCASE("Returns false when codes don't match")
		{
			CHECK_FALSE(error == ClientErrorCode::No_Error);
		}
	}

	TEST_CASE("operator!= with ClientErrorCode")
	{
		ClientError error(ClientErrorCode::Invalid_Argument);
		
		SUBCASE("Returns false when codes match")
		{
			CHECK_FALSE(error != ClientErrorCode::Invalid_Argument);
		}

		SUBCASE("Returns true when codes don't match")
		{
			CHECK(error != ClientErrorCode::No_Error);
		}
	}

	TEST_CASE("Free function operator== (ClientErrorCode == ClientError)")
	{
		ClientError error(ClientErrorCode::Invalid_Argument);
		
		SUBCASE("Returns true when codes match")
		{
			CHECK(ClientErrorCode::Invalid_Argument == error);
		}

		SUBCASE("Returns false when codes don't match")
		{
			CHECK_FALSE(ClientErrorCode::No_Error == error);
		}
	}

	TEST_CASE("Free function operator!= (ClientErrorCode != ClientError)")
	{
		ClientError error(ClientErrorCode::Invalid_Argument);
		
		SUBCASE("Returns false when codes match")
		{
			CHECK_FALSE(ClientErrorCode::Invalid_Argument != error);
		}

		SUBCASE("Returns true when codes don't match")
		{
			CHECK(ClientErrorCode::No_Error != error);
		}
	}

	TEST_CASE("Base ClientErrorCode numeric values are unique")
	{
		const std::array<std::uint16_t, 12> baseCodes{
			static_cast<std::uint16_t>(ClientErrorCode::No_Error),
			static_cast<std::uint16_t>(ClientErrorCode::Unknown),
			static_cast<std::uint16_t>(ClientErrorCode::Invalid_Argument),
			static_cast<std::uint16_t>(ClientErrorCode::Missing_Argument),
			static_cast<std::uint16_t>(ClientErrorCode::Not_Connected),
			static_cast<std::uint16_t>(ClientErrorCode::Socket_Error),
			static_cast<std::uint16_t>(ClientErrorCode::MQTT_Not_Active),
			static_cast<std::uint16_t>(ClientErrorCode::Failed_Sending_Packet),
			static_cast<std::uint16_t>(ClientErrorCode::Failed_Writing_To_Persistent_Storage),
			static_cast<std::uint16_t>(ClientErrorCode::Failed_Decoding_Packet),
			static_cast<std::uint16_t>(ClientErrorCode::TimeOut),
			static_cast<std::uint16_t>(ClientErrorCode::Using_Tick_Async)
		};

		const std::set<std::uint16_t> uniqueCodes(baseCodes.begin(), baseCodes.end());
		CHECK(uniqueCodes.size() == baseCodes.size());
	}
}