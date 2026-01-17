#include <kmMqtt/Mqtt/Transport/Jobs/PubRecComposer.h>
#include <kmMqtt/Mqtt/PacketHelper.h>

namespace kmMqtt
{
	namespace mqtt
	{
		ComposeResult PubRecComposer::compose() noexcept
		{
			PublishRec packet{ createPubRecPacket(m_publishPacketId, m_reasonCode, m_options) };
			EncodeResult result{ packet.encode() };
			result.packetId = m_publishPacketId;

			return ComposeResult{ result, packet.extractDataBuffer() };
		}

		void PubRecComposer::cancel() noexcept
		{
		}
	}
}
