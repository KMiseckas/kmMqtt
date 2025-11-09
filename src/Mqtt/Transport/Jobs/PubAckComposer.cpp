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

			return ComposeResult{ result, packet.getDataBuffer() };
		}

		void PubAckComposer::cancel() noexcept
		{
		}
	}
}
