#ifndef INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_PUBRECCOMPOSER_H
#define INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_PUBRECCOMPOSER_H

#include <cleanMqtt/Mqtt/Transport/IPacketComposer.h>
#include <cleanMqtt/Mqtt/Packets/Publish/Codes/PubRecReasonCode.h>
#include <cleanMqtt/Mqtt/Params/PubRecOptions.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		class PubRecComposer : public IPacketComposer
		{
		public:
			PubRecComposer(mqtt::MqttConnectionInfo* connectionInfo,
				std::uint16_t publishPacketId,
				PubRecReasonCode reasonCode,
				PubRecOptions&& options) noexcept
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
			PubRecReasonCode m_reasonCode;
			PubRecOptions m_options;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_PUBRECCOMPOSER_H