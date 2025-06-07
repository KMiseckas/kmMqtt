#ifndef INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SENDPINGJOB_H
#define INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SENDPINGJOB_H

#include <cleanMqtt/Mqtt/Transport/ISendJob.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		class SendPingJob : public interfaces::ISendJob
		{
		public:
			SendPingJob(mqtt::MqttConnectionInfo* connectionInfo,
				PacketSendDelegate sendPacketCallback) noexcept
				: interfaces::ISendJob(connectionInfo, sendPacketCallback)
			{};

			interfaces::SendResultData send() noexcept override;
			void cancel() noexcept override;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SENDPINGJOB_H