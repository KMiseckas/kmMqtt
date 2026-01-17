#include <kmMqtt/Mqtt/Transport/Jobs/DisconnectComposer.h>
#include <kmMqtt/Mqtt/PacketHelper.h>

namespace kmMqtt
{
	namespace mqtt
	{
		ComposeResult DisconnectComposer::compose() noexcept
		{
			Disconnect packet{ createDisconnectPacket(*m_mqttConnectionInfo, m_options, m_reasonCode) };
			EncodeResult result{ packet.encode() };

			return ComposeResult{ result, packet.extractDataBuffer() };
		}

		void DisconnectComposer::cancel() noexcept
		{
			// No specific cancellation logic for Connect job
		}
	}
}
