#ifndef INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SENDCONNECTJOB_H
#define INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SENDCONNECTJOB_H

#include <cleanMqtt/Interfaces/ISendJob.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		class SendConnectJob : public interfaces::ISendJob
		{
		public:
			SendConnectJob(mqtt::MqttConnectionInfo* connectionInfo,
				PacketSendDelegate sendPacketCallback,
				bool enforceMaxPacketSize = false,
				std::size_t maxPacketSize = 0U) noexcept
				: interfaces::ISendJob(connectionInfo, sendPacketCallback, enforceMaxPacketSize, maxPacketSize)
			{};

			interfaces::SendResultData send() noexcept override;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SENDCONNECTJOB_H
