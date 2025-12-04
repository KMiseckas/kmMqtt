#ifndef INCLUDE_CLEANMQTT_MQTT_PACKETHELPER_H
#define INCLUDE_CLEANMQTT_MQTT_PACKETHELPER_H

#include <cleanMqtt/Mqtt/Packets/Connection/Connect.h>
#include <cleanMqtt/Mqtt/Packets/Connection/ConnectAck.h>
#include <cleanMqtt/Mqtt/Packets/Connection/Disconnect.h>
#include <cleanMqtt/Mqtt/Packets/Ping/PingReq.h>
#include <cleanMqtt/Mqtt/Packets/Ping/PingResp.h>
#include <cleanMqtt/Mqtt/Packets/Publish/Publish.h>
#include <cleanMqtt/Mqtt/Packets/Publish/PublishAck.h>
#include <cleanMqtt/Mqtt/Packets/Publish/PublishComp.h>
#include <cleanMqtt/Mqtt/Packets/Publish/PublishRel.h>
#include <cleanMqtt/Mqtt/Packets/Publish/PublishRec.h>
#include <cleanMqtt/Mqtt/Packets/Subscribe/Subscribe.h>
#include <cleanMqtt/Mqtt/Packets/UnSubscribe/UnSubscribe.h>
#include <cleanMqtt/Mqtt/Packets/UnSubscribe/UnSubscribeAck.h>
#include <cleanMqtt/Mqtt/Params/Topic.h>

namespace cleanMqtt
{
	namespace mqtt
	{	
		struct DisconnectArgs;
		struct PublishOptions;
		struct PubRecOptions;
		struct PubRelOptions;
		struct PubCompOptions;
		struct PubAckOptions;
		struct SubscribeOptions;
		struct UnSubscribeOptions;
		struct MqttConnectionInfo;

		Connect createConnectPacket(const MqttConnectionInfo& connectionInfo) noexcept;
		Disconnect createDisconnectPacket(const MqttConnectionInfo& connectionInfo, const DisconnectArgs& args, DisconnectReasonCode reason) noexcept;
		PingReq createPingRequestPacket() noexcept;
		PingResp createPingResponsePacket() noexcept;
		Publish createPublishPacket(const MqttConnectionInfo& connectionInfo, const char* topic, const ByteBuffer& payload, const PublishOptions& options, std::uint16_t packetId = 0) noexcept;
		PublishAck createPubAckPacket(std::uint16_t packetId, PubAckReasonCode reasonCode, const PubAckOptions& options) noexcept;
		PublishRec createPubRecPacket(std::uint16_t packetId, PubRecReasonCode reasonCode, const PubRecOptions& options) noexcept;
		PublishRel createPubRelPacket(std::uint16_t packetId, PubRelReasonCode reasonCode, const PubRelOptions& options) noexcept;
		PublishComp createPubCompPacket(std::uint16_t packetId, PubCompReasonCode reasonCode, const PubCompOptions& options) noexcept;
		Subscribe createSubscribePacket(std::uint16_t packetId, const std::vector<Topic>& topics, const SubscribeOptions& options) noexcept;
		UnSubscribe createUnSubscribePacket(std::uint16_t packetId, const std::vector<Topic>& topics, const UnSubscribeOptions& options) noexcept;
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_PACKETHELPER_H 