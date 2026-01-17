#ifndef INCLUDE_KMMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_PINGCOMPOSER_H
#define INCLUDE_KMMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_PINGCOMPOSER_H

#include <kmMqtt/Mqtt/Transport/IPacketComposer.h>

namespace kmMqtt
{
	namespace mqtt
	{
		class PingComposer : public IPacketComposer
		{
		public:
			PingComposer(mqtt::MqttConnectionInfo* connectionInfo) noexcept
				: IPacketComposer(connectionInfo)
			{};

			ComposeResult compose() noexcept override;
			void cancel() noexcept override;
		};
	}
}

#endif //INCLUDE_KMMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_PINGCOMPOSER_H