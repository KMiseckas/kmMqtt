#ifndef INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SENDPUBLISHJOB_H
#define INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SENDPUBLISHJOB_H

#include <cleanMqtt/Mqtt/Transport/ISendJob.h>
#include "cleanMqtt/Mqtt/Params/PublishOptions.h"
#include <cleanMqtt/Utils/PacketIdPool.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		class SendPublishJob : public interfaces::ISendJob
		{
		public:
			SendPublishJob(MqttConnectionInfo* connectionInfo,
				PacketSendDelegate sendPacketCallback,
				PacketIdPool* packetIdPool,
				const PacketID packetId,
				const char* topic,
				ByteBuffer&& payload,
				PublishOptions&& pubOptions) noexcept;

			interfaces::SendResultData send() noexcept override;
			void cancel() noexcept override;

		private:
			PacketIdPool* m_packetIdPool{ nullptr };
			PacketID m_packetId{ 0 };
			const char* m_topic{ nullptr };
			ByteBuffer m_payload;
			PublishOptions m_publishOptions;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SENDPUBLISHJOB_H