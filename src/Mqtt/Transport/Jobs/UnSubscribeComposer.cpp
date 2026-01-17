#include <kmMqtt/Mqtt/Transport/Jobs/UnSubscribeComposer.h>
#include <kmMqtt/Mqtt/PacketHelper.h>

namespace kmMqtt
{
	namespace mqtt
	{
		UnSubscribeComposer::UnSubscribeComposer(
			MqttConnectionInfo* connectionInfo,
			PacketIdPool* packetIdPool,
			const std::uint16_t packetId,
			std::vector<Topic> topics,
			UnSubscribeOptions&& options) noexcept :
			IPacketComposer(connectionInfo),
			m_packetIdPool{ packetIdPool },
			m_packetId{ packetId },
			m_topics{ std::move(topics) },
			m_unSubscribeOptions{ std::move(options) }
		{
		}

		ComposeResult UnSubscribeComposer::compose() noexcept
		{
			UnSubscribe packet{ createUnSubscribePacket(m_packetId, m_topics, m_unSubscribeOptions) };
			EncodeResult result{ packet.encode() };
			result.packetId = m_packetId;

			return ComposeResult{ result, packet.extractDataBuffer() };
		}

		void UnSubscribeComposer::cancel() noexcept
		{
			m_packetIdPool->releaseId(m_packetId);
		}
	}
}
