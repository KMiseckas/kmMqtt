#ifndef INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_PUBRELCOMPOSER_H
#define INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_PUBRELCOMPOSER_H

#include <cleanMqtt/Mqtt/Transport/IPacketComposer.h>
#include <cleanMqtt/Mqtt/Packets/Publish/Codes/PubRelReasonCode.h>
#include <cleanMqtt/Mqtt/Params/PubRelOptions.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		class PubRelComposer : public IPacketComposer
		{
		public:
			PubRelComposer(mqtt::MqttConnectionInfo* connectionInfo,
				std::uint16_t publishPacketId,
				PubRelReasonCode reasonCode,
				PubRelOptions&& options) noexcept
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
			PubRelReasonCode m_reasonCode;
			PubRelOptions m_options;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_PUBRELCOMPOSER_H