#ifndef INCLUDE_KMMQTT_MQTT_PACKETHELPER_H
#define INCLUDE_KMMQTT_MQTT_PACKETHELPER_H

#include <kmMqtt/Mqtt/Packets/Connection/Connect.h>
#include <kmMqtt/Mqtt/Packets/Connection/ConnectAck.h>
#include <kmMqtt/Mqtt/Packets/Connection/Disconnect.h>
#include <kmMqtt/Mqtt/Packets/Ping/PingReq.h>
#include <kmMqtt/Mqtt/Packets/Ping/PingResp.h>
#include <kmMqtt/Mqtt/Packets/Publish/Publish.h>
#include <kmMqtt/Mqtt/Packets/Publish/PublishAck.h>
#include <kmMqtt/Mqtt/Packets/Publish/PublishComp.h>
#include <kmMqtt/Mqtt/Packets/Publish/PublishRel.h>
#include <kmMqtt/Mqtt/Packets/Publish/PublishRec.h>
#include <kmMqtt/Mqtt/Packets/Subscribe/Subscribe.h>
#include <kmMqtt/Mqtt/Packets/UnSubscribe/UnSubscribe.h>
#include <kmMqtt/Mqtt/Packets/UnSubscribe/UnSubscribeAck.h>
#include <kmMqtt/Mqtt/Params/Topic.h>

namespace kmMqtt
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
		Publish createPublishPacket(const MqttConnectionInfo& connectionInfo, bool isDup, const char* topic, const ByteBuffer& payload, const PublishOptions& options, std::uint16_t packetId = 0) noexcept;
		PublishAck createPubAckPacket(std::uint16_t packetId, PubAckReasonCode reasonCode, const PubAckOptions& options) noexcept;
		PublishRec createPubRecPacket(std::uint16_t packetId, PubRecReasonCode reasonCode, const PubRecOptions& options) noexcept;
		PublishRel createPubRelPacket(std::uint16_t packetId, PubRelReasonCode reasonCode, const PubRelOptions& options) noexcept;
		PublishComp createPubCompPacket(std::uint16_t packetId, PubCompReasonCode reasonCode, const PubCompOptions& options) noexcept;
		Subscribe createSubscribePacket(std::uint16_t packetId, const std::vector<Topic>& topics, const SubscribeOptions& options) noexcept;
		UnSubscribe createUnSubscribePacket(std::uint16_t packetId, const std::vector<Topic>& topics, const UnSubscribeOptions& options) noexcept;
	}
}

#endif //INCLUDE_KMMQTT_MQTT_PACKETHELPER_H 