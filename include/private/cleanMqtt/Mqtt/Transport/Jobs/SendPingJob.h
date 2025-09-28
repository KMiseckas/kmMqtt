#ifndef INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SENDPINGJOB_H
#define INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SENDPINGJOB_H

#include <cleanMqtt/Mqtt/Transport/ISendJob.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		class SendPingJob : public ISendJob
		{
		public:
			SendPingJob(mqtt::MqttConnectionInfo* connectionInfo,
				PacketSendDelegate sendPacketCallback) noexcept
				: ISendJob(connectionInfo, sendPacketCallback)
			{};

			SendResultData send() noexcept override;
			void cancel() noexcept override;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SENDPINGJOB_H