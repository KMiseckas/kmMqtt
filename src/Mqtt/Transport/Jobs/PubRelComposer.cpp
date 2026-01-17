#include <kmMqtt/Mqtt/Transport/Jobs/PubRelComposer.h>
#include <kmMqtt/Mqtt/PacketHelper.h>

namespace kmMqtt
{
	namespace mqtt
	{
		ComposeResult PubRelComposer::compose() noexcept
		{
			PublishRel packet{ createPubRelPacket(m_publishPacketId, m_reasonCode, m_options) };
			EncodeResult result{ packet.encode() };
			result.packetId = m_publishPacketId;

			return ComposeResult{ result, packet.extractDataBuffer() };
		}

		void PubRelComposer::cancel() noexcept
		{
		}
	}
}
