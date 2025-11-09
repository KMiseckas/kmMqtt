#ifndef INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_CONNECTCOMPOSER_H
#define INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_CONNECTCOMPOSER_H

#include <cleanMqtt/Mqtt/Transport/IPacketComposer.h>

namespace cleanMqtt
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

#endif //INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_CONNECTCOMPOSER_H
