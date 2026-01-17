#include <doctest.h>
#include <kmMqtt/Mqtt/ClientError.h>

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
}