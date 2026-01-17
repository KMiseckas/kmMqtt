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
