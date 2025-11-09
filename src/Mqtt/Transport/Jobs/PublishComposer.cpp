#include <cleanMqtt/Mqtt/Transport/Jobs/PublishComposer.h>
#include <cleanMqtt/Mqtt/PacketHelper.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		PublishComposer::PublishComposer(MqttConnectionInfo* connectionInfo,
			PacketIdPool* packetIdPool,
			const std::uint16_t packetId,
			const char* topic,
			ByteBuffer&& payload,
			PublishOptions&& pubOptions) noexcept :
			IPacketComposer(connectionInfo),
			m_packetIdPool{ packetIdPool },
			m_packetId{ packetId },
			m_topic{ topic },
			m_payload{ std::move(payload) },
			m_publishOptions{ std::move(pubOptions) }
		{
		}

		ComposeResult PublishComposer::compose() noexcept
		{
			Publish packet{ createPublishPacket(*m_mqttConnectionInfo, m_topic, m_payload, m_publishOptions, m_packetId)};
			EncodeResult result{ packet.encode() };

			return ComposeResult{ result, packet.extractDataBuffer() };
		}

		void PublishComposer::cancel() noexcept
		{
			
		}
	}
}
