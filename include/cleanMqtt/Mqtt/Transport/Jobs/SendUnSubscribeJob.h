#ifndef INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SENDUNSUBSCRIBEJOB_H
#define INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SENDUNSUBSCRIBEJOB_H

#include <cleanMqtt/Mqtt/Transport/ISendJob.h>
#include <cleanMqtt/Mqtt/Params/Topic.h>
#include <cleanMqtt/Utils/PacketIdPool.h>
#include <cleanMqtt/Mqtt/Params/UnSubscribeOptions.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		class SendUnSubscribeJob : public interfaces::ISendJob
		{
		public:
			SendUnSubscribeJob(
				MqttConnectionInfo* connectionInfo,
				PacketSendDelegate sendPacketCallback,
				PacketIdPool* packetIdPool,
				const PacketID packetId,
				std::vector<Topic> topics,
				UnSubscribeOptions&& options) noexcept;

			interfaces::SendResultData send() noexcept override;
			void cancel() noexcept override;

		private:
			PacketIdPool* m_packetIdPool{ nullptr };
			PacketID m_packetId{ 0 };
			std::vector<Topic> m_topics;
			UnSubscribeOptions m_unSubscribeOptions{};
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SENDUNSUBSCRIBEJOB_H
