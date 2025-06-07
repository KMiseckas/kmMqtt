#ifndef INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SENDPUBACKJOB_H
#define INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SENDPUBACKJOB_H

#include <cleanMqtt/Mqtt/Transport/ISendJob.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		class SendPubAckJob : public interfaces::ISendJob
		{
		public:
			SendPubAckJob(mqtt::MqttConnectionInfo* connectionInfo,
				PacketSendDelegate sendPacketCallback,
				PacketID publishPacketId,
				bool enforceMaxPacketSize = false,
				std::size_t maxPacketSize = 0U) noexcept
				: interfaces::ISendJob(connectionInfo, sendPacketCallback, enforceMaxPacketSize, maxPacketSize),
				m_publishPacketId(publishPacketId)
			{
			};

			interfaces::SendResultData send() noexcept override;
			void cancel() noexcept override;

		private:
			PacketID m_publishPacketId;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SENDPUBACKJOB_H