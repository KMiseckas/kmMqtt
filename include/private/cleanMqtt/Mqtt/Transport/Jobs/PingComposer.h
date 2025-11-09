#ifndef INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_PINGCOMPOSER_H
#define INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_PINGCOMPOSER_H

#include <cleanMqtt/Mqtt/Transport/IPacketComposer.h>

namespace cleanMqtt
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

#endif //INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_PINGCOMPOSER_H