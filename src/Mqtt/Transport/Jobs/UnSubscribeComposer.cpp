#include <cleanMqtt/Mqtt/Transport/Jobs/UnSubscribeComposer.h>
#include <cleanMqtt/Mqtt/PacketHelper.h>

namespace cleanMqtt
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

			return ComposeResult{ result, packet.getDataBuffer() };
		}

		void UnSubscribeComposer::cancel() noexcept
		{
			m_packetIdPool->releaseId(m_packetId);
		}
	}
}
