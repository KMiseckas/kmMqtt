#ifndef INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SENDPUBACKJOB_H
#define INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SENDPUBACKJOB_H

#include <cleanMqtt/Mqtt/Transport/ISendJob.h>
#include <cleanMqtt/Mqtt/Packets/Publish/Codes/PubAckReasonCode.h>
#include <cleanMqtt/Mqtt/Params/PubAckOptions.h>

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
				packets::PubAckReasonCode reasonCode,
				PubAckOptions&& options) noexcept
				: interfaces::ISendJob(connectionInfo, sendPacketCallback),
				m_publishPacketId(publishPacketId),
				m_reasonCode(std::move(reasonCode)),
				m_options(std::move(options))
			{
			};

			interfaces::SendResultData send() noexcept override;
			void cancel() noexcept override;

		private:
			PacketID m_publishPacketId;
			packets::PubAckReasonCode m_reasonCode;
			PubAckOptions m_options;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SENDPUBACKJOB_H