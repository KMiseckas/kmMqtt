#include <cleanMqtt/Mqtt/Packets/PacketUtils.h>
#include <cleanMqtt/Logger/Log.h>
#include <stdexcept>

namespace cleanMqtt
{
	namespace mqtt
	{
		PacketType checkPacketType(const std::uint8_t* data, std::size_t size)
		{
			if (size == 0)
			{
				LogException("Packet Type Check", std::invalid_argument("Data buffer size is 0. Cannot determine packet type from provided data buffer."));
			}

			return static_cast<PacketType>(data[0] >> 4);
		}
	}
}