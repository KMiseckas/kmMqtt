// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <doctest.h>
#include <kmMqtt/Mqtt/Packets/Connection/Connect.h>
#include <kmMqtt/Mqtt/Packets/FixedHeader.h>

TEST_SUITE("Connect Packet Tests")
{
	using namespace kmMqtt::mqtt;

	TEST_CASE("Packet")
	{
		EncodedConnectFlags flags;
		flags.setFlagValue(ConnectFlags::CLEAN_START, true);
		flags.setFlagValue(ConnectFlags::PASSWORD, false);
		flags.setFlagValue(ConnectFlags::USER_NAME, false);
		flags.setFlagValue(ConnectFlags::WILL_QOS, 2);
		flags.setFlagValue(ConnectFlags::WILL_RETAIN, false);

		Properties properties;
		properties.tryAddProperty<PropertyType::WILL_DELAY_INTERVAL>(200); //Size = 5 (Property Id (1) + Data Type UINT32 (4))

		ConnectVariableHeader varHeader
		{
			"MQTT",
			MqttVersion::MQTT_5_0,
			43000,
			std::move(flags),
			std::move(properties)
		};

		ConnectPayloadHeader payloadHeader;
		payloadHeader.clientId = "RandomID";

		Connect connectPacket{ std::move(varHeader), std::move(payloadHeader) };

		CHECK(connectPacket.getPacketType() == PacketType::CONNECT);
		CHECK(connectPacket.getFixedHeader().remainingLength.uint32Value() == 0);
		CHECK((connectPacket.getVariableHeader().flags.getFlags() & static_cast<std::uint8_t>(ConnectFlags::RESERVED)) == 0);
		CHECK((connectPacket.getVariableHeader().flags.getFlags() & static_cast<std::uint8_t>(ConnectFlags::CLEAN_START)) == static_cast<std::uint8_t>(ConnectFlags::CLEAN_START));
		CHECK((connectPacket.getVariableHeader().flags.getFlags() & static_cast<std::uint8_t>(ConnectFlags::USER_NAME)) == 0);
		CHECK((connectPacket.getVariableHeader().flags.getFlags() & static_cast<std::uint8_t>(ConnectFlags::PASSWORD)) == 0);
		CHECK((connectPacket.getVariableHeader().flags.getFlags() & static_cast<std::uint8_t>(ConnectFlags::WILL_QOS)) == 0b0010000);

		CHECK_NOTHROW(connectPacket.encode());
		CHECK(connectPacket.getFixedHeader().packetType == PacketType::CONNECT);
		CHECK(connectPacket.getDataBuffer().headroom() == connectPacket.getDataBuffer().capacity() - connectPacket.getDataBuffer().size());
		CHECK(connectPacket.getDataBuffer().size() > 0);
	}
}