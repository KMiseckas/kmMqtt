// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_CONNECTCOMPOSER_H
#define INCLUDE_KMMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_CONNECTCOMPOSER_H

#include <kmMqtt/Mqtt/Transport/IPacketComposer.h>

namespace kmMqtt
{
	namespace mqtt
	{
		class ConnectComposer : public IPacketComposer
		{
		public:
			ConnectComposer(mqtt::MqttConnectionInfo* connectionInfo) noexcept
				: IPacketComposer(connectionInfo)
			{};

			ComposeResult compose() noexcept override;
			void cancel() noexcept override;
		};
	}
}

#endif //INCLUDE_KMMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_CONNECTCOMPOSER_H
