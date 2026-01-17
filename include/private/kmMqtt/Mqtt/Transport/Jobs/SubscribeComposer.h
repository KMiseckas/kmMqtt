// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SUBSCRIBECOMPOSER_H
#define INCLUDE_KMMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SUBSCRIBECOMPOSER_H

#include <kmMqtt/Mqtt/Transport/IPacketComposer.h>
#include <kmMqtt/Mqtt/Params/SubscribeOptions.h>
#include <kmMqtt/Mqtt/Params/Topic.h>
#include <kmMqtt/Utils/PacketIdPool.h>

namespace kmMqtt
{
	namespace mqtt
	{
		class SubscribeComposer : public IPacketComposer
		{
		public:
			SubscribeComposer(
				MqttConnectionInfo* connectionInfo,
				PacketIdPool* packetIdPool,
				const std::uint16_t packetId,
				std::vector<Topic> topics,
				SubscribeOptions&& subscribeOptions) noexcept;

			ComposeResult compose() noexcept override;
			void cancel() noexcept override;

		private:
			PacketIdPool* m_packetIdPool{ nullptr };
			std::uint16_t m_packetId{ 0 };
			std::vector<Topic> m_topics;
			SubscribeOptions m_subscribeOptions;
		};
	}
}

#endif //INCLUDE_KMMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SUBSCRIBECOMPOSER_H
