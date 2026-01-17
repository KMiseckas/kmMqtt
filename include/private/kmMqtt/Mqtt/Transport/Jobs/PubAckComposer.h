#ifndef INCLUDE_KMMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_PUBACKCOMPOSER_H
#define INCLUDE_KMMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_PUBACKCOMPOSER_H

#include <kmMqtt/Mqtt/Transport/IPacketComposer.h>
#include <kmMqtt/Mqtt/Packets/Publish/Codes/PubAckReasonCode.h>
#include <kmMqtt/Mqtt/Params/PubAckOptions.h>

namespace kmMqtt
{
	namespace mqtt
	{
		class PubAckComposer : public IPacketComposer
		{
		public:
			PubAckComposer(mqtt::MqttConnectionInfo* connectionInfo,
				std::uint16_t publishPacketId,
				PubAckReasonCode reasonCode,
				PubAckOptions&& options) noexcept
				: IPacketComposer(connectionInfo),
				m_publishPacketId(publishPacketId),
				m_reasonCode(std::move(reasonCode)),
				m_options(std::move(options))
			{
			};

			ComposeResult compose() noexcept override;
			void cancel() noexcept override;

		private:
			std::uint16_t m_publishPacketId;
			PubAckReasonCode m_reasonCode;
			PubAckOptions m_options;
		};
	}
}

#endif //INCLUDE_KMMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_PUBACKCOMPOSER_H