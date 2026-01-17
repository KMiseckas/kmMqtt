// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_DISCONNECTCOMPOSER_H
#define INCLUDE_KMMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_DISCONNECTCOMPOSER_H

#include <kmMqtt/Mqtt/Transport/IPacketComposer.h>
#include <kmMqtt/mqtt/Params/DisconnectArgs.h>
#include <utility>

namespace kmMqtt
{
	namespace mqtt
	{
		class DisconnectComposer : public IPacketComposer
		{
		public:
			DisconnectComposer(mqtt::MqttConnectionInfo* connectionInfo,
				const DisconnectArgs&& options,
				DisconnectReasonCode reasonCode) noexcept
				: IPacketComposer(connectionInfo),
				m_options(std::move(options)),
				m_reasonCode(reasonCode)
			{
			};

			ComposeResult compose() noexcept override;
			void cancel() noexcept override;

		private:
			DisconnectArgs m_options;
			DisconnectReasonCode m_reasonCode;
		};
	}
}

#endif //INCLUDE_KMMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_DISCONNECTCOMPOSER_H
