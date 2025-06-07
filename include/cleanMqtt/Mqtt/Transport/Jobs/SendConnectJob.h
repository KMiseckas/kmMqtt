#ifndef INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SENDCONNECTJOB_H
#define INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SENDCONNECTJOB_H

#include <cleanMqtt/Mqtt/Transport/ISendJob.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		class SendConnectJob : public interfaces::ISendJob
		{
		public:
			SendConnectJob(mqtt::MqttConnectionInfo* connectionInfo,
				PacketSendDelegate sendPacketCallback) noexcept
				: interfaces::ISendJob(connectionInfo, sendPacketCallback)
			{};

			interfaces::SendResultData send() noexcept override;
			void cancel() noexcept override;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SENDCONNECTJOB_H
