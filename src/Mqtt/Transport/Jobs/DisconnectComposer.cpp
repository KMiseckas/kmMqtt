#include <cleanMqtt/Mqtt/Transport/Jobs/DisconnectComposer.h>
#include <cleanMqtt/Mqtt/PacketHelper.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		ComposeResult DisconnectComposer::compose() noexcept
		{
			Connect packet{ createDisconnectPacket(*m_mqttConnectionInfo, m_options, m_reasonCode) };
			EncodeResult result{ packet.encode() };

			return ComposeResult{ result, packet.extractDataBuffer() };
		}

		void DisconnectComposer::cancel() noexcept
		{
			// No specific cancellation logic for Connect job
		}
	}
}
