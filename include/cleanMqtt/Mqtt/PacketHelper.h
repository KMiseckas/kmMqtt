#ifndef INCLUDE_CLEANMQTT_MQTT_PACKETHELPER_H
#define INCLUDE_CLEANMQTT_MQTT_PACKETHELPER_H

#include <cleanMqtt/Mqtt/MqttConnectionInfo.h>
#include <cleanMqtt/Mqtt/Params/PublishOptions.h>
#include <cleanMqtt/Utils/PacketIdPool.h>
#include <cleanMqtt/Mqtt/Packets/Connection/Connect.h>
#include <cleanMqtt/Mqtt/Packets/Connection/ConnectAck.h>
#include <cleanMqtt/Mqtt/Packets/Connection/Disconnect.h>
#include <cleanMqtt/Mqtt/Params/DisconnectArgs.h>
#include <cleanMqtt/Mqtt/Packets/Ping/PingReq.h>
#include <cleanMqtt/Mqtt/Packets/Ping/PingResp.h>
#include <cleanMqtt/Mqtt/Packets/Publish/Publish.h>
#include <cleanMqtt/Mqtt/Packets/Publish/PublishAck.h>
#include <cleanMqtt/Mqtt/Params/PubAckOptions.h>
#include <cleanMqtt/Mqtt/Packets/Subscribe/Subscribe.h>
#include <cleanMqtt/Mqtt/Params/SubscribeOptions.h>
#include <cleanMqtt/Mqtt/Packets/UnSubscribe/UnSubscribe.h>
#include <cleanMqtt/Mqtt/Packets/UnSubscribe/UnSubscribeAck.h>
#include <cleanMqtt/Mqtt/Params/UnSubscribeOptions.h>
#include <cleanMqtt/Mqtt/Params/Topic.h>

namespace cleanMqtt
{
	namespace mqtt
	{	
		using namespace packets;

		Connect createConnectPacket(const MqttConnectionInfo& connectionInfo) noexcept;
		Disconnect createDisconnectPacket(const MqttConnectionInfo& connectionInfo, const DisconnectArgs& args, DisconnectReasonCode reason) noexcept;
		PingReq createPingRequestPacket() noexcept;
		PingResp createPingResponsePacket() noexcept;
		Publish createPublishPacket(const MqttConnectionInfo& connectionInfo, const char* topic, const ByteBuffer& payload, const PublishOptions& options, PacketID packetId = 0) noexcept;
		PublishAck createPubAckPacket(std::uint16_t packetId, PubAckReasonCode reasonCode, const PubAckOptions& options) noexcept;
		Subscribe createSubscribePacket(std::uint16_t packetId, const std::vector<Topic>& topics, const SubscribeOptions& options) noexcept;
		UnSubscribe createUnSubscribePacket(std::uint16_t packetId, const std::vector<Topic>& topics, const UnSubscribeOptions& options) noexcept;
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_PACKETHELPER_H 