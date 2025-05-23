#ifndef INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SENDPUBLISHJOB_H
#define INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SENDPUBLISHJOB_H

#include <cleanMqtt/Interfaces/ISendJob.h>
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
				PacketIdPool* idPool,
				const char* topic,
				ByteBuffer&& payload,
				PublishOptions&& pubOptions,
				bool enforceMaxPacketSize = false,
				std::size_t maxPacketSize = 0U) noexcept;

			interfaces::SendResultData send() noexcept override;

		private:
			PacketIdPool* m_packetIdPool{ nullptr };
			const char* m_topic{ nullptr };
			ByteBuffer m_payload;
			PublishOptions m_publishOptions;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SENDPUBLISHJOB_H