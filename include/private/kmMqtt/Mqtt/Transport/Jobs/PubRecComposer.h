// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_PUBRECCOMPOSER_H
#define INCLUDE_KMMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_PUBRECCOMPOSER_H

#include <kmMqtt/Mqtt/Transport/IPacketComposer.h>
#include <kmMqtt/Mqtt/Packets/Publish/Codes/PubRecReasonCode.h>
#include <kmMqtt/Mqtt/Params/PubRecOptions.h>

namespace kmMqtt
{
	namespace mqtt
	{
		class PubRecComposer : public IPacketComposer
		{
		public:
			PubRecComposer(mqtt::MqttConnectionInfo* connectionInfo,
				std::uint16_t publishPacketId,
				PubRecReasonCode reasonCode,
				PubRecOptions&& options) noexcept
				: IPacketComposer(connectionInfo),
				m_publishPacketId(publishPacketId),
				m_reasonCode(std::move(reasonCode)),
				m_options(std::move(options))
			{
			};

			ComposeResult compose() noexcept override;
			void cancel() noexcept override;

		private:
			std::uint16_t m_publishPacketId;
			PubRecReasonCode m_reasonCode;
			PubRecOptions m_options;
		};
	}
}

#endif //INCLUDE_KMMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_PUBRECCOMPOSER_H