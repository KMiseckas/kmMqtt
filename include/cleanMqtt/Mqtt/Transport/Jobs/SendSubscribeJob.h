#ifndef INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SENDSUBSCRIBEJOB_H
#define INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SENDSUBSCRIBEJOB_H

#include <cleanMqtt/Mqtt/Transport/ISendJob.h>
#include <cleanMqtt/Mqtt/Params/SubscribeOptions.h>
#include <cleanMqtt/Mqtt/Params/Topic.h>
#include <cleanMqtt/Utils/PacketIdPool.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		class SendSubscribeJob : public interfaces::ISendJob
		{
		public:
			SendSubscribeJob(
				MqttConnectionInfo* connectionInfo,
				PacketSendDelegate sendPacketCallback,
				PacketIdPool* packetIdPool,
				const PacketID packetId,
				std::vector<Topic> topics,
				SubscribeOptions&& subscribeOptions) noexcept;

			interfaces::SendResultData send() noexcept override;
			void cancel() noexcept override;

		private:
			PacketIdPool* m_packetIdPool{ nullptr };
			PacketID m_packetId{ 0 };
			std::vector<Topic> m_topics;
			SubscribeOptions m_subscribeOptions;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SENDSUBSCRIBEJOB_H
