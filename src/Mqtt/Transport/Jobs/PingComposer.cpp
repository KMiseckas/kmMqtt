#include <cleanMqtt/Mqtt/Transport/Jobs/PingComposer.h>
#include <cleanMqtt/Mqtt/PacketHelper.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		ComposeResult PingComposer::compose() noexcept
		{
			auto packet{ createPingRequestPacket() };
			auto result{ packet.encode() };

			return ComposeResult{ result, packet.extractDataBuffer() };
		}

		void PingComposer::cancel() noexcept
		{
		}
	}
}
