#include <kmMqtt/Mqtt/Transport/Jobs/PubCompComposer.h>
#include <kmMqtt/Mqtt/PacketHelper.h>

namespace kmMqtt
{
	namespace mqtt
	{
		ComposeResult PubCompComposer::compose() noexcept
		{
			PublishComp packet{ createPubCompPacket(m_publishPacketId, m_reasonCode, m_options) };
			EncodeResult result{ packet.encode() };
			result.packetId = m_publishPacketId;

			return ComposeResult{ result, packet.extractDataBuffer() };
		}

		void PubCompComposer::cancel() noexcept
		{
		}
	}
}
