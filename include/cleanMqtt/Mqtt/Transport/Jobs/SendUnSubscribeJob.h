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
		class SendUnSubscribeJob : public ISendJob
		{
		public:
			SendUnSubscribeJob(
				MqttConnectionInfo* connectionInfo,
				PacketSendDelegate sendPacketCallback,
				PacketIdPool* packetIdPool,
				const std::uint16_t packetId,
				std::vector<Topic> topics,
				UnSubscribeOptions&& options) noexcept;

			SendResultData send() noexcept override;
			void cancel() noexcept override;

		private:
			PacketIdPool* m_packetIdPool{ nullptr };
			std::uint16_t m_packetId{ 0 };
			std::vector<Topic> m_topics;
			UnSubscribeOptions m_unSubscribeOptions{};
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SENDUNSUBSCRIBEJOB_H
