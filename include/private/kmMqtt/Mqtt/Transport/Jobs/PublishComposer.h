// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_PUBLISHCOMPOSER_H
#define INCLUDE_KMMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_PUBLISHCOMPOSER_H

#include <kmMqtt/Mqtt/Transport/IPacketComposer.h>
#include "kmMqtt/Mqtt/Params/PublishOptions.h"
#include <kmMqtt/Utils/PacketIdPool.h>
#include "kmMqtt/Mqtt/ReceiveMaximumTracker.h"

namespace kmMqtt
{
	namespace mqtt
	{
		class PublishComposer : public IPacketComposer
		{
		public:
			PublishComposer(MqttConnectionInfo* connectionInfo,
				PacketIdPool* packetIdPool,
				const std::uint16_t packetId,
				std::string topic,
				ByteBuffer&& payload,
				PublishOptions&& pubOptions,
				ReceiveMaximumTracker* recMaxTracker,
				bool isDup) noexcept;

			bool canSend() const noexcept override;
			ComposeResult compose() noexcept override;
			Qos getQos() const noexcept override;
			void cancel() noexcept override;

		private:
			PacketIdPool* m_packetIdPool{ nullptr };
			std::uint16_t m_packetId{ 0 };
			std::string m_topic{ nullptr };
			ByteBuffer m_payload;
			PublishOptions m_publishOptions;
			ReceiveMaximumTracker* m_recMaxTracker;
			bool m_isDup{ false };
		};
	}
}

#endif //INCLUDE_KMMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_PUBLISHCOMPOSER_H