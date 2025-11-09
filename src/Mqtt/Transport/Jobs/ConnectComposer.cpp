#include <cleanMqtt/Mqtt/Transport/Jobs/ConnectComposer.h>
#include <cleanMqtt/Mqtt/PacketHelper.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		ComposeResult ConnectComposer::compose() noexcept
		{
			Connect packet{ createConnectPacket(*m_mqttConnectionInfo) };
			EncodeResult result{ packet.encode() };

			return ComposeResult{ result, packet.getDataBuffer()};
		}

		void ConnectComposer::cancel() noexcept
		{
			// No specific cancellation logic for Connect job
		}
	}
}
