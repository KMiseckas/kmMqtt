#include <cleanMqtt/Mqtt/Transport/Jobs/PubRecComposer.h>
#include <cleanMqtt/Mqtt/PacketHelper.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		ComposeResult PubRecComposer::compose() noexcept
		{
			PublishRec packet{ createPubRecPacket(m_publishPacketId, m_reasonCode, m_options) };
			EncodeResult result{ packet.encode() };

			return ComposeResult{ result, packet.extractDataBuffer() };
		}

		void PubRecComposer::cancel() noexcept
		{
		}
	}
}
