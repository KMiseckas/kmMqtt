#include <cleanMqtt/Mqtt/Transport/Jobs/SubscribeComposer.h>
#include <cleanMqtt/Mqtt/PacketHelper.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		SubscribeComposer::SubscribeComposer(
			MqttConnectionInfo* connectionInfo,
			PacketIdPool* packetIdPool,
			const std::uint16_t packetId,
			std::vector<Topic> topics,
			SubscribeOptions&& subscribeOptions) noexcept :
			IPacketComposer(connectionInfo),
			m_packetIdPool{ packetIdPool },
			m_packetId{ packetId },
			m_topics{ std::move(topics) },
			m_subscribeOptions{ std::move(subscribeOptions) }
		{
		}

		ComposeResult SubscribeComposer::compose() noexcept
		{
			Subscribe packet{ createSubscribePacket(m_packetId, m_topics, m_subscribeOptions) };
			EncodeResult result{ packet.encode() };
			result.packetId = m_packetId;

			return ComposeResult{ result, packet.extractDataBuffer() };
		}

		void SubscribeComposer::cancel() noexcept
		{
			m_packetIdPool->releaseId(m_packetId);
		}
	}
}
