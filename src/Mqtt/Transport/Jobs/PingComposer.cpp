#include <kmMqtt/Mqtt/Transport/Jobs/PingComposer.h>
#include <kmMqtt/Mqtt/PacketHelper.h>

namespace kmMqtt
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
