#ifndef INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SENDPUBACKJOB_H
#define INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SENDPUBACKJOB_H

#include <cleanMqtt/Mqtt/Transport/ISendJob.h>
#include <cleanMqtt/Mqtt/Packets/Publish/Codes/PubAckReasonCode.h>
#include <cleanMqtt/Mqtt/Params/PubAckOptions.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		class SendPubAckJob : public ISendJob
		{
		public:
			SendPubAckJob(mqtt::MqttConnectionInfo* connectionInfo,
				PacketSendDelegate sendPacketCallback,
				std::uint16_t publishPacketId,
				PubAckReasonCode reasonCode,
				PubAckOptions&& options) noexcept
				: ISendJob(connectionInfo, sendPacketCallback),
				m_publishPacketId(publishPacketId),
				m_reasonCode(std::move(reasonCode)),
				m_options(std::move(options))
			{
			};

			SendResultData send() noexcept override;
			void cancel() noexcept override;

		private:
			std::uint16_t m_publishPacketId;
			PubAckReasonCode m_reasonCode;
			PubAckOptions m_options;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SENDPUBACKJOB_H