#include <doctest.h>
#include <kmMqtt/Mqtt/ReqResult.h>
#include <kmMqtt/Mqtt/ClientError.h>

using namespace kmMqtt::mqtt;

TEST_SUITE("ReqResult Tests")
{
	TEST_CASE("Default Constructor")
	{
		ReqResult result;

		CHECK(result.noError());
		CHECK(result.errorCode() == ClientErrorCode::No_Error);
		CHECK(result.errorMsg() == std::string(""));
		CHECK(result.packetId == NO_PACKET_ID);
		CHECK_FALSE(result.hasPacketId());
	}

	TEST_CASE("Constructor with ClientError Only")
	{
		ClientError error(ClientErrorCode::Invalid_Argument, "Test error message");
		ReqResult result(error);

		CHECK_FALSE(result.noError());
		CHECK(result.errorCode() == ClientErrorCode::Invalid_Argument);
		CHECK(result.errorMsg() == std::string("Test error message"));
		CHECK(result.packetId == NO_PACKET_ID);
		CHECK_FALSE(result.hasPacketId());
	}

	TEST_CASE("Constructor with ClientError and Packet ID")
	{
		ClientError error(ClientErrorCode::Not_Connected, "Client not connected");
		ReqResult result(error, 42);

		CHECK_FALSE(result.noError());
		CHECK(result.errorCode() == ClientErrorCode::Not_Connected);
		CHECK(result.errorMsg() == std::string("Client not connected"));
		CHECK(result.packetId == 42);
		CHECK(result.hasPacketId());
	}

	TEST_CASE("Constructor with ClientErrorCode and Message")
	{
		ReqResult result(ClientErrorCode::Socket_Connect_Failed, "Socket connection failed");

		CHECK_FALSE(result.noError());
		CHECK(result.errorCode() == ClientErrorCode::Socket_Connect_Failed);
		CHECK(result.errorMsg() == std::string("Socket connection failed"));
		CHECK(result.packetId == NO_PACKET_ID);
		CHECK_FALSE(result.hasPacketId());
	}

	TEST_CASE("Constructor with ClientErrorCode, Message, and Packet ID")
	{
		ReqResult result(ClientErrorCode::Failed_Sending_Packet, "Failed to send packet", 100);

		CHECK_FALSE(result.noError());
		CHECK(result.errorCode() == ClientErrorCode::Failed_Sending_Packet);
		CHECK(result.errorMsg() == std::string("Failed to send packet"));
		CHECK(result.packetId == 100);
		CHECK(result.hasPacketId());
	}

	TEST_CASE("noError() - Success Case")
	{
		ReqResult result(ClientError(ClientErrorCode::No_Error), 10);

		CHECK(result.noError());
		CHECK(result.errorCode() == ClientErrorCode::No_Error);
		CHECK(result.hasPacketId());
		CHECK(result.packetId == 10);
	}

	TEST_CASE("noError() - Error Case")
	{
		ReqResult result(ClientErrorCode::TimeOut, "Request timed out");

		CHECK_FALSE(result.noError());
		CHECK(result.errorCode() == ClientErrorCode::TimeOut);
	}

	TEST_CASE("hasPacketId() - Valid Packet ID")
	{
		SUBCASE("Packet ID = 1")
		{
			ReqResult result(ClientErrorCode::No_Error, "", 1);
			CHECK(result.hasPacketId());
			CHECK(result.packetId == 1);
		}

		SUBCASE("Packet ID = 65535 (max uint16_t)")
		{
			ReqResult result(ClientErrorCode::No_Error, "", 65535);
			CHECK(result.hasPacketId());
			CHECK(result.packetId == 65535);
		}

		SUBCASE("Packet ID = 12345")
		{
			ReqResult result(ClientErrorCode::No_Error, "", 12345);
			CHECK(result.hasPacketId());
			CHECK(result.packetId == 12345);
		}
	}

	TEST_CASE("hasPacketId() - No Packet ID")
	{
		SUBCASE("Default constructor")
		{
			ReqResult result;
			CHECK_FALSE(result.hasPacketId());
			CHECK(result.packetId == NO_PACKET_ID);
		}

		SUBCASE("Explicit NO_PACKET_ID")
		{
			ReqResult result(ClientErrorCode::Invalid_Argument, "Error", NO_PACKET_ID);
			CHECK_FALSE(result.hasPacketId());
			CHECK(result.packetId == NO_PACKET_ID);
		}

		SUBCASE("Packet ID = 0")
		{
			ReqResult result(ClientErrorCode::No_Error, "", 0);
			CHECK_FALSE(result.hasPacketId());
			CHECK(result.packetId == 0);
		}
	}

	TEST_CASE("errorCode() - Various Error Codes")
	{
		SUBCASE("No_Error")
		{
			ReqResult result(ClientErrorCode::No_Error, "");
			CHECK(result.errorCode() == ClientErrorCode::No_Error);
		}

		SUBCASE("Already_Connected")
		{
			ReqResult result(ClientErrorCode::Already_Connected, "Already connected");
			CHECK(result.errorCode() == ClientErrorCode::Already_Connected);
		}

		SUBCASE("Already_Connecting")
		{
			ReqResult result(ClientErrorCode::Already_Connecting, "Already connecting");
			CHECK(result.errorCode() == ClientErrorCode::Already_Connecting);
		}

		SUBCASE("Missing_Argument")
		{
			ReqResult result(ClientErrorCode::Missing_Argument, "Missing argument");
			CHECK(result.errorCode() == ClientErrorCode::Missing_Argument);
		}

		SUBCASE("Failed_Decoding_Packet")
		{
			ReqResult result(ClientErrorCode::Failed_Decoding_Packet, "Decode failed");
			CHECK(result.errorCode() == ClientErrorCode::Failed_Decoding_Packet);
		}
	}

	TEST_CASE("errorMsg() - Various Messages")
	{
		SUBCASE("Empty message")
		{
			ReqResult result(ClientErrorCode::No_Error, "");
			CHECK(result.errorMsg() == std::string(""));
		}

		SUBCASE("Short message")
		{
			ReqResult result(ClientErrorCode::Invalid_Argument, "Error");
			CHECK(result.errorMsg() == std::string("Error"));
		}

		SUBCASE("Long message")
		{
			const char* longMsg = "This is a very long error message that describes in detail what went wrong during the operation.";
			ReqResult result(ClientErrorCode::Socket_Error, longMsg);
			CHECK(result.errorMsg() == std::string(longMsg));
		}

		SUBCASE("Message with special characters")
		{
			ReqResult result(ClientErrorCode::MQTT_Not_Active, "Error: Connection failed (code: 0x01)");
			CHECK(result.errorMsg() == std::string("Error: Connection failed (code: 0x01)"));
		}
	}

	TEST_CASE("Success Result with Packet ID")
	{
		ReqResult result(ClientError(ClientErrorCode::No_Error), 255);

		CHECK(result.noError());
		CHECK(result.hasPacketId());
		CHECK(result.packetId == 255);
		CHECK(result.errorCode() == ClientErrorCode::No_Error);
		CHECK(result.errorMsg() == std::string(""));
	}

	TEST_CASE("Error Result without Packet ID")
	{
		ReqResult result(ClientErrorCode::Not_Connected, "Client is not connected to broker");

		CHECK_FALSE(result.noError());
		CHECK_FALSE(result.hasPacketId());
		CHECK(result.packetId == NO_PACKET_ID);
		CHECK(result.errorCode() == ClientErrorCode::Not_Connected);
		CHECK(result.errorMsg() == std::string("Client is not connected to broker"));
	}

	TEST_CASE("Error Result with Packet ID")
	{
		ReqResult result(ClientErrorCode::Failed_Sending_Packet, "Network error", 999);

		CHECK_FALSE(result.noError());
		CHECK(result.hasPacketId());
		CHECK(result.packetId == 999);
		CHECK(result.errorCode() == ClientErrorCode::Failed_Sending_Packet);
		CHECK(result.errorMsg() == std::string("Network error"));
	}

	TEST_CASE("Copy Semantics")
	{
		ReqResult original(ClientErrorCode::TimeOut, "Operation timed out", 123);

		SUBCASE("Copy construction")
		{
			ReqResult copy(original);

			CHECK(copy.errorCode() == original.errorCode());
			CHECK(std::strcmp(copy.errorMsg(), original.errorMsg()) == 0);
			CHECK(copy.packetId == original.packetId);
			CHECK(copy.hasPacketId() == original.hasPacketId());
			CHECK(copy.noError() == original.noError());
		}

		SUBCASE("Copy assignment")
		{
			ReqResult copy;
			copy = original;

			CHECK(copy.errorCode() == original.errorCode());
			CHECK(std::strcmp(copy.errorMsg(), original.errorMsg()) == 0);
			CHECK(copy.packetId == original.packetId);
			CHECK(copy.hasPacketId() == original.hasPacketId());
			CHECK(copy.noError() == original.noError());
		}
	}

	TEST_CASE("Move Semantics")
	{
		SUBCASE("Move construction")
		{
			ReqResult original(ClientErrorCode::Invalid_Argument, "Invalid input", 456);
			ClientErrorCode originalCode = original.errorCode();
			std::uint16_t originalPacketId = original.packetId;

			ReqResult moved(std::move(original));

			CHECK(moved.errorCode() == originalCode);
			CHECK(moved.packetId == originalPacketId);
			CHECK(moved.hasPacketId());
		}

		SUBCASE("Move assignment")
		{
			ReqResult original(ClientErrorCode::Socket_Connect_Failed, "Connection refused", 789);
			ClientErrorCode originalCode = original.errorCode();
			std::uint16_t originalPacketId = original.packetId;

			ReqResult moved;
			moved = std::move(original);

			CHECK(moved.errorCode() == originalCode);
			CHECK(moved.packetId == originalPacketId);
			CHECK(moved.hasPacketId());
		}
	}

	TEST_CASE("Boundary Values for Packet ID")
	{
		SUBCASE("Minimum valid packet ID (1)")
		{
			ReqResult result(ClientErrorCode::No_Error, "", 1);
			CHECK(result.hasPacketId());
			CHECK(result.packetId == 1);
		}

		SUBCASE("Maximum packet ID (65535)")
		{
			ReqResult result(ClientErrorCode::No_Error, "", 65535);
			CHECK(result.hasPacketId());
			CHECK(result.packetId == 65535);
		}

		SUBCASE("NO_PACKET_ID constant (0)")
		{
			CHECK(NO_PACKET_ID == 0);
			ReqResult result(ClientErrorCode::No_Error, "", NO_PACKET_ID);
			CHECK_FALSE(result.hasPacketId());
			CHECK(result.packetId == 0);
		}
	}

	TEST_CASE("Multiple Results Comparison")
	{
		ReqResult success(ClientErrorCode::No_Error, "", 100);
		ReqResult error1(ClientErrorCode::Not_Connected, "Not connected", 200);
		ReqResult error2(ClientErrorCode::TimeOut, "Timeout", 300);

		CHECK(success.noError());
		CHECK_FALSE(error1.noError());
		CHECK_FALSE(error2.noError());

		CHECK(success.errorCode() != error1.errorCode());
		CHECK(error1.errorCode() != error2.errorCode());

		CHECK(success.packetId != error1.packetId);
		CHECK(error1.packetId != error2.packetId);
	}

	TEST_CASE("Public Member Access")
	{
		ReqResult result(ClientErrorCode::Invalid_Argument, "Test", 42);

		// Direct access to public members
		CHECK(result.error.errorCode == ClientErrorCode::Invalid_Argument);
		CHECK(result.error.errorMsg == "Test");
		CHECK(result.packetId == 42);

		// Modify through public members
		result.error.errorCode = ClientErrorCode::No_Error;
		result.packetId = 100;

		CHECK(result.errorCode() == ClientErrorCode::No_Error);
		CHECK(result.packetId == 100);
		CHECK(result.hasPacketId());
	}

	TEST_CASE("Integration - Typical Usage Scenarios")
	{
		SUBCASE("Successful publish with QoS 1")
		{
			ReqResult result(ClientError(ClientErrorCode::No_Error), 1001);
			
			CHECK(result.noError());
			CHECK(result.hasPacketId());
			
			if (result.noError() && result.hasPacketId()) {
				// Packet ID would be used to track acknowledgment
				CHECK(result.packetId == 1001);
			}
		}

		SUBCASE("Failed subscribe due to not connected")
		{
			ReqResult result(ClientErrorCode::Not_Connected, "Cannot subscribe when not connected");
			
			CHECK_FALSE(result.noError());
			CHECK_FALSE(result.hasPacketId());
			
			if (!result.noError()) {
				// Error would be logged or handled
				CHECK(result.errorMsg() != std::string(""));
			}
		}

		SUBCASE("Successful publish with QoS 0 (no packet ID)")
		{
			ReqResult result(ClientError(ClientErrorCode::No_Error, ""));
			
			CHECK(result.noError());
			CHECK_FALSE(result.hasPacketId());
			CHECK(result.packetId == NO_PACKET_ID);
		}
	}

	TEST_CASE("Constexpr and Noexcept Guarantees")
	{
		//These should compile without warnings in C++14
		ReqResult result(ClientErrorCode::No_Error, "");
		
		//All these methods are marked noexcept
		CHECK_NOTHROW(result.noError());
		CHECK_NOTHROW(result.hasPacketId());
		CHECK_NOTHROW(result.errorCode());
		CHECK_NOTHROW(result.errorMsg());
	}
}