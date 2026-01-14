#ifndef INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_DISCONNECTCOMPOSER_H
#define INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_DISCONNECTCOMPOSER_H

#include <cleanMqtt/Mqtt/Transport/IPacketComposer.h>
#include <cleanMqtt/mqtt/Params/DisconnectArgs.h>
#include <utility>

namespace cleanMqtt
{
	namespace mqtt
	{
		class DisconnectComposer : public IPacketComposer
		{
		public:
			DisconnectComposer(mqtt::MqttConnectionInfo* connectionInfo,
				const DisconnectArgs&& options,
				DisconnectReasonCode reasonCode) noexcept
				: IPacketComposer(connectionInfo),
				m_options(std::move(options)),
				m_reasonCode(reasonCode)
			{
			};

			ComposeResult compose() noexcept override;
			void cancel() noexcept override;

		private:
			DisconnectArgs m_options;
			DisconnectReasonCode m_reasonCode;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_INTERFACES_SENDQUEUEJOBS_DISCONNECTCOMPOSER_H
