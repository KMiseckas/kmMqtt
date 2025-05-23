#ifndef INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SENDPINGJOB_H
#define INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SENDPINGJOB_H

#include <cleanMqtt/Interfaces/ISendJob.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		class SendPingJob : public interfaces::ISendJob
		{
		public:
			SendPingJob(mqtt::MqttConnectionInfo* connectionInfo,
				PacketSendDelegate sendPacketCallback,
				bool enforceMaxPacketSize = false,
				std::size_t maxPacketSize = 0U) noexcept
				: interfaces::ISendJob(connectionInfo, sendPacketCallback, enforceMaxPacketSize, maxPacketSize)
			{};

			interfaces::SendResultData send() noexcept override;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SENDPINGJOB_H