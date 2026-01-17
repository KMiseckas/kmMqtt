#ifndef INCLUDE_KMMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_PUBCOMPCOMPOSER_H
#define INCLUDE_KMMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_PUBCOMPCOMPOSER_H

#include <kmMqtt/Mqtt/Transport/IPacketComposer.h>
#include <kmMqtt/Mqtt/Packets/Publish/Codes/PubCompReasonCode.h>
#include <kmMqtt/Mqtt/Params/PubCompOptions.h>

namespace kmMqtt
{
	namespace mqtt
	{
		class PubCompComposer : public IPacketComposer
		{
		public:
			PubCompComposer(mqtt::MqttConnectionInfo* connectionInfo,
				std::uint16_t publishPacketId,
				PubCompReasonCode reasonCode,
				PubCompOptions&& options) noexcept
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
			PubCompReasonCode m_reasonCode;
			PubCompOptions m_options;
		};
	}
}

#endif //INCLUDE_KMMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_PUBCOMPCOMPOSER_H