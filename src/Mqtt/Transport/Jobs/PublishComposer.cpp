#include "kmMqtt/Mqtt/Transport/Jobs/PublishComposer.h"
#include "kmMqtt/Mqtt/PacketHelper.h"

namespace kmMqtt
{
	namespace mqtt
	{
		PublishComposer::PublishComposer(MqttConnectionInfo* connectionInfo,
			PacketIdPool* packetIdPool,
			const std::uint16_t packetId,
			std::string topic,
			ByteBuffer&& payload,
			PublishOptions&& pubOptions,
			ReceiveMaximumTracker* recMaxTracker,
			bool isDup) noexcept :
			IPacketComposer(connectionInfo),
			m_packetIdPool{ packetIdPool },
			m_packetId{ packetId },
			m_topic{ std::move(topic) },
			m_payload{ std::move(payload) },
			m_publishOptions{ std::move(pubOptions) },
			m_recMaxTracker{ recMaxTracker },
			m_isDup{ isDup }
		{
		}

		bool PublishComposer::canSend() const noexcept
		{
			if (m_publishOptions.qos == Qos::QOS_0)
			{
				return true;
			}

			return m_recMaxTracker->hasSendAllowance();
		}

		ComposeResult PublishComposer::compose() noexcept
		{
			Publish packet{ createPublishPacket(*m_mqttConnectionInfo, m_isDup, m_topic.c_str(), m_payload, m_publishOptions, m_packetId)};
			EncodeResult result{ packet.encode() };
			result.packetId = m_packetId;

			return ComposeResult{ result, packet.extractDataBuffer() };
		}

		Qos PublishComposer::getQos() const noexcept
		{
			return m_publishOptions.qos;
		}

		void PublishComposer::cancel() noexcept
		{
			m_packetIdPool->releaseId(m_packetId);
		}
	}
}
