#ifndef INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SUBSCRIBECOMPOSER_H
#define INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SUBSCRIBECOMPOSER_H

#include <cleanMqtt/Mqtt/Transport/IPacketComposer.h>
#include <cleanMqtt/Mqtt/Params/SubscribeOptions.h>
#include <cleanMqtt/Mqtt/Params/Topic.h>
#include <cleanMqtt/Utils/PacketIdPool.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		class SubscribeComposer : public IPacketComposer
		{
		public:
			SubscribeComposer(
				MqttConnectionInfo* connectionInfo,
				PacketIdPool* packetIdPool,
				const std::uint16_t packetId,
				std::vector<Topic> topics,
				SubscribeOptions&& subscribeOptions) noexcept;

			ComposeResult compose() noexcept override;
			void cancel() noexcept override;

		private:
			PacketIdPool* m_packetIdPool{ nullptr };
			std::uint16_t m_packetId{ 0 };
			std::vector<Topic> m_topics;
			SubscribeOptions m_subscribeOptions;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_SUBSCRIBECOMPOSER_H
