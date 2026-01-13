#include <cleanMqtt/Mqtt/Transport/Jobs/PubAckComposer.h>
#include <cleanMqtt/Mqtt/PacketHelper.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		ComposeResult PubAckComposer::compose() noexcept
		{
			PublishAck packet{ createPubAckPacket(m_publishPacketId, m_reasonCode, m_options) };
			EncodeResult result{ packet.encode() };
			result.packetId = m_publishPacketId;

			return ComposeResult{ result, packet.extractDataBuffer() };
		}

		void PubAckComposer::cancel() noexcept
		{
		}
	}
}
