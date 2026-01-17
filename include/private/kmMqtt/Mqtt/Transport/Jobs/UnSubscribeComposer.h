// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_UNSUBSCRIBECOMPOSER_H
#define INCLUDE_KMMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_UNSUBSCRIBECOMPOSER_H

#include <kmMqtt/Mqtt/Transport/IPacketComposer.h>
#include <kmMqtt/Mqtt/Params/Topic.h>
#include <kmMqtt/Utils/PacketIdPool.h>
#include <kmMqtt/Mqtt/Params/UnSubscribeOptions.h>

namespace kmMqtt
{
	namespace mqtt
	{
		class UnSubscribeComposer : public IPacketComposer
		{
		public:
			UnSubscribeComposer(
				MqttConnectionInfo* connectionInfo,
				PacketIdPool* packetIdPool,
				const std::uint16_t packetId,
				std::vector<Topic> topics,
				UnSubscribeOptions&& options) noexcept;

			ComposeResult compose() noexcept override;
			void cancel() noexcept override;

		private:
			PacketIdPool* m_packetIdPool{ nullptr };
			std::uint16_t m_packetId{ 0 };
			std::vector<Topic> m_topics;
			UnSubscribeOptions m_unSubscribeOptions{};
		};
	}
}

#endif //INCLUDE_KMMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_UNSUBSCRIBECOMPOSER_H
