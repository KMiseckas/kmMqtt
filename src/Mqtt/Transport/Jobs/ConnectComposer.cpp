#include <kmMqtt/Mqtt/Transport/Jobs/ConnectComposer.h>
#include <kmMqtt/Mqtt/PacketHelper.h>

namespace kmMqtt
{
	namespace mqtt
	{
		ComposeResult ConnectComposer::compose() noexcept
		{
			Connect packet{ createConnectPacket(*m_mqttConnectionInfo) };
			EncodeResult result{ packet.encode() };

			return ComposeResult{ result, packet.extractDataBuffer()};
		}

		void ConnectComposer::cancel() noexcept
		{
			// No specific cancellation logic for Connect job
		}
	}
}
