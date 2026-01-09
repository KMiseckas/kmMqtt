#ifndef INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_PUBLISHCOMPOSER_H
#define INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_PUBLISHCOMPOSER_H

#include <cleanMqtt/Mqtt/Transport/IPacketComposer.h>
#include "cleanMqtt/Mqtt/Params/PublishOptions.h"
#include <cleanMqtt/Utils/PacketIdPool.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		class PublishComposer : public IPacketComposer
		{
		public:
			PublishComposer(MqttConnectionInfo* connectionInfo,
				PacketIdPool* packetIdPool,
				const std::uint16_t packetId,
				std::string topic,
				ByteBuffer&& payload,
				PublishOptions&& pubOptions) noexcept;

			bool canSend() const noexcept override;
			ComposeResult compose() noexcept override;
			Qos getQos() const noexcept override;
			void cancel() noexcept override;

		private:
			PacketIdPool* m_packetIdPool{ nullptr };
			std::uint16_t m_packetId{ 0 };
			std::string m_topic{ nullptr };
			ByteBuffer m_payload;
			PublishOptions m_publishOptions;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_PUBLISHCOMPOSER_H