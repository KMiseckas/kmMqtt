#ifndef INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SENDPUBLISHJOB_H
#define INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SENDPUBLISHJOB_H

#include <cleanMqtt/Mqtt/Transport/ISendJob.h>
#include "cleanMqtt/Mqtt/Params/PublishOptions.h"
#include <cleanMqtt/Utils/PacketIdPool.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		class SendPublishJob : public ISendJob
		{
		public:
			SendPublishJob(MqttConnectionInfo* connectionInfo,
				PacketSendDelegate sendPacketCallback,
				PacketIdPool* packetIdPool,
				const std::uint16_t packetId,
				const char* topic,
				ByteBuffer&& payload,
				PublishOptions&& pubOptions) noexcept;

			SendResultData send() noexcept override;
			void cancel() noexcept override;

		private:
			PacketIdPool* m_packetIdPool{ nullptr };
			std::uint16_t m_packetId{ 0 };
			const char* m_topic{ nullptr };
			ByteBuffer m_payload;
			PublishOptions m_publishOptions;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SENDPUBLISHJOB_H