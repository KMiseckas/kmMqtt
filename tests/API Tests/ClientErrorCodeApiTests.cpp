// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <doctest.h>
#include <kmMqtt/Mqtt/Enums/ClientErrorCode.h>

#include <cstdint>

TEST_SUITE("ClientErrorCode API Tests")
{
	using namespace kmMqtt::mqtt;

	TEST_CASE("ClientErrorCode timeout and async values are stable and unique")
	{
		CHECK(static_cast<std::uint16_t>(ClientErrorCode::Failed_Writing_To_Persistent_Storage) == 8U);
		CHECK(static_cast<std::uint16_t>(ClientErrorCode::Failed_Decoding_Packet) == 9U);
		CHECK(static_cast<std::uint16_t>(ClientErrorCode::TimeOut) == 10U);
		CHECK(static_cast<std::uint16_t>(ClientErrorCode::Using_Tick_Async) == 11U);
	}
}
